//----------------------------------------------
// Module: gps.h
// Description: GPS Driver for PA1616D (Ultimate GPS)
//   NMEA sentence parsing over UART
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit Ultimate GPS FeatherWing (PA1616D) - Product ID 3133
//   - 9600 baud UART, NMEA 0183 output
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// GPS Fix Quality
//----------------------------------------------
typedef enum
{
  kGpsFixNone = 0,        // No fix
  kGpsFixGps = 1,         // GPS fix (SPS)
  kGpsFixDgps = 2,        // DGPS fix
  kGpsFixPps = 3,         // PPS fix
  kGpsFixRtk = 4,         // Real Time Kinematic
  kGpsFixFloatRtk = 5,    // Float RTK
  kGpsFixEstimated = 6,   // Estimated (dead reckoning)
  kGpsFixManual = 7,      // Manual input mode
  kGpsFixSimulation = 8   // Simulation mode
} GpsFixQuality ;

//----------------------------------------------
// GPS Data Structure
//----------------------------------------------
typedef struct
{
  // Position
  float pLatitude ;         // Degrees (positive = North)
  float pLongitude ;        // Degrees (positive = East)
  float pAltitudeMsl ;      // Altitude above mean sea level (meters)

  // Velocity
  float pSpeedKnots ;       // Ground speed in knots
  float pSpeedMps ;         // Ground speed in m/s
  float pHeadingDeg ;       // Track/heading in degrees (0-360)

  // Fix information
  GpsFixQuality pFixQuality ;
  uint8_t pSatellites ;     // Number of satellites in use
  float pHdop ;             // Horizontal dilution of precision

  // Time (UTC)
  uint8_t pHour ;
  uint8_t pMinute ;
  uint8_t pSecond ;
  uint16_t pMilliseconds ;

  // Date
  uint8_t pDay ;
  uint8_t pMonth ;
  uint16_t pYear ;

  // Status
  bool pValid ;             // Data is valid (has fix)
  uint32_t pLastFixTimeMs ; // Time of last valid fix
  uint32_t pAge ;           // Age of data in ms
} GpsData ;

//----------------------------------------------
// Function: GPS_Init
// Purpose: Initialize GPS UART and module
// Returns: true if successful
//----------------------------------------------
bool GPS_Init(void) ;

//----------------------------------------------
// Function: GPS_Enable
// Purpose: Enable or disable GPS module
// Parameters:
//   inEnable - true to enable, false to disable
//----------------------------------------------
void GPS_Enable(bool inEnable) ;

//----------------------------------------------
// Function: GPS_Update
// Purpose: Process incoming GPS data (call frequently)
//   Reads UART and parses NMEA sentences
// Parameters:
//   inCurrentTimeMs - Current system time in ms
//----------------------------------------------
void GPS_Update(uint32_t inCurrentTimeMs) ;

//----------------------------------------------
// Function: GPS_GetData
// Purpose: Get current GPS data
// Returns: Pointer to GPS data structure
//----------------------------------------------
const GpsData * GPS_GetData(void) ;

//----------------------------------------------
// Function: GPS_HasFix
// Purpose: Check if GPS has a valid fix
// Returns: true if valid fix available
//----------------------------------------------
bool GPS_HasFix(void) ;

//----------------------------------------------
// Function: GPS_GetSatellites
// Purpose: Get number of satellites in use
// Returns: Satellite count
//----------------------------------------------
uint8_t GPS_GetSatellites(void) ;

//----------------------------------------------
// Function: GPS_SendCommand
// Purpose: Send PMTK command to GPS module
// Parameters:
//   inCommand - PMTK command string (without $ or checksum)
//----------------------------------------------
void GPS_SendCommand(const char * inCommand) ;

