//----------------------------------------------
// Module: ssd1306.c
// Description: SSD1306 OLED display driver (128x64)
// Author: Mark Gavin
// Created: 2025-12-19
// Modified: 2026-01-10 (Rocket Avionics)
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "ssd1306.h"
#include "pins.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include <string.h>
#include <stdlib.h>

//----------------------------------------------
// Module State
//----------------------------------------------
static uint8_t sFrameBuffer[SSD1306_BUFFER_SIZE] ;
static bool sInitialized = false ;

//----------------------------------------------
// 5x7 Font Data (ASCII 32-126)
//----------------------------------------------
static const uint8_t sFont5x7[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, // Space
  0x00, 0x00, 0x5F, 0x00, 0x00, // !
  0x00, 0x07, 0x00, 0x07, 0x00, // "
  0x14, 0x7F, 0x14, 0x7F, 0x14, // #
  0x24, 0x2A, 0x7F, 0x2A, 0x12, // $
  0x23, 0x13, 0x08, 0x64, 0x62, // %
  0x36, 0x49, 0x55, 0x22, 0x50, // &
  0x00, 0x05, 0x03, 0x00, 0x00, // '
  0x00, 0x1C, 0x22, 0x41, 0x00, // (
  0x00, 0x41, 0x22, 0x1C, 0x00, // )
  0x08, 0x2A, 0x1C, 0x2A, 0x08, // *
  0x08, 0x08, 0x3E, 0x08, 0x08, // +
  0x00, 0x50, 0x30, 0x00, 0x00, // ,
  0x08, 0x08, 0x08, 0x08, 0x08, // -
  0x00, 0x60, 0x60, 0x00, 0x00, // .
  0x20, 0x10, 0x08, 0x04, 0x02, // /
  0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
  0x00, 0x42, 0x7F, 0x40, 0x00, // 1
  0x42, 0x61, 0x51, 0x49, 0x46, // 2
  0x21, 0x41, 0x45, 0x4B, 0x31, // 3
  0x18, 0x14, 0x12, 0x7F, 0x10, // 4
  0x27, 0x45, 0x45, 0x45, 0x39, // 5
  0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
  0x01, 0x71, 0x09, 0x05, 0x03, // 7
  0x36, 0x49, 0x49, 0x49, 0x36, // 8
  0x06, 0x49, 0x49, 0x29, 0x1E, // 9
  0x00, 0x36, 0x36, 0x00, 0x00, // :
  0x00, 0x56, 0x36, 0x00, 0x00, // ;
  0x00, 0x08, 0x14, 0x22, 0x41, // <
  0x14, 0x14, 0x14, 0x14, 0x14, // =
  0x41, 0x22, 0x14, 0x08, 0x00, // >
  0x02, 0x01, 0x51, 0x09, 0x06, // ?
  0x32, 0x49, 0x79, 0x41, 0x3E, // @
  0x7E, 0x11, 0x11, 0x11, 0x7E, // A
  0x7F, 0x49, 0x49, 0x49, 0x36, // B
  0x3E, 0x41, 0x41, 0x41, 0x22, // C
  0x7F, 0x41, 0x41, 0x22, 0x1C, // D
  0x7F, 0x49, 0x49, 0x49, 0x41, // E
  0x7F, 0x09, 0x09, 0x01, 0x01, // F
  0x3E, 0x41, 0x41, 0x51, 0x32, // G
  0x7F, 0x08, 0x08, 0x08, 0x7F, // H
  0x00, 0x41, 0x7F, 0x41, 0x00, // I
  0x20, 0x40, 0x41, 0x3F, 0x01, // J
  0x7F, 0x08, 0x14, 0x22, 0x41, // K
  0x7F, 0x40, 0x40, 0x40, 0x40, // L
  0x7F, 0x02, 0x04, 0x02, 0x7F, // M
  0x7F, 0x04, 0x08, 0x10, 0x7F, // N
  0x3E, 0x41, 0x41, 0x41, 0x3E, // O
  0x7F, 0x09, 0x09, 0x09, 0x06, // P
  0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
  0x7F, 0x09, 0x19, 0x29, 0x46, // R
  0x46, 0x49, 0x49, 0x49, 0x31, // S
  0x01, 0x01, 0x7F, 0x01, 0x01, // T
  0x3F, 0x40, 0x40, 0x40, 0x3F, // U
  0x1F, 0x20, 0x40, 0x20, 0x1F, // V
  0x7F, 0x20, 0x18, 0x20, 0x7F, // W
  0x63, 0x14, 0x08, 0x14, 0x63, // X
  0x03, 0x04, 0x78, 0x04, 0x03, // Y
  0x61, 0x51, 0x49, 0x45, 0x43, // Z
  0x00, 0x00, 0x7F, 0x41, 0x41, // [
  0x02, 0x04, 0x08, 0x10, 0x20, // backslash
  0x41, 0x41, 0x7F, 0x00, 0x00, // ]
  0x04, 0x02, 0x01, 0x02, 0x04, // ^
  0x40, 0x40, 0x40, 0x40, 0x40, // _
  0x00, 0x01, 0x02, 0x04, 0x00, // `
  0x20, 0x54, 0x54, 0x54, 0x78, // a
  0x7F, 0x48, 0x44, 0x44, 0x38, // b
  0x38, 0x44, 0x44, 0x44, 0x20, // c
  0x38, 0x44, 0x44, 0x48, 0x7F, // d
  0x38, 0x54, 0x54, 0x54, 0x18, // e
  0x08, 0x7E, 0x09, 0x01, 0x02, // f
  0x08, 0x14, 0x54, 0x54, 0x3C, // g
  0x7F, 0x08, 0x04, 0x04, 0x78, // h
  0x00, 0x44, 0x7D, 0x40, 0x00, // i
  0x20, 0x40, 0x44, 0x3D, 0x00, // j
  0x00, 0x7F, 0x10, 0x28, 0x44, // k
  0x00, 0x41, 0x7F, 0x40, 0x00, // l
  0x7C, 0x04, 0x18, 0x04, 0x78, // m
  0x7C, 0x08, 0x04, 0x04, 0x78, // n
  0x38, 0x44, 0x44, 0x44, 0x38, // o
  0x7C, 0x14, 0x14, 0x14, 0x08, // p
  0x08, 0x14, 0x14, 0x18, 0x7C, // q
  0x7C, 0x08, 0x04, 0x04, 0x08, // r
  0x48, 0x54, 0x54, 0x54, 0x20, // s
  0x04, 0x3F, 0x44, 0x40, 0x20, // t
  0x3C, 0x40, 0x40, 0x20, 0x7C, // u
  0x1C, 0x20, 0x40, 0x20, 0x1C, // v
  0x3C, 0x40, 0x30, 0x40, 0x3C, // w
  0x44, 0x28, 0x10, 0x28, 0x44, // x
  0x0C, 0x50, 0x50, 0x50, 0x3C, // y
  0x44, 0x64, 0x54, 0x4C, 0x44, // z
  0x00, 0x08, 0x36, 0x41, 0x00, // {
  0x00, 0x00, 0x7F, 0x00, 0x00, // |
  0x00, 0x41, 0x36, 0x08, 0x00, // }
  0x08, 0x08, 0x2A, 0x1C, 0x08, // ~
} ;

//----------------------------------------------
// Internal: Send Command
//----------------------------------------------
static void SendCommand(uint8_t inCmd)
{
  uint8_t theData[2] = { 0x00, inCmd } ;
  i2c_write_blocking(kI2cPort, SSD1306_I2C_ADDR, theData, 2, false) ;
}

//----------------------------------------------
// Internal: Send Data
//----------------------------------------------
static void SendData(const uint8_t * inData, size_t inLen)
{
  // Use larger chunks for more efficient transfer
  // 128 bytes = one full row of display
  uint8_t theBuffer[129] ;
  theBuffer[0] = 0x40 ; // Data mode (Co=0, D/C#=1)

  while (inLen > 0)
  {
    size_t theChunk = (inLen > 128) ? 128 : inLen ;
    memcpy(&theBuffer[1], inData, theChunk) ;
    i2c_write_blocking(kI2cPort, SSD1306_I2C_ADDR, theBuffer, theChunk + 1, false) ;
    inData += theChunk ;
    inLen -= theChunk ;
  }
}

//----------------------------------------------
// Function: SSD1306_Init
// Note: The Adafruit 128x64 OLED FeatherWing uses SH1107, not SSD1306!
//----------------------------------------------
bool SSD1306_Init(void)
{
  // Explicitly clear framebuffer first
  memset(sFrameBuffer, 0, SSD1306_BUFFER_SIZE) ;

  // Initialize I2C
  i2c_init(kI2cPort, kI2cBaudrate) ;
  gpio_set_function(kPinI2cSda, GPIO_FUNC_I2C) ;
  gpio_set_function(kPinI2cScl, GPIO_FUNC_I2C) ;
  gpio_pull_up(kPinI2cSda) ;
  gpio_pull_up(kPinI2cScl) ;

  sleep_ms(100) ; // Wait for display to power up

  // SH1107 initialization sequence (for Adafruit 128x64 OLED FeatherWing)
  SendCommand(0xAE) ;  // Display off
  SendCommand(0xDC) ;  // Set display start line
  SendCommand(0x00) ;  // Start line = 0
  SendCommand(0x81) ;  // Set contrast
  SendCommand(0x2F) ;  // Contrast value
  SendCommand(0x20) ;  // Set memory addressing mode
  SendCommand(0xA0) ;  // Segment remap (column 0 = SEG0)
  SendCommand(0xC0) ;  // COM scan direction (COM0 to COM[N-1])
  SendCommand(0xA8) ;  // Set multiplex ratio
  SendCommand(0x7F) ;  // MUX = 128
  SendCommand(0xD3) ;  // Set display offset
  SendCommand(0x60) ;  // Offset = 96 (0x60) for 128x64 FeatherWing
  SendCommand(0xD5) ;  // Set display clock
  SendCommand(0x51) ;  // Clock divider
  SendCommand(0xD9) ;  // Set precharge period
  SendCommand(0x22) ;  // Precharge value
  SendCommand(0xDB) ;  // Set VCOM deselect level
  SendCommand(0x35) ;  // VCOM value
  SendCommand(0xB0) ;  // Set page address to 0
  SendCommand(0xA4) ;  // Entire display ON (show RAM)
  SendCommand(0xA6) ;  // Normal display (not inverted)

  // Mark as initialized BEFORE clearing so Update() works
  sInitialized = true ;

  // Clear display
  SSD1306_Clear() ;
  SSD1306_Update() ;

  // Small delay to ensure display processes the data
  sleep_ms(10) ;

  // Turn on display
  SendCommand(0xAF) ;  // Display on

  return true ;
}

//----------------------------------------------
// Function: SSD1306_Clear
//----------------------------------------------
void SSD1306_Clear(void)
{
  memset(sFrameBuffer, 0, SSD1306_BUFFER_SIZE) ;
}

//----------------------------------------------
// Function: SSD1306_Update
// Note: SH1107 FeatherWing is physically 64x128
//       16 pages of 8 rows, 64 columns each
//----------------------------------------------
void SSD1306_Update(void)
{
  if (!sInitialized) return ;

  // SH1107 physical layout: 16 pages, 64 columns each
  for (uint8_t thePage = 0 ; thePage < 16 ; thePage++)
  {
    // Set page address (0xB0 + page)
    SendCommand(0xB0 + thePage) ;
    // Set column address to 0
    SendCommand(0x00) ;  // Lower column bits
    SendCommand(0x10) ;  // Upper column bits

    // Send one page (64 bytes) in chunks
    uint8_t theBuffer[33] ;
    const uint8_t * thePtr = &sFrameBuffer[thePage * 64] ;

    for (int i = 0 ; i < 64 ; i += 32)
    {
      theBuffer[0] = 0x40 ;  // Data mode
      memcpy(&theBuffer[1], thePtr + i, 32) ;
      i2c_write_blocking(kI2cPort, SSD1306_I2C_ADDR, theBuffer, 33, false) ;
    }
  }
}

//----------------------------------------------
// Function: SSD1306_SetPixel
// Note: SH1107 FeatherWing is 64x128 mounted sideways
//       We rotate 90 degrees clockwise to get 128x64
//----------------------------------------------
void SSD1306_SetPixel(int16_t inX, int16_t inY, bool inOn)
{
  // Bounds check for 128x64 logical display
  if (inX < 0 || inX >= SSD1306_WIDTH || inY < 0 || inY >= SSD1306_HEIGHT)
  {
    return ;
  }

  // Rotate 90 degrees clockwise for SH1107 FeatherWing
  // Physical display is 64 wide x 128 tall
  // Logical (128, 64) -> Physical (64 - 1 - y, x)
  int16_t thePhysX = (SSD1306_HEIGHT - 1) - inY ;  // 63 - y
  int16_t thePhysY = inX ;

  // Physical layout: 16 pages of 8 rows, 64 columns
  uint16_t theIndex = thePhysX + (thePhysY / 8) * 64 ;
  uint8_t theBit = 1 << (thePhysY % 8) ;

  if (inOn)
  {
    sFrameBuffer[theIndex] |= theBit ;
  }
  else
  {
    sFrameBuffer[theIndex] &= ~theBit ;
  }
}

//----------------------------------------------
// Function: SSD1306_DrawLine
//----------------------------------------------
void SSD1306_DrawLine(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, bool inOn)
{
  int16_t theDx = abs(inX1 - inX0) ;
  int16_t theSx = (inX0 < inX1) ? 1 : -1 ;
  int16_t theDy = -abs(inY1 - inY0) ;
  int16_t theSy = (inY0 < inY1) ? 1 : -1 ;
  int16_t theErr = theDx + theDy ;

  while (1)
  {
    SSD1306_SetPixel(inX0, inY0, inOn) ;
    if (inX0 == inX1 && inY0 == inY1) break ;

    int16_t theE2 = 2 * theErr ;
    if (theE2 >= theDy)
    {
      theErr += theDy ;
      inX0 += theSx ;
    }
    if (theE2 <= theDx)
    {
      theErr += theDx ;
      inY0 += theSy ;
    }
  }
}

//----------------------------------------------
// Function: SSD1306_DrawRect
//----------------------------------------------
void SSD1306_DrawRect(int16_t inX, int16_t inY, int16_t inW, int16_t inH, bool inOn)
{
  SSD1306_DrawLine(inX, inY, inX + inW - 1, inY, inOn) ;
  SSD1306_DrawLine(inX + inW - 1, inY, inX + inW - 1, inY + inH - 1, inOn) ;
  SSD1306_DrawLine(inX + inW - 1, inY + inH - 1, inX, inY + inH - 1, inOn) ;
  SSD1306_DrawLine(inX, inY + inH - 1, inX, inY, inOn) ;
}

//----------------------------------------------
// Function: SSD1306_FillRect
//----------------------------------------------
void SSD1306_FillRect(int16_t inX, int16_t inY, int16_t inW, int16_t inH, bool inOn)
{
  for (int16_t i = 0 ; i < inW ; i++)
  {
    for (int16_t j = 0 ; j < inH ; j++)
    {
      SSD1306_SetPixel(inX + i, inY + j, inOn) ;
    }
  }
}

//----------------------------------------------
// Function: SSD1306_DrawChar
//----------------------------------------------
int16_t SSD1306_DrawChar(int16_t inX, int16_t inY, char inChar, uint8_t inSize)
{
  if (inChar < 32 || inChar > 126)
  {
    inChar = '?' ;
  }

  const uint8_t * theGlyph = &sFont5x7[(inChar - 32) * 5] ;

  for (int16_t col = 0 ; col < 5 ; col++)
  {
    uint8_t theData = theGlyph[col] ;
    for (int16_t row = 0 ; row < 7 ; row++)
    {
      bool thePixel = (theData >> row) & 0x01 ;
      if (inSize == 1)
      {
        SSD1306_SetPixel(inX + col, inY + row, thePixel) ;
      }
      else
      {
        SSD1306_FillRect(inX + col * inSize, inY + row * inSize, inSize, inSize, thePixel) ;
      }
    }
  }

  return 6 * inSize ; // 5 pixels + 1 space
}

//----------------------------------------------
// Function: SSD1306_DrawString
//----------------------------------------------
void SSD1306_DrawString(int16_t inX, int16_t inY, const char * inStr, uint8_t inSize)
{
  while (*inStr)
  {
    inX += SSD1306_DrawChar(inX, inY, *inStr, inSize) ;
    inStr++ ;
  }
}

//----------------------------------------------
// Function: SSD1306_DrawStringCentered
//----------------------------------------------
void SSD1306_DrawStringCentered(int16_t inY, const char * inStr, uint8_t inSize)
{
  int16_t theLen = strlen(inStr) ;
  int16_t theWidth = theLen * 6 * inSize ;
  int16_t theX = (SSD1306_WIDTH - theWidth) / 2 ;
  SSD1306_DrawString(theX, inY, inStr, inSize) ;
}

//----------------------------------------------
// Function: SSD1306_SetContrast
//----------------------------------------------
void SSD1306_SetContrast(uint8_t inContrast)
{
  SendCommand(SSD1306_CMD_SET_CONTRAST) ;
  SendCommand(inContrast) ;
}

//----------------------------------------------
// Function: SSD1306_Invert
//----------------------------------------------
void SSD1306_Invert(bool inInvert)
{
  SendCommand(inInvert ? SSD1306_CMD_INVERT_DISPLAY : SSD1306_CMD_NORMAL_DISPLAY) ;
}

//----------------------------------------------
// Function: SSD1306_DisplayOn
//----------------------------------------------
void SSD1306_DisplayOn(bool inOn)
{
  SendCommand(inOn ? SSD1306_CMD_DISPLAY_ON : SSD1306_CMD_DISPLAY_OFF) ;
}
