//----------------------------------------------
// Module: flight_storage.c
// Description: Flash storage for flight data
// Author: Mark Gavin
// Created: 2026-01-13
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "flight_storage.h"
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "hardware/watchdog.h"

#include <stdio.h>
#include <string.h>

#define printf(...) ((void)0)

//----------------------------------------------
// Module Constants
//----------------------------------------------
#define kMaxSamplesPerFlight    1200    // ~120 seconds at 10 Hz
#define kSampleBufferSize       (kMaxSamplesPerFlight * sizeof(FlightSample))

//----------------------------------------------
// Module State
//----------------------------------------------
static bool sInitialized = false ;
static bool sRecording = false ;
static uint32_t sNextFlightId = 1 ;
static uint8_t sSlotUsed[kMaxStoredFlights] ;
static int8_t sCurrentSlot = -1 ;

// RAM buffer for samples during flight
static FlightSample sSampleBuffer[kMaxSamplesPerFlight] ;
static uint32_t sSampleCount = 0 ;

// Current flight header (built during flight)
static FlightHeader sCurrentHeader ;

//----------------------------------------------
// Internal: Calculate Checksum
//----------------------------------------------
static uint32_t CalculateChecksum(const void * inData, size_t inSize)
{
  const uint8_t * theBytes = (const uint8_t *)inData ;
  uint32_t theSum = 0 ;

  for (size_t i = 0 ; i < inSize ; i++)
  {
    theSum += theBytes[i] ;
  }

  return theSum ;
}

//----------------------------------------------
// Internal: Load Index from Flash
//----------------------------------------------
static bool LoadIndex(void)
{
  // Read index from flash (memory-mapped)
  const uint8_t * theIndexPtr = (const uint8_t *)(XIP_BASE + kFlightIndexOffset) ;

  // Check magic number
  uint32_t theMagic = *(const uint32_t *)theIndexPtr ;
  if (theMagic != kFlightIndexMagic)
  {
    printf("FlightStorage: No valid index found (magic=0x%08lX)\n", (unsigned long)theMagic) ;
    return false ;
  }

  // Read version
  uint32_t theVersion = *(const uint32_t *)(theIndexPtr + 4) ;
  if (theVersion != kFlightVersion)
  {
    printf("FlightStorage: Index version mismatch (%lu vs %d)\n",
      (unsigned long)theVersion, kFlightVersion) ;
    return false ;
  }

  // Read next flight ID
  sNextFlightId = *(const uint32_t *)(theIndexPtr + 8) ;

  // Read slot usage
  memcpy(sSlotUsed, theIndexPtr + 12, kMaxStoredFlights) ;

  printf("FlightStorage: Loaded index, next ID=%lu\n", (unsigned long)sNextFlightId) ;
  return true ;
}

//----------------------------------------------
// Internal: Save Index to Flash
//----------------------------------------------
static bool SaveIndex(void)
{
  // Build index data
  uint8_t theBuffer[FLASH_PAGE_SIZE] ;
  memset(theBuffer, 0xFF, sizeof(theBuffer)) ;

  // Magic, version, next ID
  *(uint32_t *)(theBuffer + 0) = kFlightIndexMagic ;
  *(uint32_t *)(theBuffer + 4) = kFlightVersion ;
  *(uint32_t *)(theBuffer + 8) = sNextFlightId ;

  // Slot usage
  memcpy(theBuffer + 12, sSlotUsed, kMaxStoredFlights) ;

  // Checksum
  uint32_t theChecksum = CalculateChecksum(theBuffer, 12 + kMaxStoredFlights) ;
  *(uint32_t *)(theBuffer + 12 + kMaxStoredFlights) = theChecksum ;

  // Write to flash
  uint32_t theInterrupts = save_and_disable_interrupts() ;
  flash_range_erase(kFlightIndexOffset, FLASH_SECTOR_SIZE) ;
  flash_range_program(kFlightIndexOffset, theBuffer, FLASH_PAGE_SIZE) ;
  restore_interrupts(theInterrupts) ;

  printf("FlightStorage: Saved index\n") ;
  return true ;
}

//----------------------------------------------
// Internal: Find Free Slot
//----------------------------------------------
static int8_t FindFreeSlot(void)
{
  for (uint8_t i = 0 ; i < kMaxStoredFlights ; i++)
  {
    if (!sSlotUsed[i])
    {
      return (int8_t)i ;
    }
  }
  return -1 ;  // No free slots
}

//----------------------------------------------
// Internal: Write Flight to Flash
//----------------------------------------------
static bool WriteFlightToFlash(uint8_t inSlot)
{
  if (inSlot >= kMaxStoredFlights)
  {
    return false ;
  }

  uint32_t theSlotOffset = kFlightSlotsOffset + (inSlot * kFlightSlotSize) ;

  printf("FlightStorage: Writing flight to slot %d at offset 0x%08lX\n",
    inSlot, (unsigned long)theSlotOffset) ;
  printf("  Samples: %lu, Header: %lu bytes\n",
    (unsigned long)sSampleCount, (unsigned long)sizeof(FlightHeader)) ;

  // Calculate number of sectors to erase
  uint32_t theDataSize = sizeof(FlightHeader) + (sSampleCount * sizeof(FlightSample)) ;
  uint32_t theSectorsNeeded = (theDataSize + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE ;
  if (theSectorsNeeded > (kFlightSlotSize / FLASH_SECTOR_SIZE))
  {
    theSectorsNeeded = kFlightSlotSize / FLASH_SECTOR_SIZE ;
  }

  printf("  Data size: %lu bytes, erasing %lu sectors\n",
    (unsigned long)theDataSize, (unsigned long)theSectorsNeeded) ;

  // Disable interrupts for flash operations
  uint32_t theInterrupts = save_and_disable_interrupts() ;

  // Erase required sectors
  flash_range_erase(theSlotOffset, theSectorsNeeded * FLASH_SECTOR_SIZE) ;
  watchdog_update() ;  // Feed watchdog after long erase

  // Write header (first page)
  uint8_t thePageBuffer[FLASH_PAGE_SIZE] ;
  memset(thePageBuffer, 0xFF, sizeof(thePageBuffer)) ;
  memcpy(thePageBuffer, &sCurrentHeader, sizeof(FlightHeader)) ;
  flash_range_program(theSlotOffset, thePageBuffer, FLASH_PAGE_SIZE) ;

  // Write samples in pages
  uint32_t theSampleOffset = theSlotOffset + FLASH_PAGE_SIZE ;
  const uint8_t * theSamplePtr = (const uint8_t *)sSampleBuffer ;
  uint32_t theBytesRemaining = sSampleCount * sizeof(FlightSample) ;

  while (theBytesRemaining > 0)
  {
    uint32_t theBytesToWrite = (theBytesRemaining < FLASH_PAGE_SIZE) ?
      theBytesRemaining : FLASH_PAGE_SIZE ;

    memset(thePageBuffer, 0xFF, sizeof(thePageBuffer)) ;
    memcpy(thePageBuffer, theSamplePtr, theBytesToWrite) ;

    flash_range_program(theSampleOffset, thePageBuffer, FLASH_PAGE_SIZE) ;
    watchdog_update() ;  // Feed watchdog between page writes

    theSampleOffset += FLASH_PAGE_SIZE ;
    theSamplePtr += theBytesToWrite ;
    theBytesRemaining -= theBytesToWrite ;
  }

  restore_interrupts(theInterrupts) ;

  printf("FlightStorage: Flight written successfully\n") ;
  return true ;
}

//----------------------------------------------
// Function: FlightStorage_Init
//----------------------------------------------
bool FlightStorage_Init(void)
{
  printf("FlightStorage: Initializing...\n") ;
  printf("  Index at offset 0x%08X\n", kFlightIndexOffset) ;
  printf("  Slots at offset 0x%08X (%d slots x %dKB)\n",
    kFlightSlotsOffset, kMaxStoredFlights, kFlightSlotSize / 1024) ;
  printf("  Max samples per flight: %d\n", kMaxSamplesPerFlight) ;

  // Try to load existing index
  if (!LoadIndex())
  {
    // Initialize fresh index
    printf("FlightStorage: Creating new index\n") ;
    sNextFlightId = 1 ;
    memset(sSlotUsed, 0, sizeof(sSlotUsed)) ;
    SaveIndex() ;
  }

  // Count used slots
  uint8_t theUsedCount = 0 ;
  for (uint8_t i = 0 ; i < kMaxStoredFlights ; i++)
  {
    if (sSlotUsed[i])
    {
      theUsedCount++ ;
    }
  }

  printf("FlightStorage: %d flights stored, %d slots free, next ID=%lu\n",
    theUsedCount, kMaxStoredFlights - theUsedCount, (unsigned long)sNextFlightId) ;

  sInitialized = true ;
  return true ;
}

//----------------------------------------------
// Function: FlightStorage_GetFlightCount
//----------------------------------------------
uint8_t FlightStorage_GetFlightCount(void)
{
  uint8_t theCount = 0 ;
  for (uint8_t i = 0 ; i < kMaxStoredFlights ; i++)
  {
    if (sSlotUsed[i])
    {
      theCount++ ;
    }
  }
  return theCount ;
}

//----------------------------------------------
// Function: FlightStorage_GetFreeSlots
//----------------------------------------------
uint8_t FlightStorage_GetFreeSlots(void)
{
  return kMaxStoredFlights - FlightStorage_GetFlightCount() ;
}

//----------------------------------------------
// Function: FlightStorage_StartFlight
//----------------------------------------------
uint32_t FlightStorage_StartFlight(
  float inGroundPressurePa,
  int32_t inLaunchLat,
  int32_t inLaunchLon)
{
  if (!sInitialized)
  {
    printf("FlightStorage: Not initialized\n") ;
    return 0 ;
  }

  if (sRecording)
  {
    printf("FlightStorage: Already recording\n") ;
    return 0 ;
  }

  // Find free slot
  sCurrentSlot = FindFreeSlot() ;
  if (sCurrentSlot < 0)
  {
    printf("FlightStorage: No free slots\n") ;
    return 0 ;
  }

  // Initialize header
  memset(&sCurrentHeader, 0, sizeof(sCurrentHeader)) ;
  sCurrentHeader.pMagic = kFlightMagic ;
  sCurrentHeader.pVersion = kFlightVersion ;
  sCurrentHeader.pFlightId = sNextFlightId ;
  sCurrentHeader.pTimestamp = 0 ;  // Could be set via LoRa command
  sCurrentHeader.pGroundPressurePa = inGroundPressurePa ;
  sCurrentHeader.pLaunchLatitude = inLaunchLat ;
  sCurrentHeader.pLaunchLongitude = inLaunchLon ;

  // Reset sample buffer
  sSampleCount = 0 ;

  sRecording = true ;

  printf("FlightStorage: Started flight %lu in slot %d\n",
    (unsigned long)sNextFlightId, sCurrentSlot) ;

  return sNextFlightId ;
}

//----------------------------------------------
// Function: FlightStorage_LogSample
//----------------------------------------------
bool FlightStorage_LogSample(const FlightSample * inSample)
{
  if (!sRecording || inSample == NULL)
  {
    return false ;
  }

  if (sSampleCount >= kMaxSamplesPerFlight)
  {
    // Buffer full - could wrap or stop
    return false ;
  }

  // Copy sample to buffer
  memcpy(&sSampleBuffer[sSampleCount], inSample, sizeof(FlightSample)) ;
  sSampleCount++ ;

  return true ;
}

//----------------------------------------------
// Function: FlightStorage_EndFlight
//----------------------------------------------
bool FlightStorage_EndFlight(
  float inMaxAltitudeM,
  float inMaxVelocityMps,
  uint32_t inApogeeTimeMs,
  uint32_t inFlightTimeMs)
{
  if (!sRecording)
  {
    return false ;
  }

  // Update header with results
  sCurrentHeader.pSampleCount = sSampleCount ;
  sCurrentHeader.pMaxAltitudeM = inMaxAltitudeM ;
  sCurrentHeader.pMaxVelocityMps = inMaxVelocityMps ;
  sCurrentHeader.pApogeeTimeMs = inApogeeTimeMs ;
  sCurrentHeader.pFlightTimeMs = inFlightTimeMs ;

  // Calculate header checksum
  sCurrentHeader.pChecksum = CalculateChecksum(&sCurrentHeader,
    offsetof(FlightHeader, pChecksum)) ;

  printf("FlightStorage: Ending flight - %lu samples, max alt %.1f m\n",
    (unsigned long)sSampleCount, inMaxAltitudeM) ;

  // Write to flash
  bool theSuccess = WriteFlightToFlash((uint8_t)sCurrentSlot) ;

  if (theSuccess)
  {
    // Update index
    sSlotUsed[sCurrentSlot] = 1 ;
    sNextFlightId++ ;
    SaveIndex() ;
  }

  sRecording = false ;
  sCurrentSlot = -1 ;

  return theSuccess ;
}

//----------------------------------------------
// Function: FlightStorage_IsRecording
//----------------------------------------------
bool FlightStorage_IsRecording(void)
{
  return sRecording ;
}

//----------------------------------------------
// Function: FlightStorage_GetHeader
//----------------------------------------------
bool FlightStorage_GetHeader(
  uint8_t inSlotIndex,
  FlightHeader * outHeader)
{
  if (!sInitialized || inSlotIndex >= kMaxStoredFlights || outHeader == NULL)
  {
    return false ;
  }

  if (!sSlotUsed[inSlotIndex])
  {
    return false ;
  }

  // Read header from flash (memory-mapped)
  uint32_t theSlotOffset = kFlightSlotsOffset + (inSlotIndex * kFlightSlotSize) ;
  const FlightHeader * theHeaderPtr = (const FlightHeader *)(XIP_BASE + theSlotOffset) ;

  // Verify magic
  if (theHeaderPtr->pMagic != kFlightMagic)
  {
    printf("FlightStorage: Invalid magic in slot %d\n", inSlotIndex) ;
    return false ;
  }

  memcpy(outHeader, theHeaderPtr, sizeof(FlightHeader)) ;
  return true ;
}

//----------------------------------------------
// Function: FlightStorage_GetSample
//----------------------------------------------
bool FlightStorage_GetSample(
  uint8_t inSlotIndex,
  uint32_t inSampleIndex,
  FlightSample * outSample)
{
  if (!sInitialized || inSlotIndex >= kMaxStoredFlights || outSample == NULL)
  {
    return false ;
  }

  if (!sSlotUsed[inSlotIndex])
  {
    return false ;
  }

  // Get header to check sample count
  uint32_t theSlotOffset = kFlightSlotsOffset + (inSlotIndex * kFlightSlotSize) ;
  const FlightHeader * theHeaderPtr = (const FlightHeader *)(XIP_BASE + theSlotOffset) ;

  if (inSampleIndex >= theHeaderPtr->pSampleCount)
  {
    return false ;
  }

  // Calculate sample offset (header is in first page)
  uint32_t theSampleOffset = theSlotOffset + FLASH_PAGE_SIZE +
    (inSampleIndex * sizeof(FlightSample)) ;

  const FlightSample * theSamplePtr = (const FlightSample *)(XIP_BASE + theSampleOffset) ;
  memcpy(outSample, theSamplePtr, sizeof(FlightSample)) ;

  return true ;
}

//----------------------------------------------
// Function: FlightStorage_DeleteFlight
//----------------------------------------------
bool FlightStorage_DeleteFlight(uint8_t inSlotIndex)
{
  if (!sInitialized || inSlotIndex >= kMaxStoredFlights)
  {
    return false ;
  }

  if (!sSlotUsed[inSlotIndex])
  {
    return false ;
  }

  printf("FlightStorage: Deleting flight in slot %d\n", inSlotIndex) ;

  // Erase the slot
  uint32_t theSlotOffset = kFlightSlotsOffset + (inSlotIndex * kFlightSlotSize) ;

  uint32_t theInterrupts = save_and_disable_interrupts() ;
  flash_range_erase(theSlotOffset, kFlightSlotSize) ;
  restore_interrupts(theInterrupts) ;

  // Update index
  sSlotUsed[inSlotIndex] = 0 ;
  SaveIndex() ;

  return true ;
}

//----------------------------------------------
// Function: FlightStorage_DeleteAllFlights
//----------------------------------------------
uint8_t FlightStorage_DeleteAllFlights(void)
{
  if (!sInitialized)
  {
    return 0 ;
  }

  printf("FlightStorage: Deleting all flights\n") ;

  uint8_t theDeletedCount = 0 ;

  for (uint8_t i = 0 ; i < kMaxStoredFlights ; i++)
  {
    if (sSlotUsed[i])
    {
      uint32_t theSlotOffset = kFlightSlotsOffset + (i * kFlightSlotSize) ;

      uint32_t theInterrupts = save_and_disable_interrupts() ;
      flash_range_erase(theSlotOffset, kFlightSlotSize) ;
      restore_interrupts(theInterrupts) ;

      sSlotUsed[i] = 0 ;
      theDeletedCount++ ;
    }
  }

  SaveIndex() ;

  printf("FlightStorage: Deleted %d flights\n", theDeletedCount) ;
  return theDeletedCount ;
}

//----------------------------------------------
// Function: FlightStorage_FindSlotByFlightId
//----------------------------------------------
int8_t FlightStorage_FindSlotByFlightId(uint32_t inFlightId)
{
  if (!sInitialized)
  {
    return -1 ;
  }

  for (uint8_t i = 0 ; i < kMaxStoredFlights ; i++)
  {
    if (sSlotUsed[i])
    {
      FlightHeader theHeader ;
      if (FlightStorage_GetHeader(i, &theHeader))
      {
        if (theHeader.pFlightId == inFlightId)
        {
          return (int8_t)i ;
        }
      }
    }
  }

  return -1 ;
}

