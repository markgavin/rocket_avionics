//----------------------------------------------
// Module: gps.c
// Description: GPS Driver for PA1616D (Ultimate GPS)
//   NMEA sentence parsing over UART
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "gps.h"
#include "pins.h"

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//----------------------------------------------
// Constants
//----------------------------------------------
#define GPS_BUFFER_SIZE     128
#define KNOTS_TO_MPS        0.514444f
#define GPS_FIX_TIMEOUT_MS  3000

//----------------------------------------------
// Module State
//----------------------------------------------
static GpsData sGpsData ;
static char sNmeaBuffer[GPS_BUFFER_SIZE] ;
static int sNmeaBufferPos = 0 ;
static bool sInitialized = false ;

//----------------------------------------------
// Internal: Calculate NMEA checksum
//----------------------------------------------
static uint8_t CalculateChecksum(const char * inSentence)
{
  uint8_t theChecksum = 0 ;
  // Skip leading $ if present
  if (*inSentence == '$') inSentence++ ;

  while (*inSentence && *inSentence != '*')
  {
    theChecksum ^= *inSentence++ ;
  }
  return theChecksum ;
}

//----------------------------------------------
// Internal: Parse NMEA coordinate (DDDMM.MMMM)
//----------------------------------------------
static float ParseCoordinate(const char * inStr, char inDirection)
{
  if (inStr == NULL || strlen(inStr) < 4) return 0.0f ;

  // Find decimal point
  const char * theDot = strchr(inStr, '.') ;
  if (theDot == NULL) return 0.0f ;

  // Degrees are the digits before the last 2 integer digits
  int theDegDigits = (theDot - inStr) - 2 ;
  if (theDegDigits < 1 || theDegDigits > 3) return 0.0f ;

  // Parse degrees
  char theDegStr[4] = {0} ;
  strncpy(theDegStr, inStr, theDegDigits) ;
  float theDegrees = (float)atof(theDegStr) ;

  // Parse minutes (rest of string)
  float theMinutes = (float)atof(inStr + theDegDigits) ;

  // Convert to decimal degrees
  float theResult = theDegrees + (theMinutes / 60.0f) ;

  // Apply direction
  if (inDirection == 'S' || inDirection == 'W')
  {
    theResult = -theResult ;
  }

  return theResult ;
}

//----------------------------------------------
// Internal: Get next field from NMEA sentence
//----------------------------------------------
static const char * GetNextField(const char * inStr, char * outField, int inMaxLen)
{
  if (inStr == NULL || outField == NULL) return NULL ;

  int i = 0 ;
  while (*inStr && *inStr != ',' && *inStr != '*' && i < inMaxLen - 1)
  {
    outField[i++] = *inStr++ ;
  }
  outField[i] = '\0' ;

  // Skip delimiter
  if (*inStr == ',' || *inStr == '*') inStr++ ;

  return (*inStr || i > 0) ? inStr : NULL ;
}

//----------------------------------------------
// Internal: Parse GGA sentence (position + fix)
// $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,47.0,M,,*47
//----------------------------------------------
static void ParseGGA(const char * inSentence)
{
  char theField[20] ;
  const char * thePtr = inSentence ;

  // Skip sentence type
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;

  // Time (HHMMSS.sss)
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;
  if (strlen(theField) >= 6)
  {
    sGpsData.pHour = (theField[0] - '0') * 10 + (theField[1] - '0') ;
    sGpsData.pMinute = (theField[2] - '0') * 10 + (theField[3] - '0') ;
    sGpsData.pSecond = (theField[4] - '0') * 10 + (theField[5] - '0') ;
    if (strlen(theField) > 7)
    {
      sGpsData.pMilliseconds = (uint16_t)(atof(theField + 6) * 1000) ;
    }
  }

  // Latitude
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;
  char theLat[20] ;
  strncpy(theLat, theField, sizeof(theLat) - 1) ;

  // Latitude direction
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;
  char theLatDir = theField[0] ;

  // Longitude
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;
  char theLon[20] ;
  strncpy(theLon, theField, sizeof(theLon) - 1) ;

  // Longitude direction
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;
  char theLonDir = theField[0] ;

  // Parse coordinates
  sGpsData.pLatitude = ParseCoordinate(theLat, theLatDir) ;
  sGpsData.pLongitude = ParseCoordinate(theLon, theLonDir) ;

  // Fix quality
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;
  sGpsData.pFixQuality = (GpsFixQuality)atoi(theField) ;
  sGpsData.pValid = (sGpsData.pFixQuality >= kGpsFixGps) ;

  // Number of satellites
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;
  sGpsData.pSatellites = (uint8_t)atoi(theField) ;

  // HDOP
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;
  sGpsData.pHdop = (float)atof(theField) ;

  // Altitude (MSL)
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;
  sGpsData.pAltitudeMsl = (float)atof(theField) ;
}

//----------------------------------------------
// Internal: Parse RMC sentence (speed + heading)
// $GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
//----------------------------------------------
static void ParseRMC(const char * inSentence)
{
  char theField[20] ;
  const char * thePtr = inSentence ;

  // Skip sentence type
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;

  // Time (already parsed from GGA)
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;

  // Status (A=active, V=void)
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;
  bool theActive = (theField[0] == 'A') ;

  // Skip lat/lon (already parsed from GGA)
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ; // Lat
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ; // N/S
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ; // Lon
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ; // E/W

  // Speed (knots)
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;
  if (theActive && strlen(theField) > 0)
  {
    sGpsData.pSpeedKnots = (float)atof(theField) ;
    sGpsData.pSpeedMps = sGpsData.pSpeedKnots * KNOTS_TO_MPS ;
  }

  // Heading (track made good)
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;
  if (theActive && strlen(theField) > 0)
  {
    sGpsData.pHeadingDeg = (float)atof(theField) ;
  }

  // Date (DDMMYY)
  thePtr = GetNextField(thePtr, theField, sizeof(theField)) ;
  if (strlen(theField) >= 6)
  {
    sGpsData.pDay = (theField[0] - '0') * 10 + (theField[1] - '0') ;
    sGpsData.pMonth = (theField[2] - '0') * 10 + (theField[3] - '0') ;
    sGpsData.pYear = 2000 + (theField[4] - '0') * 10 + (theField[5] - '0') ;
  }
}

//----------------------------------------------
// Internal: Process complete NMEA sentence
//----------------------------------------------
static void ProcessNmeaSentence(const char * inSentence, uint32_t inCurrentTimeMs)
{
  // Verify checksum
  const char * theAsterisk = strchr(inSentence, '*') ;
  if (theAsterisk != NULL)
  {
    uint8_t theExpected = (uint8_t)strtol(theAsterisk + 1, NULL, 16) ;
    uint8_t theCalculated = CalculateChecksum(inSentence) ;
    if (theExpected != theCalculated)
    {
      return ; // Checksum mismatch
    }
  }

  // Parse based on sentence type
  if (strncmp(inSentence + 3, "GGA", 3) == 0)
  {
    ParseGGA(inSentence) ;
    if (sGpsData.pValid)
    {
      sGpsData.pLastFixTimeMs = inCurrentTimeMs ;
    }
  }
  else if (strncmp(inSentence + 3, "RMC", 3) == 0)
  {
    ParseRMC(inSentence) ;
  }
  // GSA and GSV could be added for more satellite info
}

//----------------------------------------------
// Function: GPS_Init
//----------------------------------------------
bool GPS_Init(void)
{
  // Initialize GPS data structure
  memset(&sGpsData, 0, sizeof(sGpsData)) ;
  sGpsData.pFixQuality = kGpsFixNone ;

  // Initialize UART for GPS (9600 baud default)
  uart_init(kGpsUartPort, kGpsUartBaudrate) ;
  gpio_set_function(kPinGpsTx, GPIO_FUNC_UART) ;
  gpio_set_function(kPinGpsRx, GPIO_FUNC_UART) ;

  // Set UART format: 8N1
  uart_set_format(kGpsUartPort, 8, 1, UART_PARITY_NONE) ;

  // Enable UART FIFO
  uart_set_fifo_enabled(kGpsUartPort, true) ;

  // Initialize GPS enable pin (if used)
  #ifdef kPinGpsEnable
  gpio_init(kPinGpsEnable) ;
  gpio_set_dir(kPinGpsEnable, GPIO_OUT) ;
  gpio_put(kPinGpsEnable, 0) ;  // Low = enabled
  #endif

  sInitialized = true ;
  sNmeaBufferPos = 0 ;

  // Give GPS time to start
  sleep_ms(100) ;

  // Send initialization commands
  // Update rate to 1Hz (default, can increase to 5Hz or 10Hz)
  GPS_SendCommand("PMTK220,1000") ;

  // Request only GGA and RMC sentences
  GPS_SendCommand("PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0") ;

  return true ;
}

//----------------------------------------------
// Function: GPS_Enable
//----------------------------------------------
void GPS_Enable(bool inEnable)
{
  #ifdef kPinGpsEnable
  gpio_put(kPinGpsEnable, inEnable ? 0 : 1) ;  // Active low
  #else
  (void)inEnable ;
  #endif
}

//----------------------------------------------
// Function: GPS_Update
//----------------------------------------------
void GPS_Update(uint32_t inCurrentTimeMs)
{
  if (!sInitialized) return ;

  // Update age of data
  if (sGpsData.pLastFixTimeMs > 0)
  {
    sGpsData.pAge = inCurrentTimeMs - sGpsData.pLastFixTimeMs ;

    // Invalidate if too old
    if (sGpsData.pAge > GPS_FIX_TIMEOUT_MS)
    {
      sGpsData.pValid = false ;
    }
  }

  // Read available UART data
  while (uart_is_readable(kGpsUartPort))
  {
    char theChar = uart_getc(kGpsUartPort) ;

    if (theChar == '$')
    {
      // Start of new sentence
      sNmeaBufferPos = 0 ;
      sNmeaBuffer[sNmeaBufferPos++] = theChar ;
    }
    else if (theChar == '\r' || theChar == '\n')
    {
      // End of sentence
      if (sNmeaBufferPos > 0)
      {
        sNmeaBuffer[sNmeaBufferPos] = '\0' ;
        ProcessNmeaSentence(sNmeaBuffer, inCurrentTimeMs) ;
        sNmeaBufferPos = 0 ;
      }
    }
    else if (sNmeaBufferPos > 0 && sNmeaBufferPos < GPS_BUFFER_SIZE - 1)
    {
      // Add to buffer
      sNmeaBuffer[sNmeaBufferPos++] = theChar ;
    }
  }
}

//----------------------------------------------
// Function: GPS_GetData
//----------------------------------------------
const GpsData * GPS_GetData(void)
{
  return &sGpsData ;
}

//----------------------------------------------
// Function: GPS_HasFix
//----------------------------------------------
bool GPS_HasFix(void)
{
  return sGpsData.pValid ;
}

//----------------------------------------------
// Function: GPS_GetSatellites
//----------------------------------------------
uint8_t GPS_GetSatellites(void)
{
  return sGpsData.pSatellites ;
}

//----------------------------------------------
// Function: GPS_SendCommand
//----------------------------------------------
void GPS_SendCommand(const char * inCommand)
{
  if (!sInitialized || inCommand == NULL) return ;

  // Calculate checksum
  uint8_t theChecksum = 0 ;
  const char * thePtr = inCommand ;
  while (*thePtr)
  {
    theChecksum ^= *thePtr++ ;
  }

  // Send with $ prefix and checksum
  char theBuffer[GPS_BUFFER_SIZE] ;
  snprintf(theBuffer, sizeof(theBuffer), "$%s*%02X\r\n", inCommand, theChecksum) ;

  uart_puts(kGpsUartPort, theBuffer) ;
}

