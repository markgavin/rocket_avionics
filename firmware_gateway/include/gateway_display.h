//----------------------------------------------
// Module: gateway_display.h
// Description: Gateway Display UI for TFT
//   Shows connection status and signal strength
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// Display Layout Constants
//----------------------------------------------
#define DISPLAY_HEADER_HEIGHT       40
#define DISPLAY_FOOTER_HEIGHT       30
#define DISPLAY_MARGIN              10
#define DISPLAY_SIGNAL_BAR_WIDTH    30
#define DISPLAY_SIGNAL_BAR_HEIGHT   120
#define DISPLAY_SIGNAL_BAR_GAP      20

//----------------------------------------------
// Connection States
//----------------------------------------------
typedef enum {
  kConnectionDisconnected = 0,
  kConnectionSearching,
  kConnectionConnected
} ConnectionState ;

//----------------------------------------------
// Signal Quality Thresholds
//----------------------------------------------
// RSSI: Received Signal Strength Indicator (dBm)
//   -50 to -30: Excellent
//   -70 to -50: Good
//   -90 to -70: Fair
//   < -90:      Poor
#define RSSI_EXCELLENT      -50
#define RSSI_GOOD           -70
#define RSSI_FAIR           -90

// SNR: Signal to Noise Ratio (dB)
//   > 10: Excellent
//   5-10: Good
//   0-5:  Fair
//   < 0:  Poor
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
// Function: GatewayDisplay_ShowMessage
// Purpose: Display a status message
// Parameters:
//   inMessage - Message to display
//   inIsError - true for error (red), false for info (white)
//----------------------------------------------
void GatewayDisplay_ShowMessage(const char * inMessage, bool inIsError) ;

//----------------------------------------------
// Function: GatewayDisplay_Refresh
// Purpose: Redraw the entire display
//----------------------------------------------
void GatewayDisplay_Refresh(void) ;

