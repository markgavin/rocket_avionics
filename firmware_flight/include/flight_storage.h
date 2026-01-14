//----------------------------------------------
// Module: flight_storage.h
// Description: Flash storage for flight data
//   Stores flight samples to onboard flash memory
//   for later download via LoRa
// Author: Mark Gavin
// Created: 2026-01-13
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Feather RP2040 with 8MB flash
//   - Uses last portion of flash for data storage
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// Flash Layout Constants
//----------------------------------------------
// Feather RP2040 has 8MB (0x800000) flash
// Reserve last 512KB for flight storage
#define kFlashTotalSize         0x800000    // 8MB total flash
#define kFlightStorageSize      0x80000     // 512KB for flight data
#define kFlightStorageOffset    (kFlashTotalSize - kFlightStorageSize)  // 0x780000

// Calibration sector (last 4KB of storage area)
#define kCalibrationOffset      (kFlashTotalSize - 0x1000)  // 0x7FF000

// Flight index sector (before calibration)
#define kFlightIndexOffset      (kCalibrationOffset - 0x1000)  // 0x7FE000

// Flight slots start after firmware, before index
// Each slot is 64KB (16 sectors) to hold ~60 seconds at 100Hz
#define kFlightSlotSize         0x10000     // 64KB per flight
#define kMaxStoredFlights       7           // 7 flights x 64KB = 448KB

// Flight slots start at storage base
#define kFlightSlotsOffset      kFlightStorageOffset  // 0x780000

//----------------------------------------------
// Flash Constants
//----------------------------------------------
#define FLASH_SECTOR_SIZE       4096        // 4KB erase sector
#define FLASH_PAGE_SIZE         256         // 256-byte program page

//----------------------------------------------
// Magic Numbers and Version
//----------------------------------------------
#define kFlightMagic            0x54484746  // "FGHT" (Flight)
#define kFlightVersion          1
#define kFlightIndexMagic       0x58444E49  // "INDX"

//----------------------------------------------
// Flight Sample Structure (52 bytes)
// Logged at 100 Hz during flight
//----------------------------------------------
typedef struct __attribute__((packed))
{
  // Time (4 bytes)
  uint32_t pTimeMs ;              // Time since launch (ms)

  // Barometric data (14 bytes)
  int32_t pAltitudeCm ;           // Altitude in centimeters
  int16_t pVelocityCmps ;         // Velocity in cm/s
  uint32_t pPressurePa ;          // Pressure in Pascals
  int16_t pTemperatureC10 ;       // Temperature * 10

  // GPS data (15 bytes)
  int32_t pGpsLatitude ;          // Latitude in microdegrees
  int32_t pGpsLongitude ;         // Longitude in microdegrees
  int16_t pGpsSpeedCmps ;         // Ground speed in cm/s
  uint16_t pGpsHeadingDeg10 ;     // Heading * 10
  uint8_t pGpsSatellites ;        // Satellite count

  // Accelerometer (6 bytes) - milli-g
  int16_t pAccelX ;
  int16_t pAccelY ;
  int16_t pAccelZ ;

  // Gyroscope (6 bytes) - 0.1 degrees/second
  int16_t pGyroX ;
  int16_t pGyroY ;
  int16_t pGyroZ ;

  // Magnetometer (6 bytes) - milligauss
  int16_t pMagX ;
  int16_t pMagY ;
  int16_t pMagZ ;

  // Status (1 byte)
  uint8_t pState ;                // Flight state + flags
} FlightSample ;                  // Total: 52 bytes

// At 10 Hz (matching telemetry rate):
// 64KB slot holds: 64000 / 52 = ~1230 samples = 123 seconds
// Typical 60-second flight = 600 samples = 31KB

//----------------------------------------------
// Flight Header Structure (stored at slot start)
//----------------------------------------------
typedef struct __attribute__((packed))
{
  uint32_t pMagic ;               // kFlightMagic
  uint32_t pVersion ;             // kFlightVersion
  uint32_t pFlightId ;            // Sequential flight ID
  uint32_t pTimestamp ;           // Unix timestamp (if available)
  uint32_t pSampleCount ;         // Number of samples recorded

  // Flight results
  float pMaxAltitudeM ;           // Peak altitude (meters)
  float pMaxVelocityMps ;         // Peak velocity (m/s)
  uint32_t pApogeeTimeMs ;        // Time to apogee
  uint32_t pFlightTimeMs ;        // Total flight duration
  float pGroundPressurePa ;       // Ground reference pressure

  // GPS launch location
  int32_t pLaunchLatitude ;       // Launch latitude (microdegrees)
  int32_t pLaunchLongitude ;      // Launch longitude (microdegrees)

  // Padding and checksum
  uint8_t pReserved[20] ;         // Reserved for future use
  uint32_t pChecksum ;            // Header checksum
} FlightHeader ;                  // 80 bytes

//----------------------------------------------
// Function: FlightStorage_Init
// Purpose: Initialize flight storage system
// Returns: true if successful
//----------------------------------------------
bool FlightStorage_Init(void) ;

//----------------------------------------------
// Function: FlightStorage_GetFlightCount
// Purpose: Get number of stored flights
// Returns: Number of flights (0 to kMaxStoredFlights)
//----------------------------------------------
uint8_t FlightStorage_GetFlightCount(void) ;

//----------------------------------------------
// Function: FlightStorage_GetFreeSlots
// Purpose: Get number of free slots
// Returns: Number of available slots
//----------------------------------------------
uint8_t FlightStorage_GetFreeSlots(void) ;

//----------------------------------------------
// Function: FlightStorage_StartFlight
// Purpose: Begin recording a new flight
// Parameters:
//   inGroundPressurePa - Ground reference pressure
//   inLaunchLat - Launch latitude (microdegrees)
//   inLaunchLon - Launch longitude (microdegrees)
// Returns: Flight ID (0 if failed/full)
//----------------------------------------------
uint32_t FlightStorage_StartFlight(
  float inGroundPressurePa,
  int32_t inLaunchLat,
  int32_t inLaunchLon) ;

//----------------------------------------------
// Function: FlightStorage_LogSample
// Purpose: Log a flight sample to flash buffer
// Parameters:
//   inSample - Sample data to log
// Returns: true if logged successfully
//----------------------------------------------
bool FlightStorage_LogSample(const FlightSample * inSample) ;

//----------------------------------------------
// Function: FlightStorage_EndFlight
// Purpose: Finalize flight recording
// Parameters:
//   inMaxAltitudeM - Peak altitude
//   inMaxVelocityMps - Peak velocity
//   inApogeeTimeMs - Time to apogee
//   inFlightTimeMs - Total flight time
// Returns: true if saved successfully
//----------------------------------------------
bool FlightStorage_EndFlight(
  float inMaxAltitudeM,
  float inMaxVelocityMps,
  uint32_t inApogeeTimeMs,
  uint32_t inFlightTimeMs) ;

//----------------------------------------------
// Function: FlightStorage_IsRecording
// Purpose: Check if currently recording
// Returns: true if recording active
//----------------------------------------------
bool FlightStorage_IsRecording(void) ;

//----------------------------------------------
// Function: FlightStorage_GetHeader
// Purpose: Get flight header by slot index
// Parameters:
//   inSlotIndex - Slot index (0 to kMaxStoredFlights-1)
//   outHeader - Receives header data
// Returns: true if valid flight in slot
//----------------------------------------------
bool FlightStorage_GetHeader(
  uint8_t inSlotIndex,
  FlightHeader * outHeader) ;

//----------------------------------------------
// Function: FlightStorage_GetSample
// Purpose: Get a sample from stored flight
// Parameters:
//   inSlotIndex - Slot index
//   inSampleIndex - Sample index within flight
//   outSample - Receives sample data
// Returns: true if sample read successfully
//----------------------------------------------
bool FlightStorage_GetSample(
  uint8_t inSlotIndex,
  uint32_t inSampleIndex,
  FlightSample * outSample) ;

//----------------------------------------------
// Function: FlightStorage_DeleteFlight
// Purpose: Delete a stored flight
// Parameters:
//   inSlotIndex - Slot to delete
// Returns: true if deleted successfully
//----------------------------------------------
bool FlightStorage_DeleteFlight(uint8_t inSlotIndex) ;

//----------------------------------------------
// Function: FlightStorage_DeleteAllFlights
// Purpose: Delete all stored flights
// Returns: Number of flights deleted
//----------------------------------------------
uint8_t FlightStorage_DeleteAllFlights(void) ;

//----------------------------------------------
// Function: FlightStorage_FindSlotByFlightId
// Purpose: Find slot containing a flight ID
// Parameters:
//   inFlightId - Flight ID to find
// Returns: Slot index, or -1 if not found
//----------------------------------------------
int8_t FlightStorage_FindSlotByFlightId(uint32_t inFlightId) ;

