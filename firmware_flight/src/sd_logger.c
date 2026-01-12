//----------------------------------------------
// Module: sd_logger.c
// Description: SD card flight data logging using FatFs
// Author: Mark Gavin
// Created: 2025-12-19
// Modified: 2026-01-10 (Rocket Avionics)
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "sd_logger.h"
#include "pins.h"
#include "ff.h"

#include "pico/stdlib.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//----------------------------------------------
// Module State
//----------------------------------------------
static bool sSdAvailable = false ;
static bool sFlightInProgress = false ;
static char sCurrentFilename[64] = "" ;
static char sLastFilename[64] = "" ;

// FatFs objects
static FATFS sFatFs ;
static FIL sFile ;
static bool sFileOpen = false ;

// Sample counter for file
static uint32_t sSampleCount = 0 ;

//----------------------------------------------
// Function: SdLogger_Init
//----------------------------------------------
bool SdLogger_Init(void)
{
  printf("SD: Initializing with FatFs...\n") ;

  // Mount the filesystem
  FRESULT theResult = f_mount(&sFatFs, "", 1) ; // Mount immediately

  if (theResult != FR_OK)
  {
    printf("SD: Mount failed (error %d)\n", theResult) ;
    sSdAvailable = false ;
    return false ;
  }

  printf("SD: Filesystem mounted successfully\n") ;

  // Create flights directory if it doesn't exist
  theResult = f_mkdir("flights") ;
  if (theResult != FR_OK && theResult != FR_EXIST)
  {
    printf("SD: Warning - could not create flights directory (error %d)\n", theResult) ;
  }

  sSdAvailable = true ;
  return true ;
}

//----------------------------------------------
// Function: SdLogger_IsAvailable
//----------------------------------------------
bool SdLogger_IsAvailable(void)
{
  return sSdAvailable ;
}

//----------------------------------------------
// Function: SdLogger_GetFreeSpace
//----------------------------------------------
uint32_t SdLogger_GetFreeSpace(void)
{
  if (!sSdAvailable) return 0 ;

  DWORD theFreeClusters ;
  FATFS * theFs ;
  FRESULT theResult = f_getfree("", &theFreeClusters, &theFs) ;

  if (theResult != FR_OK) return 0 ;

  // Calculate free space in bytes
  // Note: This may overflow for very large cards, but that's OK for our purposes
  uint32_t theFreeBytes = theFreeClusters * theFs->csize * 512 ;
  return theFreeBytes ;
}

//----------------------------------------------
// Internal: Write string to file
//----------------------------------------------
static bool WriteString(const char * inStr)
{
  UINT theBytesWritten ;
  FRESULT theResult = f_write(&sFile, inStr, strlen(inStr), &theBytesWritten) ;
  return (theResult == FR_OK && theBytesWritten == strlen(inStr)) ;
}

//----------------------------------------------
// Function: SdLogger_StartFlight
//----------------------------------------------
bool SdLogger_StartFlight(uint32_t inTimestamp)
{
  if (!sSdAvailable) return false ;

  // End any previous flight
  if (sFlightInProgress)
  {
    SdLogger_EndFlight(0, 0, 0) ;
  }

  // Create filename from timestamp
  snprintf(sCurrentFilename, sizeof(sCurrentFilename),
           "flights/flight_%lu.csv", (unsigned long)inTimestamp) ;

  printf("SD: Creating file: %s\n", sCurrentFilename) ;

  // Open file for writing
  FRESULT theResult = f_open(&sFile, sCurrentFilename, FA_WRITE | FA_CREATE_ALWAYS) ;
  if (theResult != FR_OK)
  {
    printf("SD: Failed to create file (error %d)\n", theResult) ;
    return false ;
  }

  sFileOpen = true ;

  // Write CSV header
  WriteString("time_ms,altitude_m,velocity_mps,pressure_pa,temperature_c\n") ;

  sSampleCount = 0 ;
  sFlightInProgress = true ;

  printf("SD: Started flight log\n") ;
  return true ;
}

//----------------------------------------------
// Function: SdLogger_LogSample
//----------------------------------------------
bool SdLogger_LogSample(const SdFlightSample * inSample)
{
  if (!sFlightInProgress || !sFileOpen || inSample == NULL) return false ;

  // Format and write sample to file
  char theBuffer[128] ;
  snprintf(theBuffer, sizeof(theBuffer), "%lu,%.2f,%.2f,%.1f,%.2f\n",
           (unsigned long)inSample->timeMs,
           inSample->altitudeM,
           inSample->velocityMps,
           inSample->pressurePa,
           inSample->temperatureC) ;
  WriteString(theBuffer) ;

  sSampleCount++ ;

  // Sync periodically (every 100 samples) to prevent data loss
  if ((sSampleCount % 100) == 0)
  {
    f_sync(&sFile) ;
  }

  return true ;
}

//----------------------------------------------
// Function: SdLogger_EndFlight
//----------------------------------------------
bool SdLogger_EndFlight(float inMaxAltitudeM, uint32_t inFlightTimeMs, float inMaxVelocityMps)
{
  if (!sFlightInProgress) return false ;

  // Write summary comment at end of file
  if (sFileOpen)
  {
    char theBuffer[64] ;

    WriteString("# Summary\n") ;

    snprintf(theBuffer, sizeof(theBuffer), "# Samples: %lu\n", (unsigned long)sSampleCount) ;
    WriteString(theBuffer) ;

    snprintf(theBuffer, sizeof(theBuffer), "# Max Altitude: %.2f m\n", inMaxAltitudeM) ;
    WriteString(theBuffer) ;

    snprintf(theBuffer, sizeof(theBuffer), "# Max Velocity: %.2f m/s\n", inMaxVelocityMps) ;
    WriteString(theBuffer) ;

    snprintf(theBuffer, sizeof(theBuffer), "# Flight Time: %lu ms\n", (unsigned long)inFlightTimeMs) ;
    WriteString(theBuffer) ;

    // Close file
    f_close(&sFile) ;
    sFileOpen = false ;

    printf("SD: Flight complete - %lu samples saved to %s\n",
           (unsigned long)sSampleCount, sCurrentFilename) ;
  }

  sFlightInProgress = false ;
  strcpy(sLastFilename, sCurrentFilename) ;
  sCurrentFilename[0] = '\0' ;

  return true ;
}

//----------------------------------------------
// Function: SdLogger_ListFlights
//----------------------------------------------
int SdLogger_ListFlights(char ** outFilenames, int inMaxCount)
{
  if (!sSdAvailable || outFilenames == NULL || inMaxCount <= 0) return 0 ;

  DIR theDir ;
  FILINFO theFileInfo ;
  int theCount = 0 ;

  FRESULT theResult = f_opendir(&theDir, "flights") ;
  if (theResult != FR_OK) return 0 ;

  while (theCount < inMaxCount)
  {
    theResult = f_readdir(&theDir, &theFileInfo) ;
    if (theResult != FR_OK || theFileInfo.fname[0] == 0) break ;

    // Skip directories
    if (theFileInfo.fattrib & AM_DIR) continue ;

    // Check for .csv extension
    size_t theLen = strlen(theFileInfo.fname) ;
    if (theLen > 4 && strcmp(&theFileInfo.fname[theLen - 4], ".csv") == 0)
    {
      // Copy filename
      outFilenames[theCount] = malloc(theLen + 1) ;
      if (outFilenames[theCount] != NULL)
      {
        strcpy(outFilenames[theCount], theFileInfo.fname) ;
        theCount++ ;
      }
    }
  }

  f_closedir(&theDir) ;
  return theCount ;
}

//----------------------------------------------
// Function: SdLogger_DeleteFlight
//----------------------------------------------
bool SdLogger_DeleteFlight(const char * inFilename)
{
  if (!sSdAvailable || inFilename == NULL) return false ;

  char thePath[80] ;
  snprintf(thePath, sizeof(thePath), "flights/%s", inFilename) ;

  FRESULT theResult = f_unlink(thePath) ;
  return (theResult == FR_OK) ;
}

//----------------------------------------------
// Function: SdLogger_GetLastFilename
//----------------------------------------------
const char * SdLogger_GetLastFilename(void)
{
  if (sLastFilename[0] == '\0') return NULL ;
  return sLastFilename ;
}

//----------------------------------------------
// Function: SdLogger_GetFlightCount
//----------------------------------------------
int SdLogger_GetFlightCount(void)
{
  if (!sSdAvailable) return 0 ;

  DIR theDir ;
  FILINFO theFileInfo ;
  int theCount = 0 ;

  FRESULT theResult = f_opendir(&theDir, "flights") ;
  if (theResult != FR_OK) return 0 ;

  while (1)
  {
    theResult = f_readdir(&theDir, &theFileInfo) ;
    if (theResult != FR_OK || theFileInfo.fname[0] == 0) break ;

    // Skip directories
    if (theFileInfo.fattrib & AM_DIR) continue ;

    // Check for .csv extension
    size_t theLen = strlen(theFileInfo.fname) ;
    if (theLen > 4 && strcmp(&theFileInfo.fname[theLen - 4], ".csv") == 0)
    {
      theCount++ ;
    }
  }

  f_closedir(&theDir) ;
  return theCount ;
}

//----------------------------------------------
// Function: SdLogger_GetFileList
//----------------------------------------------
int SdLogger_GetFileList(SdFileInfo * outFiles, int inMaxCount)
{
  if (!sSdAvailable || outFiles == NULL || inMaxCount <= 0) return 0 ;

  DIR theDir ;
  FILINFO theFileInfo ;
  int theCount = 0 ;

  FRESULT theResult = f_opendir(&theDir, "flights") ;
  if (theResult != FR_OK) return 0 ;

  while (theCount < inMaxCount)
  {
    theResult = f_readdir(&theDir, &theFileInfo) ;
    if (theResult != FR_OK || theFileInfo.fname[0] == 0) break ;

    // Skip directories
    if (theFileInfo.fattrib & AM_DIR) continue ;

    // Check for .csv extension
    size_t theLen = strlen(theFileInfo.fname) ;
    if (theLen > 4 && strcmp(&theFileInfo.fname[theLen - 4], ".csv") == 0)
    {
      strncpy(outFiles[theCount].pFilename, theFileInfo.fname, sizeof(outFiles[theCount].pFilename) - 1) ;
      outFiles[theCount].pFilename[sizeof(outFiles[theCount].pFilename) - 1] = '\0' ;
      outFiles[theCount].pSize = theFileInfo.fsize ;

      // Extract date/time from FILINFO
      // FatFs date format: bits 15-9 = year since 1980, bits 8-5 = month, bits 4-0 = day
      outFiles[theCount].pYear = ((theFileInfo.fdate >> 9) & 0x7F) + 1980 ;
      outFiles[theCount].pMonth = (theFileInfo.fdate >> 5) & 0x0F ;
      outFiles[theCount].pDay = theFileInfo.fdate & 0x1F ;

      // FatFs time format: bits 15-11 = hour, bits 10-5 = minute, bits 4-0 = second/2
      outFiles[theCount].pHour = (theFileInfo.ftime >> 11) & 0x1F ;
      outFiles[theCount].pMinute = (theFileInfo.ftime >> 5) & 0x3F ;

      theCount++ ;
    }
  }

  f_closedir(&theDir) ;
  return theCount ;
}

//----------------------------------------------
// Function: SdLogger_ReadFile
//----------------------------------------------
uint32_t SdLogger_ReadFile(
  const char * inFilename,
  uint8_t * outBuffer,
  uint32_t inOffset,
  uint32_t inMaxLen)
{
  if (!sSdAvailable || inFilename == NULL || outBuffer == NULL) return 0 ;

  char thePath[80] ;
  snprintf(thePath, sizeof(thePath), "flights/%s", inFilename) ;

  FIL theFile ;
  FRESULT theResult = f_open(&theFile, thePath, FA_READ) ;
  if (theResult != FR_OK) return 0 ;

  // Seek to offset
  if (inOffset > 0)
  {
    theResult = f_lseek(&theFile, inOffset) ;
    if (theResult != FR_OK)
    {
      f_close(&theFile) ;
      return 0 ;
    }
  }

  // Read data
  UINT theBytesRead = 0 ;
  theResult = f_read(&theFile, outBuffer, inMaxLen, &theBytesRead) ;
  f_close(&theFile) ;

  if (theResult != FR_OK) return 0 ;
  return theBytesRead ;
}

//----------------------------------------------
// Function: SdLogger_GetFileSize
//----------------------------------------------
uint32_t SdLogger_GetFileSize(const char * inFilename)
{
  if (!sSdAvailable || inFilename == NULL) return 0 ;

  char thePath[80] ;
  snprintf(thePath, sizeof(thePath), "flights/%s", inFilename) ;

  FILINFO theFileInfo ;
  FRESULT theResult = f_stat(thePath, &theFileInfo) ;
  if (theResult != FR_OK) return 0 ;

  return theFileInfo.fsize ;
}
