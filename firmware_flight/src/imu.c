//----------------------------------------------
// Module: imu.c
// Description: IMU driver (LSM6DSOX or ICM-20649, + LIS3MDL)
//   for Rocket Avionics Flight Computer
// Author: Mark Gavin
// Created: 2026-01-12
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "imu.h"
#include "pins.h"

#include "hardware/i2c.h"
#include "pico/time.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

//----------------------------------------------
// Local Function Declarations
//----------------------------------------------
static bool WriteRegister(uint8_t inAddr, uint8_t inReg, uint8_t inValue) ;
static bool ReadRegister(uint8_t inAddr, uint8_t inReg, uint8_t * outValue) ;
static bool ReadRegisters(uint8_t inAddr, uint8_t inReg, uint8_t * outBuffer, uint8_t inLen) ;
static bool InitAccelGyro(Imu * ioImu) ;
static bool InitICM20649(Imu * ioImu) ;
static bool ICM20649_SetBank(uint8_t inAddr, uint8_t inBank) ;
static bool InitMag(Imu * ioImu) ;

//----------------------------------------------
// Function: IMU_Init
//----------------------------------------------
bool IMU_Init(Imu * ioImu)
{
  if (ioImu == NULL) return false ;

  // Initialize structure
  memset(ioImu, 0, sizeof(Imu)) ;
  ioImu->pImuType = kImuTypeNone ;
  ioImu->pMagAddr = kI2cAddrLIS3MDL ;

  // Try ICM-20649 first (wide-range, preferred for flight)
  if (InitICM20649(ioImu))
  {
    ioImu->pImuType = kImuTypeICM20649 ;
    ioImu->pAccelGyroOk = true ;
    printf("ICM-20649 initialized at 0x%02X\n", ioImu->pAccelGyroAddr) ;
  }
  else
  {
    printf("ICM-20649 not found, trying LSM6DSOX...\n") ;

    // Fall back to LSM6DSOX (lower range, higher precision)
    ioImu->pAccelGyroAddr = kI2cAddrLSM6DSOX ;
    if (InitAccelGyro(ioImu))
    {
      ioImu->pImuType = kImuTypeLSM6DSOX ;
      ioImu->pAccelGyroOk = true ;
      printf("LSM6DSOX initialized at 0x%02X\n", ioImu->pAccelGyroAddr) ;
    }
    else
    {
      printf("WARNING: No accel/gyro IMU found\n") ;
    }
  }

  // Initialize LIS3MDL (magnetometer) - works with either IMU
  if (InitMag(ioImu))
  {
    ioImu->pMagOk = true ;
    printf("LIS3MDL initialized at 0x%02X\n", ioImu->pMagAddr) ;
  }
  else
  {
    printf("WARNING: LIS3MDL initialization failed\n") ;
  }

  // Return true if at least accel/gyro works
  return ioImu->pAccelGyroOk ;
}

//----------------------------------------------
// Function: InitAccelGyro
//----------------------------------------------
static bool InitAccelGyro(Imu * ioImu)
{
  uint8_t theWhoAmI = 0 ;

  // Check WHO_AM_I register
  if (!ReadRegister(ioImu->pAccelGyroAddr, LSM6DSOX_WHO_AM_I, &theWhoAmI))
  {
    printf("LSM6DSOX: Failed to read WHO_AM_I\n") ;
    return false ;
  }

  if (theWhoAmI != LSM6DSOX_WHO_AM_I_VALUE)
  {
    printf("LSM6DSOX: Unexpected WHO_AM_I: 0x%02X (expected 0x%02X)\n",
           theWhoAmI, LSM6DSOX_WHO_AM_I_VALUE) ;
    return false ;
  }

  // Software reset
  WriteRegister(ioImu->pAccelGyroAddr, LSM6DSOX_CTRL3_C, 0x01) ;
  sleep_ms(10) ;

  // Wait for reset to complete
  uint8_t theCtrl3 = 0 ;
  for (int i = 0 ; i < 10 ; i++)
  {
    ReadRegister(ioImu->pAccelGyroAddr, LSM6DSOX_CTRL3_C, &theCtrl3) ;
    if ((theCtrl3 & 0x01) == 0) break ;
    sleep_ms(1) ;
  }

  // Configure accelerometer: 416 Hz, ±8g
  uint8_t theAccelConfig = LSM6DSOX_ODR_416_HZ | LSM6DSOX_FS_XL_8G ;
  if (!WriteRegister(ioImu->pAccelGyroAddr, LSM6DSOX_CTRL1_XL, theAccelConfig))
  {
    printf("LSM6DSOX: Failed to configure accel\n") ;
    return false ;
  }
  ioImu->pAccelScale = 0.000244f ;  // 8g: 0.244 mg/LSB = 0.000244 g/LSB

  // Configure gyroscope: 416 Hz, ±1000 dps
  uint8_t theGyroConfig = LSM6DSOX_ODR_G_416_HZ | LSM6DSOX_FS_G_1000DPS ;
  if (!WriteRegister(ioImu->pAccelGyroAddr, LSM6DSOX_CTRL2_G, theGyroConfig))
  {
    printf("LSM6DSOX: Failed to configure gyro\n") ;
    return false ;
  }
  ioImu->pGyroScale = 0.035f ;  // 1000dps: 35 mdps/LSB = 0.035 dps/LSB

  // Enable BDU (Block Data Update) to prevent reading during update
  WriteRegister(ioImu->pAccelGyroAddr, LSM6DSOX_CTRL3_C, 0x44) ;  // BDU + IF_INC

  return true ;
}

//----------------------------------------------
// Function: ICM20649_SetBank
//----------------------------------------------
static bool ICM20649_SetBank(uint8_t inAddr, uint8_t inBank)
{
  return WriteRegister(inAddr, ICM20649_REG_BANK_SEL, inBank) ;
}

//----------------------------------------------
// Function: InitICM20649
//----------------------------------------------
static bool InitICM20649(Imu * ioImu)
{
  uint8_t theWhoAmI = 0 ;

  // Try primary address (0x68), then alternate (0x69)
  uint8_t theAddresses[] = { kI2cAddrICM20649, 0x69 } ;
  bool theFound = false ;

  for (int i = 0 ; i < 2 ; i++)
  {
    ioImu->pAccelGyroAddr = theAddresses[i] ;

    // Must be in bank 0 to read WHO_AM_I
    ICM20649_SetBank(ioImu->pAccelGyroAddr, ICM20649_BANK_0) ;

    if (ReadRegister(ioImu->pAccelGyroAddr, ICM20649_WHO_AM_I, &theWhoAmI))
    {
      if (theWhoAmI == ICM20649_WHO_AM_I_VALUE)
      {
        printf("ICM-20649: Found at address 0x%02X (WHO_AM_I=0x%02X)\n",
               ioImu->pAccelGyroAddr, theWhoAmI) ;
        theFound = true ;
        break ;
      }
      else
      {
        printf("ICM-20649: Wrong WHO_AM_I at 0x%02X: 0x%02X\n",
               ioImu->pAccelGyroAddr, theWhoAmI) ;
      }
    }
  }

  if (!theFound) return false ;

  // Reset device
  ICM20649_SetBank(ioImu->pAccelGyroAddr, ICM20649_BANK_0) ;
  WriteRegister(ioImu->pAccelGyroAddr, ICM20649_PWR_MGMT_1, ICM20649_RESET) ;
  sleep_ms(20) ;

  // Wake up, auto-select clock
  WriteRegister(ioImu->pAccelGyroAddr, ICM20649_PWR_MGMT_1, ICM20649_CLKSEL_AUTO) ;
  sleep_ms(10) ;

  // Enable all accel + gyro axes
  WriteRegister(ioImu->pAccelGyroAddr, ICM20649_PWR_MGMT_2, 0x00) ;

  // Switch to bank 2 for configuration
  ICM20649_SetBank(ioImu->pAccelGyroAddr, ICM20649_BANK_2) ;

  // Configure gyro: ±1000 dps, DLPF enabled, BW setting 3 (~51 Hz 3dB)
  uint8_t theGyroConfig = ICM20649_FS_G_1000DPS | ICM20649_DLPF_ENABLE | ICM20649_DLPF_BW_3 ;
  if (!WriteRegister(ioImu->pAccelGyroAddr, ICM20649_GYRO_CONFIG_1, theGyroConfig))
  {
    printf("ICM-20649: Failed to configure gyro\n") ;
    return false ;
  }
  ioImu->pGyroScale = 1.0f / 32.8f ;  // ±1000 dps: 32.8 LSB/dps

  // Gyro sample rate divider = 0 (1.1 kHz / (1+0) = 1.1 kHz)
  WriteRegister(ioImu->pAccelGyroAddr, ICM20649_GYRO_SMPLRT_DIV, 0x00) ;

  // Configure accel: ±8g, DLPF enabled, BW setting 3 (~50 Hz 3dB)
  uint8_t theAccelConfig = ICM20649_FS_A_8G | ICM20649_DLPF_ENABLE | ICM20649_DLPF_BW_3 ;
  if (!WriteRegister(ioImu->pAccelGyroAddr, ICM20649_ACCEL_CONFIG_1, theAccelConfig))
  {
    printf("ICM-20649: Failed to configure accel\n") ;
    return false ;
  }
  ioImu->pAccelScale = 1.0f / 4096.0f ;  // ±8g: 4096 LSB/g

  // Accel sample rate divider = 0 (1.125 kHz / (1+0) = 1.125 kHz)
  WriteRegister(ioImu->pAccelGyroAddr, ICM20649_ACCEL_SMPLRT_DIV_1, 0x00) ;
  WriteRegister(ioImu->pAccelGyroAddr, ICM20649_ACCEL_SMPLRT_DIV_2, 0x00) ;

  // Return to bank 0 for data reads
  ICM20649_SetBank(ioImu->pAccelGyroAddr, ICM20649_BANK_0) ;

  return true ;
}

//----------------------------------------------
// Function: InitMag
//----------------------------------------------
static bool InitMag(Imu * ioImu)
{
  uint8_t theWhoAmI = 0 ;

  // Try primary address first (0x1E), then alternate (0x1C)
  uint8_t theAddresses[] = { 0x1E, 0x1C } ;
  bool theFound = false ;

  for (int i = 0 ; i < 2 ; i++)
  {
    ioImu->pMagAddr = theAddresses[i] ;
    printf("LIS3MDL: Trying address 0x%02X...\n", ioImu->pMagAddr) ;

    if (ReadRegister(ioImu->pMagAddr, LIS3MDL_WHO_AM_I, &theWhoAmI))
    {
      if (theWhoAmI == LIS3MDL_WHO_AM_I_VALUE)
      {
        printf("LIS3MDL: Found at address 0x%02X\n", ioImu->pMagAddr) ;
        theFound = true ;
        break ;
      }
      else
      {
        printf("LIS3MDL: Wrong WHO_AM_I at 0x%02X: 0x%02X\n", ioImu->pMagAddr, theWhoAmI) ;
      }
    }
  }

  if (!theFound)
  {
    printf("LIS3MDL: Not found at any address\n") ;
    return false ;
  }

  // Configure: 80 Hz, high performance mode
  // CTRL_REG1: TEMP_EN=1, OM=11 (ultra-high perf), ODR=111 (80Hz), FAST_ODR=0, ST=0
  uint8_t theCtrl1 = 0x80 | 0x60 | LIS3MDL_ODR_80_HZ ;  // Temp enable + ultra-high perf + 80Hz
  if (!WriteRegister(ioImu->pMagAddr, LIS3MDL_CTRL_REG1, theCtrl1))
  {
    printf("LIS3MDL: Failed to configure CTRL_REG1\n") ;
    return false ;
  }

  // CTRL_REG2: FS = ±4 gauss
  if (!WriteRegister(ioImu->pMagAddr, LIS3MDL_CTRL_REG2, LIS3MDL_FS_4G))
  {
    printf("LIS3MDL: Failed to configure CTRL_REG2\n") ;
    return false ;
  }
  ioImu->pMagScale = 0.0001464f ;  // 4 gauss: 6842 LSB/gauss = 1/6842 gauss/LSB

  // CTRL_REG3: Continuous conversion mode
  if (!WriteRegister(ioImu->pMagAddr, LIS3MDL_CTRL_REG3, LIS3MDL_MODE_CONTINUOUS))
  {
    printf("LIS3MDL: Failed to configure CTRL_REG3\n") ;
    return false ;
  }

  // CTRL_REG4: Z-axis ultra-high performance
  if (!WriteRegister(ioImu->pMagAddr, LIS3MDL_CTRL_REG4, 0x0C))
  {
    printf("LIS3MDL: Failed to configure CTRL_REG4\n") ;
    return false ;
  }

  // CTRL_REG5: BDU enabled
  if (!WriteRegister(ioImu->pMagAddr, LIS3MDL_CTRL_REG5, 0x40))
  {
    printf("LIS3MDL: Failed to configure CTRL_REG5\n") ;
    return false ;
  }

  return true ;
}

//----------------------------------------------
// Function: IMU_Configure
//----------------------------------------------
bool IMU_Configure(
  Imu * ioImu,
  uint8_t inAccelRange,
  uint16_t inGyroRange,
  uint8_t inMagRange)
{
  if (ioImu == NULL) return false ;

  if (ioImu->pImuType == kImuTypeLSM6DSOX)
  {
    // Configure LSM6DSOX accelerometer range
    uint8_t theAccelFs = 0 ;
    switch (inAccelRange)
    {
      case 2:
        theAccelFs = LSM6DSOX_FS_XL_2G ;
        ioImu->pAccelScale = 0.000061f ;  // 0.061 mg/LSB
        break ;
      case 4:
        theAccelFs = LSM6DSOX_FS_XL_4G ;
        ioImu->pAccelScale = 0.000122f ;  // 0.122 mg/LSB
        break ;
      case 8:
        theAccelFs = LSM6DSOX_FS_XL_8G ;
        ioImu->pAccelScale = 0.000244f ;  // 0.244 mg/LSB
        break ;
      case 16:
        theAccelFs = LSM6DSOX_FS_XL_16G ;
        ioImu->pAccelScale = 0.000488f ;  // 0.488 mg/LSB
        break ;
      default:
        theAccelFs = LSM6DSOX_FS_XL_8G ;
        ioImu->pAccelScale = 0.000244f ;
        break ;
    }

    // Configure LSM6DSOX gyroscope range
    uint8_t theGyroFs = 0 ;
    switch (inGyroRange)
    {
      case 125:
        theGyroFs = LSM6DSOX_FS_G_125DPS ;
        ioImu->pGyroScale = 0.004375f ;  // 4.375 mdps/LSB
        break ;
      case 250:
        theGyroFs = LSM6DSOX_FS_G_250DPS ;
        ioImu->pGyroScale = 0.00875f ;   // 8.75 mdps/LSB
        break ;
      case 500:
        theGyroFs = LSM6DSOX_FS_G_500DPS ;
        ioImu->pGyroScale = 0.0175f ;    // 17.5 mdps/LSB
        break ;
      case 1000:
        theGyroFs = LSM6DSOX_FS_G_1000DPS ;
        ioImu->pGyroScale = 0.035f ;     // 35 mdps/LSB
        break ;
      case 2000:
        theGyroFs = LSM6DSOX_FS_G_2000DPS ;
        ioImu->pGyroScale = 0.070f ;     // 70 mdps/LSB
        break ;
      default:
        theGyroFs = LSM6DSOX_FS_G_1000DPS ;
        ioImu->pGyroScale = 0.035f ;
        break ;
    }

    // Write accel config
    uint8_t theAccelConfig = LSM6DSOX_ODR_416_HZ | theAccelFs ;
    WriteRegister(ioImu->pAccelGyroAddr, LSM6DSOX_CTRL1_XL, theAccelConfig) ;

    // Write gyro config
    uint8_t theGyroConfig = LSM6DSOX_ODR_G_416_HZ | theGyroFs ;
    WriteRegister(ioImu->pAccelGyroAddr, LSM6DSOX_CTRL2_G, theGyroConfig) ;
  }
  else if (ioImu->pImuType == kImuTypeICM20649)
  {
    // Configure ICM-20649 (bank 2 for config registers)
    ICM20649_SetBank(ioImu->pAccelGyroAddr, ICM20649_BANK_2) ;

    // Configure accelerometer range
    uint8_t theAccelFs = 0 ;
    switch (inAccelRange)
    {
      case 4:
        theAccelFs = ICM20649_FS_A_4G ;
        ioImu->pAccelScale = 1.0f / 8192.0f ;
        break ;
      case 8:
        theAccelFs = ICM20649_FS_A_8G ;
        ioImu->pAccelScale = 1.0f / 4096.0f ;
        break ;
      case 16:
        theAccelFs = ICM20649_FS_A_16G ;
        ioImu->pAccelScale = 1.0f / 2048.0f ;
        break ;
      case 30:
        theAccelFs = ICM20649_FS_A_30G ;
        ioImu->pAccelScale = 1.0f / 1024.0f ;
        break ;
      default:
        theAccelFs = ICM20649_FS_A_8G ;
        ioImu->pAccelScale = 1.0f / 4096.0f ;
        break ;
    }

    uint8_t theAccelConfig = theAccelFs | ICM20649_DLPF_ENABLE | ICM20649_DLPF_BW_3 ;
    WriteRegister(ioImu->pAccelGyroAddr, ICM20649_ACCEL_CONFIG_1, theAccelConfig) ;

    // Configure gyroscope range
    uint8_t theGyroFs = 0 ;
    switch (inGyroRange)
    {
      case 500:
        theGyroFs = ICM20649_FS_G_500DPS ;
        ioImu->pGyroScale = 1.0f / 65.5f ;
        break ;
      case 1000:
        theGyroFs = ICM20649_FS_G_1000DPS ;
        ioImu->pGyroScale = 1.0f / 32.8f ;
        break ;
      case 2000:
        theGyroFs = ICM20649_FS_G_2000DPS ;
        ioImu->pGyroScale = 1.0f / 16.4f ;
        break ;
      case 4000:
        theGyroFs = ICM20649_FS_G_4000DPS ;
        ioImu->pGyroScale = 1.0f / 8.2f ;
        break ;
      default:
        theGyroFs = ICM20649_FS_G_1000DPS ;
        ioImu->pGyroScale = 1.0f / 32.8f ;
        break ;
    }

    uint8_t theGyroConfig = theGyroFs | ICM20649_DLPF_ENABLE | ICM20649_DLPF_BW_3 ;
    WriteRegister(ioImu->pAccelGyroAddr, ICM20649_GYRO_CONFIG_1, theGyroConfig) ;

    // Return to bank 0
    ICM20649_SetBank(ioImu->pAccelGyroAddr, ICM20649_BANK_0) ;
  }

  // Configure magnetometer range (shared between both IMU types)
  if (ioImu->pMagOk)
  {
    uint8_t theMagFs = 0 ;
    switch (inMagRange)
    {
      case 4:
        theMagFs = LIS3MDL_FS_4G ;
        ioImu->pMagScale = 0.0001464f ;  // 1/6842
        break ;
      case 8:
        theMagFs = LIS3MDL_FS_8G ;
        ioImu->pMagScale = 0.0002924f ;  // 1/3421
        break ;
      case 12:
        theMagFs = LIS3MDL_FS_12G ;
        ioImu->pMagScale = 0.0004384f ;  // 1/2281
        break ;
      case 16:
        theMagFs = LIS3MDL_FS_16G ;
        ioImu->pMagScale = 0.0005844f ;  // 1/1711
        break ;
      default:
        theMagFs = LIS3MDL_FS_4G ;
        ioImu->pMagScale = 0.0001464f ;
        break ;
    }

    WriteRegister(ioImu->pMagAddr, LIS3MDL_CTRL_REG2, theMagFs) ;
  }

  return true ;
}

//----------------------------------------------
// Function: IMU_Read
//----------------------------------------------
bool IMU_Read(Imu * ioImu)
{
  if (ioImu == NULL) return false ;

  bool theResult = false ;

  if (ioImu->pAccelGyroOk)
  {
    theResult = IMU_ReadAccelGyro(ioImu) ;
  }

  if (ioImu->pMagOk)
  {
    IMU_ReadMag(ioImu) ;
  }

  // Calculate derived values
  IMU_CalculateOrientation(ioImu) ;

  return theResult ;
}

//----------------------------------------------
// Function: IMU_ReadAccelGyro
//----------------------------------------------
bool IMU_ReadAccelGyro(Imu * ioImu)
{
  if (ioImu == NULL || !ioImu->pAccelGyroOk) return false ;

  if (ioImu->pImuType == kImuTypeLSM6DSOX)
  {
    // Check data ready
    uint8_t theStatus = 0 ;
    ReadRegister(ioImu->pAccelGyroAddr, LSM6DSOX_STATUS_REG, &theStatus) ;

    ioImu->pData.pAccelGyroReady = (theStatus & 0x01) != 0 ;  // XLDA bit

    if (!ioImu->pData.pAccelGyroReady) return false ;

    // Read gyro data (6 bytes starting at OUTX_L_G) - LSB first
    uint8_t theGyroData[6] ;
    if (ReadRegisters(ioImu->pAccelGyroAddr, LSM6DSOX_OUTX_L_G, theGyroData, 6))
    {
      ioImu->pData.pGyroRawX = (int16_t)(theGyroData[0] | (theGyroData[1] << 8)) ;
      ioImu->pData.pGyroRawY = (int16_t)(theGyroData[2] | (theGyroData[3] << 8)) ;
      ioImu->pData.pGyroRawZ = (int16_t)(theGyroData[4] | (theGyroData[5] << 8)) ;
    }

    // Read accel data (6 bytes starting at OUTX_L_A) - LSB first
    uint8_t theAccelData[6] ;
    if (ReadRegisters(ioImu->pAccelGyroAddr, LSM6DSOX_OUTX_L_A, theAccelData, 6))
    {
      ioImu->pData.pAccelRawX = (int16_t)(theAccelData[0] | (theAccelData[1] << 8)) ;
      ioImu->pData.pAccelRawY = (int16_t)(theAccelData[2] | (theAccelData[3] << 8)) ;
      ioImu->pData.pAccelRawZ = (int16_t)(theAccelData[4] | (theAccelData[5] << 8)) ;
    }
  }
  else if (ioImu->pImuType == kImuTypeICM20649)
  {
    // Read 12 bytes: accel XYZ (6) + gyro XYZ (6), starting at ACCEL_XOUT_H
    // ICM-20649 data is MSB first (big-endian)
    uint8_t theData[12] ;
    if (ReadRegisters(ioImu->pAccelGyroAddr, ICM20649_ACCEL_XOUT_H, theData, 12))
    {
      // Accel: MSB first
      ioImu->pData.pAccelRawX = (int16_t)((theData[0] << 8) | theData[1]) ;
      ioImu->pData.pAccelRawY = (int16_t)((theData[2] << 8) | theData[3]) ;
      ioImu->pData.pAccelRawZ = (int16_t)((theData[4] << 8) | theData[5]) ;

      // Gyro: MSB first
      ioImu->pData.pGyroRawX = (int16_t)((theData[6] << 8) | theData[7]) ;
      ioImu->pData.pGyroRawY = (int16_t)((theData[8] << 8) | theData[9]) ;
      ioImu->pData.pGyroRawZ = (int16_t)((theData[10] << 8) | theData[11]) ;

      ioImu->pData.pAccelGyroReady = true ;
    }
    else
    {
      ioImu->pData.pAccelGyroReady = false ;
      return false ;
    }
  }
  else
  {
    return false ;
  }

  // Scale raw values (common for both IMU types)
  ioImu->pData.pAccelX = ioImu->pData.pAccelRawX * ioImu->pAccelScale ;
  ioImu->pData.pAccelY = ioImu->pData.pAccelRawY * ioImu->pAccelScale ;
  ioImu->pData.pAccelZ = ioImu->pData.pAccelRawZ * ioImu->pAccelScale ;

  ioImu->pData.pGyroX = ioImu->pData.pGyroRawX * ioImu->pGyroScale ;
  ioImu->pData.pGyroY = ioImu->pData.pGyroRawY * ioImu->pGyroScale ;
  ioImu->pData.pGyroZ = ioImu->pData.pGyroRawZ * ioImu->pGyroScale ;

  // Calculate magnitude
  ioImu->pData.pAccelMagnitude = sqrtf(
    ioImu->pData.pAccelX * ioImu->pData.pAccelX +
    ioImu->pData.pAccelY * ioImu->pData.pAccelY +
    ioImu->pData.pAccelZ * ioImu->pData.pAccelZ) ;

  return true ;
}

//----------------------------------------------
// Function: IMU_ReadMag
//----------------------------------------------
bool IMU_ReadMag(Imu * ioImu)
{
  if (ioImu == NULL || !ioImu->pMagOk) return false ;

  // Check data ready
  uint8_t theStatus = 0 ;
  ReadRegister(ioImu->pMagAddr, LIS3MDL_STATUS_REG, &theStatus) ;

  ioImu->pData.pMagReady = (theStatus & 0x08) != 0 ;  // ZYXDA bit

  // Read mag data even if not ready (for debugging)
  // LIS3MDL requires bit 7 set for auto-increment on multi-byte reads
  uint8_t theMagData[6] ;
  if (ReadRegisters(ioImu->pMagAddr, LIS3MDL_OUT_X_L | 0x80, theMagData, 6))
  {
    ioImu->pData.pMagRawX = (int16_t)(theMagData[0] | (theMagData[1] << 8)) ;
    ioImu->pData.pMagRawY = (int16_t)(theMagData[2] | (theMagData[3] << 8)) ;
    ioImu->pData.pMagRawZ = (int16_t)(theMagData[4] | (theMagData[5] << 8)) ;

    ioImu->pData.pMagX = ioImu->pData.pMagRawX * ioImu->pMagScale ;
    ioImu->pData.pMagY = ioImu->pData.pMagRawY * ioImu->pMagScale ;
    ioImu->pData.pMagZ = ioImu->pData.pMagRawZ * ioImu->pMagScale ;

    return true ;
  }

  return false ;
}

//----------------------------------------------
// Function: IMU_CalculateOrientation
//----------------------------------------------
void IMU_CalculateOrientation(Imu * ioImu)
{
  if (ioImu == NULL) return ;

  // Calculate pitch and roll from accelerometer
  // Rocket mounting: Y-axis vertical (up), X-axis right, Z-axis toward observer
  float theAccelX = ioImu->pData.pAccelX ;
  float theAccelY = ioImu->pData.pAccelY ;
  float theAccelZ = ioImu->pData.pAccelZ ;

  // Pitch: rotation around X-axis (tilting nose forward/backward)
  // When nose tips backward (away from observer), Z decreases, pitch positive
  ioImu->pData.pPitchDeg = atan2f(-theAccelZ,
    sqrtf(theAccelX * theAccelX + theAccelY * theAccelY)) * 180.0f / 3.14159265f ;

  // Roll: rotation around Z-axis (tilting left/right)
  // When rocket tips right, X increases, roll positive
  ioImu->pData.pRollDeg = atan2f(theAccelX, theAccelY) * 180.0f / 3.14159265f ;

  // Calculate heading from magnetometer
  // LIS3MDL chip Z-axis is perpendicular to board surface
  // When board is vertical (display facing observer), chip Z points up
  // Horizontal plane is chip X-Y
  if (ioImu->pMagOk)
  {
    float theMagX = ioImu->pData.pMagX ;
    float theMagY = ioImu->pData.pMagY ;

    // Heading from horizontal components (X-Y plane when Z is up)
    float theHeading = atan2f(theMagX, theMagY) * 180.0f / 3.14159265f ;
    if (theHeading < 0) theHeading += 360.0f ;

    ioImu->pData.pHeadingDeg = theHeading ;
  }
}

//----------------------------------------------
// Function: IMU_GetData
//----------------------------------------------
const ImuData * IMU_GetData(const Imu * inImu)
{
  if (inImu == NULL) return NULL ;
  return &inImu->pData ;
}

//----------------------------------------------
// Function: IMU_DataReady
//----------------------------------------------
bool IMU_DataReady(const Imu * inImu)
{
  if (inImu == NULL || !inImu->pAccelGyroOk) return false ;

  uint8_t theStatus = 0 ;

  if (inImu->pImuType == kImuTypeLSM6DSOX)
  {
    ReadRegister(inImu->pAccelGyroAddr, LSM6DSOX_STATUS_REG, &theStatus) ;
    return (theStatus & 0x01) != 0 ;  // XLDA bit
  }
  else if (inImu->pImuType == kImuTypeICM20649)
  {
    // ICM-20649 runs at 1.1 kHz, always has data at our 100 Hz read rate
    return true ;
  }

  return false ;
}

//----------------------------------------------
// Function: IMU_GetTypeName
//----------------------------------------------
const char * IMU_GetTypeName(const Imu * inImu)
{
  if (inImu == NULL) return "None" ;

  switch (inImu->pImuType)
  {
    case kImuTypeLSM6DSOX:
      return inImu->pMagOk ? "LSM6DSOX+LIS3MDL" : "LSM6DSOX" ;
    case kImuTypeICM20649:
      return inImu->pMagOk ? "ICM20649+LIS3MDL" : "ICM-20649" ;
    default:
      return "None" ;
  }
}

//----------------------------------------------
// I2C Helper Functions
//----------------------------------------------

static bool WriteRegister(uint8_t inAddr, uint8_t inReg, uint8_t inValue)
{
  uint8_t theData[2] = { inReg, inValue } ;
  absolute_time_t theTimeout = make_timeout_time_ms(100) ;
  int theResult = i2c_write_blocking_until(kI2cPort, inAddr, theData, 2, false, theTimeout) ;
  return theResult == 2 ;
}

static bool ReadRegister(uint8_t inAddr, uint8_t inReg, uint8_t * outValue)
{
  absolute_time_t theTimeout = make_timeout_time_ms(100) ;
  int theResult = i2c_write_blocking_until(kI2cPort, inAddr, &inReg, 1, true, theTimeout) ;
  if (theResult != 1) return false ;

  theTimeout = make_timeout_time_ms(100) ;
  theResult = i2c_read_blocking_until(kI2cPort, inAddr, outValue, 1, false, theTimeout) ;
  return theResult == 1 ;
}

static bool ReadRegisters(uint8_t inAddr, uint8_t inReg, uint8_t * outBuffer, uint8_t inLen)
{
  absolute_time_t theTimeout = make_timeout_time_ms(100) ;
  int theResult = i2c_write_blocking_until(kI2cPort, inAddr, &inReg, 1, true, theTimeout) ;
  if (theResult != 1) return false ;

  theTimeout = make_timeout_time_ms(100) ;
  theResult = i2c_read_blocking_until(kI2cPort, inAddr, outBuffer, inLen, false, theTimeout) ;
  return theResult == inLen ;
}

