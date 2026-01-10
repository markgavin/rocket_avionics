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
  char * outJson,
  int inMaxLen)
{
  if (inPacket == NULL || outJson == NULL || inMaxLen < 128) return 0 ;

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

  // Build JSON telemetry message with GPS data and ground reference
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
    "\"state\":\"%s\","
    "\"flags\":%u,"
    "\"rssi\":%d,"
    "\"snr\":%d}\n",
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
    theStateName,
    inPacket->pFlags,
    inRssi,
    inSnr) ;

  return theLen ;
}

//----------------------------------------------
// Function: GatewayProtocol_ParseCommand
//----------------------------------------------
bool GatewayProtocol_ParseCommand(
  const char * inJson,
  UsbCommandType * outCommandType,
  uint32_t * outCommandId)
{
  if (inJson == NULL || outCommandType == NULL || outCommandId == NULL) return false ;

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

  return true ;
}

//----------------------------------------------
// Function: GatewayProtocol_BuildLoRaCommand
//----------------------------------------------
int GatewayProtocol_BuildLoRaCommand(
  UsbCommandType inCommandType,
  uint8_t * outPacket,
  int inMaxLen)
{
  if (outPacket == NULL || inMaxLen < 4) return 0 ;

  // Build simple command packet
  outPacket[0] = kLoRaMagic ;
  outPacket[1] = kLoRaPacketCommand ;

  switch (inCommandType)
  {
    case kUsbCmdArm:
      outPacket[2] = 0x01 ;
      break ;
    case kUsbCmdDisarm:
      outPacket[2] = 0x02 ;
      break ;
    case kUsbCmdStatus:
      outPacket[2] = 0x03 ;
      break ;
    case kUsbCmdReset:
      outPacket[2] = 0x04 ;
      break ;
    case kUsbCmdDownload:
      outPacket[2] = 0x05 ;
      break ;
    case kUsbCmdPing:
      outPacket[2] = 0x06 ;
      break ;
    default:
      return 0 ;
  }

  outPacket[3] = 0 ;  // Reserved / CRC placeholder

  return 4 ;
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
