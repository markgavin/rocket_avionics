//----------------------------------------------
// Module: base64.c
// Description: Base64 encoding/decoding for
//   binary file transfer over JSON
// Author: Mark Gavin
// Created: 2025-12-27
// Copyright: (c) 2025 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "base64.h"
#include <string.h>

//----------------------------------------------
// Base64 Character Tables
//----------------------------------------------
static const char kEncodeTable[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" ;

static const uint8_t kDecodeTable[128] = {
  // 0-31: invalid
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255,
  // 32-47: space through /
  255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255,  62, 255, 255, 255,  63,  // + is 62, / is 63
  // 48-63: 0-9 and : through ?
   52,  53,  54,  55,  56,  57,  58,  59,  // 0-7
   60,  61, 255, 255, 255,   0, 255, 255,  // 8-9, = is padding (0)
  // 64-79: @ and A-O
  255,   0,   1,   2,   3,   4,   5,   6,
    7,   8,   9,  10,  11,  12,  13,  14,
  // 80-95: P-Z and [ through _
   15,  16,  17,  18,  19,  20,  21,  22,
   23,  24,  25, 255, 255, 255, 255, 255,
  // 96-111: ` and a-o
  255,  26,  27,  28,  29,  30,  31,  32,
   33,  34,  35,  36,  37,  38,  39,  40,
  // 112-127: p-z and { through DEL
   41,  42,  43,  44,  45,  46,  47,  48,
   49,  50,  51, 255, 255, 255, 255, 255
} ;

//----------------------------------------------
// Function: Base64_EncodedLength
//----------------------------------------------
size_t Base64_EncodedLength(size_t inLen)
{
  // 4 output chars per 3 input bytes, rounded up, plus null
  return ((inLen + 2) / 3) * 4 + 1 ;
}

//----------------------------------------------
// Function: Base64_DecodedLength
//----------------------------------------------
size_t Base64_DecodedLength(const char * inStr)
{
  size_t theLen = strlen(inStr) ;
  if (theLen == 0) return 0 ;

  // Count padding
  size_t thePadding = 0 ;
  if (theLen >= 1 && inStr[theLen - 1] == '=') thePadding++ ;
  if (theLen >= 2 && inStr[theLen - 2] == '=') thePadding++ ;

  return (theLen * 3) / 4 - thePadding ;
}

//----------------------------------------------
// Function: Base64_Encode
//----------------------------------------------
size_t Base64_Encode(
  const uint8_t * inData ,
  size_t inLen ,
  char * outStr ,
  size_t outMaxLen)
{
  if (outMaxLen < Base64_EncodedLength(inLen))
  {
    if (outMaxLen > 0) outStr[0] = '\0' ;
    return 0 ;
  }

  size_t theOutIdx = 0 ;
  size_t i = 0 ;

  // Process 3 bytes at a time
  while (i + 2 < inLen)
  {
    uint32_t theTriple =
      ((uint32_t)inData[i] << 16) |
      ((uint32_t)inData[i + 1] << 8) |
      ((uint32_t)inData[i + 2]) ;

    outStr[theOutIdx++] = kEncodeTable[(theTriple >> 18) & 0x3F] ;
    outStr[theOutIdx++] = kEncodeTable[(theTriple >> 12) & 0x3F] ;
    outStr[theOutIdx++] = kEncodeTable[(theTriple >> 6) & 0x3F] ;
    outStr[theOutIdx++] = kEncodeTable[theTriple & 0x3F] ;

    i += 3 ;
  }

  // Handle remaining bytes
  if (i < inLen)
  {
    uint32_t theTriple = (uint32_t)inData[i] << 16 ;
    if (i + 1 < inLen) theTriple |= (uint32_t)inData[i + 1] << 8 ;

    outStr[theOutIdx++] = kEncodeTable[(theTriple >> 18) & 0x3F] ;
    outStr[theOutIdx++] = kEncodeTable[(theTriple >> 12) & 0x3F] ;

    if (i + 1 < inLen)
    {
      outStr[theOutIdx++] = kEncodeTable[(theTriple >> 6) & 0x3F] ;
    }
    else
    {
      outStr[theOutIdx++] = '=' ;
    }
    outStr[theOutIdx++] = '=' ;
  }

  outStr[theOutIdx] = '\0' ;
  return theOutIdx ;
}

//----------------------------------------------
// Function: Base64_Decode
//----------------------------------------------
size_t Base64_Decode(
  const char * inStr ,
  uint8_t * outData ,
  size_t outMaxLen)
{
  size_t theInLen = strlen(inStr) ;
  if (theInLen == 0) return 0 ;

  // Must be multiple of 4
  if (theInLen % 4 != 0) return 0 ;

  size_t theOutIdx = 0 ;
  size_t i = 0 ;

  while (i < theInLen)
  {
    // Get 4 characters
    uint8_t a = (inStr[i] < 128) ? kDecodeTable[(uint8_t)inStr[i]] : 255 ;
    uint8_t b = (inStr[i + 1] < 128) ? kDecodeTable[(uint8_t)inStr[i + 1]] : 255 ;
    uint8_t c = (inStr[i + 2] < 128) ? kDecodeTable[(uint8_t)inStr[i + 2]] : 255 ;
    uint8_t d = (inStr[i + 3] < 128) ? kDecodeTable[(uint8_t)inStr[i + 3]] : 255 ;

    // Check for invalid characters (except padding which decodes to 0)
    if (a == 255 || b == 255) return theOutIdx ;
    if (c == 255 && inStr[i + 2] != '=') return theOutIdx ;
    if (d == 255 && inStr[i + 3] != '=') return theOutIdx ;

    // First byte always valid
    if (theOutIdx < outMaxLen)
    {
      outData[theOutIdx++] = (a << 2) | (b >> 4) ;
    }

    // Second byte if not padding
    if (inStr[i + 2] != '=' && theOutIdx < outMaxLen)
    {
      outData[theOutIdx++] = (b << 4) | (c >> 2) ;
    }

    // Third byte if not padding
    if (inStr[i + 3] != '=' && theOutIdx < outMaxLen)
    {
      outData[theOutIdx++] = (c << 6) | d ;
    }

    i += 4 ;
  }

  return theOutIdx ;
}
