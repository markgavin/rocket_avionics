//----------------------------------------------
// Module: heartbeat_led.h
// Description: NeoPixel RGB LED heartbeat indicator
//   (Flight Avionics version)
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Built-in NeoPixel on Feather RP2040 (GPIO16)
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "flight_control.h"

//----------------------------------------------
// Heartbeat LED Colors (GRB format for WS2812)
// Format: 0x00GGRRBB
// All colors at ~5% brightness (0x0D = 13/255)
//----------------------------------------------
#define kLedColorOff        0x00000000
#define kLedColorGreen      0x000D0000  // Idle - slow pulse
#define kLedColorYellow     0x000D0D00  // Armed - medium blink
#define kLedColorRed        0x00000D00  // Boost - fast blink
#define kLedColorOrange     0x00050D00  // Coast - steady
#define kLedColorMagenta    0x00000D0D  // Apogee - flash
#define kLedColorBlue       0x0000000D  // Descent - medium blink
#define kLedColorCyan       0x000D000D  // Landed - slow pulse
#define kLedColorWhite      0x000D0D0D  // Complete - steady

// Dimmed versions for pulsing effect (~1% brightness)
#define kLedColorGreenDim   0x00030000
#define kLedColorYellowDim  0x00030300
#define kLedColorRedDim     0x00000300
#define kLedColorBlueDim    0x00000003

//----------------------------------------------
// Function: HeartbeatLED_Init
// Purpose: Initialize the NeoPixel LED
// Parameters:
//   inPin - GPIO pin number for NeoPixel data
// Returns: true if successful
//----------------------------------------------
bool HeartbeatLED_Init(uint8_t inPin) ;

//----------------------------------------------
// Function: HeartbeatLED_Update
// Purpose: Update LED based on current flight state
// Parameters:
//   inState - current flight state
//   inCurrentTimeMs - current system time in ms
// Notes: Call this frequently in main loop
//----------------------------------------------
void HeartbeatLED_Update(FlightState inState, uint32_t inCurrentTimeMs) ;

//----------------------------------------------
// Function: HeartbeatLED_SetColor
// Purpose: Set LED to specific color
// Parameters:
//   inColor - 32-bit GRB color value
//----------------------------------------------
void HeartbeatLED_SetColor(uint32_t inColor) ;

//----------------------------------------------
// Function: HeartbeatLED_Off
// Purpose: Turn off the LED
//----------------------------------------------
void HeartbeatLED_Off(void) ;

//----------------------------------------------
// Function: HeartbeatLED_Flash
// Purpose: Flash the LED a specific color
// Parameters:
//   inColor - Color to flash
//   inDurationMs - Flash duration
//----------------------------------------------
void HeartbeatLED_Flash(uint32_t inColor, uint32_t inDurationMs) ;
