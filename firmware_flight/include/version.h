//----------------------------------------------
// Module: version.h
// Description: Firmware version information
// Author: Mark Gavin
// Created: 2026-01-10
// Modified: 2026-01-17 (Added build date/time externs)
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#pragma once

//----------------------------------------------
// Version Components
//----------------------------------------------
#define FIRMWARE_NAME           "RocketAvionics"
#define FIRMWARE_VERSION_MAJOR  1
#define FIRMWARE_VERSION_MINOR  0
#define FIRMWARE_VERSION_PATCH  0
#define FIRMWARE_VERSION_STRING "1.0.0"

#define PROTOCOL_VERSION        "1.0"

//----------------------------------------------
// Build Timestamp (defined in version.c)
// These are updated on every build via force-rebuild
//----------------------------------------------
extern const char * const kBuildDate ;
extern const char * const kBuildTime ;
