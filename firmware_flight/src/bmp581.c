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

#define printf(...) ((void)0)

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
// Internal: Write Multiple Registers (atomic)
//----------------------------------------------
static bool WriteRegisters(uint8_t inAddr, uint8_t inReg, const uint8_t * inData, size_t inLen)
{
  uint8_t theBuffer[8] ;
  if (inLen > 7) return false ;

  theBuffer[0] = inReg ;
  memcpy(&theBuffer[1], inData, inLen) ;

  absolute_time_t theTimeout = make_timeout_time_ms(100) ;
  int theResult = i2c_write_blocking_until(kI2cPort, inAddr, theBuffer, inLen + 1, false, theTimeout) ;
  return theResult == (int)(inLen + 1) ;
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
  outSensor->pLastError = 0 ;

  // Check chip ID and print for diagnostics
  uint8_t theChipId = 0 ;
  if (!ReadRegister(inI2cAddr, BMP581_REG_CHIP_ID, &theChipId))
  {
    outSensor->pLastError = 1 ;  // I2C read failed
    return false ;
  }
  if (theChipId != BMP581_CHIP_ID_580 && theChipId != BMP581_CHIP_ID_581)
  {
    outSensor->pLastError = 2 ;  // Wrong chip ID
    printf("BMP581: Bad chip ID 0x%02X\n", theChipId) ;
    return false ;
  }
  printf("BMP581: Chip ID = 0x%02X (%s)\n", theChipId,
    theChipId == BMP581_CHIP_ID_581 ? "BMP581" : "BMP580") ;

  // Soft reset (includes POR completion check and INT_STATUS clear)
  if (!BMP581_SoftReset(outSensor))
  {
    outSensor->pLastError = 3 ;  // Reset failed
    printf("BMP581: Soft reset failed\n") ;
    return false ;
  }

  // Power-up check: verify NVM status (non-fatal, for diagnostics)
  bool theNvmOk = false ;
  for (int i = 0 ; i < 50 ; i++)
  {
    uint8_t theStatus = 0 ;
    if (ReadRegister(inI2cAddr, BMP581_REG_STATUS, &theStatus))
    {
      // Bit 0 = status_nvm_rdy, bit 1 = status_nvm_err
      if ((theStatus & 0x01) && !(theStatus & 0x02))
      {
        theNvmOk = true ;
        break ;
      }
    }
    sleep_ms(1) ;
  }
  printf("BMP581: NVM check %s\n", theNvmOk ? "OK" : "FAILED (continuing anyway)") ;

  // Read INT_STATUS for diagnostics (clears latched flags)
  {
    uint8_t theIntStatus = 0 ;
    if (ReadRegister(inI2cAddr, BMP581_REG_INT_STATUS, &theIntStatus))
    {
      printf("BMP581: INT_STATUS after powerup = 0x%02X\n", theIntStatus) ;
    }
  }

  // Configure sensor (OSR, ODR, IIR)
  if (!BMP581_Configure(outSensor, BMP581_OSR_16X, BMP581_OSR_2X, BMP581_ODR_50_HZ, BMP581_IIR_COEF_1))
  {
    outSensor->pLastError = 4 ;  // Configure failed
    printf("BMP581: Configuration failed\n") ;
    return false ;
  }

  // Enable continuous mode (Normal power mode)
  if (!BMP581_SetMode(outSensor, true))
  {
    outSensor->pLastError = 5 ;  // SetMode failed
    printf("BMP581: Failed to set normal mode\n") ;
    return false ;
  }

  // Enable data-ready interrupt
  if (!WriteRegister(inI2cAddr, BMP581_REG_INT_SOURCE, 0x01))
  {
    outSensor->pLastError = 6 ;  // INT_SOURCE write failed
    return false ;
  }

  // INT_CONFIG: latched, active-high, push-pull, enabled
  if (!WriteRegister(inI2cAddr, BMP581_REG_INT_CONFIG, 0x0B))
  {
    outSensor->pLastError = 7 ;  // INT_CONFIG write failed
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

  // Read first data sample for diagnostic comparison
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
  // --- OSR_CONFIG + ODR_CONFIG (atomic 2-byte write at 0x36-0x37) ---
  // Matches Bosch bmp5_set_osr_odr_press_config() read-modify-write pattern
  uint8_t theOsrOdr[2] = { 0, 0 } ;
  if (!ReadRegisters(ioSensor->pI2cAddr, BMP581_REG_OSR_CONFIG, theOsrOdr, 2))
  {
    return false ;
  }

  // OSR_CONFIG (0x36):
  //   Bits 2:0 = osr_t (temperature oversampling)
  //   Bits 5:3 = osr_p (pressure oversampling)
  //   Bit 6    = press_en (MUST be 1 to enable pressure measurement)
  theOsrOdr[0] = (theOsrOdr[0] & 0x80) ;  // Preserve bit 7 (reserved)
  theOsrOdr[0] |= 0x40 | (inPressOsr << 3) | inTempOsr ;  // PRESS_EN + OSR

  // ODR_CONFIG (0x37):
  //   Bits 1:0 = pwr_mode (set to standby during config)
  //   Bits 6:2 = odr_sel
  //   Bit 7    = deep_disable
  theOsrOdr[1] = 0x80 | ((inOdr & 0x1F) << 2) ;  // DEEP_DISABLE + ODR, standby mode

  if (!WriteRegisters(ioSensor->pI2cAddr, BMP581_REG_OSR_CONFIG, theOsrOdr, 2))
  {
    return false ;
  }

  // --- DSP_CONFIG + DSP_IIR (atomic 2-byte write at 0x30-0x31) ---
  // Matches Bosch set_iir_config() read-modify-write pattern
  uint8_t theDsp[2] = { 0, 0 } ;
  if (!ReadRegisters(ioSensor->pI2cAddr, BMP581_REG_DSP_CONFIG, theDsp, 2))
  {
    return false ;
  }

  // DSP_CONFIG (0x30):
  //   Bit 2 = IIR_FLUSH_FORCED_EN
  //   Bit 3 = SHDW_SET_IIR_TEMP (select IIR filtered temp output)
  //   Bit 4 = SET_FIFO_IIR_TEMP
  //   Bit 5 = SHDW_SET_IIR_PRESS (select IIR filtered press output)
  //   Bit 6 = SET_FIFO_IIR_PRESS
  theDsp[0] = (theDsp[0] & 0x03) ;  // Preserve bits 0-1 (reserved/unknown)
  theDsp[0] |= (1 << 5) | (1 << 3) | (1 << 2) ;  // SHDW_IIR_P + SHDW_IIR_T + IIR_FLUSH

  // DSP_IIR (0x31):
  //   Bits 2:0 = set_iir_t (temp IIR coefficient)
  //   Bits 5:3 = set_iir_p (press IIR coefficient)
  theDsp[1] = (inFilter & 0x07) | ((inFilter & 0x07) << 3) ;  // Same for both

  if (!WriteRegisters(ioSensor->pI2cAddr, BMP581_REG_DSP_CONFIG, theDsp, 2))
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

  // BMP581 outputs pre-compensated data (per Bosch BMP5 API)
  // Temperature in degrees C = raw / 65536
  // Pressure in Pa = raw / 64
  float theTemperature = (float)theRawTemp / 65536.0f ;
  float thePressure = (float)theRawPress / 64.0f ;

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
  // Write soft reset command
  if (!WriteRegister(ioSensor->pI2cAddr, BMP581_REG_CMD, BMP581_CMD_SOFT_RESET))
  {
    return false ;
  }

  // Wait for reset, then poll INT_STATUS for POR complete
  // Bit 4 (0x10) = POR/soft-reset complete (per Bosch BMP5 API)
  // Retry with increasing delays — some chips are slower
  bool thePorOk = false ;
  uint8_t theIntStatus = 0 ;
  for (int i = 0 ; i < 10 ; i++)
  {
    sleep_ms(2 + i * 2) ;  // 2, 4, 6, 8, ... 20ms
    if (ReadRegister(ioSensor->pI2cAddr, BMP581_REG_INT_STATUS, &theIntStatus))
    {
      if (theIntStatus & 0x10)
      {
        thePorOk = true ;
        break ;
      }
    }
  }

  printf("BMP581: POR check INT_STATUS=0x%02X %s\n",
    theIntStatus, thePorOk ? "OK" : "(not set, continuing anyway)") ;

  // Continue even if POR bit not set — the sensor may still work
  // Reading INT_STATUS clears latched flags regardless
  return true ;
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
