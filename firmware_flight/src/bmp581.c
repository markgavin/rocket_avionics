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
  return (theChipId == BMP581_CHIP_ID_580 || theChipId == BMP581_CHIP_ID_581) ;
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

  // Enable data-ready interrupt (matches Adafruit initialization)
  // INT_SOURCE: bit 0 = drdy_data_reg_en
  if (!WriteRegister(inI2cAddr, BMP581_REG_INT_SOURCE, 0x01))
  {
    return false ;
  }

  // INT_CONFIG: latched, active-high, push-pull, enabled
  if (!WriteRegister(inI2cAddr, BMP581_REG_INT_CONFIG, 0x0B))
  {
    return false ;
  }

  // Configure to match Adafruit defaults:
  // Pressure: 16x oversampling
  // Temperature: 2x oversampling (Adafruit default)
  // ODR: 50 Hz
  // IIR: coefficient 1 (Adafruit default)
  if (!BMP581_Configure(outSensor, BMP581_OSR_16X, BMP581_OSR_2X, BMP581_ODR_50_HZ, BMP581_IIR_COEF_1))
  {
    return false ;
  }

  // Enable continuous mode
  if (!BMP581_SetMode(outSensor, true))
  {
    return false ;
  }

  // Diagnostic: verify configuration registers
  {
    uint8_t theRegVal = 0 ;
    ReadRegister(inI2cAddr, BMP581_REG_OSR_CONFIG, &theRegVal) ;
    printf("BMP581 cfg: OSR=0x%02X", theRegVal) ;
    ReadRegister(inI2cAddr, BMP581_REG_ODR_CONFIG, &theRegVal) ;
    printf(" ODR=0x%02X", theRegVal) ;
    ReadRegister(inI2cAddr, BMP581_REG_DSP_CONFIG, &theRegVal) ;
    printf(" DSP=0x%02X", theRegVal) ;
    ReadRegister(inI2cAddr, BMP581_REG_DSP_IIR, &theRegVal) ;
    printf(" IIR=0x%02X", theRegVal) ;
    ReadRegister(inI2cAddr, BMP581_REG_OSR_EFF, &theRegVal) ;
    printf(" EFF=0x%02X\n", theRegVal) ;
  }

  // Read first data sample for diagnostic
  sleep_ms(100) ;
  {
    uint8_t theDiagData[6] ;
    if (ReadRegisters(inI2cAddr, BMP581_REG_TEMP_DATA_XLSB, theDiagData, 6))
    {
      int32_t theRawT = (int32_t)theDiagData[0] |
                        ((int32_t)theDiagData[1] << 8) |
                        ((int32_t)theDiagData[2] << 16) ;
      if (theRawT & 0x800000) theRawT |= 0xFF000000 ;
      uint32_t theRawP = (uint32_t)theDiagData[3] |
                         ((uint32_t)theDiagData[4] << 8) |
                         ((uint32_t)theDiagData[5] << 16) ;
      printf("BMP581 raw: [%02X %02X %02X | %02X %02X %02X]\n",
        theDiagData[0], theDiagData[1], theDiagData[2],
        theDiagData[3], theDiagData[4], theDiagData[5]) ;
      printf("BMP581 data: T=%ld  P=%lu\n", (long)theRawT, (unsigned long)theRawP) ;
      printf("BMP581 Bosch:     T=%.2fC  P=%.1fPa\n",
        (float)theRawT / 65536.0f, (float)theRawP / 64.0f) ;
      printf("BMP581 Empirical: T=%.2fC  P=%.1fPa\n",
        (float)theRawT / 4096.0f, (float)theRawP / 5.0f) ;
    }
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
  // Bit 6 = press_en (MUST be 1 to enable pressure measurement!)
  // Bits 5:3 = osr_p (pressure oversampling)
  // Bits 2:0 = osr_t (temperature oversampling)
  uint8_t theOsr = 0x40 | (inPressOsr << 3) | inTempOsr ;  // 0x40 = PRESS_EN
  if (!WriteRegister(ioSensor->pI2cAddr, BMP581_REG_OSR_CONFIG, theOsr))
  {
    return false ;
  }

  // Set ODR (ODR_CONFIG register)
  // Bit 7: DEEP_DISABLE, Bits 6:2 = odr_sel, Bits 1:0 = pwr_mode
  // Set ODR in standby mode (pwr_mode = 0), enable deep disable
  uint8_t theOdrConfig = 0x80 | ((inOdr & 0x1F) << 2) ;  // DEEP_DISABLE + ODR shifted to bits 6:2
  if (!WriteRegister(ioSensor->pI2cAddr, BMP581_REG_ODR_CONFIG, theOdrConfig))
  {
    return false ;
  }

  // Enable IIR filter (matching Adafruit config)
  // DSP_IIR register: Bits 5:3 = set_iir_t, Bits 2:0 = set_iir_p
  // Values: 0=bypass, 1=coef1, 2=coef3, 3=coef7, 4=coef15, 5=coef31, 6=coef63, 7=coef127
  // Adafruit uses coef_1 (value 1)
  uint8_t theIirConfig = (inFilter << 3) | inFilter ;  // Same coefficient for both temp and pressure
  if (!WriteRegister(ioSensor->pI2cAddr, BMP581_REG_DSP_IIR, theIirConfig))
  {
    return false ;
  }

  // DSP_CONFIG register - enable IIR shadow registers (like Adafruit)
  // Bit 5: SHDW_SET_IIR_PRESS (1 = select IIR pressure output)
  // Bit 3: SHDW_SET_IIR_TEMP (1 = select IIR temperature output)
  uint8_t theDspConfig = (1 << 5) | (1 << 3) | (1 << 2) ;  // shdw_iir_p + shdw_iir_t + IIR_FLUSH
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

  // ODR_CONFIG register layout:
  // Bit 7: DEEP_DISABLE (set to 1 to disable deep standby)
  // Bits 6:2: ODR selection
  // Bits 1:0: POWERMODE (0=standby, 1=normal, 2=forced, 3=continuous)

  // Clear power mode bits (1:0) and set new mode
  theOdrConfig &= 0xFC ;  // Keep bits 7:2 (deep disable and ODR)
  theOdrConfig |= 0x80 ;  // Set DEEP_DISABLE bit to prevent deep standby

  if (inContinuousMode)
  {
    theOdrConfig |= BMP581_PWR_MODE_NORMAL ;  // Normal mode for continuous sampling
  }
  else
  {
    theOdrConfig |= BMP581_PWR_MODE_STANDBY ;
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

  // Read all 6 bytes (temp + pressure) in one I2C transaction for atomic data
  // Registers 0x1D-0x22: TEMP_XLSB, TEMP_LSB, TEMP_MSB, PRESS_XLSB, PRESS_LSB, PRESS_MSB
  uint8_t theData[6] ;
  if (!ReadRegisters(inSensor->pI2cAddr, BMP581_REG_TEMP_DATA_XLSB, theData, 6))
  {
    return false ;
  }

  // Parse raw temperature (24-bit signed) - LSB first per Bosch datasheet
  int32_t theRawTemp = (int32_t)theData[0] |
                       ((int32_t)theData[1] << 8) |
                       ((int32_t)theData[2] << 16) ;
  // Sign extend 24-bit to 32-bit
  if (theRawTemp & 0x800000)
  {
    theRawTemp |= 0xFF000000 ;
  }

  // Parse raw pressure (24-bit unsigned) - LSB first per Bosch datasheet
  uint32_t theRawPress = (uint32_t)theData[3] |
                         ((uint32_t)theData[4] << 8) |
                         ((uint32_t)theData[5] << 16) ;

  // BMP581 outputs pre-compensated data
  // Bosch docs say raw/65536 (temp) and raw/64 (pressure), but our BMP581
  // consistently gives values ~16x and ~12.8x too low with those divisors.
  // Empirical testing shows raw/4096 and raw/5.0 give correct readings.
  // This may be a BMP580 vs BMP581 data format difference.
  float theTemperature = (float)theRawTemp / 4096.0f ;
  float thePressure = (float)theRawPress / 5.0f ;

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
