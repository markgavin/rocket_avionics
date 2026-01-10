//----------------------------------------------
// Module: storage.c
// Description: Flash storage implementation for
//   calibration data persistence
// Author: Mark Gavin
// Created: 2025-11-29
// Copyright: (c) 2025 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "storage.h"

#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#include <stdio.h>
#include <string.h>

//----------------------------------------------
// Flash Layout Constants
//----------------------------------------------
// Adafruit Feather RP2350 has 8MB (0x800000) flash
// Use last sector for calibration storage
#define kFeatherFlashSize     0x800000    // 8MB
#define kFlashTargetOffset    (kFeatherFlashSize - FLASH_SECTOR_SIZE)

// Pointer to flash storage location (read as memory-mapped)
#define kFlashStoragePtr ((const CalibrationData *)(XIP_BASE + kFlashTargetOffset))

//----------------------------------------------
// Function: Storage_Init
//----------------------------------------------
bool Storage_Init(void)
{
  // Flash is always available on Pico
  // Print storage addresses for debugging
  printf("Flash storage: calibration at 0x%08X (offset 0x%08X)\n",
         (unsigned int)(XIP_BASE + kFlashTargetOffset),
         (unsigned int)kFlashTargetOffset) ;

  return true ;
} // end Storage_Init

//----------------------------------------------
// Function: Storage_SaveCalibration
//----------------------------------------------
bool Storage_SaveCalibration(
  int32_t inOffset ,
  float inScaleFactor)
{
  printf("Storage: Saving calibration (offset=%ld, scale=%.4f)...\n",
         (long)inOffset, inScaleFactor) ;

  // Prepare calibration data
  CalibrationData theData ;
  theData.pMagic = kCalibrationMagic ;
  theData.pVersion = kCalibrationVersion ;
  theData.pOffset = inOffset ;
  theData.pScaleFactor = inScaleFactor ;

  // Calculate checksum (simple sum of all bytes)
  uint32_t theChecksum = 0 ;
  const uint8_t * theBytes = (const uint8_t *)&theData ;
  for (size_t theIndex = 0 ; theIndex < offsetof(CalibrationData, pChecksum) ; theIndex++)
  {
    theChecksum += theBytes[theIndex] ;
  } // end for checksum
  theData.pChecksum = theChecksum ;

  // Prepare buffer aligned to flash page size
  uint8_t theBuffer[FLASH_PAGE_SIZE] ;
  memset(theBuffer, 0xFF, sizeof(theBuffer)) ;
  memcpy(theBuffer, &theData, sizeof(CalibrationData)) ;

  // Flush stdio before flash operations (USB CDC may stall)
  stdio_flush() ;

  // Disable interrupts during flash operations
  uint32_t theInterrupts = save_and_disable_interrupts() ;

  // Erase the sector
  flash_range_erase(kFlashTargetOffset, FLASH_SECTOR_SIZE) ;

  // Program the data
  flash_range_program(kFlashTargetOffset, theBuffer, FLASH_PAGE_SIZE) ;

  restore_interrupts(theInterrupts) ;

  printf("Storage: Calibration saved\n") ;

  // Verify write
  return Storage_HasCalibration() ;
} // end Storage_SaveCalibration

//----------------------------------------------
// Function: Storage_LoadCalibration
//----------------------------------------------
bool Storage_LoadCalibration(
  int32_t * outOffset ,
  float * outScaleFactor)
{
  if (outOffset == NULL || outScaleFactor == NULL)
  {
    return false ;
  } // end if null

  // Check if valid calibration exists
  if (!Storage_HasCalibration())
  {
    return false ;
  } // end if no calibration

  // Read from flash (memory-mapped)
  *outOffset = kFlashStoragePtr->pOffset ;
  *outScaleFactor = kFlashStoragePtr->pScaleFactor ;

  return true ;
} // end Storage_LoadCalibration

//----------------------------------------------
// Function: Storage_HasCalibration
//----------------------------------------------
bool Storage_HasCalibration(void)
{
  // Check magic number
  if (kFlashStoragePtr->pMagic != kCalibrationMagic)
  {
    return false ;
  } // end if wrong magic

  // Check version
  if (kFlashStoragePtr->pVersion != kCalibrationVersion)
  {
    return false ;
  } // end if wrong version

  // Verify checksum
  uint32_t theChecksum = 0 ;
  const uint8_t * theBytes = (const uint8_t *)kFlashStoragePtr ;
  for (size_t theIndex = 0 ; theIndex < offsetof(CalibrationData, pChecksum) ; theIndex++)
  {
    theChecksum += theBytes[theIndex] ;
  } // end for checksum

  if (theChecksum != kFlashStoragePtr->pChecksum)
  {
    return false ;
  } // end if checksum mismatch

  return true ;
} // end Storage_HasCalibration

//----------------------------------------------
// Function: Storage_ClearCalibration
//----------------------------------------------
bool Storage_ClearCalibration(void)
{
  // Disable interrupts during flash operations
  uint32_t theInterrupts = save_and_disable_interrupts() ;

  // Erase the sector (sets all bytes to 0xFF)
  flash_range_erase(kFlashTargetOffset, FLASH_SECTOR_SIZE) ;

  restore_interrupts(theInterrupts) ;

  // Verify erase
  return !Storage_HasCalibration() ;
} // end Storage_ClearCalibration
