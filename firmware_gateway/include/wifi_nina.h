//----------------------------------------------
// Module: wifi_nina.h
// Description: NINA WiFi driver for AirLift FeatherWing
//   Implements SPI protocol for ESP32 co-processor
// Author: Mark Gavin
// Created: 2026-01-14
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit AirLift FeatherWing (4264)
//   - ESP32 running NINA firmware
//
// Note: Default AirLift pins conflict with OLED buttons.
//   Requires solder jumper modification on AirLift:
//   - Move CS from D13 to D10
//   - Move BUSY from D6 to D11
//   - Move RESET from D5 to D12
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// NINA SPI Protocol Constants
//----------------------------------------------
#define NINA_START_CMD          0xE0
#define NINA_END_CMD            0xEE
#define NINA_ERR_CMD            0xEF
#define NINA_REPLY_FLAG         0x80
#define NINA_DATA_FLAG          0x40

#define NINA_DUMMY_DATA         0xFF
#define NINA_CMD_POS            1
#define NINA_PARAM_LEN_POS      2

//----------------------------------------------
// NINA Command Set (subset for AP mode + TCP)
//----------------------------------------------

// WiFi commands
#define NINA_CMD_SET_NET            0x10
#define NINA_CMD_SET_PASSPHRASE     0x11
#define NINA_CMD_SET_KEY            0x12
#define NINA_CMD_SET_IP_CONFIG      0x14
#define NINA_CMD_SET_DNS_CONFIG     0x15
#define NINA_CMD_SET_HOSTNAME       0x16
#define NINA_CMD_SET_POWER_MODE     0x17
#define NINA_CMD_SET_AP_NET         0x18
#define NINA_CMD_SET_AP_PASSPHRASE  0x19
#define NINA_CMD_SET_DEBUG          0x1A
#define NINA_CMD_GET_TEMPERATURE    0x1B
#define NINA_CMD_GET_REASON_CODE    0x1F

// Connection commands
#define NINA_CMD_GET_CONN_STATUS    0x20
#define NINA_CMD_GET_IPADDR         0x21
#define NINA_CMD_GET_MACADDR        0x22
#define NINA_CMD_GET_CURR_SSID      0x23
#define NINA_CMD_GET_CURR_BSSID     0x24
#define NINA_CMD_GET_CURR_RSSI      0x25
#define NINA_CMD_GET_CURR_ENCT      0x26
#define NINA_CMD_SCAN_NETWORKS      0x27
#define NINA_CMD_START_SERVER_TCP   0x28
#define NINA_CMD_GET_STATE_TCP      0x29
#define NINA_CMD_DATA_SENT_TCP      0x2A
#define NINA_CMD_AVAIL_DATA_TCP     0x2B
#define NINA_CMD_GET_DATA_TCP       0x2C
#define NINA_CMD_START_CLIENT_TCP   0x2D
#define NINA_CMD_STOP_CLIENT_TCP    0x2E
#define NINA_CMD_GET_CLIENT_STATE   0x2F

// Extended commands
#define NINA_CMD_DISCONNECT         0x30
#define NINA_CMD_GET_IDX_RSSI       0x32
#define NINA_CMD_GET_IDX_ENCT       0x33
#define NINA_CMD_REQ_HOST_BY_NAME   0x34
#define NINA_CMD_GET_HOST_BY_NAME   0x35
#define NINA_CMD_START_SCAN_NET     0x36
#define NINA_CMD_GET_FW_VERSION     0x37
#define NINA_CMD_SEND_DATA_UDP      0x39
#define NINA_CMD_GET_REMOTE_DATA    0x3A
#define NINA_CMD_GET_TIME           0x3B
#define NINA_CMD_GET_IDX_BSSID      0x3C
#define NINA_CMD_GET_IDX_CHANNEL    0x3D
#define NINA_CMD_PING               0x3E
#define NINA_CMD_GET_SOCKET         0x3F

// Server/client commands
#define NINA_CMD_SET_CLIENT_SOCK    0x40
#define NINA_CMD_SET_SOCK_OPT       0x41
#define NINA_CMD_GET_SOCK_OPT       0x42
#define NINA_CMD_INSERT_DATA_BUF    0x44
#define NINA_CMD_SEND_DATA_TCP      0x45
#define NINA_CMD_GET_DATABUF_TCP    0x46
#define NINA_CMD_INSERT_DATABUF     0x47

// Digital/analog I/O (ESP32 GPIO)
#define NINA_CMD_SET_PIN_MODE       0x50
#define NINA_CMD_SET_DIGITAL_WRITE  0x51
#define NINA_CMD_SET_ANALOG_WRITE   0x52
#define NINA_CMD_GET_DIGITAL_READ   0x53
#define NINA_CMD_GET_ANALOG_READ    0x54

//----------------------------------------------
// WiFi Connection Status
//----------------------------------------------
typedef enum {
    kWifiStatusIdle             = 0,
    kWifiStatusNoSsidAvail      = 1,
    kWifiStatusScanCompleted    = 2,
    kWifiStatusConnected        = 3,
    kWifiStatusConnectFailed    = 4,
    kWifiStatusConnectionLost   = 5,
    kWifiStatusDisconnected     = 6,
    kWifiStatusApListening      = 7,
    kWifiStatusApConnected      = 8,
    kWifiStatusApFailed         = 9
} WifiStatus ;

//----------------------------------------------
// TCP Socket States
//----------------------------------------------
typedef enum {
    kTcpStateClosed         = 0,
    kTcpStateListen         = 1,
    kTcpStateSynSent        = 2,
    kTcpStateSynRcvd        = 3,
    kTcpStateEstablished    = 4,
    kTcpStateFinWait1       = 5,
    kTcpStateFinWait2       = 6,
    kTcpStateCloseWait      = 7,
    kTcpStateClosing        = 8,
    kTcpStateLastAck        = 9,
    kTcpStateTimeWait       = 10
} TcpState ;

//----------------------------------------------
// WiFi Encryption Types
//----------------------------------------------
typedef enum {
    kWifiEncryptNone        = 0,
    kWifiEncryptWep         = 1,
    kWifiEncryptWpaPsk      = 2,
    kWifiEncryptWpa2Psk     = 3,
    kWifiEncryptWpaWpa2Psk  = 4
} WifiEncryption ;

//----------------------------------------------
// Configuration Constants
//----------------------------------------------
#define WIFI_MAX_SSID_LEN       32
#define WIFI_MAX_PASS_LEN       64
#define WIFI_MAX_SOCKETS        4
#define WIFI_TCP_BUFFER_SIZE    1024
#define WIFI_SPI_TIMEOUT_MS     1000
#define WIFI_WAIT_READY_MS      10

//----------------------------------------------
// Function: WiFi_Init
// Purpose: Initialize WiFi hardware and SPI
// Returns: true if ESP32 responds, false on timeout
//----------------------------------------------
bool WiFi_Init(void) ;

//----------------------------------------------
// Function: WiFi_GetFirmwareVersion
// Purpose: Get NINA firmware version string
// Parameters:
//   outVersion - Buffer for version string (min 12 bytes)
// Returns: true if successful
//----------------------------------------------
bool WiFi_GetFirmwareVersion(char * outVersion) ;

//----------------------------------------------
// Function: WiFi_GetMacAddress
// Purpose: Get ESP32 MAC address
// Parameters:
//   outMac - Buffer for MAC address (6 bytes)
// Returns: true if successful
//----------------------------------------------
bool WiFi_GetMacAddress(uint8_t * outMac) ;

//----------------------------------------------
// Function: WiFi_Connect
// Purpose: Connect to WiFi network (station mode)
// Parameters:
//   inSsid - Network name
//   inPassword - WPA2 password (or NULL for open network)
// Returns: true if connection initiated (check status for result)
//----------------------------------------------
bool WiFi_Connect(const char * inSsid, const char * inPassword) ;

//----------------------------------------------
// Function: WiFi_WaitForConnection
// Purpose: Wait for WiFi connection to complete
// Parameters:
//   inTimeoutMs - Maximum wait time in milliseconds
// Returns: true if connected, false on timeout or failure
//----------------------------------------------
bool WiFi_WaitForConnection(uint32_t inTimeoutMs) ;

//----------------------------------------------
// Function: WiFi_GetSSID
// Purpose: Get current connected SSID
// Parameters:
//   outSsid - Buffer for SSID string (min 33 bytes)
// Returns: true if connected and SSID available
//----------------------------------------------
bool WiFi_GetSSID(char * outSsid) ;

//----------------------------------------------
// Function: WiFi_GetRSSI
// Purpose: Get current signal strength
// Returns: RSSI in dBm, or 0 if not connected
//----------------------------------------------
int8_t WiFi_GetRSSI(void) ;

//----------------------------------------------
// Function: WiFi_StartAP
// Purpose: Start WiFi Access Point
// Parameters:
//   inSsid - Access point name
//   inPassword - WPA2 password (min 8 chars, or NULL for open)
//   inChannel - WiFi channel (1-13)
// Returns: true if AP started successfully
//----------------------------------------------
bool WiFi_StartAP(const char * inSsid, const char * inPassword, uint8_t inChannel) ;

//----------------------------------------------
// Function: WiFi_GetStatus
// Purpose: Get current WiFi status
// Returns: WifiStatus enum value
//----------------------------------------------
WifiStatus WiFi_GetStatus(void) ;

//----------------------------------------------
// Function: WiFi_GetLocalIP
// Purpose: Get local IP address
// Parameters:
//   outIp - Buffer for IP address (4 bytes)
// Returns: true if connected and IP available
//----------------------------------------------
bool WiFi_GetLocalIP(uint8_t * outIp) ;

//----------------------------------------------
// Function: WiFi_StartServer
// Purpose: Start TCP server on specified port
// Parameters:
//   inPort - TCP port number
// Returns: Socket handle (0-3), or -1 on error
//----------------------------------------------
int8_t WiFi_StartServer(uint16_t inPort) ;

//----------------------------------------------
// Function: WiFi_ServerHasClient
// Purpose: Check if a client is connected to server
// Parameters:
//   inServerSocket - Server socket handle
// Returns: Client socket handle (0-3), or -1 if no client
//----------------------------------------------
int8_t WiFi_ServerHasClient(int8_t inServerSocket) ;

//----------------------------------------------
// Function: WiFi_ClientConnected
// Purpose: Check if client socket is connected
// Parameters:
//   inSocket - Socket handle
// Returns: true if connected
//----------------------------------------------
bool WiFi_ClientConnected(int8_t inSocket) ;

//----------------------------------------------
// Function: WiFi_Available
// Purpose: Get number of bytes available to read
// Parameters:
//   inSocket - Socket handle
// Returns: Number of bytes available
//----------------------------------------------
uint16_t WiFi_Available(int8_t inSocket) ;

//----------------------------------------------
// Function: WiFi_Read
// Purpose: Read data from socket
// Parameters:
//   inSocket - Socket handle
//   outBuffer - Buffer for received data
//   inMaxLen - Maximum bytes to read
// Returns: Number of bytes read, 0 if none, -1 on error
//----------------------------------------------
int WiFi_Read(int8_t inSocket, uint8_t * outBuffer, uint16_t inMaxLen) ;

//----------------------------------------------
// Function: WiFi_Write
// Purpose: Write data to socket
// Parameters:
//   inSocket - Socket handle
//   inData - Data to send
//   inLen - Number of bytes to send
// Returns: Number of bytes sent, -1 on error
//----------------------------------------------
int WiFi_Write(int8_t inSocket, const uint8_t * inData, uint16_t inLen) ;

//----------------------------------------------
// Function: WiFi_CloseClient
// Purpose: Close client connection
// Parameters:
//   inSocket - Socket handle
//----------------------------------------------
void WiFi_CloseClient(int8_t inSocket) ;

//----------------------------------------------
// Function: WiFi_Disconnect
// Purpose: Disconnect from WiFi network / stop AP
//----------------------------------------------
void WiFi_Disconnect(void) ;

//----------------------------------------------
// Function: WiFi_Poll
// Purpose: Process WiFi events (call regularly in main loop)
//----------------------------------------------
void WiFi_Poll(void) ;

//----------------------------------------------
// Function: WiFi_GetSocketState
// Purpose: Get TCP state of a socket
// Parameters:
//   inSocket - Socket handle
// Returns: TcpState enum value
//----------------------------------------------
uint8_t WiFi_GetSocketState(int8_t inSocket) ;

//----------------------------------------------
// Function: WiFi_DumpSockets
// Purpose: Debug output showing all socket states
//----------------------------------------------
void WiFi_DumpSockets(void) ;

