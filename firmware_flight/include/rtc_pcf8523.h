//----------------------------------------------
// Module: rtc_pcf8523.h
// Description: PCF8523 Real Time Clock driver
// Author: Mark Gavin
// Created: 2025-12-19
// Copyright: (c) 2025 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit Adalogger FeatherWing
//   - I2C interface at 0x68
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

//----------------------------------------------
// I2C Address
//----------------------------------------------
#define PCF8523_I2C_ADDR    0x68

//----------------------------------------------
// PCF8523 Registers
//----------------------------------------------
#define PCF8523_REG_CONTROL1    0x00
#define PCF8523_REG_CONTROL2    0x01
#define PCF8523_REG_CONTROL3    0x02
#define PCF8523_REG_SECONDS     0x03
#define PCF8523_REG_MINUTES     0x04
#define PCF8523_REG_HOURS       0x05
#define PCF8523_REG_DAYS        0x06
#define PCF8523_REG_WEEKDAYS    0x07
#define PCF8523_REG_MONTHS      0x08
#define PCF8523_REG_YEARS       0x09

//----------------------------------------------
// Date/Time Structure
//----------------------------------------------
typedef struct
{
  uint8_t seconds ;   // 0-59
  uint8_t minutes ;   // 0-59
  uint8_t hours ;     // 0-23
  uint8_t day ;       // 1-31
  uint8_t month ;     // 1-12
  uint16_t year ;     // Full year (e.g., 2025)
  uint8_t weekday ;   // 0-6 (Sunday = 0)
} RtcDateTime ;

//----------------------------------------------
// Function: RTC_Init
// Purpose: Initialize the RTC
// Returns: true if successful
//----------------------------------------------
bool RTC_Init(void) ;

//----------------------------------------------
// Function: RTC_IsRunning
// Purpose: Check if RTC oscillator is running
// Returns: true if running
//----------------------------------------------
bool RTC_IsRunning(void) ;

//----------------------------------------------
// Function: RTC_GetTime
// Purpose: Read current date/time from RTC
// Parameters:
//   outDateTime - Pointer to store result
// Returns: true if successful
//----------------------------------------------
bool RTC_GetTime(RtcDateTime * outDateTime) ;

//----------------------------------------------
// Function: RTC_SetTime
// Purpose: Set the RTC date/time
// Parameters:
//   inDateTime - Pointer to date/time to set
// Returns: true if successful
//----------------------------------------------
bool RTC_SetTime(const RtcDateTime * inDateTime) ;

//----------------------------------------------
// Function: RTC_GetUnixTimestamp
// Purpose: Get current time as Unix timestamp
// Returns: Unix timestamp (seconds since 1970)
//----------------------------------------------
uint32_t RTC_GetUnixTimestamp(void) ;

//----------------------------------------------
// Function: RTC_SetUnixTimestamp
// Purpose: Set RTC from Unix timestamp
// Parameters:
//   inTimestamp - Unix timestamp
// Returns: true if successful
//----------------------------------------------
bool RTC_SetUnixTimestamp(uint32_t inTimestamp) ;

//----------------------------------------------
// Function: RTC_BatteryLow
// Purpose: Check if RTC battery is low
// Returns: true if battery is low or missing
//----------------------------------------------
bool RTC_BatteryLow(void) ;
