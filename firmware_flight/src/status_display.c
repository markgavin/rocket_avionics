//----------------------------------------------
// Module: status_display.c
// Description: Status display for SSD1306 OLED
//   (Flight Avionics version)
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "status_display.h"
#include "ssd1306.h"
#include "version.h"

#include <stdio.h>
#include <string.h>

//----------------------------------------------
// Module State
//----------------------------------------------
static DisplayMode sCurrentMode = kDisplayModeLive ;
static bool sInitialized = false ;

//----------------------------------------------
// Function: StatusDisplay_Init
//----------------------------------------------
bool StatusDisplay_Init(void)
{
  if (!SSD1306_Init())
  {
    return false ;
  }

  sCurrentMode = kDisplayModeLive ;
  sInitialized = true ;
  return true ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowSplash
//----------------------------------------------
void StatusDisplay_ShowSplash(void)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  // Title
  SSD1306_DrawStringCentered(8, "ROCKET", 2) ;
  SSD1306_DrawStringCentered(28, "AVIONICS", 2) ;

  // Version
  char theVersion[32] ;
  snprintf(theVersion, sizeof(theVersion), "v%s", FIRMWARE_VERSION_STRING) ;
  SSD1306_DrawStringCentered(52, theVersion, 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_Update
//----------------------------------------------
void StatusDisplay_Update(
  FlightState inState,
  float inAltitudeM,
  float inVelocityMps,
  bool inLoRaConnected)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  // State name at top
  const char * theStateName = FlightControl_GetStateName(inState) ;
  SSD1306_DrawStringCentered(0, theStateName, 1) ;

  // Draw separator line
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  // Main altitude display
  char theBuffer[32] ;

  // Altitude (large)
  snprintf(theBuffer, sizeof(theBuffer), "%.1f m", inAltitudeM) ;
  SSD1306_DrawStringCentered(16, theBuffer, 2) ;

  // Velocity
  snprintf(theBuffer, sizeof(theBuffer), "Vel: %.1f m/s", inVelocityMps) ;
  SSD1306_DrawString(0, 40, theBuffer, 1) ;

  // Status icons at bottom
  SSD1306_DrawString(0, 56, inLoRaConnected ? "LoRa:OK" : "LoRa:--", 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowArmed
//----------------------------------------------
void StatusDisplay_ShowArmed(float inGroundAltitude)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  SSD1306_DrawStringCentered(0, "** ARMED **", 1) ;
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  SSD1306_DrawStringCentered(20, "READY FOR", 1) ;
  SSD1306_DrawStringCentered(32, "LAUNCH", 2) ;

  char theBuffer[32] ;
  snprintf(theBuffer, sizeof(theBuffer), "Ground: %.0f Pa", inGroundAltitude) ;
  SSD1306_DrawStringCentered(54, theBuffer, 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowInFlight
//----------------------------------------------
void StatusDisplay_ShowInFlight(
  FlightState inState,
  float inAltitudeM,
  float inVelocityMps,
  float inMaxAltitudeM,
  uint32_t inFlightTimeMs)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  // State at top
  const char * theStateName = FlightControl_GetStateName(inState) ;
  SSD1306_DrawStringCentered(0, theStateName, 1) ;
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  char theBuffer[32] ;

  // Current altitude (large)
  snprintf(theBuffer, sizeof(theBuffer), "%.0f", inAltitudeM) ;
  SSD1306_DrawStringCentered(14, theBuffer, 2) ;
  SSD1306_DrawString(100, 22, "m", 1) ;

  // Velocity
  snprintf(theBuffer, sizeof(theBuffer), "V:%.0f m/s", inVelocityMps) ;
  SSD1306_DrawString(0, 36, theBuffer, 1) ;

  // Max altitude
  snprintf(theBuffer, sizeof(theBuffer), "Max:%.0f m", inMaxAltitudeM) ;
  SSD1306_DrawString(64, 36, theBuffer, 1) ;

  // Flight time
  uint32_t theSeconds = inFlightTimeMs / 1000 ;
  uint32_t theTenths = (inFlightTimeMs / 100) % 10 ;
  snprintf(theBuffer, sizeof(theBuffer), "T+%lu.%lus", theSeconds, theTenths) ;
  SSD1306_DrawStringCentered(52, theBuffer, 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowFlightComplete
//----------------------------------------------
void StatusDisplay_ShowFlightComplete(const FlightResults * inResults)
{
  if (!sInitialized || inResults == NULL) return ;

  SSD1306_Clear() ;

  SSD1306_DrawStringCentered(0, "FLIGHT COMPLETE", 1) ;
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  char theBuffer[32] ;

  // Max altitude
  snprintf(theBuffer, sizeof(theBuffer), "Max Alt: %.1f m", inResults->pMaxAltitudeM) ;
  SSD1306_DrawString(0, 14, theBuffer, 1) ;

  // Apogee time
  snprintf(theBuffer, sizeof(theBuffer), "Apogee: %.1f s", inResults->pApogeeTimeMs / 1000.0f) ;
  SSD1306_DrawString(0, 24, theBuffer, 1) ;

  // Max velocity
  snprintf(theBuffer, sizeof(theBuffer), "Max Vel: %.1f m/s", inResults->pMaxVelocityMps) ;
  SSD1306_DrawString(0, 34, theBuffer, 1) ;

  // Flight time
  snprintf(theBuffer, sizeof(theBuffer), "Flight: %.1f s", inResults->pFlightTimeMs / 1000.0f) ;
  SSD1306_DrawString(0, 44, theBuffer, 1) ;

  // Samples
  snprintf(theBuffer, sizeof(theBuffer), "Samples: %lu", inResults->pSampleCount) ;
  SSD1306_DrawString(0, 54, theBuffer, 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowError
//----------------------------------------------
void StatusDisplay_ShowError(const char * inMessage)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  SSD1306_DrawStringCentered(0, "!! ERROR !!", 1) ;
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  // Word-wrap the message (simple implementation)
  SSD1306_DrawString(0, 20, inMessage, 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_GetMode
//----------------------------------------------
DisplayMode StatusDisplay_GetMode(void)
{
  return sCurrentMode ;
}

//----------------------------------------------
// Function: StatusDisplay_SetMode
//----------------------------------------------
void StatusDisplay_SetMode(DisplayMode inMode)
{
  if (inMode < kDisplayModeCount)
  {
    sCurrentMode = inMode ;
  }
}

//----------------------------------------------
// Function: StatusDisplay_CycleMode
//----------------------------------------------
DisplayMode StatusDisplay_CycleMode(void)
{
  sCurrentMode = (sCurrentMode + 1) % kDisplayModeCount ;
  return sCurrentMode ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowDeviceInfo
//----------------------------------------------
void StatusDisplay_ShowDeviceInfo(
  const char * inFirmwareVersion,
  bool inBmp390Ok,
  bool inLoRaOk,
  bool inSdOk,
  bool inRtcOk)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  SSD1306_DrawStringCentered(0, "DEVICE INFO", 1) ;
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  char theBuffer[32] ;
  snprintf(theBuffer, sizeof(theBuffer), "FW: %s", inFirmwareVersion) ;
  SSD1306_DrawString(0, 14, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "BMP390: %s", inBmp390Ok ? "OK" : "FAIL") ;
  SSD1306_DrawString(0, 24, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "LoRa:   %s", inLoRaOk ? "OK" : "FAIL") ;
  SSD1306_DrawString(0, 34, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "SD:     %s", inSdOk ? "OK" : "FAIL") ;
  SSD1306_DrawString(0, 44, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "RTC:    %s", inRtcOk ? "OK" : "FAIL") ;
  SSD1306_DrawString(0, 54, theBuffer, 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowLoRaStatus
//----------------------------------------------
void StatusDisplay_ShowLoRaStatus(
  bool inConnected,
  int16_t inRssi,
  uint32_t inPacketsSent,
  uint32_t inPacketsReceived)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  SSD1306_DrawStringCentered(0, "LoRa STATUS", 1) ;
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  char theBuffer[32] ;

  snprintf(theBuffer, sizeof(theBuffer), "Link: %s", inConnected ? "CONNECTED" : "NO LINK") ;
  SSD1306_DrawString(0, 14, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "RSSI: %d dBm", inRssi) ;
  SSD1306_DrawString(0, 24, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "TX: %lu", inPacketsSent) ;
  SSD1306_DrawString(0, 34, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "RX: %lu", inPacketsReceived) ;
  SSD1306_DrawString(0, 44, theBuffer, 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowSensorReadings
//----------------------------------------------
void StatusDisplay_ShowSensorReadings(
  float inPressurePa,
  float inTemperatureC,
  float inAltitudeM)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  SSD1306_DrawStringCentered(0, "SENSORS", 1) ;
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  char theBuffer[32] ;

  snprintf(theBuffer, sizeof(theBuffer), "P: %.0f Pa", inPressurePa) ;
  SSD1306_DrawString(0, 14, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "T: %.1f C", inTemperatureC) ;
  SSD1306_DrawString(0, 28, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "Alt: %.1f m", inAltitudeM) ;
  SSD1306_DrawString(0, 42, theBuffer, 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowGpsStatus
//----------------------------------------------
void StatusDisplay_ShowGpsStatus(
  bool inHasFix,
  uint8_t inSatellites,
  float inLatitude,
  float inLongitude,
  float inSpeedMps,
  float inHeadingDeg)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  SSD1306_DrawStringCentered(0, "GPS STATUS", 1) ;
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  char theBuffer[32] ;

  // Fix status
  snprintf(theBuffer, sizeof(theBuffer), "Fix: %s  Sats: %d",
    inHasFix ? "YES" : "NO", inSatellites) ;
  SSD1306_DrawString(0, 14, theBuffer, 1) ;

  if (inHasFix)
  {
    // Latitude
    snprintf(theBuffer, sizeof(theBuffer), "Lat: %.5f", inLatitude) ;
    SSD1306_DrawString(0, 26, theBuffer, 1) ;

    // Longitude
    snprintf(theBuffer, sizeof(theBuffer), "Lon: %.5f", inLongitude) ;
    SSD1306_DrawString(0, 36, theBuffer, 1) ;

    // Speed and heading
    snprintf(theBuffer, sizeof(theBuffer), "Spd: %.1f m/s  Hdg: %.0f",
      inSpeedMps, inHeadingDeg) ;
    SSD1306_DrawString(0, 48, theBuffer, 1) ;
  }
  else
  {
    SSD1306_DrawStringCentered(32, "Acquiring...", 1) ;
  }

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_UpdateCompact
//----------------------------------------------
void StatusDisplay_UpdateCompact(
  FlightState inState,
  float inAltitudeM,
  float inVelocityMps,
  bool inGpsOk,
  bool inGpsFix,
  uint8_t inGpsSatellites,
  bool inLoRaConnected)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  // State name at top
  const char * theStateName = FlightControl_GetStateName(inState) ;
  SSD1306_DrawStringCentered(0, theStateName, 1) ;

  // Draw separator line
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  char theBuffer[32] ;

  // Altitude (smaller than before, single line)
  snprintf(theBuffer, sizeof(theBuffer), "Alt: %.1f m", inAltitudeM) ;
  SSD1306_DrawString(0, 14, theBuffer, 1) ;

  // Velocity
  snprintf(theBuffer, sizeof(theBuffer), "Vel: %.1f m/s", inVelocityMps) ;
  SSD1306_DrawString(0, 26, theBuffer, 1) ;

  // GPS status line
  if (inGpsOk)
  {
    if (inGpsFix)
    {
      snprintf(theBuffer, sizeof(theBuffer), "GPS: Fix (%d sat)", inGpsSatellites) ;
    }
    else
    {
      snprintf(theBuffer, sizeof(theBuffer), "GPS: No fix (%d sat)", inGpsSatellites) ;
    }
  }
  else
  {
    snprintf(theBuffer, sizeof(theBuffer), "GPS: --") ;
  }
  SSD1306_DrawString(0, 40, theBuffer, 1) ;

  // Gateway/LoRa status at bottom
  SSD1306_DrawString(0, 54, inLoRaConnected ? "GW: Connected" : "GW: --", 1) ;

  SSD1306_Update() ;
}
