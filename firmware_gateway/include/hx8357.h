//----------------------------------------------
// Module: hx8357.h
// Description: HX8357D TFT Display Driver
//   for Adafruit 3.5" TFT FeatherWing (480x320)
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit TFT FeatherWing 3.5" 480x320 (5872)
//   - HX8357D controller
//   - SPI interface (shared with LoRa)
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// Display Dimensions
//----------------------------------------------
#define HX8357_WIDTH        480
#define HX8357_HEIGHT       320

//----------------------------------------------
// Color Definitions (RGB565)
//----------------------------------------------
#define HX8357_BLACK        0x0000
#define HX8357_WHITE        0xFFFF
#define HX8357_RED          0xF800
#define HX8357_GREEN        0x07E0
#define HX8357_BLUE         0x001F
#define HX8357_CYAN         0x07FF
#define HX8357_MAGENTA      0xF81F
#define HX8357_YELLOW       0xFFE0
#define HX8357_ORANGE       0xFC00
#define HX8357_GRAY         0x8410
#define HX8357_DARKGRAY     0x4208
#define HX8357_LIGHTGRAY    0xC618

// Custom colors for UI
#define HX8357_DARKGREEN    0x03E0
#define HX8357_DARKBLUE     0x0010
#define HX8357_NAVY         0x000F
#define HX8357_DARKCYAN     0x03EF
#define HX8357_MAROON       0x7800
#define HX8357_PURPLE       0x780F
#define HX8357_OLIVE        0x7BE0
#define HX8357_PINK         0xF81F

//----------------------------------------------
// HX8357D Commands
//----------------------------------------------
#define HX8357_NOP          0x00
#define HX8357_SWRESET      0x01
#define HX8357_RDDID        0x04
#define HX8357_RDDST        0x09
#define HX8357_SLPIN        0x10
#define HX8357_SLPOUT       0x11
#define HX8357_INVOFF       0x20
#define HX8357_INVON        0x21
#define HX8357_DISPOFF      0x28
#define HX8357_DISPON       0x29
#define HX8357_CASET        0x2A
#define HX8357_PASET        0x2B
#define HX8357_RAMWR        0x2C
#define HX8357_RAMRD        0x2E
#define HX8357_TEON         0x35
#define HX8357_MADCTL       0x36
#define HX8357_COLMOD       0x3A
#define HX8357_SETOSC       0xB0
#define HX8357_SETPWR1      0xB1
#define HX8357_SETRGB       0xB3
#define HX8357_SETCYC       0xB4
#define HX8357_SETCOM       0xB6
#define HX8357_SETC         0xB9
#define HX8357_SETSTBA      0xC0
#define HX8357_SETPANEL     0xCC
#define HX8357_SETGAMMA     0xE0

// MADCTL bits
#define MADCTL_MY           0x80
#define MADCTL_MX           0x40
#define MADCTL_MV           0x20
#define MADCTL_ML           0x10
#define MADCTL_RGB          0x00
#define MADCTL_BGR          0x08
#define MADCTL_MH           0x04

//----------------------------------------------
// Function Prototypes
//----------------------------------------------

//----------------------------------------------
// Function: HX8357_Init
// Purpose: Initialize the HX8357D display
// Returns: true if successful
//----------------------------------------------
bool HX8357_Init(void) ;

//----------------------------------------------
// Function: HX8357_SetRotation
// Purpose: Set display rotation
// Parameters:
//   inRotation - 0=Portrait, 1=Landscape, 2=Portrait inverted, 3=Landscape inverted
//----------------------------------------------
void HX8357_SetRotation(uint8_t inRotation) ;

//----------------------------------------------
// Function: HX8357_FillScreen
// Purpose: Fill entire screen with color
// Parameters:
//   inColor - RGB565 color
//----------------------------------------------
void HX8357_FillScreen(uint16_t inColor) ;

//----------------------------------------------
// Function: HX8357_FillRect
// Purpose: Fill a rectangle with color
// Parameters:
//   inX, inY - Top-left corner
//   inWidth, inHeight - Rectangle dimensions
//   inColor - RGB565 color
//----------------------------------------------
void HX8357_FillRect(int16_t inX, int16_t inY, int16_t inWidth, int16_t inHeight, uint16_t inColor) ;

//----------------------------------------------
// Function: HX8357_DrawRect
// Purpose: Draw a rectangle outline
// Parameters:
//   inX, inY - Top-left corner
//   inWidth, inHeight - Rectangle dimensions
//   inColor - RGB565 color
//----------------------------------------------
void HX8357_DrawRect(int16_t inX, int16_t inY, int16_t inWidth, int16_t inHeight, uint16_t inColor) ;

//----------------------------------------------
// Function: HX8357_DrawPixel
// Purpose: Draw a single pixel
// Parameters:
//   inX, inY - Pixel coordinates
//   inColor - RGB565 color
//----------------------------------------------
void HX8357_DrawPixel(int16_t inX, int16_t inY, uint16_t inColor) ;

//----------------------------------------------
// Function: HX8357_DrawHLine
// Purpose: Draw a horizontal line
// Parameters:
//   inX, inY - Start position
//   inWidth - Line length
//   inColor - RGB565 color
//----------------------------------------------
void HX8357_DrawHLine(int16_t inX, int16_t inY, int16_t inWidth, uint16_t inColor) ;

//----------------------------------------------
// Function: HX8357_DrawVLine
// Purpose: Draw a vertical line
// Parameters:
//   inX, inY - Start position
//   inHeight - Line length
//   inColor - RGB565 color
//----------------------------------------------
void HX8357_DrawVLine(int16_t inX, int16_t inY, int16_t inHeight, uint16_t inColor) ;

//----------------------------------------------
// Function: HX8357_DrawLine
// Purpose: Draw a line between two points
// Parameters:
//   inX0, inY0 - Start point
//   inX1, inY1 - End point
//   inColor - RGB565 color
//----------------------------------------------
void HX8357_DrawLine(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, uint16_t inColor) ;

//----------------------------------------------
// Function: HX8357_DrawChar
// Purpose: Draw a single character
// Parameters:
//   inX, inY - Character position (top-left)
//   inChar - Character to draw
//   inFgColor - Foreground color
//   inBgColor - Background color
//   inSize - Character scale (1=6x8, 2=12x16, etc.)
//----------------------------------------------
void HX8357_DrawChar(int16_t inX, int16_t inY, char inChar, uint16_t inFgColor, uint16_t inBgColor, uint8_t inSize) ;

//----------------------------------------------
// Function: HX8357_DrawString
// Purpose: Draw a string of characters
// Parameters:
//   inX, inY - String position (top-left)
//   inStr - Null-terminated string
//   inFgColor - Foreground color
//   inBgColor - Background color
//   inSize - Character scale
//----------------------------------------------
void HX8357_DrawString(int16_t inX, int16_t inY, const char * inStr, uint16_t inFgColor, uint16_t inBgColor, uint8_t inSize) ;

//----------------------------------------------
// Function: HX8357_DrawStringCentered
// Purpose: Draw a string centered horizontally
// Parameters:
//   inY - Vertical position
//   inStr - Null-terminated string
//   inFgColor - Foreground color
//   inBgColor - Background color
//   inSize - Character scale
//----------------------------------------------
void HX8357_DrawStringCentered(int16_t inY, const char * inStr, uint16_t inFgColor, uint16_t inBgColor, uint8_t inSize) ;

//----------------------------------------------
// Function: HX8357_SetBacklight
// Purpose: Control display backlight
// Parameters:
//   inOn - true to turn on backlight
//----------------------------------------------
void HX8357_SetBacklight(bool inOn) ;

//----------------------------------------------
// Function: HX8357_RGB
// Purpose: Convert RGB888 to RGB565
// Parameters:
//   inR, inG, inB - 8-bit color components
// Returns: RGB565 color
//----------------------------------------------
uint16_t HX8357_RGB(uint8_t inR, uint8_t inG, uint8_t inB) ;

//----------------------------------------------
// Function: HX8357_GetWidth
// Purpose: Get current display width (depends on rotation)
// Returns: Display width in pixels
//----------------------------------------------
int16_t HX8357_GetWidth(void) ;

//----------------------------------------------
// Function: HX8357_GetHeight
// Purpose: Get current display height (depends on rotation)
// Returns: Display height in pixels
//----------------------------------------------
int16_t HX8357_GetHeight(void) ;
