//----------------------------------------------
// Module: rtc_pcf8523.c
// Description: PCF8523 Real Time Clock driver
// Author: Mark Gavin
// Created: 2025-12-19
// Copyright: (c) 2025 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "rtc_pcf8523.h"
#include "pins.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include <stdio.h>

#define printf(...) ((void)0)

//----------------------------------------------
// Module State
//----------------------------------------------
static bool sInitialized = false ;

//----------------------------------------------
// Internal: BCD to Binary conversion
//----------------------------------------------
static uint8_t BcdToBin(uint8_t inBcd)
{
  return (inBcd & 0x0F) + ((inBcd >> 4) * 10) ;
}

//----------------------------------------------
// Internal: Binary to BCD conversion
//----------------------------------------------
static uint8_t BinToBcd(uint8_t inBin)
{
  return ((inBin / 10) << 4) | (inBin % 10) ;
}

//----------------------------------------------
// Internal: Write register
//----------------------------------------------
static bool WriteRegister(uint8_t inReg, uint8_t inValue)
{
  uint8_t theData[2] = { inReg, inValue } ;
  int theResult = i2c_write_blocking(kI2cPort, PCF8523_I2C_ADDR, theData, 2, false) ;
  return (theResult == 2) ;
}

//----------------------------------------------
// Internal: Read register
//----------------------------------------------
static bool ReadRegister(uint8_t inReg, uint8_t * outValue)
{
  int theResult = i2c_write_blocking(kI2cPort, PCF8523_I2C_ADDR, &inReg, 1, true) ;
  if (theResult != 1) return false ;

  theResult = i2c_read_blocking(kI2cPort, PCF8523_I2C_ADDR, outValue, 1, false) ;
  return (theResult == 1) ;
}

//----------------------------------------------
// Internal: Read multiple registers
//----------------------------------------------
static bool ReadRegisters(uint8_t inStartReg, uint8_t * outData, size_t inLen)
{
  int theResult = i2c_write_blocking(kI2cPort, PCF8523_I2C_ADDR, &inStartReg, 1, true) ;
  if (theResult != 1) return false ;

  theResult = i2c_read_blocking(kI2cPort, PCF8523_I2C_ADDR, outData, inLen, false) ;
  return (theResult == (int)inLen) ;
}

//----------------------------------------------
// Function: RTC_Init
//----------------------------------------------
bool RTC_Init(void)
{
  // I2C should already be initialized by SSD1306
  // Just verify the RTC is present

  uint8_t theControl1 ;
  if (!ReadRegister(PCF8523_REG_CONTROL1, &theControl1))
  {
    printf("RTC: PCF8523 not found\n") ;
    return false ;
  }

  // Clear STOP bit to ensure oscillator is running
  if (theControl1 & 0x20) // STOP bit
  {
    theControl1 &= ~0x20 ;
    WriteRegister(PCF8523_REG_CONTROL1, theControl1) ;
    printf("RTC: Oscillator started\n") ;
  }

  // Set battery switchover mode in Control3
  // Enable battery switchover, disable low battery detection interrupt
  WriteRegister(PCF8523_REG_CONTROL3, 0x00) ;

  sInitialized = true ;
  printf("RTC: PCF8523 initialized\n") ;
  return true ;
}

//----------------------------------------------
// Function: RTC_IsRunning
//----------------------------------------------
bool RTC_IsRunning(void)
{
  uint8_t theSeconds ;
  if (!ReadRegister(PCF8523_REG_SECONDS, &theSeconds))
  {
    return false ;
  }

  // Check OS (oscillator stop) flag
  return !(theSeconds & 0x80) ;
}

//----------------------------------------------
// Function: RTC_GetTime
//----------------------------------------------
bool RTC_GetTime(RtcDateTime * outDateTime)
{
  if (!sInitialized || outDateTime == NULL)
  {
    return false ;
  }

  uint8_t theData[7] ;
  if (!ReadRegisters(PCF8523_REG_SECONDS, theData, 7))
  {
    return false ;
  }

  outDateTime->seconds = BcdToBin(theData[0] & 0x7F) ;
  outDateTime->minutes = BcdToBin(theData[1] & 0x7F) ;
  outDateTime->hours = BcdToBin(theData[2] & 0x3F) ;
  outDateTime->day = BcdToBin(theData[3] & 0x3F) ;
  outDateTime->weekday = theData[4] & 0x07 ;
  outDateTime->month = BcdToBin(theData[5] & 0x1F) ;
  outDateTime->year = 2000 + BcdToBin(theData[6]) ;

  return true ;
}

//----------------------------------------------
// Function: RTC_SetTime
//----------------------------------------------
bool RTC_SetTime(const RtcDateTime * inDateTime)
{
  if (!sInitialized || inDateTime == NULL)
  {
    return false ;
  }

  uint8_t theData[8] ;
  theData[0] = PCF8523_REG_SECONDS ;
  theData[1] = BinToBcd(inDateTime->seconds) ;
  theData[2] = BinToBcd(inDateTime->minutes) ;
  theData[3] = BinToBcd(inDateTime->hours) ;
  theData[4] = BinToBcd(inDateTime->day) ;
  theData[5] = inDateTime->weekday ;
  theData[6] = BinToBcd(inDateTime->month) ;
  theData[7] = BinToBcd(inDateTime->year - 2000) ;

  int theResult = i2c_write_blocking(kI2cPort, PCF8523_I2C_ADDR, theData, 8, false) ;
  return (theResult == 8) ;
}

//----------------------------------------------
// Function: RTC_GetUnixTimestamp
//----------------------------------------------
uint32_t RTC_GetUnixTimestamp(void)
{
  RtcDateTime theDateTime ;
  if (!RTC_GetTime(&theDateTime))
  {
    return 0 ;
  }

  struct tm theTime ;
  theTime.tm_sec = theDateTime.seconds ;
  theTime.tm_min = theDateTime.minutes ;
  theTime.tm_hour = theDateTime.hours ;
  theTime.tm_mday = theDateTime.day ;
  theTime.tm_mon = theDateTime.month - 1 ; // tm_mon is 0-11
  theTime.tm_year = theDateTime.year - 1900 ; // tm_year is years since 1900
  theTime.tm_isdst = 0 ;

  return (uint32_t)mktime(&theTime) ;
}

//----------------------------------------------
// Function: RTC_SetUnixTimestamp
//----------------------------------------------
bool RTC_SetUnixTimestamp(uint32_t inTimestamp)
{
  time_t theTime = (time_t)inTimestamp ;
  struct tm * theTm = gmtime(&theTime) ;

  if (theTm == NULL)
  {
    return false ;
  }

  RtcDateTime theDateTime ;
  theDateTime.seconds = theTm->tm_sec ;
  theDateTime.minutes = theTm->tm_min ;
  theDateTime.hours = theTm->tm_hour ;
  theDateTime.day = theTm->tm_mday ;
  theDateTime.month = theTm->tm_mon + 1 ;
  theDateTime.year = theTm->tm_year + 1900 ;
  theDateTime.weekday = theTm->tm_wday ;

  return RTC_SetTime(&theDateTime) ;
}

//----------------------------------------------
// Function: RTC_BatteryLow
//----------------------------------------------
bool RTC_BatteryLow(void)
{
  uint8_t theControl3 ;
  if (!ReadRegister(PCF8523_REG_CONTROL3, &theControl3))
  {
    return true ; // Assume low if can't read
  }

  // Check BLF (battery low flag) bit
  return (theControl3 & 0x04) != 0 ;
}
