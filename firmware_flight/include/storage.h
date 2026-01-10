//----------------------------------------------
// Module: storage.h
// Description: Flash storage for calibration
//   data persistence
// Author: Mark Gavin
// Created: 2025-11-29
// Copyright: (c) 2025 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// Calibration Data Structure
//----------------------------------------------
typedef struct
{
  uint32_t pMagic ;         // Magic number for validation
  uint32_t pVersion ;       // Data format version
  int32_t pOffset ;         // Tare offset value
  float pScaleFactor ;      // Calibration scale factor
  uint32_t pChecksum ;      // Data integrity check
} CalibrationData ;

//----------------------------------------------
// Magic number and version constants
//----------------------------------------------
#define kCalibrationMagic   0x43414C42  // "CALB"
#define kCalibrationVersion 1

//----------------------------------------------
// Function: Storage_Init
// Purpose: Initialize flash storage system
// Returns: true if successful
//----------------------------------------------
bool Storage_Init(void) ;

//----------------------------------------------
// Function: Storage_SaveCalibration
// Purpose: Save calibration data to flash
// Parameters:
//   inOffset - tare offset value
//   inScaleFactor - calibration scale factor
// Returns: true if successful
//----------------------------------------------
bool Storage_SaveCalibration(
  int32_t inOffset ,
  float inScaleFactor) ;

//----------------------------------------------
// Function: Storage_LoadCalibration
// Purpose: Load calibration data from flash
// Parameters:
//   outOffset - receives tare offset
//   outScaleFactor - receives scale factor
// Returns: true if valid data found
//----------------------------------------------
bool Storage_LoadCalibration(
  int32_t * outOffset ,
  float * outScaleFactor) ;

//----------------------------------------------
// Function: Storage_HasCalibration
// Purpose: Check if valid calibration exists
// Returns: true if calibration data present
//----------------------------------------------
bool Storage_HasCalibration(void) ;

//----------------------------------------------
// Function: Storage_ClearCalibration
// Purpose: Erase stored calibration data
// Returns: true if successful
//----------------------------------------------
bool Storage_ClearCalibration(void) ;
