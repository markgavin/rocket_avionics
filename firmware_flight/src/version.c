//----------------------------------------------
// Module: version.c
// Description: Firmware version definitions
// Author: Mark Gavin
// Created: 2026-01-17
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Note: This file is force-rebuilt on every build
//       to ensure BUILD_DATE and BUILD_TIME are current.
//----------------------------------------------

#include "version.h"

//----------------------------------------------
// Build Timestamp
// Automatically updated on each build
//----------------------------------------------
const char * const kBuildDate = __DATE__ ;
const char * const kBuildTime = __TIME__ ;
