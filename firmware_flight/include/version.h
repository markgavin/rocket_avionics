//----------------------------------------------
// Module: version.h
// Description: Firmware version information
// Author: Mark Gavin
// Created: 2026-01-10
// Modified: 2026-01-31 (Switched to auto-incrementing build number)
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#pragma once

#include "build_number.h"

//----------------------------------------------
// Firmware Identification
//----------------------------------------------
#define FIRMWARE_NAME           "RocketAvionics"
#define FIRMWARE_TYPE           "Flight"

// Build number is auto-incremented on each build
// Defined in build_number.h (auto-generated)
#define FIRMWARE_VERSION_STRING "Build " FIRMWARE_BUILD_STRING

#define PROTOCOL_VERSION        "2.0"

//----------------------------------------------
// Build Timestamp (defined in version.c)
// These are updated on every build via force-rebuild
//----------------------------------------------
extern const char * const kBuildDate ;
extern const char * const kBuildTime ;
