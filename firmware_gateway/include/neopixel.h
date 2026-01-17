//----------------------------------------------
// Module: neopixel.h
// Description: NeoPixel (WS2812) driver for RP2040
// Author: Mark Gavin
// Created: 2026-01-14
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// Predefined Colors (GRB format)
//----------------------------------------------
#define NEOPIXEL_OFF        0x000000
#define NEOPIXEL_RED        0x00FF00
#define NEOPIXEL_GREEN      0xFF0000
#define NEOPIXEL_BLUE       0x0000FF
#define NEOPIXEL_YELLOW     0xFFFF00
#define NEOPIXEL_CYAN       0xFF00FF
#define NEOPIXEL_MAGENTA    0x00FFFF
#define NEOPIXEL_WHITE      0xFFFFFF
#define NEOPIXEL_ORANGE     0x80FF00
#define NEOPIXEL_PURPLE     0x0080FF

// Dim versions (very low brightness for status indication)
#define NEOPIXEL_DIM_RED    0x000800
#define NEOPIXEL_DIM_GREEN  0x080000
#define NEOPIXEL_DIM_BLUE   0x000008
#define NEOPIXEL_DIM_YELLOW 0x080800
#define NEOPIXEL_DIM_CYAN   0x080008
#define NEOPIXEL_DIM_WHITE  0x080808
#define NEOPIXEL_DIM_ORANGE 0x040800

//----------------------------------------------
// Public Functions
//----------------------------------------------

// Initialize NeoPixel on specified GPIO pin
bool NeoPixel_Init(uint8_t inPin) ;

// Set color (32-bit GRB value)
void NeoPixel_SetColor(uint32_t inColor) ;

// Set RGB values separately
void NeoPixel_SetRGB(uint8_t inRed, uint8_t inGreen, uint8_t inBlue) ;

// Turn off
void NeoPixel_Off(void) ;

// Helper to create color from RGB with brightness (0-255)
uint32_t NeoPixel_RGB(uint8_t inRed, uint8_t inGreen, uint8_t inBlue, uint8_t inBrightness) ;
