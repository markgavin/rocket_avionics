//----------------------------------------------
// Module: base64.h
// Description: Base64 encoding/decoding for
//   binary file transfer over JSON
// Author: Mark Gavin
// Created: 2025-12-27
// Copyright: (c) 2025 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stddef.h>

//----------------------------------------------
// Function: Base64_Encode
// Purpose: Encode binary data to base64 string
// Parameters:
//   inData - input binary data
//   inLen - input data length
//   outStr - output buffer for base64 string
//   outMaxLen - size of output buffer
// Returns: Length of encoded string (excluding null)
// Notes: Output will be null-terminated
//----------------------------------------------
size_t Base64_Encode(
  const uint8_t * inData ,
  size_t inLen ,
  char * outStr ,
  size_t outMaxLen) ;

//----------------------------------------------
// Function: Base64_Decode
// Purpose: Decode base64 string to binary data
// Parameters:
//   inStr - input base64 string
//   outData - output buffer for binary data
//   outMaxLen - size of output buffer
// Returns: Length of decoded data
//----------------------------------------------
size_t Base64_Decode(
  const char * inStr ,
  uint8_t * outData ,
  size_t outMaxLen) ;

//----------------------------------------------
// Function: Base64_EncodedLength
// Purpose: Calculate required buffer size for encoding
// Parameters:
//   inLen - length of binary data to encode
// Returns: Required buffer size (including null terminator)
//----------------------------------------------
size_t Base64_EncodedLength(size_t inLen) ;

//----------------------------------------------
// Function: Base64_DecodedLength
// Purpose: Calculate maximum decoded data length
// Parameters:
//   inStr - base64 encoded string
// Returns: Maximum decoded data length
//----------------------------------------------
size_t Base64_DecodedLength(const char * inStr) ;
