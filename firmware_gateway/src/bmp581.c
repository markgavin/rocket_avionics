//----------------------------------------------
// Module: bmp581.c
// Description: BMP581 Barometric Pressure Sensor Driver
// Author: Mark Gavin
// Created: 2026-02-02
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Notes:
//   BMP581 outputs pre-compensated data - no calibration needed.
//   This makes the driver much simpler than BMP390.
//----------------------------------------------

#include "bmp581.h"
#include "pins.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include <stdio.h>
#include <string.h>

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
// Function: BMP581_IsConnected
//----------------------------------------------
bool BMP581_IsConnected(uint8_t inI2cAddr)
{
  uint8_t theChipId = 0 ;
  if (!ReadRegister(inI2cAddr, BMP581_REG_CHIP_ID, &theChipId))
  {
    return false ;
  }
  return theChipId == BMP581_CHIP_ID ;
}

//----------------------------------------------
// Function: BMP581_Init
//----------------------------------------------
bool BMP581_Init(BMP581 * outSensor, uint8_t inI2cAddr)
{
  memset(outSensor, 0, sizeof(BMP581)) ;
  outSensor->pI2cAddr = inI2cAddr ;
  outSensor->pInitialized = false ;

  // Check chip ID
  if (!BMP581_IsConnected(inI2cAddr))
  {
    return false ;
  }

  // Soft reset
  if (!BMP581_SoftReset(outSensor))
  {
    return false ;
  }

  sleep_ms(10) ;

  // Wait for device ready
  for (int i = 0 ; i < 50 ; i++)
  {
    uint8_t theStatus = 0 ;
    if (ReadRegister(inI2cAddr, BMP581_REG_STATUS, &theStatus))
    {
      // Bit 0 = status_nvm_rdy, bit 1 = status_nvm_err
      if ((theStatus & 0x01) && !(theStatus & 0x02))
      {
        break ;
      }
    }
    sleep_ms(1) ;
  }

  // Configure for high-resolution altitude measurement
  // Pressure: 16x oversampling
  // Temperature: 1x oversampling
  // ODR: 50 Hz
  // IIR: coefficient 3
  if (!BMP581_Configure(outSensor, BMP581_OSR_16X, BMP581_OSR_1X, BMP581_ODR_50_HZ, BMP581_IIR_COEF_3))
  {
    return false ;
  }

  // Enable continuous mode
  if (!BMP581_SetMode(outSensor, true))
  {
    return false ;
  }

  outSensor->pInitialized = true ;
  return true ;
}

//----------------------------------------------
// Function: BMP581_Configure
//----------------------------------------------
bool BMP581_Configure(
  BMP581 * ioSensor,
  BMP581_Oversampling inPressOsr,
  BMP581_Oversampling inTempOsr,
  BMP581_OutputDataRate inOdr,
  BMP581_IIRFilter inFilter)
{
  // Set oversampling (OSR_CONFIG register)
  // Bits 5:3 = osr_t (temperature), Bits 2:0 = osr_p (pressure)
  uint8_t theOsr = (inTempOsr << 3) | inPressOsr ;
  if (!WriteRegister(ioSensor->pI2cAddr, BMP581_REG_OSR_CONFIG, theOsr))
  {
    return false ;
  }

  // Set ODR (ODR_CONFIG register)
  // Bits 6:5 = pwr_mode (set later), Bits 4:0 = odr_sel
  // For now, set ODR in standby mode (pwr_mode = 0)
  uint8_t theOdrConfig = inOdr & 0x1F ;
  if (!WriteRegister(ioSensor->pI2cAddr, BMP581_REG_ODR_CONFIG, theOdrConfig))
  {
    return false ;
  }

  // Set IIR filter (DSP_IIR register)
  // Bits 5:3 = set_iir_t, Bits 2:0 = set_iir_p
  uint8_t theIir = (inFilter << 3) | inFilter ;
  if (!WriteRegister(ioSensor->pI2cAddr, BMP581_REG_DSP_IIR, theIir))
  {
    return false ;
  }

  // Enable IIR filter in DSP_CONFIG register
  // Bit 0 = comp_pt_en (enable pressure temp compensation)
  // Bit 1 = iir_flush_forced_en
  // Bits 5:3 = shdw_sel_iir_t, Bits 6 = shdw_sel_iir_p
  // Enable pressure and temperature IIR filtering to shadow registers
  uint8_t theDspConfig = 0x01 | (1 << 3) | (1 << 6) ;
  if (!WriteRegister(ioSensor->pI2cAddr, BMP581_REG_DSP_CONFIG, theDspConfig))
  {
    return false ;
  }

  return true ;
}

//----------------------------------------------
// Function: BMP581_SetMode
//----------------------------------------------
bool BMP581_SetMode(BMP581 * ioSensor, bool inContinuousMode)
{
  // Read current ODR config
  uint8_t theOdrConfig = 0 ;
  if (!ReadRegister(ioSensor->pI2cAddr, BMP581_REG_ODR_CONFIG, &theOdrConfig))
  {
    return false ;
  }

  // Clear power mode bits (6:5) and set new mode
  theOdrConfig &= 0x1F ;  // Keep ODR selection

  if (inContinuousMode)
  {
    theOdrConfig |= (BMP581_PWR_MODE_CONTINUOUS << 5) ;
  }
  else
  {
    theOdrConfig |= (BMP581_PWR_MODE_STANDBY << 5) ;
  }

  return WriteRegister(ioSensor->pI2cAddr, BMP581_REG_ODR_CONFIG, theOdrConfig) ;
}

//----------------------------------------------
// Function: BMP581_ReadPressureTemperature
//----------------------------------------------
bool BMP581_ReadPressureTemperature(
  BMP581 * inSensor,
  float * outPressurePa,
  float * outTemperatureC)
{
  if (!inSensor->pInitialized)
  {
    return false ;
  }

  // Read temperature data (3 bytes at 0x1D)
  uint8_t theTempData[3] ;
  if (!ReadRegisters(inSensor->pI2cAddr, BMP581_REG_TEMP_DATA_XLSB, theTempData, 3))
  {
    return false ;
  }

  // Read pressure data (3 bytes at 0x20)
  uint8_t thePressData[3] ;
  if (!ReadRegisters(inSensor->pI2cAddr, BMP581_REG_PRESS_DATA_XLSB, thePressData, 3))
  {
    return false ;
  }

  // Parse raw temperature (24-bit signed)
  int32_t theRawTemp = (int32_t)theTempData[0] |
                       ((int32_t)theTempData[1] << 8) |
                       ((int32_t)theTempData[2] << 16) ;
  // Sign extend 24-bit to 32-bit
  if (theRawTemp & 0x800000)
  {
    theRawTemp |= 0xFF000000 ;
  }

  // Parse raw pressure (24-bit unsigned)
  uint32_t theRawPress = (uint32_t)thePressData[0] |
                         ((uint32_t)thePressData[1] << 8) |
                         ((uint32_t)thePressData[2] << 16) ;

  // BMP581 outputs pre-compensated data
  // Temperature: 1/65536 °C per LSB
  // Pressure: 1/64 Pa per LSB
  float theTemperature = (float)theRawTemp / 65536.0f ;
  float thePressure = (float)theRawPress / 64.0f ;

  // Debug: print raw values occasionally
  static uint32_t sDebugCounter = 0 ;
  if ((++sDebugCounter % 100) == 1)
  {
    printf("BMP581 RAW: P=%lu (%.1f Pa) T=%ld (%.2f C)\n",
      (unsigned long)theRawPress, thePressure,
      (long)theRawTemp, theTemperature) ;
  }

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
// Function: BMP581_SoftReset
//----------------------------------------------
bool BMP581_SoftReset(BMP581 * ioSensor)
{
  return WriteRegister(ioSensor->pI2cAddr, BMP581_REG_CMD, BMP581_CMD_SOFT_RESET) ;
}

//----------------------------------------------
// Function: BMP581_DataReady
//----------------------------------------------
bool BMP581_DataReady(BMP581 * inSensor)
{
  uint8_t theIntStatus = 0 ;
  if (!ReadRegister(inSensor->pI2cAddr, BMP581_REG_INT_STATUS, &theIntStatus))
  {
    return false ;
  }

  // Bit 0 = drdy_data_reg
  return (theIntStatus & 0x01) != 0 ;
}
