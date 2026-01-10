//----------------------------------------------
// Module: gateway_display.c
// Description: Gateway Display UI for TFT
//   Shows connection status and signal strength
//   OPTIMIZED: Only redraws changed values
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "gateway_display.h"
#include "hx8357.h"
#include <stdio.h>
#include <string.h>

//----------------------------------------------
// Private State - Current values
//----------------------------------------------
static ConnectionState sConnectionState = kConnectionDisconnected ;
static bool sUsbConnected = false ;
static int16_t sLastRssi = -120 ;
static int8_t sLastSnr = -20 ;
static uint32_t sPacketsReceived = 0 ;
static uint32_t sPacketsLost = 0 ;
static float sLastAltitude = 0.0f ;
static float sLastVelocity = 0.0f ;
static char sFlightState[16] = "---" ;
static char sLastMessage[64] = "" ;
static bool sLastMessageIsError = false ;

//----------------------------------------------
// Private State - Previous values (for change detection)
//----------------------------------------------
static ConnectionState sPrevConnectionState = kConnectionDisconnected ;
static bool sPrevUsbConnected = true ;  // Different to force initial draw
static int16_t sPrevRssi = -121 ;  // Different to force initial draw
static int8_t sPrevSnr = -21 ;
static uint32_t sPrevPacketsReceived = 0xFFFFFFFF ;
static uint32_t sPrevPacketsLost = 0xFFFFFFFF ;
static float sPrevAltitude = -999.0f ;
static float sPrevVelocity = -999.0f ;
static char sPrevFlightState[16] = "" ;
static char sPrevMessage[64] = "!" ;

//----------------------------------------------
// UI Layout Positions (480x320 landscape)
//----------------------------------------------
#define HEADER_Y            0
#define SIGNAL_AREA_Y       50
#define TELEMETRY_AREA_Y    180
#define STATS_AREA_Y        250
#define MESSAGE_AREA_Y      290

#define RSSI_BAR_X          50
#define SNR_BAR_X           200
#define SIGNAL_BAR_WIDTH    80
#define SIGNAL_BAR_HEIGHT   100

//----------------------------------------------
// Private: Map value to percentage (0-100)
//----------------------------------------------
static int MapToPercent(int inValue, int inMin, int inMax)
{
  if (inValue <= inMin) return 0 ;
  if (inValue >= inMax) return 100 ;
  return ((inValue - inMin) * 100) / (inMax - inMin) ;
}

//----------------------------------------------
// Private: Get color for signal quality
//----------------------------------------------
static uint16_t GetSignalColor(int inPercent)
{
  if (inPercent >= 75) return HX8357_GREEN ;
  if (inPercent >= 50) return HX8357_YELLOW ;
  if (inPercent >= 25) return HX8357_ORANGE ;
  return HX8357_RED ;
}

//----------------------------------------------
// Private: Draw Header (only when state changes)
//----------------------------------------------
static void DrawHeader(bool inForce)
{
  bool theLoRaChanged = inForce || (sConnectionState != sPrevConnectionState) ;
  bool theUsbChanged = inForce || (sUsbConnected != sPrevUsbConnected) ;

  if (!theLoRaChanged && !theUsbChanged)
  {
    return ;
  }

  sPrevConnectionState = sConnectionState ;
  sPrevUsbConnected = sUsbConnected ;

  // Dark blue header background
  HX8357_FillRect(0, HEADER_Y, HX8357_WIDTH, 45, HX8357_NAVY) ;

  // Title
  HX8357_DrawString(10, 8, "ROCKET AVIONICS", HX8357_WHITE, HX8357_NAVY, 3) ;
  HX8357_DrawString(10, 32, "Ground Gateway", HX8357_LIGHTGRAY, HX8357_NAVY, 1) ;

  // USB connection indicator (center-right)
  uint16_t theUsbColor = sUsbConnected ? HX8357_GREEN : HX8357_RED ;
  const char * theUsbText = sUsbConnected ? "USB:OK" : "USB:--" ;
  HX8357_FillRect(280, 10, 12, 12, theUsbColor) ;
  HX8357_DrawString(295, 10, theUsbText, theUsbColor, HX8357_NAVY, 1) ;

  // LoRa connection status indicator (right side)
  uint16_t theLoRaColor ;
  const char * theLoRaText ;

  switch (sConnectionState)
  {
    case kConnectionConnected:
      theLoRaColor = HX8357_GREEN ;
      theLoRaText = "LORA:OK" ;
      break ;
    case kConnectionSearching:
      theLoRaColor = HX8357_YELLOW ;
      theLoRaText = "LORA:.." ;
      break ;
    default:
      theLoRaColor = HX8357_RED ;
      theLoRaText = "LORA:--" ;
      break ;
  }

  // LoRa status indicator
  HX8357_FillRect(280, 26, 12, 12, theLoRaColor) ;
  HX8357_DrawString(295, 26, theLoRaText, theLoRaColor, HX8357_NAVY, 1) ;

  // Overall status box (far right)
  bool theBothConnected = sUsbConnected && (sConnectionState == kConnectionConnected) ;
  uint16_t theOverallColor = theBothConnected ? HX8357_GREEN :
                             (sUsbConnected || sConnectionState == kConnectionConnected) ? HX8357_YELLOW :
                             HX8357_RED ;
  HX8357_FillRect(400, 8, 70, 30, theOverallColor) ;
  HX8357_DrawString(408, 15, theBothConnected ? "READY" : "WAIT", HX8357_BLACK, theOverallColor, 2) ;
}

//----------------------------------------------
// Private: Draw a single signal bar (RSSI or SNR)
//----------------------------------------------
static void DrawSignalBar(int16_t inX, int16_t inY, const char * inLabel,
                          int inValue, const char * inUnit, int inMin, int inMax)
{
  char theBuffer[32] ;

  // Label (static - no clear needed)
  HX8357_DrawString(inX, inY, inLabel, HX8357_WHITE, HX8357_BLACK, 2) ;

  // Value text - clear only the value area
  HX8357_FillRect(inX, inY + 20, 100, 18, HX8357_BLACK) ;
  snprintf(theBuffer, sizeof(theBuffer), "%d %s", inValue, inUnit) ;
  HX8357_DrawString(inX, inY + 20, theBuffer, HX8357_CYAN, HX8357_BLACK, 2) ;

  // Bar background and outline
  int16_t theBarY = inY + 45 ;
  HX8357_FillRect(inX, theBarY, SIGNAL_BAR_WIDTH, SIGNAL_BAR_HEIGHT, HX8357_DARKGRAY) ;
  HX8357_DrawRect(inX, theBarY, SIGNAL_BAR_WIDTH, SIGNAL_BAR_HEIGHT, HX8357_WHITE) ;

  // Bar fill
  int thePercent = MapToPercent(inValue, inMin, inMax) ;
  int theFillHeight = (SIGNAL_BAR_HEIGHT - 4) * thePercent / 100 ;
  uint16_t theColor = GetSignalColor(thePercent) ;

  if (theFillHeight > 0)
  {
    HX8357_FillRect(inX + 2, theBarY + SIGNAL_BAR_HEIGHT - 2 - theFillHeight,
                    SIGNAL_BAR_WIDTH - 4, theFillHeight, theColor) ;
  }

  // Quality label - clear and redraw
  HX8357_FillRect(inX, theBarY + SIGNAL_BAR_HEIGHT + 5, 70, 12, HX8357_BLACK) ;
  const char * theQuality ;
  if (thePercent >= 75) theQuality = "EXCELLENT" ;
  else if (thePercent >= 50) theQuality = "GOOD" ;
  else if (thePercent >= 25) theQuality = "FAIR" ;
  else theQuality = "POOR" ;

  HX8357_DrawString(inX, theBarY + SIGNAL_BAR_HEIGHT + 5, theQuality, theColor, HX8357_BLACK, 1) ;
}

//----------------------------------------------
// Private: Draw Signal Area (only changed values)
//----------------------------------------------
static void DrawSignalArea(bool inForce)
{
  // Check if anything changed
  if (!inForce && sLastRssi == sPrevRssi && sLastSnr == sPrevSnr)
  {
    return ;
  }

  bool theRssiChanged = inForce || (sLastRssi != sPrevRssi) ;
  bool theSnrChanged = inForce || (sLastSnr != sPrevSnr) ;

  sPrevRssi = sLastRssi ;
  sPrevSnr = sLastSnr ;

  // Only redraw changed bars
  if (theRssiChanged)
  {
    DrawSignalBar(RSSI_BAR_X, SIGNAL_AREA_Y + 15, "RSSI", sLastRssi, "dBm", -120, -30) ;
  }

  if (theSnrChanged)
  {
    DrawSignalBar(SNR_BAR_X, SIGNAL_AREA_Y + 15, "SNR", sLastSnr, "dB", -20, 15) ;
  }

  // Update overall summary (right side) - always update if either changed
  if (theRssiChanged || theSnrChanged)
  {
    int theRssiPct = MapToPercent(sLastRssi, -120, -30) ;
    int theSnrPct = MapToPercent(sLastSnr, -20, 15) ;
    int theOverallPct = (theRssiPct + theSnrPct) / 2 ;

    // Clear and redraw percentage
    HX8357_FillRect(320, SIGNAL_AREA_Y + 45, 120, 40, HX8357_BLACK) ;
    char theBuffer[16] ;
    snprintf(theBuffer, sizeof(theBuffer), "%d%%", theOverallPct) ;
    HX8357_DrawString(320, SIGNAL_AREA_Y + 45, theBuffer, GetSignalColor(theOverallPct), HX8357_BLACK, 4) ;

    // Update signal bars graphic
    int16_t theBarBaseX = 320 ;
    int16_t theBarBaseY = SIGNAL_AREA_Y + 90 ;
    int theNumBars = (theOverallPct + 19) / 20 ;

    // Clear old bars and redraw
    HX8357_FillRect(theBarBaseX, theBarBaseY, 130, 55, HX8357_BLACK) ;
    for (int i = 0 ; i < 5 ; i++)
    {
      int16_t theBarHeight = 10 + (i * 8) ;
      uint16_t theColor = (i < theNumBars) ? GetSignalColor(theOverallPct) : HX8357_DARKGRAY ;
      HX8357_FillRect(theBarBaseX + (i * 25), theBarBaseY + (50 - theBarHeight),
                      20, theBarHeight, theColor) ;
    }
  }
}

//----------------------------------------------
// Private: Draw Telemetry Area (only changed values)
//----------------------------------------------
static void DrawTelemetryArea(bool inForce)
{
  char theBuffer[32] ;

  bool theAltChanged = inForce || (sLastAltitude != sPrevAltitude) ;
  bool theVelChanged = inForce || (sLastVelocity != sPrevVelocity) ;
  bool theStateChanged = inForce || (strcmp(sFlightState, sPrevFlightState) != 0) ;

  if (!theAltChanged && !theVelChanged && !theStateChanged)
  {
    return ;
  }

  // Update cached values
  sPrevAltitude = sLastAltitude ;
  sPrevVelocity = sLastVelocity ;
  strncpy(sPrevFlightState, sFlightState, sizeof(sPrevFlightState) - 1) ;

  // Altitude - clear value area only
  if (theAltChanged)
  {
    HX8357_FillRect(70, TELEMETRY_AREA_Y + 15, 100, 18, HX8357_BLACK) ;
    snprintf(theBuffer, sizeof(theBuffer), "%.1f m", sLastAltitude) ;
    HX8357_DrawString(70, TELEMETRY_AREA_Y + 15, theBuffer, HX8357_CYAN, HX8357_BLACK, 2) ;
  }

  // Velocity - clear value area only
  if (theVelChanged)
  {
    HX8357_FillRect(240, TELEMETRY_AREA_Y + 15, 100, 18, HX8357_BLACK) ;
    snprintf(theBuffer, sizeof(theBuffer), "%.1f m/s", sLastVelocity) ;
    HX8357_DrawString(240, TELEMETRY_AREA_Y + 15, theBuffer, HX8357_CYAN, HX8357_BLACK, 2) ;
  }

  // Flight State - clear value area only
  if (theStateChanged)
  {
    HX8357_FillRect(90, TELEMETRY_AREA_Y + 40, 100, 18, HX8357_BLACK) ;

    uint16_t theStateColor = HX8357_WHITE ;
    if (strcmp(sFlightState, "boost") == 0) theStateColor = HX8357_RED ;
    else if (strcmp(sFlightState, "coast") == 0) theStateColor = HX8357_ORANGE ;
    else if (strcmp(sFlightState, "apogee") == 0) theStateColor = HX8357_YELLOW ;
    else if (strcmp(sFlightState, "descent") == 0) theStateColor = HX8357_BLUE ;
    else if (strcmp(sFlightState, "landed") == 0) theStateColor = HX8357_GREEN ;
    else if (strcmp(sFlightState, "armed") == 0) theStateColor = HX8357_MAGENTA ;
    else if (strcmp(sFlightState, "idle") == 0) theStateColor = HX8357_CYAN ;

    HX8357_DrawString(90, TELEMETRY_AREA_Y + 40, sFlightState, theStateColor, HX8357_BLACK, 2) ;
  }
}

//----------------------------------------------
// Private: Draw Stats Area (only changed values)
//----------------------------------------------
static void DrawStatsArea(bool inForce)
{
  char theBuffer[32] ;

  bool theRxChanged = inForce || (sPacketsReceived != sPrevPacketsReceived) ;
  bool theLostChanged = inForce || (sPacketsLost != sPrevPacketsLost) ;

  if (!theRxChanged && !theLostChanged)
  {
    return ;
  }

  sPrevPacketsReceived = sPacketsReceived ;
  sPrevPacketsLost = sPacketsLost ;

  // Packets received - clear value area only
  if (theRxChanged)
  {
    HX8357_FillRect(50, STATS_AREA_Y + 5, 80, 18, HX8357_BLACK) ;
    snprintf(theBuffer, sizeof(theBuffer), "%lu", sPacketsReceived) ;
    HX8357_DrawString(50, STATS_AREA_Y + 5, theBuffer, HX8357_GREEN, HX8357_BLACK, 2) ;
  }

  // Packets lost - clear value area only
  if (theLostChanged)
  {
    HX8357_FillRect(210, STATS_AREA_Y + 5, 80, 18, HX8357_BLACK) ;
    snprintf(theBuffer, sizeof(theBuffer), "%lu", sPacketsLost) ;
    uint16_t theLostColor = (sPacketsLost > 0) ? HX8357_RED : HX8357_GREEN ;
    HX8357_DrawString(210, STATS_AREA_Y + 5, theBuffer, theLostColor, HX8357_BLACK, 2) ;
  }

  // Packet success rate - update if either changed
  if (theRxChanged || theLostChanged)
  {
    HX8357_FillRect(320, STATS_AREA_Y + 5, 140, 18, HX8357_BLACK) ;
    if (sPacketsReceived > 0)
    {
      uint32_t theTotal = sPacketsReceived + sPacketsLost ;
      int theSuccessRate = (sPacketsReceived * 100) / theTotal ;
      snprintf(theBuffer, sizeof(theBuffer), "%d%% success", theSuccessRate) ;
      uint16_t theRateColor = (theSuccessRate >= 95) ? HX8357_GREEN :
                              (theSuccessRate >= 80) ? HX8357_YELLOW : HX8357_RED ;
      HX8357_DrawString(320, STATS_AREA_Y + 5, theBuffer, theRateColor, HX8357_BLACK, 2) ;
    }
  }
}

//----------------------------------------------
// Private: Draw Message Area (only when changed)
//----------------------------------------------
static void DrawMessageArea(bool inForce)
{
  if (!inForce && strcmp(sLastMessage, sPrevMessage) == 0)
  {
    return ;
  }
  strncpy(sPrevMessage, sLastMessage, sizeof(sPrevMessage) - 1) ;

  // Clear message area
  HX8357_FillRect(0, MESSAGE_AREA_Y, HX8357_WIDTH, 30, HX8357_BLACK) ;

  if (strlen(sLastMessage) > 0)
  {
    uint16_t theColor = sLastMessageIsError ? HX8357_RED : HX8357_WHITE ;
    HX8357_DrawString(10, MESSAGE_AREA_Y + 5, sLastMessage, theColor, HX8357_BLACK, 2) ;
  }
}

//----------------------------------------------
// Private: Draw static labels (called once at init)
//----------------------------------------------
static void DrawStaticLabels(void)
{
  // Section title - Signal
  HX8357_DrawString(10, SIGNAL_AREA_Y, "SIGNAL STRENGTH", HX8357_GRAY, HX8357_BLACK, 1) ;

  // "OVERALL" label
  HX8357_DrawString(320, SIGNAL_AREA_Y + 15, "OVERALL", HX8357_WHITE, HX8357_BLACK, 2) ;

  // Section title - Telemetry
  HX8357_DrawString(10, TELEMETRY_AREA_Y, "LAST TELEMETRY", HX8357_GRAY, HX8357_BLACK, 1) ;

  // Telemetry labels
  HX8357_DrawString(10, TELEMETRY_AREA_Y + 15, "ALT:", HX8357_WHITE, HX8357_BLACK, 2) ;
  HX8357_DrawString(180, TELEMETRY_AREA_Y + 15, "VEL:", HX8357_WHITE, HX8357_BLACK, 2) ;
  HX8357_DrawString(10, TELEMETRY_AREA_Y + 40, "STATE:", HX8357_WHITE, HX8357_BLACK, 2) ;

  // Stats labels
  HX8357_DrawString(10, STATS_AREA_Y + 5, "RX:", HX8357_GRAY, HX8357_BLACK, 2) ;
  HX8357_DrawString(150, STATS_AREA_Y + 5, "LOST:", HX8357_GRAY, HX8357_BLACK, 2) ;
}

//----------------------------------------------
// Public Functions
//----------------------------------------------

bool GatewayDisplay_Init(void)
{
  if (!HX8357_Init())
  {
    return false ;
  }

  // Set landscape orientation
  HX8357_SetRotation(1) ;

  // Clear screen once
  HX8357_FillScreen(HX8357_BLACK) ;

  // Draw static labels (never redrawn)
  DrawStaticLabels() ;

  // Force draw all elements
  DrawHeader(true) ;
  DrawSignalArea(true) ;
  DrawTelemetryArea(true) ;
  DrawStatsArea(true) ;
  DrawMessageArea(true) ;

  return true ;
}

void GatewayDisplay_SetConnectionState(ConnectionState inState)
{
  sConnectionState = inState ;
  DrawHeader(false) ;
}

void GatewayDisplay_SetUsbConnected(bool inConnected)
{
  sUsbConnected = inConnected ;
  DrawHeader(false) ;
}

void GatewayDisplay_UpdateSignal(int16_t inRssi, int8_t inSnr)
{
  sLastRssi = inRssi ;
  sLastSnr = inSnr ;
  DrawSignalArea(false) ;
}

void GatewayDisplay_UpdatePacketStats(uint32_t inReceived, uint32_t inLost)
{
  sPacketsReceived = inReceived ;
  sPacketsLost = inLost ;
  DrawStatsArea(false) ;
}

void GatewayDisplay_UpdateTelemetry(float inAltitude, float inVelocity, const char * inState)
{
  sLastAltitude = inAltitude ;
  sLastVelocity = inVelocity ;
  strncpy(sFlightState, inState, sizeof(sFlightState) - 1) ;
  sFlightState[sizeof(sFlightState) - 1] = '\0' ;
  DrawTelemetryArea(false) ;
}

void GatewayDisplay_ShowMessage(const char * inMessage, bool inIsError)
{
  strncpy(sLastMessage, inMessage, sizeof(sLastMessage) - 1) ;
  sLastMessage[sizeof(sLastMessage) - 1] = '\0' ;
  sLastMessageIsError = inIsError ;
  DrawMessageArea(false) ;
}

void GatewayDisplay_Refresh(void)
{
  // Force redraw everything
  DrawHeader(true) ;
  DrawSignalArea(true) ;
  DrawTelemetryArea(true) ;
  DrawStatsArea(true) ;
  DrawMessageArea(true) ;
}
