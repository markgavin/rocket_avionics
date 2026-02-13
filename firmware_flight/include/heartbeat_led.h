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
// All colors at ~20% brightness (0x33 = 51/255)
//----------------------------------------------
#define kLedColorOff        0x00000000
#define kLedColorGreen      0x00330000  // Idle - slow pulse
#define kLedColorYellow     0x00333300  // Armed - medium blink
#define kLedColorRed        0x00003300  // Boost - fast blink
#define kLedColorOrange     0x001A3300  // Coast - steady
#define kLedColorMagenta    0x00003333  // Apogee - flash
#define kLedColorBlue       0x00000033  // Descent - medium blink
#define kLedColorCyan       0x00330033  // Landed - slow pulse
#define kLedColorWhite      0x00333333  // Complete - steady

// Dimmed versions for pulsing effect (~4% brightness)
#define kLedColorGreenDim   0x00060000
#define kLedColorYellowDim  0x00060600
#define kLedColorRedDim     0x00000600
#define kLedColorBlueDim    0x00000006

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
