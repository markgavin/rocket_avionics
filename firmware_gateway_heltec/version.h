//----------------------------------------------
// Module: version.h
// Description: Firmware version information
//   for Heltec Wireless Tracker Gateway
// Author: Mark Gavin
// Created: 2026-01-17
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#pragma once

//----------------------------------------------
// Version Components
//----------------------------------------------
#define FIRMWARE_VERSION_MAJOR      1
#define FIRMWARE_VERSION_MINOR      4
#define FIRMWARE_VERSION_PATCH      1
#define FIRMWARE_VERSION_STRING     "1.4.1"
#define FIRMWARE_BUILD_TYPE         "HeltecGateway"

//----------------------------------------------
// Build Timestamp
// Arduino IDE rebuilds all files, so these are always current
//----------------------------------------------
#define FIRMWARE_BUILD_DATE         __DATE__
#define FIRMWARE_BUILD_TIME         __TIME__
