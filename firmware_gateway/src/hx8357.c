//----------------------------------------------
// Module: hx8357.c
// Description: HX8357D TFT Display Driver
//   for Adafruit 3.5" TFT FeatherWing (480x320)
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "hx8357.h"
#include "pins.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

//----------------------------------------------
// Module Constants
//----------------------------------------------
#define SPI_TFT_SPEED       40000000  // 40 MHz for TFT (faster than LoRa)

//----------------------------------------------
// 5x7 Font Data (ASCII 32-127)
//----------------------------------------------
static const uint8_t sFont5x7[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, // (space)
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
  0x08, 0x08, 0x2A, 0x1C, 0x08, // ->
  0x08, 0x1C, 0x2A, 0x08, 0x08  // <-
} ;

//----------------------------------------------
// Module State
//----------------------------------------------
static uint8_t sRotation = 1 ;  // Default to landscape
static int16_t sWidth = HX8357_WIDTH ;
static int16_t sHeight = HX8357_HEIGHT ;

//----------------------------------------------
// Local Function Prototypes
//----------------------------------------------
static void WriteCommand(uint8_t inCmd) ;
static void WriteData(uint8_t inData) ;
static void WriteData16(uint16_t inData) ;
static void WriteCommandWithData(uint8_t inCmd, const uint8_t * inData, size_t inLen) ;
static void SetAddrWindow(int16_t inX, int16_t inY, int16_t inW, int16_t inH) ;
static void SpiBegin(void) ;
static void SpiEnd(void) ;

//----------------------------------------------
// Function: HX8357_Init
//----------------------------------------------
bool HX8357_Init(void)
{
  printf("HX8357: Initializing TFT display...\n") ;

  // Note: CS and DC pins already initialized in main.c InitializeSPI()
  // Just ensure they're in the correct state
  gpio_put(kPinTftCs, 1) ;  // Deselect TFT
  gpio_put(kPinTftDc, 1) ;  // Data mode

  // Hardware reset (only if reset pin is defined)
  #if kPinTftRst >= 0
  gpio_init(kPinTftRst) ;
  gpio_set_dir(kPinTftRst, GPIO_OUT) ;
  gpio_put(kPinTftRst, 1) ;
  sleep_ms(10) ;
  gpio_put(kPinTftRst, 0) ;
  sleep_ms(10) ;
  gpio_put(kPinTftRst, 1) ;
  sleep_ms(120) ;
  #endif

  // Software reset (always do this)
  WriteCommand(HX8357_SWRESET) ;
  sleep_ms(150) ;  // Wait for reset to complete

  // Enable extended commands
  WriteCommand(HX8357_SETC) ;
  WriteData(0xFF) ;
  WriteData(0x83) ;
  WriteData(0x57) ;
  sleep_ms(300) ;

  // Set power
  WriteCommand(HX8357_SETRGB) ;
  WriteData(0x80) ;
  WriteData(0x00) ;
  WriteData(0x06) ;
  WriteData(0x06) ;

  WriteCommand(HX8357_SETCOM) ;
  WriteData(0x25) ;

  WriteCommand(HX8357_SETOSC) ;
  WriteData(0x68) ;

  WriteCommand(HX8357_SETPANEL) ;
  WriteData(0x05) ;

  WriteCommand(HX8357_SETPWR1) ;
  WriteData(0x00) ;
  WriteData(0x15) ;
  WriteData(0x1C) ;
  WriteData(0x1C) ;
  WriteData(0x83) ;
  WriteData(0xAA) ;

  WriteCommand(HX8357_SETSTBA) ;
  WriteData(0x50) ;
  WriteData(0x50) ;
  WriteData(0x01) ;
  WriteData(0x3C) ;
  WriteData(0x1E) ;
  WriteData(0x08) ;

  WriteCommand(HX8357_SETCYC) ;
  WriteData(0x02) ;
  WriteData(0x40) ;
  WriteData(0x00) ;
  WriteData(0x2A) ;
  WriteData(0x2A) ;
  WriteData(0x0D) ;
  WriteData(0x78) ;

  // Set gamma
  WriteCommand(HX8357_SETGAMMA) ;
  WriteData(0x02) ;
  WriteData(0x0A) ;
  WriteData(0x11) ;
  WriteData(0x1D) ;
  WriteData(0x23) ;
  WriteData(0x35) ;
  WriteData(0x41) ;
  WriteData(0x4B) ;
  WriteData(0x4B) ;
  WriteData(0x42) ;
  WriteData(0x3A) ;
  WriteData(0x27) ;
  WriteData(0x1B) ;
  WriteData(0x08) ;
  WriteData(0x09) ;
  WriteData(0x03) ;
  WriteData(0x02) ;
  WriteData(0x0A) ;
  WriteData(0x11) ;
  WriteData(0x1D) ;
  WriteData(0x23) ;
  WriteData(0x35) ;
  WriteData(0x41) ;
  WriteData(0x4B) ;
  WriteData(0x4B) ;
  WriteData(0x42) ;
  WriteData(0x3A) ;
  WriteData(0x27) ;
  WriteData(0x1B) ;
  WriteData(0x08) ;
  WriteData(0x09) ;
  WriteData(0x03) ;
  WriteData(0x00) ;
  WriteData(0x01) ;

  // 16-bit color
  WriteCommand(HX8357_COLMOD) ;
  WriteData(0x55) ;

  // Memory access control (rotation)
  WriteCommand(HX8357_MADCTL) ;
  WriteData(MADCTL_MX | MADCTL_MV | MADCTL_RGB) ;

  // Tearing effect line on
  WriteCommand(HX8357_TEON) ;
  WriteData(0x00) ;

  // Exit sleep
  WriteCommand(HX8357_SLPOUT) ;
  sleep_ms(150) ;

  // Display on
  WriteCommand(HX8357_DISPON) ;
  sleep_ms(50) ;

  // Set default rotation (landscape)
  HX8357_SetRotation(1) ;

  printf("HX8357: Display initialized (%dx%d)\n", sWidth, sHeight) ;

  return true ;
}

//----------------------------------------------
// Function: HX8357_SetRotation
//----------------------------------------------
void HX8357_SetRotation(uint8_t inRotation)
{
  sRotation = inRotation & 0x03 ;

  WriteCommand(HX8357_MADCTL) ;

  switch (sRotation)
  {
    case 0:  // Portrait
      WriteData(MADCTL_MX | MADCTL_RGB) ;
      sWidth = HX8357_HEIGHT ;
      sHeight = HX8357_WIDTH ;
      break ;

    case 1:  // Landscape
      WriteData(MADCTL_MX | MADCTL_MV | MADCTL_RGB) ;
      sWidth = HX8357_WIDTH ;
      sHeight = HX8357_HEIGHT ;
      break ;

    case 2:  // Portrait inverted
      WriteData(MADCTL_MY | MADCTL_RGB) ;
      sWidth = HX8357_HEIGHT ;
      sHeight = HX8357_WIDTH ;
      break ;

    case 3:  // Landscape inverted
      WriteData(MADCTL_MY | MADCTL_MV | MADCTL_RGB) ;
      sWidth = HX8357_WIDTH ;
      sHeight = HX8357_HEIGHT ;
      break ;
  }
}

//----------------------------------------------
// Function: HX8357_FillScreen
//----------------------------------------------
void HX8357_FillScreen(uint16_t inColor)
{
  HX8357_FillRect(0, 0, sWidth, sHeight, inColor) ;
}

//----------------------------------------------
// Function: HX8357_FillRect
//----------------------------------------------
void HX8357_FillRect(int16_t inX, int16_t inY, int16_t inWidth, int16_t inHeight, uint16_t inColor)
{
  if (inX >= sWidth || inY >= sHeight) return ;
  if (inX + inWidth > sWidth) inWidth = sWidth - inX ;
  if (inY + inHeight > sHeight) inHeight = sHeight - inY ;

  SetAddrWindow(inX, inY, inWidth, inHeight) ;

  uint8_t hi = inColor >> 8 ;
  uint8_t lo = inColor & 0xFF ;
  uint32_t theCount = (uint32_t)inWidth * (uint32_t)inHeight ;

  SpiBegin() ;
  gpio_put(kPinTftDc, 1) ;

  // Send pixels in chunks for efficiency
  uint8_t theBuffer[512] ;
  for (int i = 0 ; i < 256 ; i++)
  {
    theBuffer[i * 2] = hi ;
    theBuffer[i * 2 + 1] = lo ;
  }

  while (theCount >= 256)
  {
    spi_write_blocking(kSpiPort, theBuffer, 512) ;
    theCount -= 256 ;
  }

  if (theCount > 0)
  {
    spi_write_blocking(kSpiPort, theBuffer, theCount * 2) ;
  }

  SpiEnd() ;
}

//----------------------------------------------
// Function: HX8357_DrawRect
//----------------------------------------------
void HX8357_DrawRect(int16_t inX, int16_t inY, int16_t inWidth, int16_t inHeight, uint16_t inColor)
{
  HX8357_DrawHLine(inX, inY, inWidth, inColor) ;
  HX8357_DrawHLine(inX, inY + inHeight - 1, inWidth, inColor) ;
  HX8357_DrawVLine(inX, inY, inHeight, inColor) ;
  HX8357_DrawVLine(inX + inWidth - 1, inY, inHeight, inColor) ;
}

//----------------------------------------------
// Function: HX8357_DrawPixel
//----------------------------------------------
void HX8357_DrawPixel(int16_t inX, int16_t inY, uint16_t inColor)
{
  if (inX < 0 || inX >= sWidth || inY < 0 || inY >= sHeight) return ;

  SetAddrWindow(inX, inY, 1, 1) ;
  WriteData16(inColor) ;
}

//----------------------------------------------
// Function: HX8357_DrawHLine
//----------------------------------------------
void HX8357_DrawHLine(int16_t inX, int16_t inY, int16_t inWidth, uint16_t inColor)
{
  HX8357_FillRect(inX, inY, inWidth, 1, inColor) ;
}

//----------------------------------------------
// Function: HX8357_DrawVLine
//----------------------------------------------
void HX8357_DrawVLine(int16_t inX, int16_t inY, int16_t inHeight, uint16_t inColor)
{
  HX8357_FillRect(inX, inY, 1, inHeight, inColor) ;
}

//----------------------------------------------
// Function: HX8357_DrawLine
//----------------------------------------------
void HX8357_DrawLine(int16_t inX0, int16_t inY0, int16_t inX1, int16_t inY1, uint16_t inColor)
{
  // Bresenham's line algorithm
  int16_t steep = abs(inY1 - inY0) > abs(inX1 - inX0) ;
  int16_t tmp ;

  if (steep)
  {
    tmp = inX0 ; inX0 = inY0 ; inY0 = tmp ;
    tmp = inX1 ; inX1 = inY1 ; inY1 = tmp ;
  }

  if (inX0 > inX1)
  {
    tmp = inX0 ; inX0 = inX1 ; inX1 = tmp ;
    tmp = inY0 ; inY0 = inY1 ; inY1 = tmp ;
  }

  int16_t dx = inX1 - inX0 ;
  int16_t dy = abs(inY1 - inY0) ;
  int16_t err = dx / 2 ;
  int16_t ystep = (inY0 < inY1) ? 1 : -1 ;

  for ( ; inX0 <= inX1 ; inX0++)
  {
    if (steep)
    {
      HX8357_DrawPixel(inY0, inX0, inColor) ;
    }
    else
    {
      HX8357_DrawPixel(inX0, inY0, inColor) ;
    }
    err -= dy ;
    if (err < 0)
    {
      inY0 += ystep ;
      err += dx ;
    }
  }
}

//----------------------------------------------
// Function: HX8357_DrawChar
//----------------------------------------------
void HX8357_DrawChar(int16_t inX, int16_t inY, char inChar, uint16_t inFgColor, uint16_t inBgColor, uint8_t inSize)
{
  if (inChar < 32 || inChar > 127) inChar = '?' ;

  for (int8_t i = 0 ; i < 5 ; i++)
  {
    uint8_t theLine = sFont5x7[(inChar - 32) * 5 + i] ;

    for (int8_t j = 0 ; j < 8 ; j++)
    {
      uint16_t theColor = (theLine & 0x01) ? inFgColor : inBgColor ;

      if (inSize == 1)
      {
        HX8357_DrawPixel(inX + i, inY + j, theColor) ;
      }
      else
      {
        HX8357_FillRect(inX + i * inSize, inY + j * inSize, inSize, inSize, theColor) ;
      }

      theLine >>= 1 ;
    }
  }

  // Draw background for spacing column
  if (inSize == 1)
  {
    for (int8_t j = 0 ; j < 8 ; j++)
    {
      HX8357_DrawPixel(inX + 5, inY + j, inBgColor) ;
    }
  }
  else
  {
    HX8357_FillRect(inX + 5 * inSize, inY, inSize, 8 * inSize, inBgColor) ;
  }
}

//----------------------------------------------
// Function: HX8357_DrawString
//----------------------------------------------
void HX8357_DrawString(int16_t inX, int16_t inY, const char * inStr, uint16_t inFgColor, uint16_t inBgColor, uint8_t inSize)
{
  while (*inStr)
  {
    HX8357_DrawChar(inX, inY, *inStr, inFgColor, inBgColor, inSize) ;
    inX += 6 * inSize ;
    inStr++ ;
  }
}

//----------------------------------------------
// Function: HX8357_DrawStringCentered
//----------------------------------------------
void HX8357_DrawStringCentered(int16_t inY, const char * inStr, uint16_t inFgColor, uint16_t inBgColor, uint8_t inSize)
{
  int theLen = strlen(inStr) ;
  int16_t theWidth = theLen * 6 * inSize ;
  int16_t theX = (sWidth - theWidth) / 2 ;
  HX8357_DrawString(theX, inY, inStr, inFgColor, inBgColor, inSize) ;
}

//----------------------------------------------
// Function: HX8357_SetBacklight
//----------------------------------------------
void HX8357_SetBacklight(bool inOn)
{
  #ifdef kPinTftLite
  gpio_put(kPinTftLite, inOn ? 1 : 0) ;
  #endif
}

//----------------------------------------------
// Function: HX8357_RGB
//----------------------------------------------
uint16_t HX8357_RGB(uint8_t inR, uint8_t inG, uint8_t inB)
{
  return ((inR & 0xF8) << 8) | ((inG & 0xFC) << 3) | (inB >> 3) ;
}

//----------------------------------------------
// Function: HX8357_GetWidth
//----------------------------------------------
int16_t HX8357_GetWidth(void)
{
  return sWidth ;
}

//----------------------------------------------
// Function: HX8357_GetHeight
//----------------------------------------------
int16_t HX8357_GetHeight(void)
{
  return sHeight ;
}

//----------------------------------------------
// Local Functions
//----------------------------------------------

static void SpiBegin(void)
{
  // Set faster SPI speed for TFT
  spi_set_baudrate(kSpiPort, kSpiTftBaudrate) ;

  // Deselect LoRa, select TFT
  gpio_put(kPinLoRaCs, 1) ;
  gpio_put(kPinTftCs, 0) ;
}

static void SpiEnd(void)
{
  gpio_put(kPinTftCs, 1) ;

  // Restore LoRa SPI speed
  spi_set_baudrate(kSpiPort, kSpiLoRaBaudrate) ;
}

static void WriteCommand(uint8_t inCmd)
{
  SpiBegin() ;
  gpio_put(kPinTftDc, 0) ;  // Command mode
  spi_write_blocking(kSpiPort, &inCmd, 1) ;
  gpio_put(kPinTftDc, 1) ;  // Back to data mode
  SpiEnd() ;
}

static void WriteData(uint8_t inData)
{
  SpiBegin() ;
  gpio_put(kPinTftDc, 1) ;  // Data mode
  spi_write_blocking(kSpiPort, &inData, 1) ;
  SpiEnd() ;
}

static void WriteData16(uint16_t inData)
{
  uint8_t theData[2] = { inData >> 8, inData & 0xFF } ;
  SpiBegin() ;
  gpio_put(kPinTftDc, 1) ;  // Data mode
  spi_write_blocking(kSpiPort, theData, 2) ;
  SpiEnd() ;
}

// Write command followed by multiple data bytes efficiently
static void WriteCommandWithData(uint8_t inCmd, const uint8_t * inData, size_t inLen)
{
  SpiBegin() ;
  gpio_put(kPinTftDc, 0) ;  // Command mode
  spi_write_blocking(kSpiPort, &inCmd, 1) ;
  gpio_put(kPinTftDc, 1) ;  // Data mode
  if (inLen > 0)
  {
    spi_write_blocking(kSpiPort, inData, inLen) ;
  }
  SpiEnd() ;
}

static void SetAddrWindow(int16_t inX, int16_t inY, int16_t inW, int16_t inH)
{
  uint16_t theX1 = inX + inW - 1 ;
  uint16_t theY1 = inY + inH - 1 ;

  WriteCommand(HX8357_CASET) ;
  WriteData(inX >> 8) ;
  WriteData(inX & 0xFF) ;
  WriteData(theX1 >> 8) ;
  WriteData(theX1 & 0xFF) ;

  WriteCommand(HX8357_PASET) ;
  WriteData(inY >> 8) ;
  WriteData(inY & 0xFF) ;
  WriteData(theY1 >> 8) ;
  WriteData(theY1 & 0xFF) ;

  WriteCommand(HX8357_RAMWR) ;
}
