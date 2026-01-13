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
  kDisplayModeImu ,           // IMU data display (horizon + accel)
  kDisplayModeSpin ,          // Spin rate display
  kDisplayModeCompass ,       // Compass heading display
  kDisplayModeAbout ,         // About screen with version/copyright
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
// Function: StatusDisplay_PrevMode
// Purpose: Cycle to previous display mode
// Returns: New display mode
//----------------------------------------------
DisplayMode StatusDisplay_PrevMode(void) ;

//----------------------------------------------
// Function: StatusDisplay_ShowDeviceInfo
// Purpose: Show device information screen
// Parameters:
//   inFirmwareVersion - Firmware version string
//   inBmp390Ok - BMP390 sensor status
//   inLoRaOk - LoRa radio status
//   inImuOk - IMU sensor status
//   inGpsOk - GPS status
//----------------------------------------------
void StatusDisplay_ShowDeviceInfo(
  const char * inFirmwareVersion,
  bool inBmp390Ok,
  bool inLoRaOk,
  bool inImuOk,
  bool inGpsOk) ;

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

//----------------------------------------------
// Function: StatusDisplay_ShowImu
// Purpose: Show IMU visualization with horizon indicator
// Parameters:
//   inPitchDeg - Pitch angle in degrees
//   inRollDeg - Roll angle in degrees
//   inAccelX - Acceleration X in g
//   inAccelY - Acceleration Y in g
//   inAccelZ - Acceleration Z in g
//   inAccelMag - Total acceleration magnitude in g
//   inGyroX - Gyro rate X in dps
//   inGyroY - Gyro rate Y in dps
//   inGyroZ - Gyro rate Z in dps
//   inHeadingDeg - Compass heading in degrees
//----------------------------------------------
void StatusDisplay_ShowImu(
  float inPitchDeg,
  float inRollDeg,
  float inAccelX,
  float inAccelY,
  float inAccelZ,
  float inAccelMag,
  float inGyroX,
  float inGyroY,
  float inGyroZ,
  float inHeadingDeg) ;

//----------------------------------------------
// Function: StatusDisplay_ShowSpin
// Purpose: Show spin rate around vertical axis
// Parameters:
//   inSpinRate - Spin rate in degrees per second
//----------------------------------------------
void StatusDisplay_ShowSpin(float inSpinRate) ;

//----------------------------------------------
// Function: StatusDisplay_ShowCompass
// Purpose: Show compass heading display
// Parameters:
//   inHeadingDeg - Current heading in degrees (0-360)
//   inMagX - Raw magnetometer X
//   inMagY - Raw magnetometer Y
//   inMagZ - Raw magnetometer Z
//----------------------------------------------
void StatusDisplay_ShowCompass(
  float inHeadingDeg,
  float inMagX,
  float inMagY,
  float inMagZ) ;

//----------------------------------------------
// Function: StatusDisplay_ShowAbout
// Purpose: Show about screen with version and copyright
// Parameters:
//   inVersion - Firmware version string
//   inBuildDate - Build date string
//   inBuildTime - Build time string
//----------------------------------------------
void StatusDisplay_ShowAbout(
  const char * inVersion,
  const char * inBuildDate,
  const char * inBuildTime) ;

