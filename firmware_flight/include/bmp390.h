//----------------------------------------------
// Module: bmp390.h
// Description: BMP390 Barometric Pressure Sensor Driver
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit BMP390 Breakout (Product ID: 4816)
//   - I2C interface via STEMMA QT
//   - Address: 0x77 (default) or 0x76
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// I2C Address
//----------------------------------------------
#define BMP390_I2C_ADDR_DEFAULT     0x77
#define BMP390_I2C_ADDR_ALT         0x76

//----------------------------------------------
// Chip ID
//----------------------------------------------
#define BMP390_CHIP_ID              0x60

//----------------------------------------------
// Register Addresses
//----------------------------------------------
#define BMP390_REG_CHIP_ID          0x00
#define BMP390_REG_ERR_REG          0x02
#define BMP390_REG_STATUS           0x03
#define BMP390_REG_DATA_0           0x04  // Pressure XLSB
#define BMP390_REG_DATA_1           0x05  // Pressure LSB
#define BMP390_REG_DATA_2           0x06  // Pressure MSB
#define BMP390_REG_DATA_3           0x07  // Temperature XLSB
#define BMP390_REG_DATA_4           0x08  // Temperature LSB
#define BMP390_REG_DATA_5           0x09  // Temperature MSB
#define BMP390_REG_SENSORTIME_0     0x0C
#define BMP390_REG_SENSORTIME_1     0x0D
#define BMP390_REG_SENSORTIME_2     0x0E
#define BMP390_REG_EVENT            0x10
#define BMP390_REG_INT_STATUS       0x11
#define BMP390_REG_FIFO_LENGTH_0    0x12
#define BMP390_REG_FIFO_LENGTH_1    0x13
#define BMP390_REG_FIFO_DATA        0x14
#define BMP390_REG_FIFO_WTM_0       0x15
#define BMP390_REG_FIFO_WTM_1       0x16
#define BMP390_REG_FIFO_CONFIG_1    0x17
#define BMP390_REG_FIFO_CONFIG_2    0x18
#define BMP390_REG_INT_CTRL         0x19
#define BMP390_REG_IF_CONF          0x1A
#define BMP390_REG_PWR_CTRL         0x1B
#define BMP390_REG_OSR              0x1C  // Oversampling
#define BMP390_REG_ODR              0x1D  // Output data rate
#define BMP390_REG_CONFIG           0x1F
#define BMP390_REG_CAL_DATA         0x31  // Calibration data start
#define BMP390_REG_CMD              0x7E

//----------------------------------------------
// Power Control Register Bits
//----------------------------------------------
#define BMP390_PWR_PRESS_EN         0x01
#define BMP390_PWR_TEMP_EN          0x02
#define BMP390_PWR_MODE_SLEEP       0x00
#define BMP390_PWR_MODE_FORCED      0x10
#define BMP390_PWR_MODE_NORMAL      0x30

//----------------------------------------------
// Oversampling Settings
//----------------------------------------------
typedef enum
{
  BMP390_OSR_1X   = 0x00 ,
  BMP390_OSR_2X   = 0x01 ,
  BMP390_OSR_4X   = 0x02 ,
  BMP390_OSR_8X   = 0x03 ,
  BMP390_OSR_16X  = 0x04 ,
  BMP390_OSR_32X  = 0x05
} BMP390_Oversampling ;

//----------------------------------------------
// Output Data Rate
//----------------------------------------------
typedef enum
{
  BMP390_ODR_200_HZ   = 0x00 ,
  BMP390_ODR_100_HZ   = 0x01 ,
  BMP390_ODR_50_HZ    = 0x02 ,
  BMP390_ODR_25_HZ    = 0x03 ,
  BMP390_ODR_12_5_HZ  = 0x04 ,
  BMP390_ODR_6_25_HZ  = 0x05 ,
  BMP390_ODR_3_1_HZ   = 0x06 ,
  BMP390_ODR_1_5_HZ   = 0x07 ,
  BMP390_ODR_0_78_HZ  = 0x08 ,
  BMP390_ODR_0_39_HZ  = 0x09 ,
  BMP390_ODR_0_2_HZ   = 0x0A ,
  BMP390_ODR_0_1_HZ   = 0x0B ,
  BMP390_ODR_0_05_HZ  = 0x0C ,
  BMP390_ODR_0_02_HZ  = 0x0D ,
  BMP390_ODR_0_01_HZ  = 0x0E ,
  BMP390_ODR_0_006_HZ = 0x0F ,
  BMP390_ODR_0_003_HZ = 0x10 ,
  BMP390_ODR_0_001_HZ = 0x11
} BMP390_OutputDataRate ;

//----------------------------------------------
// IIR Filter Coefficients
//----------------------------------------------
typedef enum
{
  BMP390_IIR_COEF_0   = 0x00 ,   // Bypass
  BMP390_IIR_COEF_1   = 0x01 ,
  BMP390_IIR_COEF_3   = 0x02 ,
  BMP390_IIR_COEF_7   = 0x03 ,
  BMP390_IIR_COEF_15  = 0x04 ,
  BMP390_IIR_COEF_31  = 0x05 ,
  BMP390_IIR_COEF_63  = 0x06 ,
  BMP390_IIR_COEF_127 = 0x07
} BMP390_IIRFilter ;

//----------------------------------------------
// Calibration Data Structure
//----------------------------------------------
typedef struct
{
  // Temperature compensation
  float pParT1 ;
  float pParT2 ;
  float pParT3 ;

  // Pressure compensation
  float pParP1 ;
  float pParP2 ;
  float pParP3 ;
  float pParP4 ;
  float pParP5 ;
  float pParP6 ;
  float pParP7 ;
  float pParP8 ;
  float pParP9 ;
  float pParP10 ;
  float pParP11 ;
} BMP390_CalibData ;

//----------------------------------------------
// Sensor State Structure
//----------------------------------------------
typedef struct
{
  uint8_t pI2cAddr ;
  bool pInitialized ;
  BMP390_CalibData pCalib ;

  // Last compensated temperature (needed for pressure compensation)
  float pLastTemperatureC ;
} BMP390 ;

//----------------------------------------------
// Function: BMP390_Init
// Purpose: Initialize the BMP390 sensor
// Parameters:
//   outSensor - Sensor structure to initialize
//   inI2cAddr - I2C address (0x77 or 0x76)
// Returns: true if successful
//----------------------------------------------
bool BMP390_Init(BMP390 * outSensor, uint8_t inI2cAddr) ;

//----------------------------------------------
// Function: BMP390_IsConnected
// Purpose: Check if sensor is connected
// Returns: true if sensor responds with correct chip ID
//----------------------------------------------
bool BMP390_IsConnected(uint8_t inI2cAddr) ;

//----------------------------------------------
// Function: BMP390_Configure
// Purpose: Configure sensor settings
// Parameters:
//   ioSensor - Sensor to configure
//   inPressOsr - Pressure oversampling
//   inTempOsr - Temperature oversampling
//   inOdr - Output data rate
//   inFilter - IIR filter coefficient
// Returns: true if successful
//----------------------------------------------
bool BMP390_Configure(
  BMP390 * ioSensor,
  BMP390_Oversampling inPressOsr,
  BMP390_Oversampling inTempOsr,
  BMP390_OutputDataRate inOdr,
  BMP390_IIRFilter inFilter) ;

//----------------------------------------------
// Function: BMP390_SetMode
// Purpose: Set power mode
// Parameters:
//   ioSensor - Sensor
//   inNormalMode - true for continuous, false for sleep
// Returns: true if successful
//----------------------------------------------
bool BMP390_SetMode(BMP390 * ioSensor, bool inNormalMode) ;

//----------------------------------------------
// Function: BMP390_ReadPressureTemperature
// Purpose: Read both pressure and temperature
// Parameters:
//   inSensor - Sensor to read
//   outPressurePa - Pressure in Pascals
//   outTemperatureC - Temperature in Celsius
// Returns: true if successful
//----------------------------------------------
bool BMP390_ReadPressureTemperature(
  BMP390 * inSensor,
  float * outPressurePa,
  float * outTemperatureC) ;

//----------------------------------------------
// Function: BMP390_ReadPressure
// Purpose: Read pressure only
// Parameters:
//   inSensor - Sensor to read
//   outPressurePa - Pressure in Pascals
// Returns: true if successful
//----------------------------------------------
bool BMP390_ReadPressure(BMP390 * inSensor, float * outPressurePa) ;

//----------------------------------------------
// Function: BMP390_ReadTemperature
// Purpose: Read temperature only
// Parameters:
//   inSensor - Sensor to read
//   outTemperatureC - Temperature in Celsius
// Returns: true if successful
//----------------------------------------------
bool BMP390_ReadTemperature(BMP390 * inSensor, float * outTemperatureC) ;

//----------------------------------------------
// Function: BMP390_SoftReset
// Purpose: Perform software reset
// Parameters:
//   ioSensor - Sensor to reset
// Returns: true if successful
//----------------------------------------------
bool BMP390_SoftReset(BMP390 * ioSensor) ;

//----------------------------------------------
// Function: BMP390_GetStatus
// Purpose: Read sensor status register
// Parameters:
//   inSensor - Sensor to read
//   outStatus - Status byte
// Returns: true if successful
//----------------------------------------------
bool BMP390_GetStatus(BMP390 * inSensor, uint8_t * outStatus) ;

//----------------------------------------------
// Function: BMP390_DataReady
// Purpose: Check if new data is available
// Parameters:
//   inSensor - Sensor to check
// Returns: true if new data ready
//----------------------------------------------
bool BMP390_DataReady(BMP390 * inSensor) ;
