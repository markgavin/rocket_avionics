//----------------------------------------------
// Module: gateway_protocol.h
// Description: Gateway protocol handler for
//   LoRa to USB serial bridge
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// LoRa Packet Types (must match flight_control.h)
//----------------------------------------------
#define kLoRaMagic              0xAF
#define kLoRaPacketTelemetry    0x01
#define kLoRaPacketStatus       0x02
#define kLoRaPacketCommand      0x03
#define kLoRaPacketAck          0x04
#define kLoRaPacketData         0x05

//----------------------------------------------
// LoRa Telemetry Packet (binary, 28 bytes)
// Must match flight_control.h exactly!
//----------------------------------------------
typedef struct __attribute__((packed))
{
  uint8_t pMagic ;                // 0xAF (Avionics Flight)
  uint8_t pPacketType ;           // 0x01 = telemetry
  uint16_t pSequence ;            // Packet sequence number
  uint32_t pTimeMs ;              // Mission time (ms)
  int32_t pAltitudeCm ;           // Altitude in centimeters
  int16_t pVelocityCmps ;         // Velocity in cm/s
  uint32_t pPressurePa ;          // Pressure (Pa)
  int16_t pTemperatureC10 ;       // Temperature * 10 (0.1C resolution)
  int16_t pAccelX ;               // Accelerometer X (raw)
  int16_t pAccelY ;               // Accelerometer Y
  int16_t pAccelZ ;               // Accelerometer Z
  uint8_t pState ;                // Flight state enum
  uint8_t pFlags ;                // Status flags
  uint8_t pCrc ;                  // CRC-8
} LoRaTelemetryPacket ;

//----------------------------------------------
// USB Command Types (from desktop app)
//----------------------------------------------
typedef enum
{
  kUsbCmdArm = 1 ,
  kUsbCmdDisarm ,
  kUsbCmdStatus ,
  kUsbCmdDownload ,
  kUsbCmdReset ,
  kUsbCmdPing
} UsbCommandType ;

//----------------------------------------------
// Gateway State
//----------------------------------------------
typedef struct
{
  bool pConnected ;               // LoRa link active
  uint32_t pLastPacketTimeMs ;    // Time of last received packet
  uint32_t pPacketsReceived ;     // Total packets received
  uint32_t pPacketsSent ;         // Total packets sent
  uint32_t pPacketsLost ;         // Packets lost (CRC errors, etc.)
  int16_t pLastRssi ;             // RSSI of last packet
  int8_t pLastSnr ;               // SNR of last packet
} GatewayState ;

//----------------------------------------------
// Function: GatewayProtocol_Init
// Purpose: Initialize the gateway protocol handler
//----------------------------------------------
void GatewayProtocol_Init(GatewayState * ioState) ;

//----------------------------------------------
// Function: GatewayProtocol_TelemetryToJson
// Purpose: Convert LoRa telemetry packet to JSON
// Parameters:
//   inPacket - Binary telemetry packet
//   inRssi - RSSI of received packet
//   inSnr - SNR of received packet
//   outJson - Buffer for JSON string
//   inMaxLen - Maximum JSON length
// Returns: Length of JSON string
//----------------------------------------------
int GatewayProtocol_TelemetryToJson(
  const LoRaTelemetryPacket * inPacket,
  int16_t inRssi,
  int8_t inSnr,
  char * outJson,
  int inMaxLen) ;

//----------------------------------------------
// Function: GatewayProtocol_ParseCommand
// Purpose: Parse USB JSON command
// Parameters:
//   inJson - JSON command string
//   outCommandType - Parsed command type
//   outCommandId - Command ID for response
// Returns: true if valid command parsed
//----------------------------------------------
bool GatewayProtocol_ParseCommand(
  const char * inJson,
  UsbCommandType * outCommandType,
  uint32_t * outCommandId) ;

//----------------------------------------------
// Function: GatewayProtocol_BuildLoRaCommand
// Purpose: Build binary LoRa command packet
// Parameters:
//   inCommandType - Command to send
//   outPacket - Buffer for packet data
//   inMaxLen - Maximum packet length
// Returns: Packet length
//----------------------------------------------
int GatewayProtocol_BuildLoRaCommand(
  UsbCommandType inCommandType,
  uint8_t * outPacket,
  int inMaxLen) ;

//----------------------------------------------
// Function: GatewayProtocol_BuildStatusJson
// Purpose: Build gateway status JSON response
// Parameters:
//   inState - Gateway state
//   inCommandId - Command ID for response
//   outJson - Buffer for JSON string
//   inMaxLen - Maximum JSON length
// Returns: Length of JSON string
//----------------------------------------------
int GatewayProtocol_BuildStatusJson(
  const GatewayState * inState,
  uint32_t inCommandId,
  char * outJson,
  int inMaxLen) ;

//----------------------------------------------
// Function: GatewayProtocol_BuildAckJson
// Purpose: Build command acknowledgment JSON
// Parameters:
//   inCommandId - Command ID being acknowledged
//   inSuccess - Whether command succeeded
//   outJson - Buffer for JSON string
//   inMaxLen - Maximum JSON length
// Returns: Length of JSON string
//----------------------------------------------
int GatewayProtocol_BuildAckJson(
  uint32_t inCommandId,
  bool inSuccess,
  char * outJson,
  int inMaxLen) ;

//----------------------------------------------
// Function: GatewayProtocol_GetStateName
// Purpose: Get flight state name string
// Parameters:
//   inState - Flight state code
// Returns: State name string
//----------------------------------------------
const char * GatewayProtocol_GetStateName(uint8_t inState) ;
