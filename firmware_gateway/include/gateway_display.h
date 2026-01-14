//----------------------------------------------
// Module: gateway_display.h
// Description: Gateway Display UI for OLED
//   Shows connection status, signal, and telemetry
// Author: Mark Gavin
// Created: 2026-01-10
// Modified: 2026-01-13 (Switched to OLED display)
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit 128x64 OLED FeatherWing (4650)
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// Display Modes
//----------------------------------------------
typedef enum
{
  kGwDisplayModeTelemetry = 0 ,   // Live telemetry (default)
  kGwDisplayModeSignal ,          // Signal strength (RSSI/SNR)
  kGwDisplayModeStats ,           // Packet statistics
  kGwDisplayModeLoRa ,            // LoRa configuration
  kGwDisplayModeBarometer ,       // Ground barometer
  kGwDisplayModeGps ,             // GPS location and status
  kGwDisplayModeAbout ,           // About screen
  kGwDisplayModeCount             // Number of modes
} GwDisplayMode ;

//----------------------------------------------
// Connection States
//----------------------------------------------
typedef enum
{
  kConnectionDisconnected = 0,
  kConnectionSearching,
  kConnectionConnected
} ConnectionState ;

//----------------------------------------------
// Signal Quality Thresholds
//----------------------------------------------
// RSSI: Received Signal Strength Indicator (dBm)
#define RSSI_EXCELLENT      -50
#define RSSI_GOOD           -70
#define RSSI_FAIR           -90

// SNR: Signal to Noise Ratio (dB)
#define SNR_EXCELLENT       10
#define SNR_GOOD            5
#define SNR_FAIR            0

//----------------------------------------------
// Function: GatewayDisplay_Init
// Purpose: Initialize the display UI
// Returns: true if successful
//----------------------------------------------
bool GatewayDisplay_Init(void) ;

//----------------------------------------------
// Function: GatewayDisplay_ShowSplash
// Purpose: Show splash screen
//----------------------------------------------
void GatewayDisplay_ShowSplash(void) ;

//----------------------------------------------
// Function: GatewayDisplay_GetMode
// Purpose: Get current display mode
// Returns: Current display mode
//----------------------------------------------
GwDisplayMode GatewayDisplay_GetMode(void) ;

//----------------------------------------------
// Function: GatewayDisplay_SetMode
// Purpose: Set display mode
// Parameters:
//   inMode - Display mode to set
//----------------------------------------------
void GatewayDisplay_SetMode(GwDisplayMode inMode) ;

//----------------------------------------------
// Function: GatewayDisplay_CycleMode
// Purpose: Cycle to next display mode
// Returns: New display mode
//----------------------------------------------
GwDisplayMode GatewayDisplay_CycleMode(void) ;

//----------------------------------------------
// Function: GatewayDisplay_PrevMode
// Purpose: Cycle to previous display mode
// Returns: New display mode
//----------------------------------------------
GwDisplayMode GatewayDisplay_PrevMode(void) ;

//----------------------------------------------
// Function: GatewayDisplay_SetConnectionState
// Purpose: Update LoRa connection state display
// Parameters:
//   inState - Connection state to flight computer
//----------------------------------------------
void GatewayDisplay_SetConnectionState(ConnectionState inState) ;

//----------------------------------------------
// Function: GatewayDisplay_SetUsbConnected
// Purpose: Update USB connection status display
// Parameters:
//   inConnected - true if USB is connected to desktop app
//----------------------------------------------
void GatewayDisplay_SetUsbConnected(bool inConnected) ;

//----------------------------------------------
// Function: GatewayDisplay_UpdateSignal
// Purpose: Update signal strength indicators
// Parameters:
//   inRssi - RSSI value in dBm
//   inSnr - SNR value in dB
//----------------------------------------------
void GatewayDisplay_UpdateSignal(int16_t inRssi, int8_t inSnr) ;

//----------------------------------------------
// Function: GatewayDisplay_UpdatePacketStats
// Purpose: Update packet statistics display
// Parameters:
//   inReceived - Total packets received
//   inLost - Packets lost/corrupted
//----------------------------------------------
void GatewayDisplay_UpdatePacketStats(uint32_t inReceived, uint32_t inLost) ;

//----------------------------------------------
// Function: GatewayDisplay_UpdateTelemetry
// Purpose: Update telemetry preview area
// Parameters:
//   inAltitude - Current altitude in meters
//   inVelocity - Current velocity in m/s
//   inState - Flight state string
//----------------------------------------------
void GatewayDisplay_UpdateTelemetry(float inAltitude, float inVelocity, const char * inState) ;

//----------------------------------------------
// Function: GatewayDisplay_UpdateBarometer
// Purpose: Update ground barometer display
// Parameters:
//   inPressurePa - Pressure in Pascals
//   inTemperatureC - Temperature in Celsius
//----------------------------------------------
void GatewayDisplay_UpdateBarometer(float inPressurePa, float inTemperatureC) ;

//----------------------------------------------
// Function: GatewayDisplay_ShowMessage
// Purpose: Display a status message
// Parameters:
//   inMessage - Message to display
//   inIsError - true for error display style
//----------------------------------------------
void GatewayDisplay_ShowMessage(const char * inMessage, bool inIsError) ;

//----------------------------------------------
// Function: GatewayDisplay_Update
// Purpose: Update display based on current mode
//----------------------------------------------
void GatewayDisplay_Update(void) ;

//----------------------------------------------
// Function: GatewayDisplay_ShowAbout
// Purpose: Show about screen with version/copyright
// Parameters:
//   inVersion - Firmware version string
//   inBuildDate - Build date string
//   inBuildTime - Build time string
//----------------------------------------------
void GatewayDisplay_ShowAbout(
  const char * inVersion,
  const char * inBuildDate,
  const char * inBuildTime) ;

//----------------------------------------------
// Function: GatewayDisplay_ShowLoRaConfig
// Purpose: Show LoRa radio configuration
// Parameters:
//   inLoRaOk - LoRa initialized successfully
//   inFrequencyHz - LoRa frequency in Hz
//   inSpreadFactor - Spreading factor
//   inTxPower - TX power in dBm
//----------------------------------------------
void GatewayDisplay_ShowLoRaConfig(
  bool inLoRaOk,
  uint32_t inFrequencyHz,
  uint8_t inSpreadFactor,
  int8_t inTxPower) ;

//----------------------------------------------
// Function: GatewayDisplay_UpdateGps
// Purpose: Update GPS display
// Parameters:
//   inGpsOk - GPS module initialized
//   inHasFix - GPS has valid fix
//   inLatitude - Latitude in degrees
//   inLongitude - Longitude in degrees
//   inSatellites - Number of satellites
//   inSpeedMps - Ground speed in m/s
//   inHeadingDeg - Heading in degrees
//----------------------------------------------
void GatewayDisplay_UpdateGps(
  bool inGpsOk,
  bool inHasFix,
  float inLatitude,
  float inLongitude,
  uint8_t inSatellites,
  float inSpeedMps,
  float inHeadingDeg) ;

