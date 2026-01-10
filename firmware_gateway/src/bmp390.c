//----------------------------------------------
// Module: bmp390.c
// Description: BMP390 Barometric Pressure Sensor Driver
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "bmp390.h"
#include "pins.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

//----------------------------------------------
// Internal: Read Register
//----------------------------------------------
static bool ReadRegister(uint8_t inAddr, uint8_t inReg, uint8_t * outValue)
{
  absolute_time_t theTimeout = make_timeout_time_ms(100) ;

  int theResult = i2c_write_blocking_until(kI2cPort, inAddr, &inReg, 1, true, theTimeout) ;
  if (theResult != 1) return false ;

  theTimeout = make_timeout_time_ms(100) ;
  theResult = i2c_read_blocking_until(kI2cPort, inAddr, outValue, 1, false, theTimeout) ;
  return theResult == 1 ;
}

//----------------------------------------------
// Internal: Read Multiple Registers
//----------------------------------------------
static bool ReadRegisters(uint8_t inAddr, uint8_t inReg, uint8_t * outData, size_t inLen)
{
  absolute_time_t theTimeout = make_timeout_time_ms(100) ;

  int theResult = i2c_write_blocking_until(kI2cPort, inAddr, &inReg, 1, true, theTimeout) ;
  if (theResult != 1) return false ;

  theTimeout = make_timeout_time_ms(100) ;
  theResult = i2c_read_blocking_until(kI2cPort, inAddr, outData, inLen, false, theTimeout) ;
  return theResult == (int)inLen ;
}

//----------------------------------------------
// Internal: Write Register
//----------------------------------------------
static bool WriteRegister(uint8_t inAddr, uint8_t inReg, uint8_t inValue)
{
  uint8_t theData[2] = { inReg, inValue } ;
  absolute_time_t theTimeout = make_timeout_time_ms(100) ;

  int theResult = i2c_write_blocking_until(kI2cPort, inAddr, theData, 2, false, theTimeout) ;
  return theResult == 2 ;
}

//----------------------------------------------
// Internal: Read Calibration Data
//----------------------------------------------
static bool ReadCalibration(BMP390 * ioSensor)
{
  uint8_t theCalData[21] ;

  if (!ReadRegisters(ioSensor->pI2cAddr, BMP390_REG_CAL_DATA, theCalData, 21))
  {
    return false ;
  }

  // Parse temperature calibration coefficients
  uint16_t theNvmParT1 = (uint16_t)(theCalData[1] << 8) | theCalData[0] ;
  uint16_t theNvmParT2 = (uint16_t)(theCalData[3] << 8) | theCalData[2] ;
  int8_t theNvmParT3 = (int8_t)theCalData[4] ;

  ioSensor->pCalib.pParT1 = (float)theNvmParT1 / powf(2.0f, -8.0f) ;
  ioSensor->pCalib.pParT2 = (float)theNvmParT2 / powf(2.0f, 30.0f) ;
  ioSensor->pCalib.pParT3 = (float)theNvmParT3 / powf(2.0f, 48.0f) ;

  // Parse pressure calibration coefficients
  int16_t theNvmParP1 = (int16_t)((theCalData[6] << 8) | theCalData[5]) ;
  int16_t theNvmParP2 = (int16_t)((theCalData[8] << 8) | theCalData[7]) ;
  int8_t theNvmParP3 = (int8_t)theCalData[9] ;
  int8_t theNvmParP4 = (int8_t)theCalData[10] ;
  uint16_t theNvmParP5 = (uint16_t)(theCalData[12] << 8) | theCalData[11] ;
  uint16_t theNvmParP6 = (uint16_t)(theCalData[14] << 8) | theCalData[13] ;
  int8_t theNvmParP7 = (int8_t)theCalData[15] ;
  int8_t theNvmParP8 = (int8_t)theCalData[16] ;
  int16_t theNvmParP9 = (int16_t)((theCalData[18] << 8) | theCalData[17]) ;
  int8_t theNvmParP10 = (int8_t)theCalData[19] ;
  int8_t theNvmParP11 = (int8_t)theCalData[20] ;

  ioSensor->pCalib.pParP1 = ((float)theNvmParP1 - powf(2.0f, 14.0f)) / powf(2.0f, 20.0f) ;
  ioSensor->pCalib.pParP2 = ((float)theNvmParP2 - powf(2.0f, 14.0f)) / powf(2.0f, 29.0f) ;
  ioSensor->pCalib.pParP3 = (float)theNvmParP3 / powf(2.0f, 32.0f) ;
  ioSensor->pCalib.pParP4 = (float)theNvmParP4 / powf(2.0f, 37.0f) ;
  ioSensor->pCalib.pParP5 = (float)theNvmParP5 / powf(2.0f, -3.0f) ;
  ioSensor->pCalib.pParP6 = (float)theNvmParP6 / powf(2.0f, 6.0f) ;
  ioSensor->pCalib.pParP7 = (float)theNvmParP7 / powf(2.0f, 8.0f) ;
  ioSensor->pCalib.pParP8 = (float)theNvmParP8 / powf(2.0f, 15.0f) ;
  ioSensor->pCalib.pParP9 = (float)theNvmParP9 / powf(2.0f, 48.0f) ;
  ioSensor->pCalib.pParP10 = (float)theNvmParP10 / powf(2.0f, 48.0f) ;
  ioSensor->pCalib.pParP11 = (float)theNvmParP11 / powf(2.0f, 65.0f) ;

  return true ;
}

//----------------------------------------------
// Internal: Compensate Temperature
//----------------------------------------------
static float CompensateTemperature(BMP390 * ioSensor, uint32_t inRawTemp)
{
  float thePartialData1 = (float)inRawTemp - ioSensor->pCalib.pParT1 ;
  float thePartialData2 = thePartialData1 * ioSensor->pCalib.pParT2 ;

  float theTemperature = thePartialData2 +
                         (thePartialData1 * thePartialData1) * ioSensor->pCalib.pParT3 ;

  // Store for pressure compensation
  ioSensor->pLastTemperatureC = theTemperature ;

  return theTemperature ;
}

//----------------------------------------------
// Internal: Compensate Pressure
//----------------------------------------------
static float CompensatePressure(BMP390 * inSensor, uint32_t inRawPress)
{
  float theT = inSensor->pLastTemperatureC ;
  float theT2 = theT * theT ;
  float theT3 = theT2 * theT ;

  float thePartialData1 = inSensor->pCalib.pParP6 * theT ;
  float thePartialData2 = inSensor->pCalib.pParP7 * theT2 ;
  float thePartialData3 = inSensor->pCalib.pParP8 * theT3 ;
  float thePartialOut1 = inSensor->pCalib.pParP5 + thePartialData1 + thePartialData2 + thePartialData3 ;

  thePartialData1 = inSensor->pCalib.pParP2 * theT ;
  thePartialData2 = inSensor->pCalib.pParP3 * theT2 ;
  thePartialData3 = inSensor->pCalib.pParP4 * theT3 ;
  float thePartialOut2 = (float)inRawPress *
                         (inSensor->pCalib.pParP1 + thePartialData1 + thePartialData2 + thePartialData3) ;

  thePartialData1 = (float)inRawPress * (float)inRawPress ;
  thePartialData2 = inSensor->pCalib.pParP9 + inSensor->pCalib.pParP10 * theT ;
  thePartialData3 = thePartialData1 * thePartialData2 ;
  float thePartialData4 = thePartialData3 + ((float)inRawPress * (float)inRawPress * (float)inRawPress) *
                          inSensor->pCalib.pParP11 ;

  float thePressure = thePartialOut1 + thePartialOut2 + thePartialData4 ;

  return thePressure ;
}

//----------------------------------------------
// Function: BMP390_IsConnected
//----------------------------------------------
bool BMP390_IsConnected(uint8_t inI2cAddr)
{
  uint8_t theChipId = 0 ;
  if (!ReadRegister(inI2cAddr, BMP390_REG_CHIP_ID, &theChipId))
  {
    return false ;
  }
  return theChipId == BMP390_CHIP_ID ;
}

//----------------------------------------------
// Function: BMP390_Init
//----------------------------------------------
bool BMP390_Init(BMP390 * outSensor, uint8_t inI2cAddr)
{
  memset(outSensor, 0, sizeof(BMP390)) ;
  outSensor->pI2cAddr = inI2cAddr ;
  outSensor->pInitialized = false ;

  // Check chip ID
  if (!BMP390_IsConnected(inI2cAddr))
  {
    return false ;
  }

  // Soft reset
  if (!BMP390_SoftReset(outSensor))
  {
    return false ;
  }

  sleep_ms(10) ;

  // Read calibration data
  if (!ReadCalibration(outSensor))
  {
    return false ;
  }

  // Configure for high-resolution altitude measurement
  // Pressure: 8x oversampling
  // Temperature: 1x oversampling
  // ODR: 50 Hz
  // IIR: coefficient 3
  if (!BMP390_Configure(outSensor, BMP390_OSR_8X, BMP390_OSR_1X, BMP390_ODR_50_HZ, BMP390_IIR_COEF_3))
  {
    return false ;
  }

  // Enable normal mode (continuous measurement)
  if (!BMP390_SetMode(outSensor, true))
  {
    return false ;
  }

  outSensor->pInitialized = true ;
  return true ;
}

//----------------------------------------------
// Function: BMP390_Configure
//----------------------------------------------
bool BMP390_Configure(
  BMP390 * ioSensor,
  BMP390_Oversampling inPressOsr,
  BMP390_Oversampling inTempOsr,
  BMP390_OutputDataRate inOdr,
  BMP390_IIRFilter inFilter)
{
  // Set oversampling (pressure in bits 2:0, temp in bits 5:3)
  uint8_t theOsr = (inTempOsr << 3) | inPressOsr ;
  if (!WriteRegister(ioSensor->pI2cAddr, BMP390_REG_OSR, theOsr))
  {
    return false ;
  }

  // Set output data rate
  if (!WriteRegister(ioSensor->pI2cAddr, BMP390_REG_ODR, inOdr))
  {
    return false ;
  }

  // Set IIR filter (bits 3:1)
  uint8_t theConfig = (inFilter << 1) ;
  if (!WriteRegister(ioSensor->pI2cAddr, BMP390_REG_CONFIG, theConfig))
  {
    return false ;
  }

  return true ;
}

//----------------------------------------------
// Function: BMP390_SetMode
//----------------------------------------------
bool BMP390_SetMode(BMP390 * ioSensor, bool inNormalMode)
{
  uint8_t thePwrCtrl = BMP390_PWR_PRESS_EN | BMP390_PWR_TEMP_EN ;

  if (inNormalMode)
  {
    thePwrCtrl |= BMP390_PWR_MODE_NORMAL ;
  }
  else
  {
    thePwrCtrl |= BMP390_PWR_MODE_SLEEP ;
  }

  return WriteRegister(ioSensor->pI2cAddr, BMP390_REG_PWR_CTRL, thePwrCtrl) ;
}

//----------------------------------------------
// Function: BMP390_ReadPressureTemperature
//----------------------------------------------
bool BMP390_ReadPressureTemperature(
  BMP390 * inSensor,
  float * outPressurePa,
  float * outTemperatureC)
{
  if (!inSensor->pInitialized)
  {
    return false ;
  }

  // Trigger a forced measurement for reliable reads
  // This ensures we always get fresh data
  uint8_t thePwrCtrl = BMP390_PWR_PRESS_EN | BMP390_PWR_TEMP_EN | BMP390_PWR_MODE_FORCED ;
  if (!WriteRegister(inSensor->pI2cAddr, BMP390_REG_PWR_CTRL, thePwrCtrl))
  {
    return false ;
  }

  // Wait for measurement to complete (worst case ~40ms with 8x oversampling)
  // Check data ready status
  for (int i = 0 ; i < 50 ; i++)
  {
    sleep_ms(1) ;
    uint8_t theStatus = 0 ;
    if (ReadRegister(inSensor->pI2cAddr, BMP390_REG_STATUS, &theStatus))
    {
      // Bit 5 = pressure ready, bit 6 = temp ready
      if ((theStatus & 0x60) == 0x60)
      {
        break ;
      }
    }
  }

  // Read all 6 data bytes at once (pressure + temperature)
  uint8_t theData[6] ;
  if (!ReadRegisters(inSensor->pI2cAddr, BMP390_REG_DATA_0, theData, 6))
  {
    return false ;
  }

  // Parse raw pressure (24-bit, unsigned)
  uint32_t theRawPress = (uint32_t)theData[2] << 16 |
                         (uint32_t)theData[1] << 8 |
                         (uint32_t)theData[0] ;

  // Parse raw temperature (24-bit, unsigned)
  uint32_t theRawTemp = (uint32_t)theData[5] << 16 |
                        (uint32_t)theData[4] << 8 |
                        (uint32_t)theData[3] ;

  // Debug: print raw values occasionally
  static uint32_t sDebugCounter = 0 ;
  if ((++sDebugCounter % 100) == 1)
  {
    printf("GND BMP RAW: P=%lu T=%lu bytes=[%02X %02X %02X %02X %02X %02X]\n",
      (unsigned long)theRawPress, (unsigned long)theRawTemp,
      theData[0], theData[1], theData[2], theData[3], theData[4], theData[5]) ;
  }

  // Compensate temperature first (needed for pressure compensation)
  float theTemperature = CompensateTemperature((BMP390 *)inSensor, theRawTemp) ;

  // Compensate pressure
  float thePressure = CompensatePressure(inSensor, theRawPress) ;

  if (outTemperatureC != NULL)
  {
    *outTemperatureC = theTemperature ;
  }

  if (outPressurePa != NULL)
  {
    *outPressurePa = thePressure ;
  }

  return true ;
}

//----------------------------------------------
// Function: BMP390_ReadPressure
//----------------------------------------------
bool BMP390_ReadPressure(BMP390 * inSensor, float * outPressurePa)
{
  return BMP390_ReadPressureTemperature(inSensor, outPressurePa, NULL) ;
}

//----------------------------------------------
// Function: BMP390_ReadTemperature
//----------------------------------------------
bool BMP390_ReadTemperature(BMP390 * inSensor, float * outTemperatureC)
{
  return BMP390_ReadPressureTemperature(inSensor, NULL, outTemperatureC) ;
}

//----------------------------------------------
// Function: BMP390_SoftReset
//----------------------------------------------
bool BMP390_SoftReset(BMP390 * ioSensor)
{
  // Write soft reset command (0xB6)
  return WriteRegister(ioSensor->pI2cAddr, BMP390_REG_CMD, 0xB6) ;
}

//----------------------------------------------
// Function: BMP390_GetStatus
//----------------------------------------------
bool BMP390_GetStatus(BMP390 * inSensor, uint8_t * outStatus)
{
  return ReadRegister(inSensor->pI2cAddr, BMP390_REG_STATUS, outStatus) ;
}

//----------------------------------------------
// Function: BMP390_DataReady
//----------------------------------------------
bool BMP390_DataReady(BMP390 * inSensor)
{
  uint8_t theStatus = 0 ;
  if (!BMP390_GetStatus(inSensor, &theStatus))
  {
    return false ;
  }

  // Bit 5: pressure data ready, Bit 6: temperature data ready
  return (theStatus & 0x60) == 0x60 ;
}
