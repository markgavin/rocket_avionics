//----------------------------------------------
// Module: gateway_protocol.c
// Description: Gateway protocol handler
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "gateway_protocol.h"
#include "pins.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

//----------------------------------------------
// Barometric Constants
//----------------------------------------------
#define kSeaLevelPressurePa     101325.0f   // Standard sea level pressure
#define kSeaLevelTempK          288.15f     // Standard temperature at sea level (K)
#define kTempLapseRate          0.0065f     // Temperature lapse rate (K/m)
#define kGasConstant            8.31447f    // Universal gas constant (J/(mol*K))
#define kMolarMass              0.0289644f  // Molar mass of air (kg/mol)
#define kGravity                9.80665f    // Gravitational acceleration (m/s^2)

//----------------------------------------------
// Internal: Calculate altitude from pressure
//----------------------------------------------
static float CalculateAltitude(float inPressurePa, float inReferencePressurePa)
{
  if (inReferencePressurePa <= 0.0f || inPressurePa <= 0.0f)
  {
    return 0.0f ;
  }

  float theExponent = (kGasConstant * kTempLapseRate) / (kGravity * kMolarMass) ;
  float theRatio = inPressurePa / inReferencePressurePa ;
  float theAltitude = (kSeaLevelTempK / kTempLapseRate) * (1.0f - powf(theRatio, theExponent)) ;

  return theAltitude ;
}

//----------------------------------------------
// Flight State Names (must match flight_control.h)
//----------------------------------------------
static const char * sFlightStateNames[] =
{
  "idle",
  "armed",
  "boost",
  "coast",
  "apogee",
  "descent",
  "landed",
  "complete"
} ;

//----------------------------------------------
// Function: GatewayProtocol_Init
//----------------------------------------------
void GatewayProtocol_Init(GatewayState * ioState)
{
  if (ioState == NULL) return ;

  ioState->pConnected = false ;
  ioState->pLastPacketTimeMs = 0 ;
  ioState->pPacketsReceived = 0 ;
  ioState->pPacketsSent = 0 ;
  ioState->pPacketsLost = 0 ;
  ioState->pLastRssi = 0 ;
  ioState->pLastSnr = 0 ;
}

//----------------------------------------------
// Function: GatewayProtocol_TelemetryToJson
//----------------------------------------------
int GatewayProtocol_TelemetryToJson(
  const LoRaTelemetryPacket * inPacket,
  int16_t inRssi,
  int8_t inSnr,
  float inGroundPressurePa,
  bool inGwGpsValid,
  float inGwGpsLat,
  float inGwGpsLon,
  char * outJson,
  int inMaxLen)
{
  if (inPacket == NULL || outJson == NULL || inMaxLen < 256) return 0 ;

  // Validate magic byte
  if (inPacket->pMagic != kLoRaMagic) return 0 ;

  // Convert barometric units
  float theAltitudeM = inPacket->pAltitudeCm / 100.0f ;
  float theVelocityMps = inPacket->pVelocityCmps / 100.0f ;
  float theTemperatureC = inPacket->pTemperatureC10 / 10.0f ;

  // Convert GPS units
  float theLatitude = inPacket->pGpsLatitude / 1000000.0f ;
  float theLongitude = inPacket->pGpsLongitude / 1000000.0f ;
  float theGpsSpeedMps = inPacket->pGpsSpeedCmps / 100.0f ;
  float theHeadingDeg = inPacket->pGpsHeadingDeg10 / 10.0f ;
  bool theHasGpsFix = (inPacket->pFlags & kFlagGpsFix) != 0 ;

  // Convert IMU units
  // Accelerometer: milli-g to g
  float theAccelX = inPacket->pAccelX / 1000.0f ;
  float theAccelY = inPacket->pAccelY / 1000.0f ;
  float theAccelZ = inPacket->pAccelZ / 1000.0f ;

  // Gyroscope: 0.1 dps to dps
  float theGyroX = inPacket->pGyroX / 10.0f ;
  float theGyroY = inPacket->pGyroY / 10.0f ;
  float theGyroZ = inPacket->pGyroZ / 10.0f ;

  // Magnetometer: already in milligauss, send as-is
  int16_t theMagX = inPacket->pMagX ;
  int16_t theMagY = inPacket->pMagY ;
  int16_t theMagZ = inPacket->pMagZ ;

  // Get state name
  const char * theStateName = GatewayProtocol_GetStateName(inPacket->pState) ;

  // Calculate ground altitude (using sea level reference) and differential altitude
  float theGroundAltitudeM = CalculateAltitude(inGroundPressurePa, kSeaLevelPressurePa) ;

  // Calculate differential altitude: flight computer relative to ground station
  // This is more accurate than the flight computer's self-referenced altitude
  float theDiffAltitudeM = 0.0f ;
  if (inGroundPressurePa > 0.0f && inPacket->pPressurePa > 0)
  {
    theDiffAltitudeM = CalculateAltitude((float)inPacket->pPressurePa, inGroundPressurePa) ;
  }

  // Build JSON telemetry message with GPS, IMU data, ground reference, and gateway GPS
  int theLen = snprintf(outJson, inMaxLen,
    "{\"type\":\"tel\","
    "\"seq\":%u,"
    "\"t\":%lu,"
    "\"alt\":%.2f,"
    "\"dalt\":%.2f,"
    "\"vel\":%.2f,"
    "\"pres\":%lu,"
    "\"gpres\":%.0f,"
    "\"galt\":%.1f,"
    "\"temp\":%.1f,"
    "\"lat\":%.6f,"
    "\"lon\":%.6f,"
    "\"gspd\":%.2f,"
    "\"hdg\":%.1f,"
    "\"sat\":%u,"
    "\"gps\":%s,"
    "\"ax\":%.3f,"
    "\"ay\":%.3f,"
    "\"az\":%.3f,"
    "\"gx\":%.1f,"
    "\"gy\":%.1f,"
    "\"gz\":%.1f,"
    "\"mx\":%d,"
    "\"my\":%d,"
    "\"mz\":%d,"
    "\"state\":\"%s\","
    "\"flags\":%u,"
    "\"rssi\":%d,"
    "\"snr\":%d,"
    "\"gw_gps\":%s,"
    "\"gw_lat\":%.6f,"
    "\"gw_lon\":%.6f}\n",
    inPacket->pSequence,
    (unsigned long)inPacket->pTimeMs,
    theAltitudeM,
    theDiffAltitudeM,
    theVelocityMps,
    (unsigned long)inPacket->pPressurePa,
    inGroundPressurePa,
    theGroundAltitudeM,
    theTemperatureC,
    theLatitude,
    theLongitude,
    theGpsSpeedMps,
    theHeadingDeg,
    inPacket->pGpsSatellites,
    theHasGpsFix ? "true" : "false",
    theAccelX,
    theAccelY,
    theAccelZ,
    theGyroX,
    theGyroY,
    theGyroZ,
    theMagX,
    theMagY,
    theMagZ,
    theStateName,
    inPacket->pFlags,
    inRssi,
    inSnr,
    inGwGpsValid ? "true" : "false",
    inGwGpsValid ? inGwGpsLat : 0.0f,
    inGwGpsValid ? inGwGpsLon : 0.0f) ;

  return theLen ;
}

//----------------------------------------------
// Function: GatewayProtocol_ParseCommand
//----------------------------------------------
bool GatewayProtocol_ParseCommand(
  const char * inJson,
  UsbCommandType * outCommandType,
  uint32_t * outCommandId,
  int8_t * outRocketId)
{
  if (inJson == NULL || outCommandType == NULL || outCommandId == NULL || outRocketId == NULL) return false ;

  *outRocketId = -1 ;  // -1 means not specified

  // Simple JSON parsing (no external library needed for these simple commands)
  // Expected format: {"cmd":"arm","id":1}

  // Find command
  const char * theCmdStart = strstr(inJson, "\"cmd\":\"") ;
  if (theCmdStart == NULL) return false ;
  theCmdStart += 7 ;  // Skip past "cmd":"

  // Find command end
  const char * theCmdEnd = strchr(theCmdStart, '"') ;
  if (theCmdEnd == NULL) return false ;

  int theCmdLen = theCmdEnd - theCmdStart ;

  // Parse command type
  if (strncmp(theCmdStart, "arm", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdArm ;
  }
  else if (strncmp(theCmdStart, "disarm", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdDisarm ;
  }
  else if (strncmp(theCmdStart, "status", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdStatus ;
  }
  else if (strncmp(theCmdStart, "download", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdDownload ;
  }
  else if (strncmp(theCmdStart, "reset", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdReset ;
  }
  else if (strncmp(theCmdStart, "ping", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdPing ;
  }
  else if (strncmp(theCmdStart, "info", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdInfo ;
  }
  else if (strncmp(theCmdStart, "gw_info", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdGatewayInfo ;
  }
  // Storage commands
  else if (strncmp(theCmdStart, "sd_list", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdSdList ;
  }
  else if (strncmp(theCmdStart, "sd_read", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdSdRead ;
  }
  else if (strncmp(theCmdStart, "sd_delete", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdSdDelete ;
  }
  else if (strncmp(theCmdStart, "flash_list", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdFlashList ;
  }
  else if (strncmp(theCmdStart, "flash_read", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdFlashRead ;
  }
  else if (strncmp(theCmdStart, "flash_delete", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdFlashDelete ;
  }
  else if (strncmp(theCmdStart, "orientation_mode", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdOrientationMode ;
  }
  // WiFi configuration commands
  else if (strncmp(theCmdStart, "wifi_list", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdWifiList ;
  }
  else if (strncmp(theCmdStart, "wifi_add", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdWifiAdd ;
  }
  else if (strncmp(theCmdStart, "wifi_remove", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdWifiRemove ;
  }
  else if (strncmp(theCmdStart, "wifi_save", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdWifiSave ;
  }
  else if (strncmp(theCmdStart, "wifi_status", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdWifiStatus ;
  }
  else if (strncmp(theCmdStart, "wifi_set_ap", theCmdLen) == 0)
  {
    *outCommandType = kUsbCmdWifiSetAp ;
  }
  else
  {
    return false ;
  }

  // Find command ID
  *outCommandId = 0 ;
  const char * theIdStart = strstr(inJson, "\"id\":") ;
  if (theIdStart != NULL)
  {
    theIdStart += 5 ;  // Skip past "id":
    *outCommandId = (uint32_t)strtoul(theIdStart, NULL, 10) ;
  }

  // Find rocket ID: "rocket":N
  const char * theRocketStart = strstr(inJson, "\"rocket\":") ;
  if (theRocketStart != NULL)
  {
    theRocketStart += 9 ;  // Skip past "rocket":
    int theRocketId = (int)strtol(theRocketStart, NULL, 10) ;
    if (theRocketId >= 0 && theRocketId < 16)
    {
      *outRocketId = (int8_t)theRocketId ;
    }
  }

  return true ;
}

//----------------------------------------------
// Function: GatewayProtocol_BuildLoRaCommand
//----------------------------------------------
int GatewayProtocol_BuildLoRaCommand(
  UsbCommandType inCommandType,
  uint8_t inTargetRocketId,
  uint8_t * outPacket,
  int inMaxLen)
{
  if (outPacket == NULL || inMaxLen < 4) return 0 ;

  // Build command packet: magic, type, targetRocketId, commandId
  outPacket[0] = kLoRaMagic ;
  outPacket[1] = kLoRaPacketCommand ;
  outPacket[2] = inTargetRocketId ;

  switch (inCommandType)
  {
    case kUsbCmdArm:
      outPacket[3] = kCmdArm ;
      break ;
    case kUsbCmdDisarm:
      outPacket[3] = kCmdDisarm ;
      break ;
    case kUsbCmdStatus:
      outPacket[3] = kCmdStatus ;
      break ;
    case kUsbCmdReset:
      outPacket[3] = kCmdReset ;
      break ;
    case kUsbCmdDownload:
      outPacket[3] = kCmdDownload ;
      break ;
    case kUsbCmdPing:
      outPacket[3] = kCmdPing ;
      break ;
    case kUsbCmdInfo:
      outPacket[3] = kCmdInfo ;
      break ;
    // Storage commands (simple, no parameters)
    case kUsbCmdSdList:
      outPacket[3] = kCmdSdList ;
      break ;
    case kUsbCmdFlashList:
      outPacket[3] = kCmdFlashList ;
      break ;
    default:
      return 0 ;
  }

  return 4 ;
}

//----------------------------------------------
// Function: GatewayProtocol_BuildStorageReadCommand
// Purpose: Build LoRa command for sd_read or flash_read
//----------------------------------------------
int GatewayProtocol_BuildStorageReadCommand(
  UsbCommandType inCommandType,
  uint8_t inTargetRocketId,
  const char * inFilename,
  uint32_t inOffset,
  uint8_t * outPacket,
  int inMaxLen)
{
  if (outPacket == NULL || inFilename == NULL || inMaxLen < 72) return 0 ;

  outPacket[0] = kLoRaMagic ;
  outPacket[1] = kLoRaPacketCommand ;
  outPacket[2] = inTargetRocketId ;

  if (inCommandType == kUsbCmdSdRead)
  {
    outPacket[3] = kCmdSdRead ;
  }
  else if (inCommandType == kUsbCmdFlashRead)
  {
    outPacket[3] = kCmdFlashRead ;
  }
  else
  {
    return 0 ;
  }

  // Add offset (4 bytes, little-endian)
  outPacket[4] = (uint8_t)(inOffset & 0xFF) ;
  outPacket[5] = (uint8_t)((inOffset >> 8) & 0xFF) ;
  outPacket[6] = (uint8_t)((inOffset >> 16) & 0xFF) ;
  outPacket[7] = (uint8_t)((inOffset >> 24) & 0xFF) ;

  // Add filename (up to 63 chars + null)
  int theNameLen = strlen(inFilename) ;
  if (theNameLen > 63) theNameLen = 63 ;
  memcpy(&outPacket[8], inFilename, theNameLen) ;
  outPacket[8 + theNameLen] = '\0' ;

  return 8 + theNameLen + 1 ;
}

//----------------------------------------------
// Function: GatewayProtocol_BuildStorageDeleteCommand
// Purpose: Build LoRa command for sd_delete or flash_delete
//----------------------------------------------
int GatewayProtocol_BuildStorageDeleteCommand(
  UsbCommandType inCommandType,
  uint8_t inTargetRocketId,
  const char * inFilename,
  uint8_t * outPacket,
  int inMaxLen)
{
  if (outPacket == NULL || inFilename == NULL || inMaxLen < 68) return 0 ;

  outPacket[0] = kLoRaMagic ;
  outPacket[1] = kLoRaPacketCommand ;
  outPacket[2] = inTargetRocketId ;

  if (inCommandType == kUsbCmdSdDelete)
  {
    outPacket[3] = kCmdSdDelete ;
  }
  else if (inCommandType == kUsbCmdFlashDelete)
  {
    outPacket[3] = kCmdFlashDelete ;
  }
  else
  {
    return 0 ;
  }

  // Add filename (up to 63 chars + null)
  int theNameLen = strlen(inFilename) ;
  if (theNameLen > 63) theNameLen = 63 ;
  memcpy(&outPacket[4], inFilename, theNameLen) ;
  outPacket[4 + theNameLen] = '\0' ;

  return 4 + theNameLen + 1 ;
}

//----------------------------------------------
// Function: GatewayProtocol_BuildStatusJson
//----------------------------------------------
int GatewayProtocol_BuildStatusJson(
  const GatewayState * inState,
  uint32_t inCommandId,
  char * outJson,
  int inMaxLen)
{
  if (inState == NULL || outJson == NULL || inMaxLen < 64) return 0 ;

  int theLen = snprintf(outJson, inMaxLen,
    "{\"type\":\"status\","
    "\"id\":%lu,"
    "\"connected\":%s,"
    "\"rx\":%lu,"
    "\"tx\":%lu,"
    "\"rssi\":%d,"
    "\"snr\":%d}\n",
    (unsigned long)inCommandId,
    inState->pConnected ? "true" : "false",
    (unsigned long)inState->pPacketsReceived,
    (unsigned long)inState->pPacketsSent,
    inState->pLastRssi,
    inState->pLastSnr) ;

  return theLen ;
}

//----------------------------------------------
// Function: GatewayProtocol_BuildAckJson
//----------------------------------------------
int GatewayProtocol_BuildAckJson(
  uint32_t inCommandId,
  bool inSuccess,
  char * outJson,
  int inMaxLen)
{
  if (outJson == NULL || inMaxLen < 32) return 0 ;

  int theLen = snprintf(outJson, inMaxLen,
    "{\"type\":\"ack\",\"id\":%lu,\"ok\":%s}\n",
    (unsigned long)inCommandId,
    inSuccess ? "true" : "false") ;

  return theLen ;
}

//----------------------------------------------
// Function: GatewayProtocol_GetStateName
//----------------------------------------------
const char * GatewayProtocol_GetStateName(uint8_t inState)
{
  if (inState > 7) return "unknown" ;
  return sFlightStateNames[inState] ;
}

//----------------------------------------------
// Function: GatewayProtocol_ParseStorageParams
//----------------------------------------------
bool GatewayProtocol_ParseStorageParams(
  const char * inJson,
  char * outFilename,
  uint32_t * outOffset)
{
  if (inJson == NULL || outFilename == NULL || outOffset == NULL) return false ;

  outFilename[0] = '\0' ;
  *outOffset = 0 ;

  // Find filename: "file":"..."
  const char * theFileStart = strstr(inJson, "\"file\":\"") ;
  if (theFileStart != NULL)
  {
    theFileStart += 8 ;  // Skip past "file":"
    const char * theFileEnd = strchr(theFileStart, '"') ;
    if (theFileEnd != NULL)
    {
      int theLen = theFileEnd - theFileStart ;
      if (theLen > 63) theLen = 63 ;
      memcpy(outFilename, theFileStart, theLen) ;
      outFilename[theLen] = '\0' ;
    }
  }

  // Find offset: "offset":123
  const char * theOffsetStart = strstr(inJson, "\"offset\":") ;
  if (theOffsetStart != NULL)
  {
    theOffsetStart += 9 ;  // Skip past "offset":
    *outOffset = (uint32_t)strtoul(theOffsetStart, NULL, 10) ;
  }

  return (outFilename[0] != '\0') ;
}

//----------------------------------------------
// Function: GatewayProtocol_StorageListToJson
// Packet format: magic, type, count, then for each file:
//   name_len(1), name(n), size(4), year(2), month(1), day(1), hour(1), minute(1)
//----------------------------------------------
int GatewayProtocol_StorageListToJson(
  const uint8_t * inPacket,
  int inLen,
  bool inIsSd,
  char * outJson,
  int inMaxLen)
{
  if (inPacket == NULL || outJson == NULL || inLen < 3 || inMaxLen < 64) return 0 ;

  uint8_t theCount = inPacket[2] ;
  int theOffset = 3 ;

  // Start JSON array
  int theJsonLen = snprintf(outJson, inMaxLen,
    "{\"type\":\"%s_list\",\"files\":[",
    inIsSd ? "sd" : "flash") ;

  // Parse each file entry
  for (uint8_t i = 0 ; i < theCount && theOffset < inLen ; i++)
  {
    // Get name length
    uint8_t theNameLen = inPacket[theOffset++] ;
    if (theOffset + theNameLen + 10 > inLen) break ;

    // Get filename
    char theName[65] ;
    if (theNameLen > 64) theNameLen = 64 ;
    memcpy(theName, &inPacket[theOffset], theNameLen) ;
    theName[theNameLen] = '\0' ;
    theOffset += theNameLen ;

    // Get size (4 bytes, little-endian)
    uint32_t theSize = inPacket[theOffset] |
                       (inPacket[theOffset + 1] << 8) |
                       (inPacket[theOffset + 2] << 16) |
                       (inPacket[theOffset + 3] << 24) ;
    theOffset += 4 ;

    // Get date (year=2 bytes, month, day, hour, minute)
    uint16_t theYear = inPacket[theOffset] | (inPacket[theOffset + 1] << 8) ;
    theOffset += 2 ;
    uint8_t theMonth = inPacket[theOffset++] ;
    uint8_t theDay = inPacket[theOffset++] ;
    uint8_t theHour = inPacket[theOffset++] ;
    uint8_t theMinute = inPacket[theOffset++] ;

    // Add file entry to JSON
    if (i > 0) theJsonLen += snprintf(outJson + theJsonLen, inMaxLen - theJsonLen, ",") ;
    theJsonLen += snprintf(outJson + theJsonLen, inMaxLen - theJsonLen,
      "{\"name\":\"%s\",\"size\":%lu,\"date\":\"%04u-%02u-%02u %02u:%02u\"}",
      theName,
      (unsigned long)theSize,
      theYear, theMonth, theDay, theHour, theMinute) ;
  }

  // Close JSON
  theJsonLen += snprintf(outJson + theJsonLen, inMaxLen - theJsonLen, "]}\n") ;

  return theJsonLen ;
}

//----------------------------------------------
// Function: GatewayProtocol_StorageDataToJson
// Packet format: magic, type, offset(4), total(4), chunk_len(2), data(n)
//----------------------------------------------
int GatewayProtocol_StorageDataToJson(
  const uint8_t * inPacket,
  int inLen,
  bool inIsSd,
  char * outJson,
  int inMaxLen)
{
  if (inPacket == NULL || outJson == NULL || inLen < 12 || inMaxLen < 64) return 0 ;

  // Parse header
  uint32_t theOffset = inPacket[2] |
                       (inPacket[3] << 8) |
                       (inPacket[4] << 16) |
                       (inPacket[5] << 24) ;

  uint32_t theTotal = inPacket[6] |
                      (inPacket[7] << 8) |
                      (inPacket[8] << 16) |
                      (inPacket[9] << 24) ;

  uint16_t theChunkLen = inPacket[10] | (inPacket[11] << 8) ;

  // Verify chunk length matches packet
  if (theChunkLen > inLen - 12) theChunkLen = inLen - 12 ;

  // Base64 encode the chunk data (for JSON transport)
  // For simplicity, output as hex string
  int theJsonLen = snprintf(outJson, inMaxLen,
    "{\"type\":\"%s_data\",\"offset\":%lu,\"total\":%lu,\"len\":%u,\"data\":\"",
    inIsSd ? "sd" : "flash",
    (unsigned long)theOffset,
    (unsigned long)theTotal,
    theChunkLen) ;

  // Encode data as hex
  const uint8_t * theData = &inPacket[12] ;
  for (uint16_t i = 0 ; i < theChunkLen && theJsonLen < inMaxLen - 10 ; i++)
  {
    theJsonLen += snprintf(outJson + theJsonLen, inMaxLen - theJsonLen, "%02X", theData[i]) ;
  }

  theJsonLen += snprintf(outJson + theJsonLen, inMaxLen - theJsonLen, "\"}\n") ;

  return theJsonLen ;
}

//----------------------------------------------
// Function: GatewayProtocol_ParseOrientationModeEnabled
//----------------------------------------------
bool GatewayProtocol_ParseOrientationModeEnabled(
  const char * inJson,
  bool * outEnabled)
{
  if (inJson == NULL || outEnabled == NULL) return false ;

  // Look for "enabled":true or "enabled":false
  const char * theEnabledStart = strstr(inJson, "\"enabled\":") ;
  if (theEnabledStart == NULL) return false ;

  theEnabledStart += 10 ;  // Skip past "enabled":

  // Skip whitespace
  while (*theEnabledStart == ' ') theEnabledStart++ ;

  if (strncmp(theEnabledStart, "true", 4) == 0)
  {
    *outEnabled = true ;
    return true ;
  }
  else if (strncmp(theEnabledStart, "false", 5) == 0)
  {
    *outEnabled = false ;
    return true ;
  }

  return false ;
}

//----------------------------------------------
// Function: GatewayProtocol_BuildOrientationModeCommand
//----------------------------------------------
int GatewayProtocol_BuildOrientationModeCommand(
  uint8_t inTargetRocketId,
  bool inEnabled,
  uint8_t * outPacket,
  int inMaxLen)
{
  if (outPacket == NULL || inMaxLen < 5) return 0 ;

  outPacket[0] = kLoRaMagic ;
  outPacket[1] = kLoRaPacketCommand ;
  outPacket[2] = inTargetRocketId ;
  outPacket[3] = kCmdOrientationMode ;
  outPacket[4] = inEnabled ? 1 : 0 ;

  return 5 ;
}

//----------------------------------------------
// Function: GatewayProtocol_ParseFlashParams
//----------------------------------------------
bool GatewayProtocol_ParseFlashParams(
  const char * inJson,
  uint8_t * outSlot,
  uint32_t * outSample)
{
  if (inJson == NULL || outSlot == NULL || outSample == NULL) return false ;

  *outSlot = 0 ;
  *outSample = 0 ;
  bool theHasSlot = false ;

  // Find slot: "slot":N
  const char * theSlotStart = strstr(inJson, "\"slot\":") ;
  if (theSlotStart != NULL)
  {
    theSlotStart += 7 ;  // Skip past "slot":
    *outSlot = (uint8_t)strtoul(theSlotStart, NULL, 10) ;
    theHasSlot = true ;
  }

  // Find sample: "sample":N
  const char * theSampleStart = strstr(inJson, "\"sample\":") ;
  if (theSampleStart != NULL)
  {
    theSampleStart += 9 ;  // Skip past "sample":
    *outSample = (uint32_t)strtoul(theSampleStart, NULL, 10) ;
  }

  // Check for header request: "header":true
  const char * theHeaderStart = strstr(inJson, "\"header\":true") ;
  if (theHeaderStart != NULL)
  {
    *outSample = 0xFFFFFFFF ;  // Special value for header request
  }

  return theHasSlot ;
}

//----------------------------------------------
// Function: GatewayProtocol_BuildFlashReadCommand
//----------------------------------------------
int GatewayProtocol_BuildFlashReadCommand(
  uint8_t inTargetRocketId,
  uint8_t inSlot,
  uint32_t inSample,
  uint8_t * outPacket,
  int inMaxLen)
{
  if (outPacket == NULL || inMaxLen < 9) return 0 ;

  outPacket[0] = kLoRaMagic ;
  outPacket[1] = kLoRaPacketCommand ;
  outPacket[2] = inTargetRocketId ;
  outPacket[3] = kCmdFlashRead ;
  outPacket[4] = inSlot ;

  // Sample index (4 bytes, little-endian)
  outPacket[5] = (uint8_t)(inSample & 0xFF) ;
  outPacket[6] = (uint8_t)((inSample >> 8) & 0xFF) ;
  outPacket[7] = (uint8_t)((inSample >> 16) & 0xFF) ;
  outPacket[8] = (uint8_t)((inSample >> 24) & 0xFF) ;

  return 9 ;
}

//----------------------------------------------
// Function: GatewayProtocol_BuildFlashDeleteCommand
//----------------------------------------------
int GatewayProtocol_BuildFlashDeleteCommand(
  uint8_t inTargetRocketId,
  uint8_t inSlot,
  uint8_t * outPacket,
  int inMaxLen)
{
  if (outPacket == NULL || inMaxLen < 5) return 0 ;

  outPacket[0] = kLoRaMagic ;
  outPacket[1] = kLoRaPacketCommand ;
  outPacket[2] = inTargetRocketId ;
  outPacket[3] = kCmdFlashDelete ;
  outPacket[4] = inSlot ;  // 0-6 for specific slot, 255 (0xFF) for all

  return 5 ;
}

//----------------------------------------------
// Function: GatewayProtocol_FlashListToJson
// Flight computer packet format:
//   magic, type, count, then for each flight:
//   slot(1), flightId(4), maxAltCm(4), flightTimeMs(4), sampleCount(4)
//----------------------------------------------
int GatewayProtocol_FlashListToJson(
  const uint8_t * inPacket,
  int inLen,
  char * outJson,
  int inMaxLen)
{
  if (inPacket == NULL || outJson == NULL || inLen < 3 || inMaxLen < 64) return 0 ;

  uint8_t theCount = inPacket[2] ;
  int theOffset = 3 ;

  // Start JSON
  int theJsonLen = snprintf(outJson, inMaxLen,
    "{\"type\":\"flash_list\",\"count\":%u,\"flights\":[", theCount) ;

  // Parse each flight entry (17 bytes each)
  for (uint8_t i = 0 ; i < theCount && theOffset + 17 <= inLen ; i++)
  {
    uint8_t theSlot = inPacket[theOffset++] ;

    uint32_t theFlightId = inPacket[theOffset] |
                           (inPacket[theOffset + 1] << 8) |
                           (inPacket[theOffset + 2] << 16) |
                           (inPacket[theOffset + 3] << 24) ;
    theOffset += 4 ;

    int32_t theMaxAltCm = inPacket[theOffset] |
                          (inPacket[theOffset + 1] << 8) |
                          (inPacket[theOffset + 2] << 16) |
                          (inPacket[theOffset + 3] << 24) ;
    theOffset += 4 ;

    uint32_t theFlightTimeMs = inPacket[theOffset] |
                               (inPacket[theOffset + 1] << 8) |
                               (inPacket[theOffset + 2] << 16) |
                               (inPacket[theOffset + 3] << 24) ;
    theOffset += 4 ;

    uint32_t theSampleCount = inPacket[theOffset] |
                              (inPacket[theOffset + 1] << 8) |
                              (inPacket[theOffset + 2] << 16) |
                              (inPacket[theOffset + 3] << 24) ;
    theOffset += 4 ;

    // Add flight entry to JSON
    if (i > 0) theJsonLen += snprintf(outJson + theJsonLen, inMaxLen - theJsonLen, ",") ;
    theJsonLen += snprintf(outJson + theJsonLen, inMaxLen - theJsonLen,
      "{\"slot\":%u,\"id\":%lu,\"alt\":%.1f,\"time\":%lu,\"samples\":%lu}",
      theSlot,
      (unsigned long)theFlightId,
      theMaxAltCm / 100.0f,
      (unsigned long)theFlightTimeMs,
      (unsigned long)theSampleCount) ;
  }

  // Close JSON
  theJsonLen += snprintf(outJson + theJsonLen, inMaxLen - theJsonLen, "]}\n") ;

  return theJsonLen ;
}

//----------------------------------------------
// Function: GatewayProtocol_FlashDataToJson
// Flight computer packet format:
//   magic, type, slot(1), startSample(4), totalSamples(4), count(1), samples...
// Each sample is 52 bytes (FlightSample structure)
//----------------------------------------------
int GatewayProtocol_FlashDataToJson(
  const uint8_t * inPacket,
  int inLen,
  char * outJson,
  int inMaxLen)
{
  if (inPacket == NULL || outJson == NULL || inLen < 12 || inMaxLen < 128) return 0 ;

  uint8_t theSlot = inPacket[2] ;

  uint32_t theStartSample = inPacket[3] |
                            (inPacket[4] << 8) |
                            (inPacket[5] << 16) |
                            (inPacket[6] << 24) ;

  uint32_t theTotalSamples = inPacket[7] |
                             (inPacket[8] << 8) |
                             (inPacket[9] << 16) |
                             (inPacket[10] << 24) ;

  uint8_t theSampleCount = inPacket[11] ;

  // Check if this is a header packet (startSample == 0xFFFFFFFF)
  if (theStartSample == 0xFFFFFFFF)
  {
    // Header packet - data is FlightHeader structure (80 bytes)
    // For simplicity, encode the raw header as hex
    int theJsonLen = snprintf(outJson, inMaxLen,
      "{\"type\":\"flash_header\",\"slot\":%u,\"data\":\"",
      theSlot) ;

    int theDataLen = inLen - 7 ;  // After magic, type, slot, startSample marker
    const uint8_t * theData = &inPacket[7] ;

    for (int i = 0 ; i < theDataLen && theJsonLen < inMaxLen - 10 ; i++)
    {
      theJsonLen += snprintf(outJson + theJsonLen, inMaxLen - theJsonLen, "%02X", theData[i]) ;
    }

    theJsonLen += snprintf(outJson + theJsonLen, inMaxLen - theJsonLen, "\"}\n") ;
    return theJsonLen ;
  }

  // Sample data packet - encode samples as hex for transport
  int theJsonLen = snprintf(outJson, inMaxLen,
    "{\"type\":\"flash_data\",\"slot\":%u,\"start\":%lu,\"total\":%lu,\"count\":%u,\"data\":\"",
    theSlot,
    (unsigned long)theStartSample,
    (unsigned long)theTotalSamples,
    theSampleCount) ;

  // Encode sample data as hex
  int theDataOffset = 12 ;  // After header
  int theDataLen = inLen - theDataOffset ;
  const uint8_t * theData = &inPacket[theDataOffset] ;

  for (int i = 0 ; i < theDataLen && theJsonLen < inMaxLen - 10 ; i++)
  {
    theJsonLen += snprintf(outJson + theJsonLen, inMaxLen - theJsonLen, "%02X", theData[i]) ;
  }

  theJsonLen += snprintf(outJson + theJsonLen, inMaxLen - theJsonLen, "\"}\n") ;

  return theJsonLen ;
}

//----------------------------------------------
// Function: GatewayProtocol_ParseWifiAddParams
//----------------------------------------------
bool GatewayProtocol_ParseWifiAddParams(
  const char * inJson,
  char * outSsid,
  char * outPassword,
  uint8_t * outPriority)
{
  if (inJson == NULL || outSsid == NULL || outPassword == NULL || outPriority == NULL)
  {
    return false ;
  }

  outSsid[0] = '\0' ;
  outPassword[0] = '\0' ;
  *outPriority = 0 ;

  // Find SSID: "ssid":"..."
  const char * theSsidStart = strstr(inJson, "\"ssid\":\"") ;
  if (theSsidStart == NULL) return false ;
  theSsidStart += 8 ;  // Skip past "ssid":"
  const char * theSsidEnd = strchr(theSsidStart, '"') ;
  if (theSsidEnd == NULL) return false ;
  int theSsidLen = theSsidEnd - theSsidStart ;
  if (theSsidLen > 32) theSsidLen = 32 ;
  memcpy(outSsid, theSsidStart, theSsidLen) ;
  outSsid[theSsidLen] = '\0' ;

  // Find password: "password":"..."
  const char * thePassStart = strstr(inJson, "\"password\":\"") ;
  if (thePassStart != NULL)
  {
    thePassStart += 12 ;  // Skip past "password":"
    const char * thePassEnd = strchr(thePassStart, '"') ;
    if (thePassEnd != NULL)
    {
      int thePassLen = thePassEnd - thePassStart ;
      if (thePassLen > 64) thePassLen = 64 ;
      memcpy(outPassword, thePassStart, thePassLen) ;
      outPassword[thePassLen] = '\0' ;
    }
  }

  // Find priority: "priority":N
  const char * thePriorityStart = strstr(inJson, "\"priority\":") ;
  if (thePriorityStart != NULL)
  {
    thePriorityStart += 11 ;  // Skip past "priority":
    *outPriority = (uint8_t)strtoul(thePriorityStart, NULL, 10) ;
  }

  return (outSsid[0] != '\0') ;
}

//----------------------------------------------
// Function: GatewayProtocol_ParseWifiRemoveParams
//----------------------------------------------
bool GatewayProtocol_ParseWifiRemoveParams(
  const char * inJson,
  uint8_t * outIndex)
{
  if (inJson == NULL || outIndex == NULL) return false ;

  *outIndex = 0 ;

  // Find index: "index":N
  const char * theIndexStart = strstr(inJson, "\"index\":") ;
  if (theIndexStart == NULL) return false ;

  theIndexStart += 8 ;  // Skip past "index":
  *outIndex = (uint8_t)strtoul(theIndexStart, NULL, 10) ;

  return true ;
}

//----------------------------------------------
// Function: GatewayProtocol_ParseWifiApParams
//----------------------------------------------
bool GatewayProtocol_ParseWifiApParams(
  const char * inJson,
  char * outSsid,
  char * outPassword,
  uint8_t * outChannel)
{
  if (inJson == NULL || outSsid == NULL || outPassword == NULL || outChannel == NULL)
  {
    return false ;
  }

  outSsid[0] = '\0' ;
  outPassword[0] = '\0' ;
  *outChannel = 0 ;

  bool theHasAnyParam = false ;

  // Find SSID: "ssid":"..."
  const char * theSsidStart = strstr(inJson, "\"ssid\":\"") ;
  if (theSsidStart != NULL)
  {
    theSsidStart += 8 ;  // Skip past "ssid":"
    const char * theSsidEnd = strchr(theSsidStart, '"') ;
    if (theSsidEnd != NULL)
    {
      int theSsidLen = theSsidEnd - theSsidStart ;
      if (theSsidLen > 32) theSsidLen = 32 ;
      memcpy(outSsid, theSsidStart, theSsidLen) ;
      outSsid[theSsidLen] = '\0' ;
      theHasAnyParam = true ;
    }
  }

  // Find password: "password":"..."
  const char * thePassStart = strstr(inJson, "\"password\":\"") ;
  if (thePassStart != NULL)
  {
    thePassStart += 12 ;  // Skip past "password":"
    const char * thePassEnd = strchr(thePassStart, '"') ;
    if (thePassEnd != NULL)
    {
      int thePassLen = thePassEnd - thePassStart ;
      if (thePassLen > 64) thePassLen = 64 ;
      memcpy(outPassword, thePassStart, thePassLen) ;
      outPassword[thePassLen] = '\0' ;
      theHasAnyParam = true ;
    }
  }

  // Find channel: "channel":N
  const char * theChannelStart = strstr(inJson, "\"channel\":") ;
  if (theChannelStart != NULL)
  {
    theChannelStart += 10 ;  // Skip past "channel":
    *outChannel = (uint8_t)strtoul(theChannelStart, NULL, 10) ;
    theHasAnyParam = true ;
  }

  return theHasAnyParam ;
}
