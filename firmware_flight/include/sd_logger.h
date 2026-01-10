//----------------------------------------------
// Module: sd_logger.h
// Description: SD card data logging
// Author: Mark Gavin
// Created: 2025-12-19
// Copyright: (c) 2025 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit Adalogger FeatherWing
//   - SPI interface with CS on GPIO10
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// Flight Data Sample Structure
//----------------------------------------------
typedef struct
{
  uint32_t timeMs ;        // Time since launch (ms)
  float altitudeM ;        // Altitude AGL (meters)
  float velocityMps ;      // Vertical velocity (m/s)
  float pressurePa ;       // Raw pressure (Pascals)
  float temperatureC ;     // Temperature (Celsius)
} SdFlightSample ;

//----------------------------------------------
// Function: SdLogger_Init
// Purpose: Initialize SD card and file system
// Returns: true if SD card is present and mounted
//----------------------------------------------
bool SdLogger_Init(void) ;

//----------------------------------------------
// Function: SdLogger_IsAvailable
// Purpose: Check if SD card is available
// Returns: true if SD card is mounted
//----------------------------------------------
bool SdLogger_IsAvailable(void) ;

//----------------------------------------------
// Function: SdLogger_GetFreeSpace
// Purpose: Get free space on SD card
// Returns: Free space in bytes (0 if unavailable)
//----------------------------------------------
uint32_t SdLogger_GetFreeSpace(void) ;

//----------------------------------------------
// Function: SdLogger_StartFlight
// Purpose: Start a new flight log file
// Parameters:
//   inTimestamp - Unix timestamp for filename
// Returns: true if file opened successfully
//----------------------------------------------
bool SdLogger_StartFlight(uint32_t inTimestamp) ;

//----------------------------------------------
// Function: SdLogger_LogSample
// Purpose: Log a single flight sample
// Parameters:
//   inSample - Sample data to log
// Returns: true if written successfully
//----------------------------------------------
bool SdLogger_LogSample(const SdFlightSample * inSample) ;

//----------------------------------------------
// Function: SdLogger_EndFlight
// Purpose: Finalize and close flight log file
// Parameters:
//   inMaxAltitudeM - Maximum altitude in meters
//   inFlightTimeMs - Total flight time in ms
//   inMaxVelocityMps - Maximum velocity in m/s
// Returns: true if closed successfully
//----------------------------------------------
bool SdLogger_EndFlight(float inMaxAltitudeM, uint32_t inFlightTimeMs, float inMaxVelocityMps) ;

//----------------------------------------------
// Function: SdLogger_ListFlights
// Purpose: List all flight files on SD card
// Parameters:
//   outFilenames - Array to store filenames
//   inMaxCount - Maximum number of filenames
// Returns: Number of flight files found
//----------------------------------------------
int SdLogger_ListFlights(char ** outFilenames, int inMaxCount) ;

//----------------------------------------------
// Function: SdLogger_DeleteFlight
// Purpose: Delete a flight log file
// Parameters:
//   inFilename - Name of file to delete
// Returns: true if deleted successfully
//----------------------------------------------
bool SdLogger_DeleteFlight(const char * inFilename) ;

//----------------------------------------------
// Function: SdLogger_GetLastFilename
// Purpose: Get filename of last logged flight
// Returns: Pointer to filename string (or NULL)
//----------------------------------------------
const char * SdLogger_GetLastFilename(void) ;

//----------------------------------------------
// Function: SdLogger_GetFlightCount
// Purpose: Get number of flight files on SD card
// Returns: Number of flight files
//----------------------------------------------
int SdLogger_GetFlightCount(void) ;
