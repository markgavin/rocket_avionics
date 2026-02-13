//----------------------------------------------
// Framebuffer Drawing Library
// Physical landscape (296x128) -> UC8151D buffer (128w x 296h)
//----------------------------------------------

#include <string.h>
#include "framebuffer.h"
#include "font5x7.h"
#include "uc8151d.h"

//----------------------------------------------
// Coordinate mapping
//
// Physical display (landscape): X=0..295, Y=0..127
// Buffer layout (portrait):     row=0..295, 16 bytes/row
//
// Mapping: buffer row = physical X
//          byte within row = physical Y / 8
//          bit within byte = 7 - (physical Y % 8)   [MSB first]
//----------------------------------------------

void FB_Clear(uint8_t * ioBuffer, uint8_t inColor)
{
  uint8_t theFill = (inColor == kColorWhite) ? 0xFF : 0x00 ;
  memset(ioBuffer, theFill, kEpdBufferSize) ;
}

void FB_SetPixel(uint8_t * ioBuffer, int inX, int inY, uint8_t inColor)
{
  // Bounds check
  if (inX < 0 || inX >= kEpdWidth || inY < 0 || inY >= kEpdHeight)
    return ;

  // Rotate 180°: ribbon on left side
  int theX = (kEpdWidth - 1) - inX ;
  int theY = inY ;

  int theRow = theX ;
  int theByteIdx = theY / 8 ;
  int theBitIdx = 7 - (theY % 8) ;
  int theOffset = theRow * kEpdBytesPerRow + theByteIdx ;

  if (inColor == kColorWhite)
    ioBuffer[theOffset] |= (1 << theBitIdx) ;
  else
    ioBuffer[theOffset] &= ~(1 << theBitIdx) ;
}

void FB_DrawHLine(uint8_t * ioBuffer, int inX, int inY, int inWidth, uint8_t inColor)
{
  for (int i = 0 ; i < inWidth ; i++)
    FB_SetPixel(ioBuffer, inX + i, inY, inColor) ;
}

void FB_DrawVLine(uint8_t * ioBuffer, int inX, int inY, int inHeight, uint8_t inColor)
{
  for (int i = 0 ; i < inHeight ; i++)
    FB_SetPixel(ioBuffer, inX, inY + i, inColor) ;
}

void FB_DrawRect(uint8_t * ioBuffer, int inX, int inY, int inW, int inH, uint8_t inColor)
{
  FB_DrawHLine(ioBuffer, inX, inY, inW, inColor) ;
  FB_DrawHLine(ioBuffer, inX, inY + inH - 1, inW, inColor) ;
  FB_DrawVLine(ioBuffer, inX, inY, inH, inColor) ;
  FB_DrawVLine(ioBuffer, inX + inW - 1, inY, inH, inColor) ;
}

void FB_FillRect(uint8_t * ioBuffer, int inX, int inY, int inW, int inH, uint8_t inColor)
{
  for (int theY = inY ; theY < inY + inH ; theY++)
    FB_DrawHLine(ioBuffer, inX, theY, inW, inColor) ;
}

void FB_DrawChar(uint8_t * ioBuffer, int inX, int inY, char inCh, uint8_t inColor, int inScale)
{
  if (inCh < kFontFirstChar || inCh > kFontLastChar)
    return ;

  int theIdx = inCh - kFontFirstChar ;
  const uint8_t * theGlyph = kFont5x7[theIdx] ;

  for (int theCol = 0 ; theCol < kFontCharWidth ; theCol++)
  {
    uint8_t theColData = theGlyph[theCol] ;

    for (int theRow = 0 ; theRow < kFontCharHeight ; theRow++)
    {
      if (theColData & (1 << theRow))
      {
        // Pixel is set in the font
        for (int theSy = 0 ; theSy < inScale ; theSy++)
          for (int theSx = 0 ; theSx < inScale ; theSx++)
            FB_SetPixel(ioBuffer,
                        inX + theCol * inScale + theSx,
                        inY + theRow * inScale + theSy,
                        inColor) ;
      }
    }
  }
}

void FB_DrawString(uint8_t * ioBuffer, int inX, int inY, const char * inStr, uint8_t inColor, int inScale)
{
  int theCellWidth = (kFontCharWidth + 1) * inScale ;   // +1 for spacing

  while (*inStr)
  {
    FB_DrawChar(ioBuffer, inX, inY, *inStr, inColor, inScale) ;
    inX += theCellWidth ;
    inStr++ ;
  }
}

void FB_DrawStringInverted(uint8_t * ioBuffer, int inX, int inY, const char * inStr, int inScale)
{
  int theCellWidth = (kFontCharWidth + 1) * inScale ;
  int theCellHeight = (kFontCharHeight + 1) * inScale ;
  int theLen = 0 ;
  const char * p = inStr ;
  while (*p++) theLen++ ;

  // Draw black background with 2px padding
  FB_FillRect(ioBuffer, inX - 2, inY - 1,
              theLen * theCellWidth + 4, theCellHeight + 2, kColorBlack) ;

  // Draw white text
  FB_DrawString(ioBuffer, inX, inY, inStr, kColorWhite, inScale) ;
}
