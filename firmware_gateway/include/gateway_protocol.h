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
#define kLoRaPacketStorageList  0x06  // Storage list response
#define kLoRaPacketStorageData  0x07  // Storage data chunk
#define kLoRaPacketInfo         0x08  // Device info response

//----------------------------------------------
// Command IDs (sent in kLoRaPacketCommand)
//----------------------------------------------
#define kCmdArm             0x01
#define kCmdDisarm          0x02
#define kCmdStatus          0x03
#define kCmdReset           0x04
#define kCmdDownload        0x05
#define kCmdPing            0x06
#define kCmdInfo            0x07  // Request device info

// Storage commands
#define kCmdSdList          0x10
#define kCmdSdRead          0x11
#define kCmdSdDelete        0x12
#define kCmdFlashList       0x20
#define kCmdFlashRead       0x21
#define kCmdFlashDelete     0x22

//----------------------------------------------
// LoRa Telemetry Packet (binary, 42 bytes)
// Must match flight_control.h exactly!
//----------------------------------------------
typedef struct __attribute__((packed))
{
  // Header (4 bytes)
  uint8_t pMagic ;                // 0xAF (Avionics Flight)
  uint8_t pPacketType ;           // 0x01 = telemetry
  uint16_t pSequence ;            // Packet sequence number

  // Time (4 bytes)
  uint32_t pTimeMs ;              // Mission time (ms)

  // Barometric data (12 bytes)
  int32_t pAltitudeCm ;           // Barometric altitude in centimeters
  int16_t pVelocityCmps ;         // Vertical velocity in cm/s
  uint32_t pPressurePa ;          // Pressure (Pa)
  int16_t pTemperatureC10 ;       // Temperature * 10 (0.1C resolution)

  // GPS data (13 bytes)
  int32_t pGpsLatitude ;          // Latitude in microdegrees (deg * 1e6)
  int32_t pGpsLongitude ;         // Longitude in microdegrees (deg * 1e6)
  int16_t pGpsSpeedCmps ;         // GPS ground speed in cm/s
  uint16_t pGpsHeadingDeg10 ;     // Heading * 10 (0-3600 = 0-360.0 deg)
  uint8_t pGpsSatellites ;        // Number of satellites in use

  // Accelerometer (6 bytes)
  int16_t pAccelX ;               // Accelerometer X (raw)
  int16_t pAccelY ;               // Accelerometer Y
  int16_t pAccelZ ;               // Accelerometer Z

  // Status (3 bytes)
  uint8_t pState ;                // Flight state enum
  uint8_t pFlags ;                // Status flags
  uint8_t pCrc ;                  // CRC-8
} LoRaTelemetryPacket ;

// Flags byte bit definitions
#define kFlagGpsFix             0x10  // GPS has valid fix

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
  kUsbCmdPing ,
  kUsbCmdInfo ,        // Request flight computer device info
  kUsbCmdGatewayInfo , // Request gateway device info
  // Storage commands (10+)
  kUsbCmdSdList = 10 ,
  kUsbCmdSdRead ,
  kUsbCmdSdDelete ,
  kUsbCmdFlashList = 20 ,
  kUsbCmdFlashRead ,
  kUsbCmdFlashDelete
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
//   inGroundPressurePa - Ground barometer pressure (0 if not available)
//   outJson - Buffer for JSON string
//   inMaxLen - Maximum JSON length
// Returns: Length of JSON string
//----------------------------------------------
int GatewayProtocol_TelemetryToJson(
  const LoRaTelemetryPacket * inPacket,
  int16_t inRssi,
  int8_t inSnr,
  float inGroundPressurePa,
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

//----------------------------------------------
// Function: GatewayProtocol_BuildStorageReadCommand
// Purpose: Build LoRa command for sd_read or flash_read
// Parameters:
//   inCommandType - kUsbCmdSdRead or kUsbCmdFlashRead
//   inFilename - Filename to read
//   inOffset - Byte offset to start reading
//   outPacket - Buffer for packet data
//   inMaxLen - Maximum packet length
// Returns: Packet length
//----------------------------------------------
int GatewayProtocol_BuildStorageReadCommand(
  UsbCommandType inCommandType,
  const char * inFilename,
  uint32_t inOffset,
  uint8_t * outPacket,
  int inMaxLen) ;

//----------------------------------------------
// Function: GatewayProtocol_BuildStorageDeleteCommand
// Purpose: Build LoRa command for sd_delete or flash_delete
// Parameters:
//   inCommandType - kUsbCmdSdDelete or kUsbCmdFlashDelete
//   inFilename - Filename to delete
//   outPacket - Buffer for packet data
//   inMaxLen - Maximum packet length
// Returns: Packet length
//----------------------------------------------
int GatewayProtocol_BuildStorageDeleteCommand(
  UsbCommandType inCommandType,
  const char * inFilename,
  uint8_t * outPacket,
  int inMaxLen) ;

//----------------------------------------------
// Function: GatewayProtocol_ParseStorageParams
// Purpose: Parse filename and offset from JSON command
// Parameters:
//   inJson - JSON string to parse
//   outFilename - Buffer for filename (64 bytes)
//   outOffset - Pointer to offset value
// Returns: true if parameters found
//----------------------------------------------
bool GatewayProtocol_ParseStorageParams(
  const char * inJson,
  char * outFilename,
  uint32_t * outOffset) ;

//----------------------------------------------
// Function: GatewayProtocol_StorageListToJson
// Purpose: Convert storage list LoRa packet to JSON
// Parameters:
//   inPacket - Binary packet data
//   inLen - Packet length
//   inIsSd - true for SD card, false for flash
//   outJson - Buffer for JSON string
//   inMaxLen - Maximum JSON length
// Returns: Length of JSON string
//----------------------------------------------
int GatewayProtocol_StorageListToJson(
  const uint8_t * inPacket,
  int inLen,
  bool inIsSd,
  char * outJson,
  int inMaxLen) ;

//----------------------------------------------
// Function: GatewayProtocol_StorageDataToJson
// Purpose: Convert storage data chunk LoRa packet to JSON
// Parameters:
//   inPacket - Binary packet data
//   inLen - Packet length
//   inIsSd - true for SD card, false for flash
//   outJson - Buffer for JSON string
//   inMaxLen - Maximum JSON length
// Returns: Length of JSON string
//----------------------------------------------
int GatewayProtocol_StorageDataToJson(
  const uint8_t * inPacket,
  int inLen,
  bool inIsSd,
  char * outJson,
  int inMaxLen) ;
