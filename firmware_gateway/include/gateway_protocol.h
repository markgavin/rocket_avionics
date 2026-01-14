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
#define kCmdOrientationMode 0x08  // Enable/disable high-rate orientation testing

// Storage commands
#define kCmdSdList          0x10
#define kCmdSdRead          0x11
#define kCmdSdDelete        0x12
#define kCmdFlashList       0x20
#define kCmdFlashRead       0x21
#define kCmdFlashDelete     0x22

//----------------------------------------------
// LoRa Telemetry Packet (binary, 54 bytes)
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

  // Accelerometer (6 bytes) - values in milli-g (mg)
  int16_t pAccelX ;               // Accelerometer X
  int16_t pAccelY ;               // Accelerometer Y
  int16_t pAccelZ ;               // Accelerometer Z

  // Gyroscope (6 bytes) - values in 0.1 degrees/second
  int16_t pGyroX ;                // Gyroscope X
  int16_t pGyroY ;                // Gyroscope Y
  int16_t pGyroZ ;                // Gyroscope Z

  // Magnetometer (6 bytes) - values in milligauss
  int16_t pMagX ;                 // Magnetometer X
  int16_t pMagY ;                 // Magnetometer Y
  int16_t pMagZ ;                 // Magnetometer Z

  // Status (3 bytes)
  uint8_t pState ;                // Flight state enum
  uint8_t pFlags ;                // Status flags
  uint8_t pCrc ;                  // CRC-8
} LoRaTelemetryPacket ;

// Flags byte bit definitions
#define kFlagGpsFix             0x10  // GPS has valid fix
#define kFlagOrientationMode    0x80  // Orientation testing mode active

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
  kUsbCmdInfo ,            // Request flight computer device info
  kUsbCmdGatewayInfo ,     // Request gateway device info
  kUsbCmdOrientationMode , // Enable/disable orientation testing mode
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
//   inGwGpsValid - Gateway GPS has valid fix
//   inGwGpsLat - Gateway GPS latitude
//   inGwGpsLon - Gateway GPS longitude
//   outJson - Buffer for JSON string
//   inMaxLen - Maximum JSON length
// Returns: Length of JSON string
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

//----------------------------------------------
// Function: GatewayProtocol_ParseOrientationModeEnabled
// Purpose: Parse enabled flag from orientation_mode command
// Parameters:
//   inJson - JSON string to parse
//   outEnabled - Pointer to store enabled flag
// Returns: true if enabled flag found
//----------------------------------------------
bool GatewayProtocol_ParseOrientationModeEnabled(
  const char * inJson,
  bool * outEnabled) ;

//----------------------------------------------
// Function: GatewayProtocol_BuildOrientationModeCommand
// Purpose: Build LoRa command for orientation mode
// Parameters:
//   inEnabled - Enable or disable orientation mode
//   outPacket - Buffer for packet data
//   inMaxLen - Maximum packet length
// Returns: Packet length
//----------------------------------------------
int GatewayProtocol_BuildOrientationModeCommand(
  bool inEnabled,
  uint8_t * outPacket,
  int inMaxLen) ;

//----------------------------------------------
// Function: GatewayProtocol_ParseFlashParams
// Purpose: Parse slot and sample offset from JSON command
// Parameters:
//   inJson - JSON string to parse
//   outSlot - Pointer to slot number (0-6, or 255 for delete all)
//   outSample - Pointer to sample index
// Returns: true if parameters found
//----------------------------------------------
bool GatewayProtocol_ParseFlashParams(
  const char * inJson,
  uint8_t * outSlot,
  uint32_t * outSample) ;

//----------------------------------------------
// Function: GatewayProtocol_BuildFlashReadCommand
// Purpose: Build LoRa command for flash_read
// Parameters:
//   inSlot - Flight slot (0-6)
//   inSample - Starting sample index (0xFFFFFFFF for header)
//   outPacket - Buffer for packet data
//   inMaxLen - Maximum packet length
// Returns: Packet length
//----------------------------------------------
int GatewayProtocol_BuildFlashReadCommand(
  uint8_t inSlot,
  uint32_t inSample,
  uint8_t * outPacket,
  int inMaxLen) ;

//----------------------------------------------
// Function: GatewayProtocol_BuildFlashDeleteCommand
// Purpose: Build LoRa command for flash_delete
// Parameters:
//   inSlot - Flight slot to delete (0-6, or 255 for all)
//   outPacket - Buffer for packet data
//   inMaxLen - Maximum packet length
// Returns: Packet length
//----------------------------------------------
int GatewayProtocol_BuildFlashDeleteCommand(
  uint8_t inSlot,
  uint8_t * outPacket,
  int inMaxLen) ;

//----------------------------------------------
// Function: GatewayProtocol_FlashListToJson
// Purpose: Convert flash storage list LoRa packet to JSON
// Parameters:
//   inPacket - Binary packet data
//   inLen - Packet length
//   outJson - Buffer for JSON string
//   inMaxLen - Maximum JSON length
// Returns: Length of JSON string
//----------------------------------------------
int GatewayProtocol_FlashListToJson(
  const uint8_t * inPacket,
  int inLen,
  char * outJson,
  int inMaxLen) ;

//----------------------------------------------
// Function: GatewayProtocol_FlashDataToJson
// Purpose: Convert flash data chunk LoRa packet to JSON
// Parameters:
//   inPacket - Binary packet data
//   inLen - Packet length
//   outJson - Buffer for JSON string
//   inMaxLen - Maximum JSON length
// Returns: Length of JSON string
//----------------------------------------------
int GatewayProtocol_FlashDataToJson(
  const uint8_t * inPacket,
  int inLen,
  char * outJson,
  int inMaxLen) ;
