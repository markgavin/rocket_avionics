//----------------------------------------------
// Module: gateway_display.c
// Description: Gateway Display UI for OLED
//   Shows connection status, signal, and telemetry
// Author: Mark Gavin
// Created: 2026-01-10
// Modified: 2026-01-13 (Switched to OLED display)
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "gateway_display.h"
#include "ssd1306.h"
#include "pins.h"

#include <stdio.h>
#include <string.h>
#include <math.h>


//----------------------------------------------
// Module State
//----------------------------------------------
static GwDisplayMode sCurrentMode = kGwDisplayModeTelemetry ;
static ConnectionState sConnectionState = kConnectionDisconnected ;
static bool sUsbConnected = false ;
static bool sInitialized = false ;

// Cached telemetry data
static float sCachedAltitude = 0.0f ;
static float sCachedVelocity = 0.0f ;
static char sCachedState[16] = "---" ;

// Cached signal data
static int16_t sCachedRssi = -120 ;
static int8_t sCachedSnr = -20 ;

// Cached stats
static uint32_t sCachedPacketsReceived = 0 ;
static uint32_t sCachedPacketsLost = 0 ;

// Cached barometer data
static float sCachedPressurePa = 0.0f ;
static float sCachedTemperatureC = 0.0f ;

// Cached GPS data
static bool sCachedGpsOk = false ;
static bool sCachedGpsHasFix = false ;
static float sCachedGpsLatitude = 0.0f ;
static float sCachedGpsLongitude = 0.0f ;
static uint8_t sCachedGpsSatellites = 0 ;
static float sCachedGpsSpeedMps = 0.0f ;
static float sCachedGpsHeadingDeg = 0.0f ;

// Status message
static char sStatusMessage[32] = "" ;
static bool sStatusIsError = false ;

// Cached hardware status (for device info screen)
static char sCachedFirmwareVersion[16] = "0.0.0" ;
static bool sCachedLoRaOk = false ;
static bool sCachedBmp390Ok = false ;
static bool sCachedDisplayOk = false ;

//----------------------------------------------
// Internal: Draw Header
//----------------------------------------------
static void DrawHeader(const char * inTitle)
{
  // Draw title centered at top
  SSD1306_DrawStringCentered(0, inTitle, 1) ;

  // Draw separator line
  SSD1306_DrawLine(0, 9, 127, 9, true) ;

  // Draw connection status icon in top-right
  // Small box: filled = connected, empty = disconnected
  if (sConnectionState == kConnectionConnected)
  {
    SSD1306_FillRect(120, 1, 6, 6, true) ;
  }
  else
  {
    SSD1306_DrawRect(120, 1, 6, 6, true) ;
  }

  // USB indicator in top-right (below LoRa)
  if (sUsbConnected)
  {
    // Draw "U" for USB
    SSD1306_DrawChar(110, 1, 'U', 1) ;
  }
}


//----------------------------------------------
// Internal: Draw Telemetry Screen
//----------------------------------------------
static void DrawTelemetryScreen(void)
{
  DrawHeader("GATEWAY") ;

  char theBuffer[32] ;

  // Flight state from telemetry
  snprintf(theBuffer, sizeof(theBuffer), "State: %s", sCachedState) ;
  SSD1306_DrawString(4, 14, theBuffer, 1) ;

  // Altitude
  snprintf(theBuffer, sizeof(theBuffer), "Alt: %.1f m", sCachedAltitude) ;
  SSD1306_DrawString(4, 26, theBuffer, 1) ;

  // Velocity
  snprintf(theBuffer, sizeof(theBuffer), "Vel: %.1f m/s", sCachedVelocity) ;
  SSD1306_DrawString(4, 38, theBuffer, 1) ;

  // LoRa signal quality (Good/Ok/Poor based on RSSI and SNR)
  const char * theLoRaStatus ;
  if (sConnectionState != kConnectionConnected)
  {
    theLoRaStatus = "LoRa --" ;
  }
  else if (sCachedRssi > RSSI_GOOD && sCachedSnr > SNR_GOOD)
  {
    theLoRaStatus = "LoRa Good" ;
  }
  else if (sCachedRssi > RSSI_FAIR && sCachedSnr > SNR_FAIR)
  {
    theLoRaStatus = "LoRa Ok" ;
  }
  else
  {
    theLoRaStatus = "LoRa Poor" ;
  }
  SSD1306_DrawString(4, 50, theLoRaStatus, 1) ;

  // Host connection (USB or WiFi or --)
  const char * theHostStatus = sUsbConnected ? "Host USB" : "Host --" ;
  SSD1306_DrawString(70, 50, theHostStatus, 1) ;
}

//----------------------------------------------
// Internal: Draw Signal Strength Screen
//----------------------------------------------
static void DrawSignalScreen(void)
{
  DrawHeader("SIGNAL") ;

  char theBuffer[32] ;

  // RSSI value and bar
  snprintf(theBuffer, sizeof(theBuffer), "RSSI: %d dBm", sCachedRssi) ;
  SSD1306_DrawString(4, 14, theBuffer, 1) ;

  // RSSI bar (0-100 scale, -120 to -30 dBm range)
  int theRssiPercent = (sCachedRssi + 120) * 100 / 90 ;
  if (theRssiPercent < 0) theRssiPercent = 0 ;
  if (theRssiPercent > 100) theRssiPercent = 100 ;
  SSD1306_DrawRect(4, 24, 120, 8, true) ;
  SSD1306_FillRect(5, 25, (theRssiPercent * 118) / 100, 6, true) ;

  // SNR value and bar
  snprintf(theBuffer, sizeof(theBuffer), "SNR: %d dB", sCachedSnr) ;
  SSD1306_DrawString(4, 36, theBuffer, 1) ;

  // SNR bar (0-100 scale, -20 to 20 dB range)
  int theSnrPercent = (sCachedSnr + 20) * 100 / 40 ;
  if (theSnrPercent < 0) theSnrPercent = 0 ;
  if (theSnrPercent > 100) theSnrPercent = 100 ;
  SSD1306_DrawRect(4, 46, 120, 8, true) ;
  SSD1306_FillRect(5, 47, (theSnrPercent * 118) / 100, 6, true) ;

}

//----------------------------------------------
// Internal: Draw Stats Screen
//----------------------------------------------
static void DrawStatsScreen(void)
{
  DrawHeader("STATISTICS") ;

  char theBuffer[32] ;

  // Packets received
  snprintf(theBuffer, sizeof(theBuffer), "RX: %lu", (unsigned long)sCachedPacketsReceived) ;
  SSD1306_DrawString(4, 16, theBuffer, 1) ;

  // Packets lost
  snprintf(theBuffer, sizeof(theBuffer), "Lost: %lu", (unsigned long)sCachedPacketsLost) ;
  SSD1306_DrawString(4, 26, theBuffer, 1) ;

  // Success rate
  if (sCachedPacketsReceived > 0)
  {
    uint32_t theTotal = sCachedPacketsReceived + sCachedPacketsLost ;
    int theRate = (sCachedPacketsReceived * 100) / theTotal ;
    snprintf(theBuffer, sizeof(theBuffer), "Success: %d%%", theRate) ;
  }
  else
  {
    snprintf(theBuffer, sizeof(theBuffer), "Success: ---") ;
  }
  SSD1306_DrawString(4, 36, theBuffer, 1) ;

  // USB status
  snprintf(theBuffer, sizeof(theBuffer), "USB: %s", sUsbConnected ? "Connected" : "---") ;
  SSD1306_DrawString(4, 46, theBuffer, 1) ;

}

//----------------------------------------------
// Internal: Draw LoRa Config Screen
//----------------------------------------------
static void DrawLoRaScreen(void)
{
  DrawHeader("LORA CONFIG") ;

  char theBuffer[32] ;

  // Frequency
  float theFreqMhz = kLoRaFrequency / 1000000.0f ;
  snprintf(theBuffer, sizeof(theBuffer), "Freq: %.1f MHz", theFreqMhz) ;
  SSD1306_DrawString(4, 16, theBuffer, 1) ;

  // Spreading factor
  snprintf(theBuffer, sizeof(theBuffer), "SF: %d", kLoRaSpreadFactor) ;
  SSD1306_DrawString(4, 26, theBuffer, 1) ;

  // TX Power
  snprintf(theBuffer, sizeof(theBuffer), "Power: %d dBm", kLoRaTxPower) ;
  SSD1306_DrawString(4, 36, theBuffer, 1) ;

  // Sync word
  snprintf(theBuffer, sizeof(theBuffer), "Sync: 0x%02X", kLoRaSyncWord) ;
  SSD1306_DrawString(4, 46, theBuffer, 1) ;

}

//----------------------------------------------
// Internal: Draw Barometer Screen
//----------------------------------------------
static void DrawBarometerScreen(void)
{
  DrawHeader("GROUND BARO") ;

  char theBuffer[32] ;

  // Pressure in Pa
  snprintf(theBuffer, sizeof(theBuffer), "Pres: %.0f Pa", sCachedPressurePa) ;
  SSD1306_DrawString(0, 14, theBuffer, 1) ;

  // Pressure in hPa/mbar
  float theHpa = sCachedPressurePa / 100.0f ;
  snprintf(theBuffer, sizeof(theBuffer), "      %.2f hPa", theHpa) ;
  SSD1306_DrawString(0, 24, theBuffer, 1) ;

  // Temperature
  snprintf(theBuffer, sizeof(theBuffer), "Temp: %.1f C", sCachedTemperatureC) ;
  SSD1306_DrawString(0, 36, theBuffer, 1) ;

  // Altitude above sea level (barometric formula)
  // altitude = 44330 * (1 - (P/P0)^0.1903) where P0 = 101325 Pa
  float theAltitudeM = 0.0f ;
  if (sCachedPressurePa > 0.0f)
  {
    float theRatio = sCachedPressurePa / 101325.0f ;
    theAltitudeM = 44330.0f * (1.0f - powf(theRatio, 0.1903f)) ;
  }
  snprintf(theBuffer, sizeof(theBuffer), "Alt:  %.0f m ASL", theAltitudeM) ;
  SSD1306_DrawString(0, 48, theBuffer, 1) ;
}

//----------------------------------------------
// Internal: Draw GPS Screen
//----------------------------------------------
static void DrawGpsScreen(void)
{
  DrawHeader("GPS STATUS") ;

  char theBuffer[32] ;

  if (!sCachedGpsOk)
  {
    SSD1306_DrawStringCentered(28, "GPS NOT FOUND", 1) ;
    return ;
  }

  // Fix status and satellite count (matches flight computer style)
  snprintf(theBuffer, sizeof(theBuffer), "Fix: %s  Sats: %d",
    sCachedGpsHasFix ? "YES" : "NO", sCachedGpsSatellites) ;
  SSD1306_DrawString(0, 14, theBuffer, 1) ;

  if (sCachedGpsHasFix)
  {
    // Latitude
    snprintf(theBuffer, sizeof(theBuffer), "Lat: %.5f", sCachedGpsLatitude) ;
    SSD1306_DrawString(0, 26, theBuffer, 1) ;

    // Longitude
    snprintf(theBuffer, sizeof(theBuffer), "Lon: %.5f", sCachedGpsLongitude) ;
    SSD1306_DrawString(0, 36, theBuffer, 1) ;

    // Speed and heading (matches flight computer)
    snprintf(theBuffer, sizeof(theBuffer), "Spd: %.1f m/s  Hdg: %.0f",
      sCachedGpsSpeedMps, sCachedGpsHeadingDeg) ;
    SSD1306_DrawString(0, 48, theBuffer, 1) ;
  }
  else
  {
    SSD1306_DrawStringCentered(28, "Acquiring...", 1) ;
  }
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
// Internal: Draw Device Info Screen
//----------------------------------------------
static void DrawDeviceInfoScreen(void)
{
  DrawHeader("DEVICE INFO") ;

  char theBuffer[32] ;
  snprintf(theBuffer, sizeof(theBuffer), "FW: %s", sCachedFirmwareVersion) ;
  SSD1306_DrawString(0, 14, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "LoRa:   %s", sCachedLoRaOk ? "OK" : "FAIL") ;
  SSD1306_DrawString(0, 24, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "BMP390: %s", sCachedBmp390Ok ? "OK" : "FAIL") ;
  SSD1306_DrawString(0, 34, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "GPS:    %s", sCachedGpsOk ? "OK" : "FAIL") ;
  SSD1306_DrawString(0, 44, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "OLED:   %s", sCachedDisplayOk ? "OK" : "FAIL") ;
  SSD1306_DrawString(0, 54, theBuffer, 1) ;
}

//----------------------------------------------
// Internal: Draw About Screen (placeholder - actual content in ShowAbout)
//----------------------------------------------
static void DrawAboutScreen(void)
{
  SSD1306_DrawStringCentered(4, "ROCKET AVIONICS", 1) ;
  SSD1306_DrawStringCentered(14, "Ground Gateway", 1) ;

  SSD1306_DrawLine(0, 24, 127, 24, true) ;

  // Placeholder - actual values filled in by ShowAbout
  SSD1306_DrawStringCentered(32, "v-.-.- --.--.--", 1) ;

  SSD1306_DrawLine(0, 42, 127, 42, true) ;

  SSD1306_DrawStringCentered(48, "(c) 2026 Mark Gavin", 1) ;
}

//----------------------------------------------
// Function: GatewayDisplay_Init
//----------------------------------------------
bool GatewayDisplay_Init(void)
{
  if (!SSD1306_Init())
  {
    return false ;
  }

  sInitialized = true ;
  return true ;
}

//----------------------------------------------
// Function: GatewayDisplay_ShowSplash
//----------------------------------------------
void GatewayDisplay_ShowSplash(void)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;
  SSD1306_DrawStringCentered(8, "ROCKET", 2) ;
  SSD1306_DrawStringCentered(26, "AVIONICS", 2) ;
  SSD1306_DrawLine(20, 44, 107, 44, true) ;
  SSD1306_DrawStringCentered(50, "Ground Gateway", 1) ;
  SSD1306_Update() ;
}

//----------------------------------------------
// Function: GatewayDisplay_GetMode
//----------------------------------------------
GwDisplayMode GatewayDisplay_GetMode(void)
{
  return sCurrentMode ;
}

//----------------------------------------------
// Function: GatewayDisplay_SetMode
//----------------------------------------------
void GatewayDisplay_SetMode(GwDisplayMode inMode)
{
  if (inMode < kGwDisplayModeCount)
  {
    sCurrentMode = inMode ;
  }
}

//----------------------------------------------
// Function: GatewayDisplay_CycleMode
//----------------------------------------------
GwDisplayMode GatewayDisplay_CycleMode(void)
{
  sCurrentMode = (sCurrentMode + 1) % kGwDisplayModeCount ;
  return sCurrentMode ;
}

//----------------------------------------------
// Function: GatewayDisplay_PrevMode
//----------------------------------------------
GwDisplayMode GatewayDisplay_PrevMode(void)
{
  if (sCurrentMode == 0)
  {
    sCurrentMode = kGwDisplayModeCount - 1 ;
  }
  else
  {
    sCurrentMode-- ;
  }
  return sCurrentMode ;
}

//----------------------------------------------
// Function: GatewayDisplay_SetConnectionState
//----------------------------------------------
void GatewayDisplay_SetConnectionState(ConnectionState inState)
{
  sConnectionState = inState ;
}

//----------------------------------------------
// Function: GatewayDisplay_SetUsbConnected
//----------------------------------------------
void GatewayDisplay_SetUsbConnected(bool inConnected)
{
  sUsbConnected = inConnected ;
}

//----------------------------------------------
// Function: GatewayDisplay_UpdateSignal
//----------------------------------------------
void GatewayDisplay_UpdateSignal(int16_t inRssi, int8_t inSnr)
{
  sCachedRssi = inRssi ;
  sCachedSnr = inSnr ;
}

//----------------------------------------------
// Function: GatewayDisplay_UpdatePacketStats
//----------------------------------------------
void GatewayDisplay_UpdatePacketStats(uint32_t inReceived, uint32_t inLost)
{
  sCachedPacketsReceived = inReceived ;
  sCachedPacketsLost = inLost ;
}

//----------------------------------------------
// Function: GatewayDisplay_UpdateTelemetry
//----------------------------------------------
void GatewayDisplay_UpdateTelemetry(float inAltitude, float inVelocity, const char * inState)
{
  sCachedAltitude = inAltitude ;
  sCachedVelocity = inVelocity ;
  if (inState != NULL)
  {
    strncpy(sCachedState, inState, sizeof(sCachedState) - 1) ;
    sCachedState[sizeof(sCachedState) - 1] = '\0' ;
  }
}

//----------------------------------------------
// Function: GatewayDisplay_UpdateBarometer
//----------------------------------------------
void GatewayDisplay_UpdateBarometer(float inPressurePa, float inTemperatureC)
{
  sCachedPressurePa = inPressurePa ;
  sCachedTemperatureC = inTemperatureC ;
}

//----------------------------------------------
// Function: GatewayDisplay_UpdateGps
//----------------------------------------------
void GatewayDisplay_UpdateGps(
  bool inGpsOk,
  bool inHasFix,
  float inLatitude,
  float inLongitude,
  uint8_t inSatellites,
  float inSpeedMps,
  float inHeadingDeg)
{
  sCachedGpsOk = inGpsOk ;
  sCachedGpsHasFix = inHasFix ;
  sCachedGpsLatitude = inLatitude ;
  sCachedGpsLongitude = inLongitude ;
  sCachedGpsSatellites = inSatellites ;
  sCachedGpsSpeedMps = inSpeedMps ;
  sCachedGpsHeadingDeg = inHeadingDeg ;
}

//----------------------------------------------
// Function: GatewayDisplay_ShowMessage
//----------------------------------------------
void GatewayDisplay_ShowMessage(const char * inMessage, bool inIsError)
{
  strncpy(sStatusMessage, inMessage, sizeof(sStatusMessage) - 1) ;
  sStatusMessage[sizeof(sStatusMessage) - 1] = '\0' ;
  sStatusIsError = inIsError ;
}

//----------------------------------------------
// Function: GatewayDisplay_Update
//----------------------------------------------
void GatewayDisplay_Update(void)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  switch (sCurrentMode)
  {
    case kGwDisplayModeTelemetry:
      DrawTelemetryScreen() ;
      break ;

    case kGwDisplayModeSignal:
      DrawSignalScreen() ;
      break ;

    case kGwDisplayModeStats:
      DrawStatsScreen() ;
      break ;

    case kGwDisplayModeLoRa:
      DrawLoRaScreen() ;
      break ;

    case kGwDisplayModeBarometer:
      DrawBarometerScreen() ;
      break ;

    case kGwDisplayModeGps:
      DrawGpsScreen() ;
      break ;

    case kGwDisplayModeDeviceInfo:
      DrawDeviceInfoScreen() ;
      break ;

    case kGwDisplayModeAbout:
      DrawAboutScreen() ;
      break ;

    default:
      DrawTelemetryScreen() ;
      break ;
  }

  SSD1306_Update() ;
}

//----------------------------------------------
// Function: GatewayDisplay_ShowAbout
//----------------------------------------------
void GatewayDisplay_ShowAbout(
  const char * inVersion,
  const char * inBuildDate,
  const char * inBuildTime)
{
  (void)inBuildTime ;  // Not used in compact format

  if (!sInitialized) return ;

  SSD1306_Clear() ;

  SSD1306_DrawStringCentered(4, "ROCKET AVIONICS", 1) ;
  SSD1306_DrawStringCentered(14, "Ground Gateway", 1) ;

  SSD1306_DrawLine(0, 24, 127, 24, true) ;

  // Version and date on same line
  char theDate[16] ;
  FormatBuildDate(inBuildDate, theDate, sizeof(theDate)) ;

  char theBuffer[32] ;
  snprintf(theBuffer, sizeof(theBuffer), "v%s  %s", inVersion, theDate) ;
  SSD1306_DrawStringCentered(32, theBuffer, 1) ;

  SSD1306_DrawLine(0, 42, 127, 42, true) ;

  SSD1306_DrawStringCentered(48, "(c) 2026 Mark Gavin", 1) ;


  SSD1306_Update() ;
}

//----------------------------------------------
// Function: GatewayDisplay_ShowLoRaConfig
//----------------------------------------------
void GatewayDisplay_ShowLoRaConfig(
  bool inLoRaOk,
  uint32_t inFrequencyHz,
  uint8_t inSpreadFactor,
  int8_t inTxPower)
{
  if (!sInitialized) return ;

  SSD1306_Clear() ;

  DrawHeader("LORA CONFIG") ;

  char theBuffer[32] ;

  // Status
  snprintf(theBuffer, sizeof(theBuffer), "Status: %s", inLoRaOk ? "OK" : "FAIL") ;
  SSD1306_DrawString(4, 16, theBuffer, 1) ;

  // Frequency
  float theFreqMhz = inFrequencyHz / 1000000.0f ;
  snprintf(theBuffer, sizeof(theBuffer), "Freq: %.1f MHz", theFreqMhz) ;
  SSD1306_DrawString(4, 26, theBuffer, 1) ;

  // Spreading factor
  snprintf(theBuffer, sizeof(theBuffer), "SF: %d", inSpreadFactor) ;
  SSD1306_DrawString(4, 36, theBuffer, 1) ;

  // TX Power
  snprintf(theBuffer, sizeof(theBuffer), "Power: %d dBm", inTxPower) ;
  SSD1306_DrawString(4, 46, theBuffer, 1) ;


  SSD1306_Update() ;
}

//----------------------------------------------
// Function: GatewayDisplay_ShowDeviceInfo
//----------------------------------------------
void GatewayDisplay_ShowDeviceInfo(
  const char * inFirmwareVersion,
  bool inLoRaOk,
  bool inBmp390Ok,
  bool inGpsOk,
  bool inDisplayOk)
{
  // Cache the values for use when cycling to device info mode
  strncpy(sCachedFirmwareVersion, inFirmwareVersion, sizeof(sCachedFirmwareVersion) - 1) ;
  sCachedFirmwareVersion[sizeof(sCachedFirmwareVersion) - 1] = '\0' ;
  sCachedLoRaOk = inLoRaOk ;
  sCachedBmp390Ok = inBmp390Ok ;
  sCachedGpsOk = inGpsOk ;
  sCachedDisplayOk = inDisplayOk ;

  if (!sInitialized) return ;

  SSD1306_Clear() ;

  SSD1306_DrawStringCentered(0, "DEVICE INFO", 1) ;
  SSD1306_DrawLine(0, 10, 127, 10, true) ;

  char theBuffer[32] ;
  snprintf(theBuffer, sizeof(theBuffer), "FW: %s", inFirmwareVersion) ;
  SSD1306_DrawString(0, 14, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "LoRa:   %s", inLoRaOk ? "OK" : "FAIL") ;
  SSD1306_DrawString(0, 24, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "BMP390: %s", inBmp390Ok ? "OK" : "FAIL") ;
  SSD1306_DrawString(0, 34, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "GPS:    %s", inGpsOk ? "OK" : "FAIL") ;
  SSD1306_DrawString(0, 44, theBuffer, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "OLED:   %s", inDisplayOk ? "OK" : "FAIL") ;
  SSD1306_DrawString(0, 54, theBuffer, 1) ;

  SSD1306_Update() ;
}

