//----------------------------------------------
// Module: ssd1306.h
// Description: SSD1306 OLED display driver (128x64)
// Author: Mark Gavin
// Created: 2025-12-19
// Modified: 2026-01-10 (Rocket Avionics)
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit 128x64 OLED FeatherWing
//   - I2C interface at 0x3C (or 0x3D)
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// Display Dimensions
//----------------------------------------------
#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64
#define SSD1306_BUFFER_SIZE     (SSD1306_WIDTH * SSD1306_HEIGHT / 8)

//----------------------------------------------
// I2C Address
//----------------------------------------------
#define SSD1306_I2C_ADDR        0x3C

//----------------------------------------------
// SSD1306 Commands
//----------------------------------------------
#define SSD1306_CMD_DISPLAY_OFF         0xAE
#define SSD1306_CMD_DISPLAY_ON          0xAF
#define SSD1306_CMD_SET_CONTRAST        0x81
#define SSD1306_CMD_ENTIRE_DISPLAY_RAM  0xA4
#define SSD1306_CMD_ENTIRE_DISPLAY_ON   0xA5
#define SSD1306_CMD_NORMAL_DISPLAY      0xA6
#define SSD1306_CMD_INVERT_DISPLAY      0xA7
#define SSD1306_CMD_SET_MUX_RATIO       0xA8
#define SSD1306_CMD_SET_DISPLAY_OFFSET  0xD3
#define SSD1306_CMD_SET_START_LINE      0x40
#define SSD1306_CMD_SET_SEG_REMAP       0xA1
#define SSD1306_CMD_SET_COM_SCAN_DEC    0xC8
#define SSD1306_CMD_SET_COM_PINS        0xDA
#define SSD1306_CMD_SET_PRECHARGE       0xD9
#define SSD1306_CMD_SET_VCOM_DETECT     0xDB
#define SSD1306_CMD_SET_CLOCK_DIV       0xD5
#define SSD1306_CMD_CHARGE_PUMP         0x8D
#define SSD1306_CMD_MEMORY_MODE         0x20
#define SSD1306_CMD_COLUMN_ADDR         0x21
#define SSD1306_CMD_PAGE_ADDR           0x22

//----------------------------------------------
// Function: SSD1306_Init
// Purpose: Initialize the OLED display
// Returns: true if successful
//----------------------------------------------
bool SSD1306_Init(void) ;

//----------------------------------------------
// Function: SSD1306_Clear
// Purpose: Clear the display buffer
//----------------------------------------------
void SSD1306_Clear(void) ;

//----------------------------------------------
// Function: SSD1306_Update
// Purpose: Send buffer to display
//----------------------------------------------
void SSD1306_Update(void) ;

//----------------------------------------------
// Function: SSD1306_SetPixel
// Purpose: Set a single pixel
// Parameters:
//   inX - X coordinate (0-127)
//   inY - Y coordinate (0-63)
//   inOn - true for white, false for black
//----------------------------------------------
void SSD1306_SetPixel(int16_t inX, int16_t inY, bool inOn) ;

//----------------------------------------------
// Function: SSD1306_DrawLine
// Purpose: Draw a line
// Parameters:
//   inX0, inY0 - Start point
//   inX1, inY1 - End point
//   inOn - true for white, false for black
//----------------------------------------------
void SSD1306_DrawLine(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, bool inOn) ;

//----------------------------------------------
// Function: SSD1306_DrawRect
// Purpose: Draw a rectangle outline
//----------------------------------------------
void SSD1306_DrawRect(int16_t inX, int16_t inY, int16_t inW, int16_t inH, bool inOn) ;

//----------------------------------------------
// Function: SSD1306_FillRect
// Purpose: Draw a filled rectangle
//----------------------------------------------
void SSD1306_FillRect(int16_t inX, int16_t inY, int16_t inW, int16_t inH, bool inOn) ;

//----------------------------------------------
// Function: SSD1306_DrawChar
// Purpose: Draw a single character
// Parameters:
//   inX, inY - Top-left position
//   inChar - Character to draw
//   inSize - Size multiplier (1, 2, etc.)
// Returns: Width of character drawn
//----------------------------------------------
int16_t SSD1306_DrawChar(int16_t inX, int16_t inY, char inChar, uint8_t inSize) ;

//----------------------------------------------
// Function: SSD1306_DrawString
// Purpose: Draw a string
// Parameters:
//   inX, inY - Top-left position
//   inStr - String to draw
//   inSize - Size multiplier
//----------------------------------------------
void SSD1306_DrawString(int16_t inX, int16_t inY, const char * inStr, uint8_t inSize) ;

//----------------------------------------------
// Function: SSD1306_DrawStringCentered
// Purpose: Draw a centered string
// Parameters:
//   inY - Y position
//   inStr - String to draw
//   inSize - Size multiplier
//----------------------------------------------
void SSD1306_DrawStringCentered(int16_t inY, const char * inStr, uint8_t inSize) ;

//----------------------------------------------
// Function: SSD1306_SetContrast
// Purpose: Set display contrast
// Parameters:
//   inContrast - 0-255
//----------------------------------------------
void SSD1306_SetContrast(uint8_t inContrast) ;

//----------------------------------------------
// Function: SSD1306_Invert
// Purpose: Invert display colors
// Parameters:
//   inInvert - true to invert
//----------------------------------------------
void SSD1306_Invert(bool inInvert) ;

//----------------------------------------------
// Function: SSD1306_DisplayOn
// Purpose: Turn display on/off
//----------------------------------------------
void SSD1306_DisplayOn(bool inOn) ;
