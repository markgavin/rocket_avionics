#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

//----------------------------------------------
// Framebuffer Drawing Library
// Maps physical landscape coordinates (296x128)
// to UC8151D buffer layout (128 wide x 296 tall)
//----------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "uc8151d.h"

// Colors
#define kColorBlack   0
#define kColorWhite   1

// Clear entire framebuffer
void FB_Clear(uint8_t * ioBuffer, uint8_t inColor) ;

// Pixel operations (physical landscape coordinates)
void FB_SetPixel(uint8_t * ioBuffer, int inX, int inY, uint8_t inColor) ;

// Line drawing
void FB_DrawHLine(uint8_t * ioBuffer, int inX, int inY, int inWidth, uint8_t inColor) ;
void FB_DrawVLine(uint8_t * ioBuffer, int inX, int inY, int inHeight, uint8_t inColor) ;

// Rectangle
void FB_DrawRect(uint8_t * ioBuffer, int inX, int inY, int inW, int inH, uint8_t inColor) ;
void FB_FillRect(uint8_t * ioBuffer, int inX, int inY, int inW, int inH, uint8_t inColor) ;

// Text (scale 1 = 6x8 cell, scale 2 = 12x16 cell)
void FB_DrawChar(uint8_t * ioBuffer, int inX, int inY, char inCh, uint8_t inColor, int inScale) ;
void FB_DrawString(uint8_t * ioBuffer, int inX, int inY, const char * inStr, uint8_t inColor, int inScale) ;

// Inverted text (white on black background)
void FB_DrawStringInverted(uint8_t * ioBuffer, int inX, int inY, const char * inStr, int inScale) ;

#endif // FRAMEBUFFER_H
