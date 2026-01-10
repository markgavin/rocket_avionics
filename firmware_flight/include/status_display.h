//----------------------------------------------
// Module: status_display.h
// Description: Status display for SSD1306 OLED
//   (Flight Avionics version)
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit 128x64 OLED FeatherWing (4650)
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "flight_control.h"

//----------------------------------------------
// Display Modes
//----------------------------------------------
typedef enum
{
  kDisplayModeLive = 0 ,      // Live altitude/velocity (default)
  kDisplayModeDeviceInfo ,    // Board, version, sensors
  kDisplayModeFlightStats ,   // Last flight statistics
  kDisplayModeLoRaStatus ,    // LoRa connection info
  kDisplayModeSensors ,       // Raw sensor readings
  kDisplayModeGpsStatus ,     // GPS status and position
  kDisplayModeCount           // Number of modes
} DisplayMode ;

//----------------------------------------------
// Function: StatusDisplay_Init
// Purpose: Initialize the status display
// Returns: true if successful
//----------------------------------------------
bool StatusDisplay_Init(void) ;

//----------------------------------------------
// Function: StatusDisplay_ShowSplash
// Purpose: Show splash screen
//----------------------------------------------
void StatusDisplay_ShowSplash(void) ;

//----------------------------------------------
// Function: StatusDisplay_Update
// Purpose: Update display with current flight status
// Parameters:
//   inState - Current flight state
//   inAltitudeM - Current altitude in meters
//   inVelocityMps - Current velocity in m/s
//   inLoRaConnected - LoRa link status
//----------------------------------------------
void StatusDisplay_Update(
  FlightState inState,
  float inAltitudeM,
  float inVelocityMps,
  bool inLoRaConnected) ;

//----------------------------------------------
// Function: StatusDisplay_ShowArmed
// Purpose: Show armed status screen
// Parameters:
//   inGroundAltitude - Ground reference altitude
//----------------------------------------------
void StatusDisplay_ShowArmed(float inGroundAltitude) ;

//----------------------------------------------
// Function: StatusDisplay_ShowInFlight
// Purpose: Show in-flight data
// Parameters:
//   inState - Current flight state
//   inAltitudeM - Current altitude
//   inVelocityMps - Current velocity
//   inMaxAltitudeM - Maximum altitude reached
//   inFlightTimeMs - Time since launch
//----------------------------------------------
void StatusDisplay_ShowInFlight(
  FlightState inState,
  float inAltitudeM,
  float inVelocityMps,
  float inMaxAltitudeM,
  uint32_t inFlightTimeMs) ;

//----------------------------------------------
// Function: StatusDisplay_ShowFlightComplete
// Purpose: Show flight complete summary
// Parameters:
//   inResults - Flight results structure
//----------------------------------------------
void StatusDisplay_ShowFlightComplete(const FlightResults * inResults) ;

//----------------------------------------------
// Function: StatusDisplay_ShowError
// Purpose: Show error message
// Parameters:
//   inMessage - Error message to display
//----------------------------------------------
void StatusDisplay_ShowError(const char * inMessage) ;

//----------------------------------------------
// Function: StatusDisplay_GetMode
// Purpose: Get current display mode
// Returns: Current display mode
//----------------------------------------------
DisplayMode StatusDisplay_GetMode(void) ;

//----------------------------------------------
// Function: StatusDisplay_SetMode
// Purpose: Set display mode
// Parameters:
//   inMode - Display mode to set
//----------------------------------------------
void StatusDisplay_SetMode(DisplayMode inMode) ;

//----------------------------------------------
// Function: StatusDisplay_CycleMode
// Purpose: Cycle to next display mode
// Returns: New display mode
//----------------------------------------------
DisplayMode StatusDisplay_CycleMode(void) ;

//----------------------------------------------
// Function: StatusDisplay_ShowDeviceInfo
// Purpose: Show device information screen
// Parameters:
//   inFirmwareVersion - Firmware version string
//   inBmp390Ok - BMP390 sensor status
//   inLoRaOk - LoRa radio status
//   inSdOk - SD card status
//   inRtcOk - RTC status
//----------------------------------------------
void StatusDisplay_ShowDeviceInfo(
  const char * inFirmwareVersion,
  bool inBmp390Ok,
  bool inLoRaOk,
  bool inSdOk,
  bool inRtcOk) ;

//----------------------------------------------
// Function: StatusDisplay_ShowLoRaStatus
// Purpose: Show LoRa connection status
// Parameters:
//   inConnected - Connection status
//   inRssi - Signal strength (dBm)
//   inPacketsSent - Packets transmitted
//   inPacketsReceived - Packets received
//----------------------------------------------
void StatusDisplay_ShowLoRaStatus(
  bool inConnected,
  int16_t inRssi,
  uint32_t inPacketsSent,
  uint32_t inPacketsReceived) ;

//----------------------------------------------
// Function: StatusDisplay_ShowSensorReadings
// Purpose: Show raw sensor data
// Parameters:
//   inPressurePa - Pressure in Pascals
//   inTemperatureC - Temperature in Celsius
//   inAltitudeM - Calculated altitude
//----------------------------------------------
void StatusDisplay_ShowSensorReadings(
  float inPressurePa,
  float inTemperatureC,
  float inAltitudeM) ;

//----------------------------------------------
// Function: StatusDisplay_ShowGpsStatus
// Purpose: Show GPS status and position
// Parameters:
//   inHasFix - GPS has valid fix
//   inSatellites - Number of satellites
//   inLatitude - Latitude in degrees
//   inLongitude - Longitude in degrees
//   inSpeedMps - Ground speed in m/s
//   inHeadingDeg - Heading in degrees
//----------------------------------------------
void StatusDisplay_ShowGpsStatus(
  bool inHasFix,
  uint8_t inSatellites,
  float inLatitude,
  float inLongitude,
  float inSpeedMps,
  float inHeadingDeg) ;

//----------------------------------------------
// Function: StatusDisplay_UpdateCompact
// Purpose: Update display with compact format showing
//   altitude, GPS, and gateway status
// Parameters:
//   inState - Current flight state
//   inAltitudeM - Current altitude in meters
//   inVelocityMps - Current velocity in m/s
//   inGpsOk - GPS initialized and working
//   inGpsFix - GPS has valid fix
//   inGpsSatellites - Number of satellites
//   inLoRaConnected - LoRa link status (gateway connected)
//----------------------------------------------
void StatusDisplay_UpdateCompact(
  FlightState inState,
  float inAltitudeM,
  float inVelocityMps,
  bool inGpsOk,
  bool inGpsFix,
  uint8_t inGpsSatellites,
  bool inLoRaConnected) ;
