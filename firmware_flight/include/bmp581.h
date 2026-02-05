//----------------------------------------------
// Module: bmp581.h
// Description: BMP581 Barometric Pressure Sensor Driver
// Author: Mark Gavin
// Created: 2026-02-02
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit BMP581 Breakout (Product ID: 5857)
//   - I2C interface via STEMMA QT
//   - Address: 0x47 (default) or 0x46
//
// Notes:
//   BMP581 is a newer barometer with better specs than BMP390:
//   - Accuracy: ±0.4 Pa (±3.3cm) vs ±3 Pa (±25cm)
//   - Max ODR: 240 Hz vs 200 Hz
//   - Noise: 1 cm vs 10 cm
//   - Price: $9.95 vs $12.50
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// I2C Address
//----------------------------------------------
#define BMP581_I2C_ADDR_DEFAULT     0x47
#define BMP581_I2C_ADDR_ALT         0x46

//----------------------------------------------
// Chip ID
//----------------------------------------------
#define BMP581_CHIP_ID_580          0x50    // BMP580 (evaluation variant)
#define BMP581_CHIP_ID_581          0x51    // BMP581 / BMP585

//----------------------------------------------
// Register Addresses
//----------------------------------------------
#define BMP581_REG_CHIP_ID          0x01
#define BMP581_REG_REV_ID           0x02
#define BMP581_REG_CHIP_STATUS      0x11
#define BMP581_REG_DRIVE_CONFIG     0x13
#define BMP581_REG_INT_CONFIG       0x14
#define BMP581_REG_INT_SOURCE       0x15
#define BMP581_REG_FIFO_CONFIG      0x16
#define BMP581_REG_FIFO_COUNT       0x17
#define BMP581_REG_FIFO_SEL         0x18
#define BMP581_REG_TEMP_DATA_XLSB   0x1D
#define BMP581_REG_TEMP_DATA_LSB    0x1E
#define BMP581_REG_TEMP_DATA_MSB    0x1F
#define BMP581_REG_PRESS_DATA_XLSB  0x20
#define BMP581_REG_PRESS_DATA_LSB   0x21
#define BMP581_REG_PRESS_DATA_MSB   0x22
#define BMP581_REG_INT_STATUS       0x27
#define BMP581_REG_STATUS           0x28
#define BMP581_REG_FIFO_DATA        0x29
#define BMP581_REG_NVM_ADDR         0x2B
#define BMP581_REG_NVM_DATA_LSB     0x2C
#define BMP581_REG_NVM_DATA_MSB     0x2D
#define BMP581_REG_DSP_CONFIG       0x30
#define BMP581_REG_DSP_IIR          0x31
#define BMP581_REG_OOR_THR_P_LSB    0x32
#define BMP581_REG_OOR_THR_P_MSB    0x33
#define BMP581_REG_OOR_RANGE        0x34
#define BMP581_REG_OOR_CONFIG       0x35
#define BMP581_REG_OSR_CONFIG       0x36
#define BMP581_REG_ODR_CONFIG       0x37
#define BMP581_REG_OSR_EFF          0x38
#define BMP581_REG_CMD              0x7E

//----------------------------------------------
// Power Modes (ODR_CONFIG register bits 1:0)
//----------------------------------------------
#define BMP581_PWR_MODE_STANDBY     0x00
#define BMP581_PWR_MODE_NORMAL      0x01
#define BMP581_PWR_MODE_FORCED      0x02
#define BMP581_PWR_MODE_CONTINUOUS  0x03

//----------------------------------------------
// Commands
//----------------------------------------------
#define BMP581_CMD_SOFT_RESET       0xB6
#define BMP581_CMD_NVM_WRITE_EN     0x6A
#define BMP581_CMD_NVM_PROG_EN      0xA0

//----------------------------------------------
// Oversampling Settings
//----------------------------------------------
typedef enum
{
  BMP581_OSR_1X   = 0x00 ,
  BMP581_OSR_2X   = 0x01 ,
  BMP581_OSR_4X   = 0x02 ,
  BMP581_OSR_8X   = 0x03 ,
  BMP581_OSR_16X  = 0x04 ,
  BMP581_OSR_32X  = 0x05 ,
  BMP581_OSR_64X  = 0x06 ,
  BMP581_OSR_128X = 0x07
} BMP581_Oversampling ;

//----------------------------------------------
// Output Data Rate
//----------------------------------------------
typedef enum
{
  BMP581_ODR_240_HZ   = 0x00 ,
  BMP581_ODR_218_HZ   = 0x01 ,
  BMP581_ODR_199_HZ   = 0x02 ,
  BMP581_ODR_179_HZ   = 0x03 ,
  BMP581_ODR_160_HZ   = 0x04 ,
  BMP581_ODR_149_HZ   = 0x05 ,
  BMP581_ODR_140_HZ   = 0x06 ,
  BMP581_ODR_129_HZ   = 0x07 ,
  BMP581_ODR_120_HZ   = 0x08 ,
  BMP581_ODR_110_HZ   = 0x09 ,
  BMP581_ODR_100_HZ   = 0x0A ,
  BMP581_ODR_89_HZ    = 0x0B ,
  BMP581_ODR_80_HZ    = 0x0C ,
  BMP581_ODR_70_HZ    = 0x0D ,
  BMP581_ODR_60_HZ    = 0x0E ,
  BMP581_ODR_50_HZ    = 0x0F ,
  BMP581_ODR_45_HZ    = 0x10 ,
  BMP581_ODR_40_HZ    = 0x11 ,
  BMP581_ODR_35_HZ    = 0x12 ,
  BMP581_ODR_30_HZ    = 0x13 ,
  BMP581_ODR_25_HZ    = 0x14 ,
  BMP581_ODR_20_HZ    = 0x15 ,
  BMP581_ODR_15_HZ    = 0x16 ,
  BMP581_ODR_10_HZ    = 0x17 ,
  BMP581_ODR_5_HZ     = 0x18 ,
  BMP581_ODR_4_HZ     = 0x19 ,
  BMP581_ODR_3_HZ     = 0x1A ,
  BMP581_ODR_2_HZ     = 0x1B ,
  BMP581_ODR_1_HZ     = 0x1C
} BMP581_OutputDataRate ;

//----------------------------------------------
// IIR Filter Coefficients
//----------------------------------------------
typedef enum
{
  BMP581_IIR_COEF_0   = 0x00 ,   // Bypass
  BMP581_IIR_COEF_1   = 0x01 ,
  BMP581_IIR_COEF_3   = 0x02 ,
  BMP581_IIR_COEF_7   = 0x03 ,
  BMP581_IIR_COEF_15  = 0x04 ,
  BMP581_IIR_COEF_31  = 0x05 ,
  BMP581_IIR_COEF_63  = 0x06 ,
  BMP581_IIR_COEF_127 = 0x07
} BMP581_IIRFilter ;

//----------------------------------------------
// Sensor State Structure
// Note: BMP581 outputs pre-compensated data, no calibration needed
//----------------------------------------------
typedef struct
{
  uint8_t pI2cAddr ;
  bool pInitialized ;
} BMP581 ;

//----------------------------------------------
// Function: BMP581_Init
// Purpose: Initialize the BMP581 sensor
// Parameters:
//   outSensor - Sensor structure to initialize
//   inI2cAddr - I2C address (0x47 or 0x46)
// Returns: true if successful
//----------------------------------------------
bool BMP581_Init(BMP581 * outSensor, uint8_t inI2cAddr) ;

//----------------------------------------------
// Function: BMP581_IsConnected
// Purpose: Check if sensor is connected
// Returns: true if sensor responds with correct chip ID
//----------------------------------------------
bool BMP581_IsConnected(uint8_t inI2cAddr) ;

//----------------------------------------------
// Function: BMP581_Configure
// Purpose: Configure sensor settings
// Parameters:
//   ioSensor - Sensor to configure
//   inPressOsr - Pressure oversampling
//   inTempOsr - Temperature oversampling
//   inOdr - Output data rate
//   inFilter - IIR filter coefficient
// Returns: true if successful
//----------------------------------------------
bool BMP581_Configure(
  BMP581 * ioSensor,
  BMP581_Oversampling inPressOsr,
  BMP581_Oversampling inTempOsr,
  BMP581_OutputDataRate inOdr,
  BMP581_IIRFilter inFilter) ;

//----------------------------------------------
// Function: BMP581_SetMode
// Purpose: Set power mode
// Parameters:
//   ioSensor - Sensor
//   inContinuousMode - true for continuous, false for standby
// Returns: true if successful
//----------------------------------------------
bool BMP581_SetMode(BMP581 * ioSensor, bool inContinuousMode) ;

//----------------------------------------------
// Function: BMP581_ReadPressureTemperature
// Purpose: Read both pressure and temperature
// Parameters:
//   inSensor - Sensor to read
//   outPressurePa - Pressure in Pascals
//   outTemperatureC - Temperature in Celsius
// Returns: true if successful
//----------------------------------------------
bool BMP581_ReadPressureTemperature(
  BMP581 * inSensor,
  float * outPressurePa,
  float * outTemperatureC) ;

//----------------------------------------------
// Function: BMP581_SoftReset
// Purpose: Perform software reset
// Parameters:
//   ioSensor - Sensor to reset
// Returns: true if successful
//----------------------------------------------
bool BMP581_SoftReset(BMP581 * ioSensor) ;

//----------------------------------------------
// Function: BMP581_DataReady
// Purpose: Check if new data is available
// Parameters:
//   inSensor - Sensor to check
// Returns: true if new data ready
//----------------------------------------------
bool BMP581_DataReady(BMP581 * inSensor) ;
