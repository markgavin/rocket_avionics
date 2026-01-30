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
#include <math.h>

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
// Function: StatusDisplay_PrevMode
//----------------------------------------------
DisplayMode StatusDisplay_PrevMode(void)
{
  if (sCurrentMode == 0)
  {
    sCurrentMode = kDisplayModeCount - 1 ;
  }
  else
  {
    sCurrentMode-- ;
  }
  return sCurrentMode ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowDeviceInfo
//----------------------------------------------
void StatusDisplay_ShowDeviceInfo(
  const char * inFirmwareVersion,
  bool inBmp390Ok,
  bool inLoRaOk,
  bool inImuOk,
  bool inGpsOk)
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

  snprintf(theBuffer, sizeof(theBuffer), "IMU:    %s", inImuOk ? "OK" : "FAIL") ;
  SSD1306_DrawString(0, 44, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "GPS:    %s", inGpsOk ? "OK" : "FAIL") ;
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
    SSD1306_DrawStringCentered(28, "Acquiring...", 1) ;
  }

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_UpdateCompact
//----------------------------------------------
void StatusDisplay_UpdateCompact(
  FlightState inState,
  bool inOrientationMode,
  uint8_t inRocketId,
  float inAltitudeM,
  float inVelocityMps,
  bool inGpsOk,
  bool inGpsFix,
  uint8_t inGpsSatellites,
  bool inLoRaConnected,
  int16_t inRssi,
  int8_t inSnr)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  // State name at top - show ORIENTATION when in orientation mode and idle
  const char * theStateName ;
  if (inOrientationMode && inState == kFlightIdle)
  {
    theStateName = "ORIENTATION" ;
  }
  else
  {
    theStateName = FlightControl_GetStateName(inState) ;
  }
  SSD1306_DrawString(0, 0, theStateName, 1) ;

  // Rocket ID in top right corner
  char theIdBuffer[8] ;
  snprintf(theIdBuffer, sizeof(theIdBuffer), "#%u", inRocketId) ;
  // Position based on string length (right-aligned)
  int16_t theIdX = 128 - (strlen(theIdBuffer) * 6) ;  // 6 pixels per char
  SSD1306_DrawString(theIdX, 0, theIdBuffer, 1) ;

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

  // Gateway/LoRa status at bottom with signal quality (Good/Ok/Poor)
  // Thresholds match gateway_display.h
  // Note: When RSSI=0 and SNR=0, it means we're transmitting but haven't received ACKs
  const char * theGwStatus ;
  if (!inLoRaConnected)
  {
    theGwStatus = "GW: --" ;
  }
  else if (inRssi == 0 && inSnr == 0)
  {
    // Transmitting but no ACK data yet - show active status
    theGwStatus = "GW: Active" ;
  }
  else if (inRssi > -70 && inSnr > 5)
  {
    theGwStatus = "GW: Good" ;
  }
  else if (inRssi > -90 && inSnr > 0)
  {
    theGwStatus = "GW: Ok" ;
  }
  else
  {
    theGwStatus = "GW: Poor" ;
  }
  SSD1306_DrawString(0, 54, theGwStatus, 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowImu
// Purpose: Show IMU visualization with horizon indicator
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
  float inHeadingDeg)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  // Title
  SSD1306_DrawString(0, 0, "IMU", 1) ;

  // Draw separator line
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  //----------------------------------------------
  // Artificial Horizon (left side, 44x44 pixels)
  // Center at (22, 36), radius 20
  //----------------------------------------------
  const int16_t theCenterX = 22 ;
  const int16_t theCenterY = 36 ;
  const int16_t theRadius = 20 ;

  // Draw horizon circle outline
  for (int16_t i = 0 ; i < 360 ; i += 6)
  {
    float theAngleRad = i * 3.14159f / 180.0f ;
    int16_t theX = theCenterX + (int16_t)(theRadius * cosf(theAngleRad)) ;
    int16_t theY = theCenterY + (int16_t)(theRadius * sinf(theAngleRad)) ;
    SSD1306_SetPixel(theX, theY, true) ;
  }

  // Calculate horizon line endpoints based on roll and pitch
  // Roll tilts the line, pitch moves it up/down
  float theRollRad = inRollDeg * 3.14159f / 180.0f ;
  float thePitchOffset = (inPitchDeg / 90.0f) * theRadius ;  // Clamp pitch to radius

  // Limit pitch offset
  if (thePitchOffset > theRadius - 2) thePitchOffset = theRadius - 2 ;
  if (thePitchOffset < -(theRadius - 2)) thePitchOffset = -(theRadius - 2) ;

  // Horizon line rotated by roll, offset by pitch
  float theCosRoll = cosf(theRollRad) ;
  float theSinRoll = sinf(theRollRad) ;

  int16_t theLineLen = theRadius - 2 ;
  int16_t theX0 = theCenterX - (int16_t)(theLineLen * theCosRoll) ;
  int16_t theY0 = theCenterY - (int16_t)(theLineLen * theSinRoll) + (int16_t)thePitchOffset ;
  int16_t theX1 = theCenterX + (int16_t)(theLineLen * theCosRoll) ;
  int16_t theY1 = theCenterY + (int16_t)(theLineLen * theSinRoll) + (int16_t)thePitchOffset ;

  SSD1306_DrawLine(theX0, theY0, theX1, theY1, true) ;

  // Draw center reference mark (small cross)
  SSD1306_DrawLine(theCenterX - 3, theCenterY, theCenterX + 3, theCenterY, true) ;
  SSD1306_DrawLine(theCenterX, theCenterY - 3, theCenterX, theCenterY + 3, true) ;

  //----------------------------------------------
  // Acceleration magnitude bar (right side of horizon)
  // Vertical bar showing 0-4g scale
  //----------------------------------------------
  const int16_t theBarX = 48 ;
  const int16_t theBarY = 16 ;
  const int16_t theBarW = 6 ;
  const int16_t theBarH = 40 ;

  // Draw bar outline
  SSD1306_DrawRect(theBarX, theBarY, theBarW, theBarH, true) ;

  // Fill bar based on acceleration magnitude (0-4g scale)
  float theAccelNorm = inAccelMag / 4.0f ;
  if (theAccelNorm > 1.0f) theAccelNorm = 1.0f ;
  int16_t theFillH = (int16_t)(theAccelNorm * (theBarH - 2)) ;
  if (theFillH > 0)
  {
    SSD1306_FillRect(theBarX + 1, theBarY + theBarH - 1 - theFillH, theBarW - 2, theFillH, true) ;
  }

  // Accel value label
  char theBuffer[16] ;
  snprintf(theBuffer, sizeof(theBuffer), "%.1fg", inAccelMag) ;
  SSD1306_DrawString(theBarX, theBarY + theBarH + 2, theBuffer, 1) ;

  //----------------------------------------------
  // Numeric values (right side)
  //----------------------------------------------
  const int16_t theTextX = 60 ;

  // Pitch/Roll
  snprintf(theBuffer, sizeof(theBuffer), "P:%+.0f", inPitchDeg) ;
  SSD1306_DrawString(theTextX, 14, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "R:%+.0f", inRollDeg) ;
  SSD1306_DrawString(theTextX + 36, 14, theBuffer, 1) ;

  // Compass heading
  snprintf(theBuffer, sizeof(theBuffer), "Hdg:%3.0f", inHeadingDeg) ;
  SSD1306_DrawString(theTextX, 26, theBuffer, 1) ;

  // Gyro rates (deg/s)
  snprintf(theBuffer, sizeof(theBuffer), "Gx:%+.0f", inGyroX) ;
  SSD1306_DrawString(theTextX, 38, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "Gy:%+.0f", inGyroY) ;
  SSD1306_DrawString(theTextX, 48, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "Gz:%+.0f", inGyroZ) ;
  SSD1306_DrawString(theTextX, 58, theBuffer, 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowSpin
// Purpose: Show spin rate around vertical axis
//----------------------------------------------
void StatusDisplay_ShowSpin(float inSpinRate)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  // Title
  SSD1306_DrawString(0, 0, "SPIN RATE", 1) ;
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  char theBuffer[20] ;

  // Large spin rate value (centered)
  snprintf(theBuffer, sizeof(theBuffer), "%+.0f", inSpinRate) ;
  SSD1306_DrawStringCentered(20, theBuffer, 2) ;

  // Units
  SSD1306_DrawStringCentered(38, "deg/sec", 1) ;

  //----------------------------------------------
  // Horizontal gauge showing spin rate
  // Scale: -500 to +500 deg/s
  //----------------------------------------------
  const int16_t theGaugeX = 10 ;
  const int16_t theGaugeY = 52 ;
  const int16_t theGaugeW = 108 ;
  const int16_t theGaugeH = 6 ;

  // Draw gauge outline
  SSD1306_DrawRect(theGaugeX, theGaugeY, theGaugeW, theGaugeH, true) ;

  // Draw center mark (zero point)
  int16_t theCenterX = theGaugeX + theGaugeW / 2 ;
  SSD1306_DrawLine(theCenterX, theGaugeY - 2, theCenterX, theGaugeY + theGaugeH + 1, true) ;

  // Draw indicator position
  // Clamp to -500 to +500 range
  float theClampedRate = inSpinRate ;
  if (theClampedRate > 500.0f) theClampedRate = 500.0f ;
  if (theClampedRate < -500.0f) theClampedRate = -500.0f ;

  // Map to gauge position
  int16_t theIndicatorX = theCenterX + (int16_t)((theClampedRate / 500.0f) * (theGaugeW / 2 - 2)) ;

  // Draw indicator (filled rectangle)
  if (inSpinRate >= 0)
  {
    // Positive: fill from center to right
    int16_t theFillW = theIndicatorX - theCenterX ;
    if (theFillW > 0)
    {
      SSD1306_FillRect(theCenterX, theGaugeY + 1, theFillW, theGaugeH - 2, true) ;
    }
  }
  else
  {
    // Negative: fill from indicator to center
    int16_t theFillW = theCenterX - theIndicatorX ;
    if (theFillW > 0)
    {
      SSD1306_FillRect(theIndicatorX, theGaugeY + 1, theFillW, theGaugeH - 2, true) ;
    }
  }

  // Scale labels
  SSD1306_DrawString(theGaugeX - 4, theGaugeY - 9, "-500", 1) ;
  SSD1306_DrawString(theCenterX - 3, theGaugeY - 9, "0", 1) ;
  SSD1306_DrawString(theGaugeX + theGaugeW - 18, theGaugeY - 9, "+500", 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowCompass
// Purpose: Show compass heading display
//----------------------------------------------
void StatusDisplay_ShowCompass(
  float inHeadingDeg,
  float inMagX,
  float inMagY,
  float inMagZ)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  // Title
  SSD1306_DrawString(0, 0, "COMPASS", 1) ;
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  //----------------------------------------------
  // Compass ellipse with arrow (left side)
  // Use ellipse to compensate for non-square pixels
  //----------------------------------------------
  const int16_t theCenterX = 32 ;
  const int16_t theCenterY = 38 ;
  const int16_t theRadiusX = 26 ;   // Horizontal radius
  const int16_t theRadiusY = 20 ;   // Vertical radius (smaller for ellipse)
  const float theAspect = (float)theRadiusY / (float)theRadiusX ;

  // Draw compass ellipse
  for (int16_t i = 0 ; i < 360 ; i += 5)
  {
    float theAngleRad = i * 3.14159f / 180.0f ;
    int16_t theX = theCenterX + (int16_t)(theRadiusX * sinf(theAngleRad)) ;
    int16_t theY = theCenterY - (int16_t)(theRadiusY * cosf(theAngleRad)) ;
    SSD1306_SetPixel(theX, theY, true) ;
  }

  // Draw cardinal direction markers
  SSD1306_DrawString(theCenterX - 2, theCenterY - theRadiusY - 9, "N", 1) ;
  SSD1306_DrawString(theCenterX + theRadiusX + 2, theCenterY - 3, "E", 1) ;
  SSD1306_DrawString(theCenterX - theRadiusX - 7, theCenterY - 3, "W", 1) ;

  // Draw arrow pointing in heading direction
  float theHeadingRad = inHeadingDeg * 3.14159f / 180.0f ;

  // Arrow tip position (on ellipse, slightly inside)
  float theTipScale = 0.80f ;
  int16_t theTipX = theCenterX + (int16_t)(theRadiusX * theTipScale * sinf(theHeadingRad)) ;
  int16_t theTipY = theCenterY - (int16_t)(theRadiusY * theTipScale * cosf(theHeadingRad)) ;

  // Line from center to tip
  SSD1306_DrawLine(theCenterX, theCenterY, theTipX, theTipY, true) ;

  // Arrowhead (scale Y for ellipse aspect ratio)
  int16_t theArrowLen = 5 ;
  float theA1Rad = theHeadingRad - 0.5f ;
  float theA2Rad = theHeadingRad + 0.5f ;
  int16_t theA1X = theTipX - (int16_t)(theArrowLen * sinf(theA1Rad)) ;
  int16_t theA1Y = theTipY + (int16_t)(theArrowLen * theAspect * cosf(theA1Rad)) ;
  int16_t theA2X = theTipX - (int16_t)(theArrowLen * sinf(theA2Rad)) ;
  int16_t theA2Y = theTipY + (int16_t)(theArrowLen * theAspect * cosf(theA2Rad)) ;
  SSD1306_DrawLine(theTipX, theTipY, theA1X, theA1Y, true) ;
  SSD1306_DrawLine(theTipX, theTipY, theA2X, theA2Y, true) ;

  // Center dot
  SSD1306_FillRect(theCenterX - 1, theCenterY - 1, 3, 3, true) ;

  //----------------------------------------------
  // Right side: Heading value and raw mag data
  //----------------------------------------------
  char theBuffer[20] ;
  const int16_t theTextX = 66 ;

  // Heading value
  snprintf(theBuffer, sizeof(theBuffer), "%5.1f", inHeadingDeg) ;
  SSD1306_DrawString(theTextX, 14, theBuffer, 1) ;
  SSD1306_DrawString(theTextX + 32, 14, "deg", 1) ;

  // Magnetometer values in milligauss (for better readability)
  snprintf(theBuffer, sizeof(theBuffer), "X:%+4.0f", inMagX * 1000.0f) ;
  SSD1306_DrawString(theTextX, 30, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "Y:%+4.0f", inMagY * 1000.0f) ;
  SSD1306_DrawString(theTextX, 42, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "Z:%+4.0f", inMagZ * 1000.0f) ;
  SSD1306_DrawString(theTextX, 54, theBuffer, 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowRates
// Purpose: Show sensor sampling rates screen
//----------------------------------------------
void StatusDisplay_ShowRates(
  uint16_t inBmp390Hz,
  uint16_t inImuAccelHz,
  uint16_t inImuGyroHz,
  uint8_t inGpsHz,
  uint8_t inTelemetryHz,
  uint8_t inDisplayHz)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  // Title
  SSD1306_DrawStringCentered(0, "SAMPLING RATES", 1) ;
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  char theBuffer[32] ;

  // BMP390 barometer
  snprintf(theBuffer, sizeof(theBuffer), "BMP390:  %3u Hz", inBmp390Hz) ;
  SSD1306_DrawString(0, 14, theBuffer, 1) ;

  // IMU Accelerometer
  snprintf(theBuffer, sizeof(theBuffer), "Accel:   %3u Hz", inImuAccelHz) ;
  SSD1306_DrawString(0, 24, theBuffer, 1) ;

  // IMU Gyroscope
  snprintf(theBuffer, sizeof(theBuffer), "Gyro:    %3u Hz", inImuGyroHz) ;
  SSD1306_DrawString(0, 34, theBuffer, 1) ;

  // GPS
  snprintf(theBuffer, sizeof(theBuffer), "GPS:     %3u Hz", inGpsHz) ;
  SSD1306_DrawString(0, 44, theBuffer, 1) ;

  // Telemetry and Display rates on bottom line
  snprintf(theBuffer, sizeof(theBuffer), "Telem:%uHz Disp:%uHz", inTelemetryHz, inDisplayHz) ;
  SSD1306_DrawString(0, 56, theBuffer, 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Internal: Format build date from "Jan 13 2026" to "26-01-13"
//----------------------------------------------
static void FormatBuildDate(const char * inDate, char * outFormatted, int inMaxLen)
{
  // __DATE__ format: "Mmm DD YYYY" e.g. "Jan 13 2026"
  static const char * kMonths[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  } ;

  int theMonth = 0 ;
  for (int i = 0 ; i < 12 ; i++)
  {
    if (inDate[0] == kMonths[i][0] &&
        inDate[1] == kMonths[i][1] &&
        inDate[2] == kMonths[i][2])
    {
      theMonth = i + 1 ;
      break ;
    }
  }

  int theDay = (inDate[4] == ' ') ? (inDate[5] - '0') : ((inDate[4] - '0') * 10 + (inDate[5] - '0')) ;
  int theYear = (inDate[9] - '0') * 10 + (inDate[10] - '0') ;  // Last 2 digits

  snprintf(outFormatted, inMaxLen, "%02d-%02d-%02d", theYear, theMonth, theDay) ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowAbout
// Purpose: Show about screen with version and copyright
//----------------------------------------------
void StatusDisplay_ShowAbout(
  const char * inVersion,
  const char * inBuildDate,
  const char * inBuildTime)
{
  (void)inBuildTime ;  // Not used in compact format

  if (!sInitialized) return ;

  SSD1306_Clear() ;

  // Product name (centered)
  SSD1306_DrawStringCentered(4, "Rocket Avionics", 1) ;
  SSD1306_DrawStringCentered(14, "Flight Computer", 1) ;

  // Separator line
  SSD1306_DrawLine(0, 24, 127, 24, true) ;

  // Version and date on same line
  char theDate[16] ;
  FormatBuildDate(inBuildDate, theDate, sizeof(theDate)) ;

  char theBuffer[32] ;
  snprintf(theBuffer, sizeof(theBuffer), "v%s  %s", inVersion, theDate) ;
  SSD1306_DrawStringCentered(30, theBuffer, 1) ;

  // Separator line
  SSD1306_DrawLine(0, 42, 127, 42, true) ;

  // Copyright
  SSD1306_DrawStringCentered(48, "(c) 2026 Mark Gavin", 1) ;

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: StatusDisplay_ShowRocketId
// Purpose: Show rocket ID setting screen
//----------------------------------------------
void StatusDisplay_ShowRocketId(
  uint8_t inRocketId,
  bool inEditing)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  // Title
  SSD1306_DrawStringCentered(0, "ROCKET ID", 1) ;
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  // Large ID number display
  char theBuffer[16] ;
  snprintf(theBuffer, sizeof(theBuffer), "%u", inRocketId) ;
  SSD1306_DrawStringCentered(20, theBuffer, 3) ;

  // ID range hint
  SSD1306_DrawStringCentered(44, "(0 - 15)", 1) ;

  // Instructions at bottom
  if (inEditing)
  {
    SSD1306_DrawStringCentered(54, "[B] to change", 1) ;
  }
  else
  {
    SSD1306_DrawStringCentered(54, "Press B to edit", 1) ;
  }

  SSD1306_Update() ;
}
