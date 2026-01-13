//----------------------------------------------
// Module: imu.h
// Description: LSM6DSOX + LIS3MDL 9-DoF IMU driver
//   for Rocket Avionics Flight Computer
// Author: Mark Gavin
// Created: 2026-01-12
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware: Adafruit LSM6DSOX + LIS3MDL FeatherWing (4565)
//   - LSM6DSOX: 3-axis accelerometer + 3-axis gyroscope
//   - LIS3MDL: 3-axis magnetometer
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// LSM6DSOX Register Definitions
//----------------------------------------------
#define LSM6DSOX_WHO_AM_I           0x0F
#define LSM6DSOX_WHO_AM_I_VALUE     0x6C

#define LSM6DSOX_CTRL1_XL           0x10  // Accel control
#define LSM6DSOX_CTRL2_G            0x11  // Gyro control
#define LSM6DSOX_CTRL3_C            0x12  // Control 3
#define LSM6DSOX_CTRL4_C            0x13  // Control 4
#define LSM6DSOX_CTRL5_C            0x14  // Control 5
#define LSM6DSOX_CTRL6_C            0x15  // Control 6
#define LSM6DSOX_CTRL7_G            0x16  // Gyro control 7
#define LSM6DSOX_CTRL8_XL           0x17  // Accel control 8

#define LSM6DSOX_STATUS_REG         0x1E
#define LSM6DSOX_OUT_TEMP_L         0x20
#define LSM6DSOX_OUT_TEMP_H         0x21
#define LSM6DSOX_OUTX_L_G           0x22  // Gyro X low
#define LSM6DSOX_OUTX_H_G           0x23  // Gyro X high
#define LSM6DSOX_OUTY_L_G           0x24
#define LSM6DSOX_OUTY_H_G           0x25
#define LSM6DSOX_OUTZ_L_G           0x26
#define LSM6DSOX_OUTZ_H_G           0x27
#define LSM6DSOX_OUTX_L_A           0x28  // Accel X low
#define LSM6DSOX_OUTX_H_A           0x29  // Accel X high
#define LSM6DSOX_OUTY_L_A           0x2A
#define LSM6DSOX_OUTY_H_A           0x2B
#define LSM6DSOX_OUTZ_L_A           0x2C
#define LSM6DSOX_OUTZ_H_A           0x2D

// Accel ODR (Output Data Rate) settings for CTRL1_XL[7:4]
#define LSM6DSOX_ODR_OFF            0x00
#define LSM6DSOX_ODR_12_5_HZ        0x10
#define LSM6DSOX_ODR_26_HZ          0x20
#define LSM6DSOX_ODR_52_HZ          0x30
#define LSM6DSOX_ODR_104_HZ         0x40
#define LSM6DSOX_ODR_208_HZ         0x50
#define LSM6DSOX_ODR_416_HZ         0x60
#define LSM6DSOX_ODR_833_HZ         0x70
#define LSM6DSOX_ODR_1660_HZ        0x80
#define LSM6DSOX_ODR_3330_HZ        0x90
#define LSM6DSOX_ODR_6660_HZ        0xA0

// Accel full-scale settings for CTRL1_XL[3:2]
#define LSM6DSOX_FS_XL_2G           0x00
#define LSM6DSOX_FS_XL_4G           0x08
#define LSM6DSOX_FS_XL_8G           0x0C
#define LSM6DSOX_FS_XL_16G          0x04

// Gyro ODR settings for CTRL2_G[7:4]
#define LSM6DSOX_ODR_G_OFF          0x00
#define LSM6DSOX_ODR_G_12_5_HZ      0x10
#define LSM6DSOX_ODR_G_26_HZ        0x20
#define LSM6DSOX_ODR_G_52_HZ        0x30
#define LSM6DSOX_ODR_G_104_HZ       0x40
#define LSM6DSOX_ODR_G_208_HZ       0x50
#define LSM6DSOX_ODR_G_416_HZ       0x60
#define LSM6DSOX_ODR_G_833_HZ       0x70
#define LSM6DSOX_ODR_G_1660_HZ      0x80
#define LSM6DSOX_ODR_G_3330_HZ      0x90
#define LSM6DSOX_ODR_G_6660_HZ      0xA0

// Gyro full-scale settings for CTRL2_G[3:2]
#define LSM6DSOX_FS_G_125DPS        0x02
#define LSM6DSOX_FS_G_250DPS        0x00
#define LSM6DSOX_FS_G_500DPS        0x04
#define LSM6DSOX_FS_G_1000DPS       0x08
#define LSM6DSOX_FS_G_2000DPS       0x0C

//----------------------------------------------
// LIS3MDL Register Definitions
//----------------------------------------------
#define LIS3MDL_WHO_AM_I            0x0F
#define LIS3MDL_WHO_AM_I_VALUE      0x3D

#define LIS3MDL_CTRL_REG1           0x20
#define LIS3MDL_CTRL_REG2           0x21
#define LIS3MDL_CTRL_REG3           0x22
#define LIS3MDL_CTRL_REG4           0x23
#define LIS3MDL_CTRL_REG5           0x24
#define LIS3MDL_STATUS_REG          0x27
#define LIS3MDL_OUT_X_L             0x28
#define LIS3MDL_OUT_X_H             0x29
#define LIS3MDL_OUT_Y_L             0x2A
#define LIS3MDL_OUT_Y_H             0x2B
#define LIS3MDL_OUT_Z_L             0x2C
#define LIS3MDL_OUT_Z_H             0x2D
#define LIS3MDL_TEMP_OUT_L          0x2E
#define LIS3MDL_TEMP_OUT_H          0x2F

// Mag ODR settings for CTRL_REG1[4:2]
#define LIS3MDL_ODR_0_625_HZ        0x00
#define LIS3MDL_ODR_1_25_HZ         0x04
#define LIS3MDL_ODR_2_5_HZ          0x08
#define LIS3MDL_ODR_5_HZ            0x0C
#define LIS3MDL_ODR_10_HZ           0x10
#define LIS3MDL_ODR_20_HZ           0x14
#define LIS3MDL_ODR_40_HZ           0x18
#define LIS3MDL_ODR_80_HZ           0x1C

// Mag full-scale settings for CTRL_REG2[6:5]
#define LIS3MDL_FS_4G               0x00
#define LIS3MDL_FS_8G               0x20
#define LIS3MDL_FS_12G              0x40
#define LIS3MDL_FS_16G              0x60

// Operating mode for CTRL_REG3[1:0]
#define LIS3MDL_MODE_CONTINUOUS     0x00
#define LIS3MDL_MODE_SINGLE         0x01
#define LIS3MDL_MODE_POWERDOWN      0x03

//----------------------------------------------
// IMU Data Structure
//----------------------------------------------
typedef struct
{
  // Accelerometer (raw and scaled)
  int16_t pAccelRawX ;
  int16_t pAccelRawY ;
  int16_t pAccelRawZ ;
  float pAccelX ;             // Acceleration in g
  float pAccelY ;
  float pAccelZ ;
  float pAccelMagnitude ;     // Total acceleration magnitude

  // Gyroscope (raw and scaled)
  int16_t pGyroRawX ;
  int16_t pGyroRawY ;
  int16_t pGyroRawZ ;
  float pGyroX ;              // Angular rate in dps
  float pGyroY ;
  float pGyroZ ;

  // Magnetometer (raw and scaled)
  int16_t pMagRawX ;
  int16_t pMagRawY ;
  int16_t pMagRawZ ;
  float pMagX ;               // Magnetic field in gauss
  float pMagY ;
  float pMagZ ;

  // Derived orientation
  float pPitchDeg ;           // Pitch angle (from accel)
  float pRollDeg ;            // Roll angle (from accel)
  float pHeadingDeg ;         // Compass heading (from mag)

  // Temperature
  float pTemperatureC ;

  // Status
  bool pAccelGyroReady ;
  bool pMagReady ;
} ImuData ;

//----------------------------------------------
// IMU State
//----------------------------------------------
typedef struct
{
  uint8_t pAccelGyroAddr ;    // LSM6DSOX I2C address
  uint8_t pMagAddr ;          // LIS3MDL I2C address
  bool pAccelGyroOk ;         // Accel/Gyro initialized
  bool pMagOk ;               // Magnetometer initialized
  float pAccelScale ;         // Scale factor for accel (g/LSB)
  float pGyroScale ;          // Scale factor for gyro (dps/LSB)
  float pMagScale ;           // Scale factor for mag (gauss/LSB)
  ImuData pData ;             // Current IMU data
} Imu ;

//----------------------------------------------
// Function: IMU_Init
// Purpose: Initialize the IMU (LSM6DSOX + LIS3MDL)
// Parameters:
//   ioImu - IMU structure to initialize
// Returns: true if successful
//----------------------------------------------
bool IMU_Init(Imu * ioImu) ;

//----------------------------------------------
// Function: IMU_Configure
// Purpose: Configure IMU settings
// Parameters:
//   ioImu - IMU structure
//   inAccelRange - Accelerometer range (2, 4, 8, 16 g)
//   inGyroRange - Gyroscope range (125, 250, 500, 1000, 2000 dps)
//   inMagRange - Magnetometer range (4, 8, 12, 16 gauss)
// Returns: true if successful
//----------------------------------------------
bool IMU_Configure(
  Imu * ioImu,
  uint8_t inAccelRange,
  uint16_t inGyroRange,
  uint8_t inMagRange) ;

//----------------------------------------------
// Function: IMU_Read
// Purpose: Read all IMU sensors
// Parameters:
//   ioImu - IMU structure
// Returns: true if data available
//----------------------------------------------
bool IMU_Read(Imu * ioImu) ;

//----------------------------------------------
// Function: IMU_ReadAccelGyro
// Purpose: Read accelerometer and gyroscope only
// Parameters:
//   ioImu - IMU structure
// Returns: true if data available
//----------------------------------------------
bool IMU_ReadAccelGyro(Imu * ioImu) ;

//----------------------------------------------
// Function: IMU_ReadMag
// Purpose: Read magnetometer only
// Parameters:
//   ioImu - IMU structure
// Returns: true if data available
//----------------------------------------------
bool IMU_ReadMag(Imu * ioImu) ;

//----------------------------------------------
// Function: IMU_CalculateOrientation
// Purpose: Calculate pitch, roll, heading from sensor data
// Parameters:
//   ioImu - IMU structure
//----------------------------------------------
void IMU_CalculateOrientation(Imu * ioImu) ;

//----------------------------------------------
// Function: IMU_GetData
// Purpose: Get pointer to current IMU data
// Parameters:
//   inImu - IMU structure
// Returns: Pointer to IMU data
//----------------------------------------------
const ImuData * IMU_GetData(const Imu * inImu) ;

//----------------------------------------------
// Function: IMU_DataReady
// Purpose: Check if new data is available
// Parameters:
//   inImu - IMU structure
// Returns: true if new data ready
//----------------------------------------------
bool IMU_DataReady(const Imu * inImu) ;

