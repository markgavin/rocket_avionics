//----------------------------------------------
// Module: eink_display.c
// Description: Status display for UC8151D eInk
//   (Flight Avionics - 296x128 monochrome)
//   Implements status_display.h API with
//   per-digit partial refresh for live data.
// Author: Mark Gavin
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "status_display.h"
#include "uc8151d.h"
#include "framebuffer.h"
#include "version.h"
#include "pins.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

//----------------------------------------------
// Layout Constants (296x128 landscape)
//----------------------------------------------

#define kHeaderY          0
#define kHeaderH          14
#define kLabel1Y          20
#define kValue1Y          28
#define kSep1Y            46
#define kLabel2Y          50
#define kValue2Y          60
#define kSep2Y            78
#define kLabel3Y          82
#define kValue3Y          92
#define kFooterY          114
#define kFooterH          14

// Per-digit cell dimensions (font 5x7 at scale 2)
#define kValueScale       2
#define kCellW            12      // (5+1) * 2
#define kCellH            16      // byte-aligned height

// Three-column layout
#define kCol1X            4
#define kCol2X            104
#define kCol3X            208

// Column bounding boxes (for full-box fallback)
#define kCol1BoxX         0
#define kCol1BoxW         100
#define kCol2BoxX         100
#define kCol2BoxW         104
#define kCol3BoxX         204
#define kCol3BoxW         92

// Unit positions (scale 1)
#define kAltUnitX         (kCol1X + 72)
#define kVelUnitX         (kCol2X + 72)
#define kAccelUnitX       (kCol3X + 60)
#define kMaxAltUnitX      (kCol1X + 72)
#define kMaxVelUnitX      (kCol2X + 72)
#define kTempUnitX        (kCol3X + 60)
#define kPressUnitX       (kCol1X + 80)
#define kUnitRow1Y        (kValue1Y + 6)
#define kUnitRow2Y        (kValue2Y + 6)
#define kUnitRow3Y        (kValue3Y + 6)

// Pyro continuity threshold (volts)
#define kPyroContinuityThresholdV  0.5f

// Content area for area-based partial refresh
#define kContentY         16
#define kContentH         98

#define kFullRefreshInterval  50

//----------------------------------------------
// Screen IDs (for tracking current display)
//----------------------------------------------
#define kScreenNone           -1
#define kScreenLive           0
#define kScreenArmed          1
#define kScreenLanded         2
#define kScreenSplash         3
#define kScreenDeviceInfo     4
#define kScreenLoRa           5
#define kScreenSensors        6
#define kScreenGps            7
#define kScreenRocketId       8
#define kScreenAbout          9
#define kScreenFlightStats    10
#define kScreenError          11
#define kScreenPyro           12

//----------------------------------------------
// eInk-compatible display modes
//----------------------------------------------
static const DisplayMode sEinkModes[] = {
  kDisplayModeLive,
  kDisplayModeFlightStats,
  kDisplayModeLoRaStatus,
  kDisplayModeSensors,
  kDisplayModePyro,
  kDisplayModeGpsStatus,
  kDisplayModeRocketId,
  kDisplayModeDeviceInfo,
  kDisplayModeAbout,
} ;
#define kEinkModeCount  ((int)(sizeof(sEinkModes) / sizeof(sEinkModes[0])))

//----------------------------------------------
// Module State
//----------------------------------------------
static DisplayMode sCurrentMode = kDisplayModeLive ;
static int sLastDrawnScreen = kScreenNone ;
static bool sInitialized = false ;
static uint32_t sUpdateCount = 0 ;

// Double buffer for partial refresh
static uint8_t sFrameBuffer[kEpdBufferSize] ;
static uint8_t sPrevBuffer[kEpdBufferSize] ;

// Per-digit change detection (shared across screens)
static char sPrevVal1[16] ;
static char sPrevVal2[16] ;
static char sPrevVal3[16] ;
static char sPrevVal4[16] ;
static char sPrevVal5[16] ;
static char sPrevVal6[16] ;
static char sPrevVal7[16] ;
static char sPrevVal8[16] ;
static char sPrevVal9[16] ;

//----------------------------------------------
// Helper: FormatFloat
//----------------------------------------------
static void FormatFloat(char * outBuf, size_t inSize, float inValue, int inDecimals)
{
  int theWhole = (int)inValue ;
  float theFrac = fabsf(inValue - (float)theWhole) ;

  float theMultiplier = (inDecimals == 1) ? 10.0f :
                        (inDecimals == 3) ? 1000.0f : 100.0f ;
  int theFracInt = (int)(theFrac * theMultiplier + 0.5f) ;
  const char * theFmt = (inDecimals == 1) ? "%d.%01d" :
                         (inDecimals == 3) ? "%d.%03d" : "%d.%02d" ;
  const char * theNegFmt = (inDecimals == 1) ? "-%d.%01d" :
                            (inDecimals == 3) ? "-%d.%03d" : "-%d.%02d" ;

  if (inValue < 0.0f && theWhole == 0)
    snprintf(outBuf, inSize, theNegFmt, 0, theFracInt) ;
  else
    snprintf(outBuf, inSize, theFmt, theWhole, theFracInt) ;
}

//----------------------------------------------
// Helper: FormatBuildDate ("Jan 13 2026" -> "26-01-13")
//----------------------------------------------
static void FormatBuildDate(const char * inDate, char * outFormatted, int inMaxLen)
{
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

  int theDay = (inDate[4] == ' ') ? (inDate[5] - '0') :
    ((inDate[4] - '0') * 10 + (inDate[5] - '0')) ;
  int theYear = (inDate[9] - '0') * 10 + (inDate[10] - '0') ;

  snprintf(outFormatted, inMaxLen, "%02d-%02d-%02d", theYear, theMonth, theDay) ;
}

//----------------------------------------------
// Helper: ClearPrevStrings
//----------------------------------------------
static void ClearPrevStrings(void)
{
  sPrevVal1[0] = '\0' ;
  sPrevVal2[0] = '\0' ;
  sPrevVal3[0] = '\0' ;
  sPrevVal4[0] = '\0' ;
  sPrevVal5[0] = '\0' ;
  sPrevVal6[0] = '\0' ;
  sPrevVal7[0] = '\0' ;
  sPrevVal8[0] = '\0' ;
  sPrevVal9[0] = '\0' ;
}

//----------------------------------------------
// Helper: RefreshValue (per-digit partial refresh)
// Compares old and new strings character by character.
// Same length: refreshes only changed digit cells (12x16 px).
// Different length: full-box fallback refresh.
// Uses sPrevBuffer (set by previous cycle's memcpy) as
// old reference. Caller must memcpy(sPrevBuffer, sFrameBuffer)
// AFTER all RefreshValue calls in each update cycle.
//----------------------------------------------
static void RefreshValue(const char * inOldStr, const char * inNewStr,
                         int inTextX, int inValueY,
                         int inBoxX, int inBoxW,
                         const char * inUnit, int inUnitX, int inUnitY)
{
  if (strcmp(inOldStr, inNewStr) == 0) return ;  // No change

  int theOldLen = (int)strlen(inOldStr) ;
  int theNewLen = (int)strlen(inNewStr) ;

  if (theOldLen == theNewLen)
  {
    // Same length — refresh only changed characters
    for (int i = 0 ; i < theNewLen ; i++)
    {
      if (inOldStr[i] != inNewStr[i])
      {
        int theDigitX = inTextX + i * kCellW ;

        // Clear and redraw this digit in framebuffer
        FB_FillRect(sFrameBuffer, theDigitX, inValueY, kCellW, kCellH, kColorWhite) ;
        FB_DrawChar(sFrameBuffer, theDigitX, inValueY, inNewStr[i], kColorBlack, kValueScale) ;

        // Partial refresh just this digit cell
        UC8151D_WritePartial(sPrevBuffer, sFrameBuffer,
                             theDigitX, inValueY, kCellW, kCellH) ;
      }
    }
  }
  else
  {
    // Length changed — clear entire box, redraw value + unit
    FB_FillRect(sFrameBuffer, inBoxX, inValueY, inBoxW, kCellH, kColorWhite) ;
    FB_DrawString(sFrameBuffer, inTextX, inValueY, inNewStr, kColorBlack, kValueScale) ;
    if (inUnit != NULL && inUnit[0] != '\0')
    {
      FB_DrawString(sFrameBuffer, inUnitX, inUnitY, inUnit, kColorBlack, 1) ;
    }

    // Partial refresh the full box region
    UC8151D_WritePartial(sPrevBuffer, sFrameBuffer,
                         inBoxX, inValueY, inBoxW, kCellH) ;
  }
}

//----------------------------------------------
// Helper: DrawHeader (inverted bar with title)
//----------------------------------------------
static void DrawHeader(const char * inLeft, const char * inRight)
{
  FB_FillRect(sFrameBuffer, 0, kHeaderY, 296, kHeaderH, kColorBlack) ;
  FB_DrawString(sFrameBuffer, 3, kHeaderY + 3, inLeft, kColorWhite, 1) ;
  if (inRight != NULL)
  {
    int theLen = (int)strlen(inRight) ;
    int theX = 296 - 3 - theLen * 6 ;
    FB_DrawString(sFrameBuffer, theX, kHeaderY + 3, inRight, kColorWhite, 1) ;
  }
  FB_DrawHLine(sFrameBuffer, 0, kHeaderH + 1, 296, kColorBlack) ;
}

//----------------------------------------------
// Helper: DrawFooter (inverted bar with version)
//----------------------------------------------
static void DrawFooter(void)
{
  FB_DrawHLine(sFrameBuffer, 0, kFooterY - 2, 296, kColorBlack) ;
  FB_FillRect(sFrameBuffer, 0, kFooterY, 296, kFooterH, kColorBlack) ;

  char theDateStr[16] ;
  FormatBuildDate(kBuildDate, theDateStr, sizeof(theDateStr)) ;

  char theFooter[48] ;
  snprintf(theFooter, sizeof(theFooter), "v%s  %s", FIRMWARE_VERSION_STRING, theDateStr) ;
  FB_DrawString(sFrameBuffer, 3, kFooterY + 3, theFooter, kColorWhite, 1) ;
}

//----------------------------------------------
// Helper: FullRefreshScreen
// Writes framebuffer to display and syncs prev
//----------------------------------------------
static void FullRefreshScreen(void)
{
  UC8151D_WriteImage(sFrameBuffer) ;
  memcpy(sPrevBuffer, sFrameBuffer, kEpdBufferSize) ;
}

//----------------------------------------------
// Helper: ContentAreaRefresh
// Partial refresh of the content area only
//----------------------------------------------
static void ContentAreaRefresh(void)
{
  UC8151D_WritePartial(sPrevBuffer, sFrameBuffer,
                       0, kContentY, 296, kContentH) ;
  memcpy(sPrevBuffer, sFrameBuffer, kEpdBufferSize) ;
}

//----------------------------------------------
// Public: StatusDisplay_Init
//----------------------------------------------
bool StatusDisplay_Init(void)
{
  if (!UC8151D_Init())
  {
    return false ;
  }

  sCurrentMode = kDisplayModeLive ;
  sLastDrawnScreen = kScreenNone ;
  sInitialized = true ;
  ClearPrevStrings() ;
  return true ;
}

//----------------------------------------------
// Public: StatusDisplay_ShowSplash
//----------------------------------------------
void StatusDisplay_ShowSplash(void)
{
  if (!sInitialized) return ;

  FB_Clear(sFrameBuffer, kColorWhite) ;

  // Centered title at scale 3
  FB_DrawString(sFrameBuffer, 28, 20, "ROCKET", kColorBlack, 3) ;
  FB_DrawString(sFrameBuffer, 10, 54, "AVIONICS", kColorBlack, 3) ;

  // Version below
  char theVersion[32] ;
  snprintf(theVersion, sizeof(theVersion), "v%s", FIRMWARE_VERSION_STRING) ;
  FB_DrawString(sFrameBuffer, 110, 96, theVersion, kColorBlack, 1) ;

  FullRefreshScreen() ;
  sLastDrawnScreen = kScreenSplash ;
}

//----------------------------------------------
// Public: StatusDisplay_GetMode / SetMode
//----------------------------------------------
DisplayMode StatusDisplay_GetMode(void)
{
  return sCurrentMode ;
}

void StatusDisplay_SetMode(DisplayMode inMode)
{
  if (inMode < kDisplayModeCount)
  {
    sCurrentMode = inMode ;
    ClearPrevStrings() ;
  }
}

//----------------------------------------------
// Public: StatusDisplay_CycleMode
// Skips graphical modes unsuitable for eInk
//----------------------------------------------
DisplayMode StatusDisplay_CycleMode(void)
{
  // Find current mode in eInk mode list
  int theIdx = -1 ;
  for (int i = 0 ; i < kEinkModeCount ; i++)
  {
    if (sEinkModes[i] == sCurrentMode)
    {
      theIdx = i ;
      break ;
    }
  }

  // Advance to next eInk-compatible mode
  theIdx = (theIdx + 1) % kEinkModeCount ;
  sCurrentMode = sEinkModes[theIdx] ;
  ClearPrevStrings() ;
  return sCurrentMode ;
}

//----------------------------------------------
// Public: StatusDisplay_PrevMode
//----------------------------------------------
DisplayMode StatusDisplay_PrevMode(void)
{
  int theIdx = -1 ;
  for (int i = 0 ; i < kEinkModeCount ; i++)
  {
    if (sEinkModes[i] == sCurrentMode)
    {
      theIdx = i ;
      break ;
    }
  }

  theIdx = (theIdx <= 0) ? kEinkModeCount - 1 : theIdx - 1 ;
  sCurrentMode = sEinkModes[theIdx] ;
  ClearPrevStrings() ;
  return sCurrentMode ;
}

//----------------------------------------------
// Public: StatusDisplay_Update (simple version)
//----------------------------------------------
void StatusDisplay_Update(
  FlightState inState,
  float inAltitudeM,
  float inVelocityMps,
  bool inLoRaConnected)
{
  // Delegate to UpdateCompact with default values
  StatusDisplay_UpdateCompact(
    inState, false, 0,
    inAltitudeM, inVelocityMps,
    0.0f, NULL,
    false, false, 0,
    0.0f, 0.0f,
    inLoRaConnected, 0, 0) ;
}

//----------------------------------------------
// Public: StatusDisplay_UpdateCompact
// Main live screen with per-digit partial refresh
// 3-column layout: alt/vel/accel + maxAlt/maxVel/gateway
//----------------------------------------------
void StatusDisplay_UpdateCompact(
  FlightState inState,
  bool inOrientationMode,
  uint8_t inRocketId,
  float inAltitudeM,
  float inVelocityMps,
  float inAccelG,
  const FlightResults * inResults,
  bool inGpsOk,
  bool inGpsFix,
  uint8_t inGpsSatellites,
  float inGpsLatitude,
  float inGpsLongitude,
  bool inLoRaConnected,
  int16_t inRssi,
  int8_t inSnr)
{
  if (!sInitialized) return ;

  sUpdateCount++ ;

  // Round to display precision to prevent flicker
  float theDispAlt = roundf(inAltitudeM * 10.0f) / 10.0f ;
  float theDispVel = roundf(inVelocityMps * 10.0f) / 10.0f ;
  float theDispAccel = roundf(inAccelG * 10.0f) / 10.0f ;
  float theDispMaxAlt = inResults ? roundf(inResults->pMaxAltitudeM * 10.0f) / 10.0f : 0.0f ;
  float theDispMaxVel = inResults ? roundf(inResults->pMaxVelocityMps * 10.0f) / 10.0f : 0.0f ;

  // Format value strings
  char theAltStr[16] ;
  char theVelStr[16] ;
  char theAccelStr[16] ;
  char theMaxAltStr[16] ;
  char theMaxVelStr[16] ;
  char theGwStr[16] ;

  FormatFloat(theAltStr, sizeof(theAltStr), theDispAlt, 1) ;
  FormatFloat(theVelStr, sizeof(theVelStr), theDispVel, 1) ;
  FormatFloat(theAccelStr, sizeof(theAccelStr), theDispAccel, 1) ;
  FormatFloat(theMaxAltStr, sizeof(theMaxAltStr), theDispMaxAlt, 1) ;
  FormatFloat(theMaxVelStr, sizeof(theMaxVelStr), theDispMaxVel, 1) ;

  // Gateway status
  if (!inLoRaConnected)
    snprintf(theGwStr, sizeof(theGwStr), "--") ;
  else if (inRssi == 0 && inSnr == 0)
    snprintf(theGwStr, sizeof(theGwStr), "Active") ;
  else if (inRssi > -70 && inSnr > 5)
    snprintf(theGwStr, sizeof(theGwStr), "Good") ;
  else if (inRssi > -90 && inSnr > 0)
    snprintf(theGwStr, sizeof(theGwStr), "Ok") ;
  else
    snprintf(theGwStr, sizeof(theGwStr), "Poor") ;

  // GPS Row 3 data
  char theSatsStr[16] ;
  char theLatStr[16] ;
  char theLonStr[16] ;

  if (inGpsOk && inGpsFix)
  {
    snprintf(theSatsStr, sizeof(theSatsStr), "%u", inGpsSatellites) ;
    FormatFloat(theLatStr, sizeof(theLatStr), inGpsLatitude, 3) ;
    FormatFloat(theLonStr, sizeof(theLonStr), inGpsLongitude, 3) ;
  }
  else if (inGpsOk)
  {
    snprintf(theSatsStr, sizeof(theSatsStr), "%u", inGpsSatellites) ;
    snprintf(theLatStr, sizeof(theLatStr), "--") ;
    snprintf(theLonStr, sizeof(theLonStr), "--") ;
  }
  else
  {
    snprintf(theSatsStr, sizeof(theSatsStr), "--") ;
    snprintf(theLatStr, sizeof(theLatStr), "--") ;
    snprintf(theLonStr, sizeof(theLonStr), "--") ;
  }

  // Determine if full refresh needed
  bool theNeedsFull = (sLastDrawnScreen != kScreenLive) ||
                      (sUpdateCount % kFullRefreshInterval == 0) ;

  if (theNeedsFull)
  {
    FB_Clear(sFrameBuffer, kColorWhite) ;

    // Header
    const char * theStateName ;
    if (inOrientationMode && inState == kFlightIdle)
      theStateName = "ORIENTATION" ;
    else
      theStateName = FlightControl_GetStateName(inState) ;

    char theHeaderLeft[32] ;
    snprintf(theHeaderLeft, sizeof(theHeaderLeft), "ROCKET AVIONICS  %s", theStateName) ;

    char theHeaderRight[16] ;
    if (inGpsOk)
      snprintf(theHeaderRight, sizeof(theHeaderRight), "R:%u  GPS:%u", inRocketId, inGpsSatellites) ;
    else
      snprintf(theHeaderRight, sizeof(theHeaderRight), "R:%u  GPS:--", inRocketId) ;

    DrawHeader(theHeaderLeft, theHeaderRight) ;

    // Row 1: ALTITUDE | VELOCITY | ACCEL
    FB_DrawString(sFrameBuffer, kCol1X, kLabel1Y, "ALTITUDE", kColorBlack, 1) ;
    FB_DrawString(sFrameBuffer, kCol2X, kLabel1Y, "VELOCITY", kColorBlack, 1) ;
    FB_DrawString(sFrameBuffer, kCol3X, kLabel1Y, "ACCEL", kColorBlack, 1) ;

    FB_DrawString(sFrameBuffer, kCol1X, kValue1Y, theAltStr, kColorBlack, kValueScale) ;
    FB_DrawString(sFrameBuffer, kAltUnitX, kUnitRow1Y, "m", kColorBlack, 1) ;

    FB_DrawString(sFrameBuffer, kCol2X, kValue1Y, theVelStr, kColorBlack, kValueScale) ;
    FB_DrawString(sFrameBuffer, kVelUnitX, kUnitRow1Y, "m/s", kColorBlack, 1) ;

    FB_DrawString(sFrameBuffer, kCol3X, kValue1Y, theAccelStr, kColorBlack, kValueScale) ;
    FB_DrawString(sFrameBuffer, kAccelUnitX, kUnitRow1Y, "g", kColorBlack, 1) ;

    FB_DrawHLine(sFrameBuffer, 4, kSep1Y, 288, kColorBlack) ;

    // Row 2: MAX ALT | MAX VEL | GATEWAY
    FB_DrawString(sFrameBuffer, kCol1X, kLabel2Y, "MAX ALT", kColorBlack, 1) ;
    FB_DrawString(sFrameBuffer, kCol2X, kLabel2Y, "MAX VEL", kColorBlack, 1) ;
    FB_DrawString(sFrameBuffer, kCol3X, kLabel2Y, "GATEWAY", kColorBlack, 1) ;

    FB_DrawString(sFrameBuffer, kCol1X, kValue2Y, theMaxAltStr, kColorBlack, kValueScale) ;
    FB_DrawString(sFrameBuffer, kMaxAltUnitX, kUnitRow2Y, "m", kColorBlack, 1) ;

    FB_DrawString(sFrameBuffer, kCol2X, kValue2Y, theMaxVelStr, kColorBlack, kValueScale) ;
    FB_DrawString(sFrameBuffer, kMaxVelUnitX, kUnitRow2Y, "m/s", kColorBlack, 1) ;

    FB_DrawString(sFrameBuffer, kCol3X, kValue2Y, theGwStr, kColorBlack, kValueScale) ;

    FB_DrawHLine(sFrameBuffer, 4, kSep2Y, 288, kColorBlack) ;

    // Row 3: SATS | LAT | LON
    FB_DrawString(sFrameBuffer, kCol1X, kLabel3Y, "SATS", kColorBlack, 1) ;
    FB_DrawString(sFrameBuffer, kCol2X, kLabel3Y, "LAT", kColorBlack, 1) ;
    FB_DrawString(sFrameBuffer, kCol3X, kLabel3Y, "LON", kColorBlack, 1) ;

    FB_DrawString(sFrameBuffer, kCol1X, kValue3Y, theSatsStr, kColorBlack, kValueScale) ;
    FB_DrawString(sFrameBuffer, kCol2X, kValue3Y, theLatStr, kColorBlack, kValueScale) ;
    FB_DrawString(sFrameBuffer, kCol3X, kValue3Y, theLonStr, kColorBlack, kValueScale) ;

    DrawFooter() ;
    FullRefreshScreen() ;
    sLastDrawnScreen = kScreenLive ;
  }
  else
  {
    // Per-digit partial refresh — only changed digits are refreshed
    RefreshValue(sPrevVal1, theAltStr, kCol1X, kValue1Y,
                 kCol1BoxX, kCol1BoxW, "m", kAltUnitX, kUnitRow1Y) ;

    RefreshValue(sPrevVal2, theVelStr, kCol2X, kValue1Y,
                 kCol2BoxX, kCol2BoxW, "m/s", kVelUnitX, kUnitRow1Y) ;

    RefreshValue(sPrevVal3, theAccelStr, kCol3X, kValue1Y,
                 kCol3BoxX, kCol3BoxW, "g", kAccelUnitX, kUnitRow1Y) ;

    RefreshValue(sPrevVal4, theMaxAltStr, kCol1X, kValue2Y,
                 kCol1BoxX, kCol1BoxW, "m", kMaxAltUnitX, kUnitRow2Y) ;

    RefreshValue(sPrevVal5, theMaxVelStr, kCol2X, kValue2Y,
                 kCol2BoxX, kCol2BoxW, "m/s", kMaxVelUnitX, kUnitRow2Y) ;

    RefreshValue(sPrevVal6, theGwStr, kCol3X, kValue2Y,
                 kCol3BoxX, kCol3BoxW, NULL, 0, 0) ;

    RefreshValue(sPrevVal7, theSatsStr, kCol1X, kValue3Y,
                 kCol1BoxX, kCol1BoxW, NULL, 0, 0) ;

    RefreshValue(sPrevVal8, theLatStr, kCol2X, kValue3Y,
                 kCol2BoxX, kCol2BoxW, NULL, 0, 0) ;

    RefreshValue(sPrevVal9, theLonStr, kCol3X, kValue3Y,
                 kCol3BoxX, kCol3BoxW, NULL, 0, 0) ;

    // Sync sPrevBuffer after all partial refreshes
    memcpy(sPrevBuffer, sFrameBuffer, kEpdBufferSize) ;
  }

  // Save current strings
  strcpy(sPrevVal1, theAltStr) ;
  strcpy(sPrevVal2, theVelStr) ;
  strcpy(sPrevVal3, theAccelStr) ;
  strcpy(sPrevVal4, theMaxAltStr) ;
  strcpy(sPrevVal5, theMaxVelStr) ;
  strcpy(sPrevVal6, theGwStr) ;
  strcpy(sPrevVal7, theSatsStr) ;
  strcpy(sPrevVal8, theLatStr) ;
  strcpy(sPrevVal9, theLonStr) ;
}

//----------------------------------------------
// Public: StatusDisplay_ShowArmed
//----------------------------------------------
void StatusDisplay_ShowArmed(float inGroundAltitude)
{
  if (!sInitialized) return ;
  if (sLastDrawnScreen == kScreenArmed) return ;

  FB_Clear(sFrameBuffer, kColorWhite) ;
  DrawHeader("** ARMED **", NULL) ;

  FB_DrawString(sFrameBuffer, 60, 30, "READY FOR", kColorBlack, 2) ;
  FB_DrawString(sFrameBuffer, 76, 54, "LAUNCH", kColorBlack, 2) ;

  char theBuffer[32] ;
  snprintf(theBuffer, sizeof(theBuffer), "Ground: %.0f Pa", inGroundAltitude) ;
  FB_DrawString(sFrameBuffer, 80, 86, theBuffer, kColorBlack, 1) ;

  DrawFooter() ;
  FullRefreshScreen() ;
  sLastDrawnScreen = kScreenArmed ;
}

//----------------------------------------------
// Public: StatusDisplay_ShowInFlight
// No-op for eInk during active flight (LoRa priority)
//----------------------------------------------
void StatusDisplay_ShowInFlight(
  FlightState inState,
  float inAltitudeM,
  float inVelocityMps,
  float inMaxAltitudeM,
  uint32_t inFlightTimeMs)
{
  (void)inState ;
  (void)inAltitudeM ;
  (void)inVelocityMps ;
  (void)inMaxAltitudeM ;
  (void)inFlightTimeMs ;
  // eInk display not updated during flight to preserve SPI1 for LoRa
}

//----------------------------------------------
// Public: StatusDisplay_ShowFlightComplete
//----------------------------------------------
void StatusDisplay_ShowFlightComplete(const FlightResults * inResults)
{
  if (!sInitialized || inResults == NULL) return ;
  if (sLastDrawnScreen == kScreenLanded) return ;

  FB_Clear(sFrameBuffer, kColorWhite) ;
  DrawHeader("FLIGHT COMPLETE", NULL) ;

  char theBuffer[32] ;

  FB_DrawString(sFrameBuffer, kCol1X, kLabel1Y, "MAX ALTITUDE", kColorBlack, 1) ;
  snprintf(theBuffer, sizeof(theBuffer), "%.1f m", inResults->pMaxAltitudeM) ;
  FB_DrawString(sFrameBuffer, kCol1X, kValue1Y, theBuffer, kColorBlack, kValueScale) ;

  FB_DrawString(sFrameBuffer, kCol2X, kLabel1Y, "MAX VELOCITY", kColorBlack, 1) ;
  snprintf(theBuffer, sizeof(theBuffer), "%.1f m/s", inResults->pMaxVelocityMps) ;
  FB_DrawString(sFrameBuffer, kCol2X, kValue1Y, theBuffer, kColorBlack, kValueScale) ;

  FB_DrawHLine(sFrameBuffer, 4, kSep1Y, 288, kColorBlack) ;

  FB_DrawString(sFrameBuffer, kCol1X, kLabel2Y, "APOGEE TIME", kColorBlack, 1) ;
  snprintf(theBuffer, sizeof(theBuffer), "%.1f s", inResults->pApogeeTimeMs / 1000.0f) ;
  FB_DrawString(sFrameBuffer, kCol1X, kValue2Y, theBuffer, kColorBlack, kValueScale) ;

  FB_DrawString(sFrameBuffer, kCol2X, kLabel2Y, "FLIGHT TIME", kColorBlack, 1) ;
  snprintf(theBuffer, sizeof(theBuffer), "%.1f s", inResults->pFlightTimeMs / 1000.0f) ;
  FB_DrawString(sFrameBuffer, kCol2X, kValue2Y, theBuffer, kColorBlack, kValueScale) ;

  FB_DrawHLine(sFrameBuffer, 4, kSep2Y, 288, kColorBlack) ;

  snprintf(theBuffer, sizeof(theBuffer), "Samples: %lu", (unsigned long)inResults->pSampleCount) ;
  FB_DrawString(sFrameBuffer, kCol1X, 96, theBuffer, kColorBlack, 1) ;

  DrawFooter() ;
  FullRefreshScreen() ;
  sLastDrawnScreen = kScreenLanded ;
}

//----------------------------------------------
// Public: StatusDisplay_ShowError
//----------------------------------------------
void StatusDisplay_ShowError(const char * inMessage)
{
  if (!sInitialized) return ;

  FB_Clear(sFrameBuffer, kColorWhite) ;
  DrawHeader("!! ERROR !!", NULL) ;
  FB_DrawString(sFrameBuffer, kCol1X, 40, inMessage, kColorBlack, 2) ;
  DrawFooter() ;
  FullRefreshScreen() ;
  sLastDrawnScreen = kScreenError ;
}

//----------------------------------------------
// Public: StatusDisplay_ShowDeviceInfo
//----------------------------------------------
void StatusDisplay_ShowDeviceInfo(
  const char * inFirmwareVersion,
  const char * inBaroType,
  bool inBaroOk,
  bool inLoRaOk,
  bool inImuOk,
  bool inGpsOk)
{
  if (!sInitialized) return ;
  if (sLastDrawnScreen == kScreenDeviceInfo) return ;

  FB_Clear(sFrameBuffer, kColorWhite) ;
  DrawHeader("DEVICE INFO", NULL) ;

  char theBuffer[32] ;
  int theY = kLabel1Y ;

  snprintf(theBuffer, sizeof(theBuffer), "FW: %s", inFirmwareVersion) ;
  FB_DrawString(sFrameBuffer, kCol1X, theY, theBuffer, kColorBlack, 1) ;
  theY += 12 ;

  snprintf(theBuffer, sizeof(theBuffer), "%s: %s", inBaroType, inBaroOk ? "OK" : "FAIL") ;
  FB_DrawString(sFrameBuffer, kCol1X, theY, theBuffer, kColorBlack, 1) ;
  theY += 12 ;

  snprintf(theBuffer, sizeof(theBuffer), "LoRa:  %s", inLoRaOk ? "OK" : "FAIL") ;
  FB_DrawString(sFrameBuffer, kCol1X, theY, theBuffer, kColorBlack, 1) ;
  theY += 12 ;

  snprintf(theBuffer, sizeof(theBuffer), "IMU:   %s", inImuOk ? "OK" : "FAIL") ;
  FB_DrawString(sFrameBuffer, kCol1X, theY, theBuffer, kColorBlack, 1) ;
  theY += 12 ;

  snprintf(theBuffer, sizeof(theBuffer), "GPS:   %s", inGpsOk ? "OK" : "FAIL") ;
  FB_DrawString(sFrameBuffer, kCol1X, theY, theBuffer, kColorBlack, 1) ;
  theY += 12 ;

  snprintf(theBuffer, sizeof(theBuffer), "Display: eInk 296x128") ;
  FB_DrawString(sFrameBuffer, kCol1X, theY, theBuffer, kColorBlack, 1) ;

  DrawFooter() ;
  FullRefreshScreen() ;
  sLastDrawnScreen = kScreenDeviceInfo ;
}

//----------------------------------------------
// Public: StatusDisplay_ShowLoRaStatus
//----------------------------------------------
void StatusDisplay_ShowLoRaStatus(
  bool inConnected,
  int16_t inRssi,
  uint32_t inPacketsSent,
  uint32_t inPacketsReceived)
{
  if (!sInitialized) return ;

  bool theNeedsFull = (sLastDrawnScreen != kScreenLoRa) ;

  if (theNeedsFull)
  {
    FB_Clear(sFrameBuffer, kColorWhite) ;
    DrawHeader("LoRa STATUS", NULL) ;
    DrawFooter() ;
  }
  else
  {
    // Clear content area for redraw
    FB_FillRect(sFrameBuffer, 0, kContentY, 296, kContentH, kColorWhite) ;
  }

  char theBuffer[32] ;

  FB_DrawString(sFrameBuffer, kCol1X, kLabel1Y, "LINK", kColorBlack, 1) ;
  snprintf(theBuffer, sizeof(theBuffer), "%s", inConnected ? "ON" : "OFF") ;
  FB_DrawString(sFrameBuffer, kCol1X, kValue1Y, theBuffer, kColorBlack, kValueScale) ;

  FB_DrawString(sFrameBuffer, kCol2X, kLabel1Y, "RSSI", kColorBlack, 1) ;
  snprintf(theBuffer, sizeof(theBuffer), "%d", inRssi) ;
  FB_DrawString(sFrameBuffer, kCol2X, kValue1Y, theBuffer, kColorBlack, kValueScale) ;

  FB_DrawString(sFrameBuffer, kCol3X, kLabel1Y, "dBm", kColorBlack, 1) ;

  FB_DrawHLine(sFrameBuffer, 4, kSep1Y, 288, kColorBlack) ;

  FB_DrawString(sFrameBuffer, kCol1X, kLabel2Y, "TX PACKETS", kColorBlack, 1) ;
  snprintf(theBuffer, sizeof(theBuffer), "%lu", (unsigned long)inPacketsSent) ;
  FB_DrawString(sFrameBuffer, kCol1X, kValue2Y, theBuffer, kColorBlack, kValueScale) ;

  FB_DrawString(sFrameBuffer, kCol2X, kLabel2Y, "RX PACKETS", kColorBlack, 1) ;
  snprintf(theBuffer, sizeof(theBuffer), "%lu", (unsigned long)inPacketsReceived) ;
  FB_DrawString(sFrameBuffer, kCol2X, kValue2Y, theBuffer, kColorBlack, kValueScale) ;

  FB_DrawHLine(sFrameBuffer, 4, kSep2Y, 288, kColorBlack) ;

  if (theNeedsFull)
  {
    FullRefreshScreen() ;
  }
  else
  {
    ContentAreaRefresh() ;
  }
  sLastDrawnScreen = kScreenLoRa ;
}

//----------------------------------------------
// Public: StatusDisplay_ShowSensorReadings
// Per-digit partial refresh (3-column layout)
//----------------------------------------------
void StatusDisplay_ShowSensorReadings(
  float inPressurePa,
  float inTemperatureC,
  float inAltitudeM)
{
  if (!sInitialized) return ;

  // Round to display precision
  float theDispPress = roundf(inPressurePa) ;
  float theDispTemp = roundf(inTemperatureC * 10.0f) / 10.0f ;
  float theDispAlt = roundf(inAltitudeM * 10.0f) / 10.0f ;

  char thePressStr[16] ;
  char theTempStr[16] ;
  char theAltStr[16] ;

  snprintf(thePressStr, sizeof(thePressStr), "%d", (int)theDispPress) ;
  FormatFloat(theTempStr, sizeof(theTempStr), theDispTemp, 1) ;
  FormatFloat(theAltStr, sizeof(theAltStr), theDispAlt, 1) ;

  bool theNeedsFull = (sLastDrawnScreen != kScreenSensors) ||
                      (sUpdateCount % kFullRefreshInterval == 0) ;

  if (theNeedsFull)
  {
    FB_Clear(sFrameBuffer, kColorWhite) ;
    DrawHeader("SENSORS", NULL) ;

    FB_DrawString(sFrameBuffer, kCol1X, kLabel1Y, "PRESSURE", kColorBlack, 1) ;
    FB_DrawString(sFrameBuffer, kCol2X, kLabel1Y, "TEMP", kColorBlack, 1) ;
    FB_DrawString(sFrameBuffer, kCol3X, kLabel1Y, "ALTITUDE", kColorBlack, 1) ;

    FB_DrawString(sFrameBuffer, kCol1X, kValue1Y, thePressStr, kColorBlack, kValueScale) ;
    FB_DrawString(sFrameBuffer, kPressUnitX, kUnitRow1Y, "Pa", kColorBlack, 1) ;

    FB_DrawString(sFrameBuffer, kCol2X, kValue1Y, theTempStr, kColorBlack, kValueScale) ;
    FB_DrawString(sFrameBuffer, kTempUnitX, kUnitRow1Y, "C", kColorBlack, 1) ;

    FB_DrawString(sFrameBuffer, kCol3X, kValue1Y, theAltStr, kColorBlack, kValueScale) ;
    FB_DrawString(sFrameBuffer, kCol3X + 60, kUnitRow1Y, "m", kColorBlack, 1) ;

    DrawFooter() ;
    FullRefreshScreen() ;
    sLastDrawnScreen = kScreenSensors ;
  }
  else
  {
    // Per-digit partial refresh — only changed digits are refreshed
    RefreshValue(sPrevVal1, thePressStr, kCol1X, kValue1Y,
                 kCol1BoxX, kCol1BoxW, "Pa", kPressUnitX, kUnitRow1Y) ;

    RefreshValue(sPrevVal2, theTempStr, kCol2X, kValue1Y,
                 kCol2BoxX, kCol2BoxW, "C", kTempUnitX, kUnitRow1Y) ;

    RefreshValue(sPrevVal3, theAltStr, kCol3X, kValue1Y,
                 kCol3BoxX, kCol3BoxW, "m", kCol3X + 60, kUnitRow1Y) ;

    // Sync sPrevBuffer after all partial refreshes
    memcpy(sPrevBuffer, sFrameBuffer, kEpdBufferSize) ;
  }

  strcpy(sPrevVal1, thePressStr) ;
  strcpy(sPrevVal2, theTempStr) ;
  strcpy(sPrevVal3, theAltStr) ;
}

//----------------------------------------------
// Public: StatusDisplay_ShowGpsStatus
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

  bool theNeedsFull = (sLastDrawnScreen != kScreenGps) ;

  if (theNeedsFull)
  {
    FB_Clear(sFrameBuffer, kColorWhite) ;
    DrawHeader("GPS STATUS", NULL) ;
    DrawFooter() ;
  }
  else
  {
    // Clear content area for redraw
    FB_FillRect(sFrameBuffer, 0, kContentY, 296, kContentH, kColorWhite) ;
  }

  char theBuffer[32] ;

  snprintf(theBuffer, sizeof(theBuffer), "Fix: %s  Sats: %d",
    inHasFix ? "YES" : "NO", inSatellites) ;
  FB_DrawString(sFrameBuffer, kCol1X, kLabel1Y, theBuffer, kColorBlack, 1) ;

  if (inHasFix)
  {
    snprintf(theBuffer, sizeof(theBuffer), "Lat: %.5f", inLatitude) ;
    FB_DrawString(sFrameBuffer, kCol1X, kValue1Y, theBuffer, kColorBlack, 1) ;

    snprintf(theBuffer, sizeof(theBuffer), "Lon: %.5f", inLongitude) ;
    FB_DrawString(sFrameBuffer, kCol1X, kValue1Y + 12, theBuffer, kColorBlack, 1) ;

    FB_DrawHLine(sFrameBuffer, 4, kSep1Y, 288, kColorBlack) ;

    snprintf(theBuffer, sizeof(theBuffer), "Speed: %.1f m/s", inSpeedMps) ;
    FB_DrawString(sFrameBuffer, kCol1X, kLabel2Y, theBuffer, kColorBlack, 1) ;

    snprintf(theBuffer, sizeof(theBuffer), "Heading: %.0f deg", inHeadingDeg) ;
    FB_DrawString(sFrameBuffer, kCol1X, kLabel2Y + 12, theBuffer, kColorBlack, 1) ;
  }
  else
  {
    FB_DrawString(sFrameBuffer, kCol1X, kValue1Y + 8, "Acquiring satellites...", kColorBlack, 1) ;
  }

  if (theNeedsFull)
  {
    FullRefreshScreen() ;
  }
  else
  {
    ContentAreaRefresh() ;
  }
  sLastDrawnScreen = kScreenGps ;
}

//----------------------------------------------
// Public: StatusDisplay_ShowAbout
//----------------------------------------------
void StatusDisplay_ShowAbout(
  const char * inVersion,
  const char * inBuildDate,
  const char * inBuildTime)
{
  (void)inBuildTime ;

  if (!sInitialized) return ;
  if (sLastDrawnScreen == kScreenAbout) return ;

  FB_Clear(sFrameBuffer, kColorWhite) ;

  FB_DrawString(sFrameBuffer, 28, 16, "Rocket Avionics", kColorBlack, 2) ;
  FB_DrawString(sFrameBuffer, 20, 38, "Flight Computer", kColorBlack, 2) ;

  FB_DrawHLine(sFrameBuffer, 20, 58, 256, kColorBlack) ;

  char theDateStr[16] ;
  FormatBuildDate(inBuildDate, theDateStr, sizeof(theDateStr)) ;

  char theBuffer[32] ;
  snprintf(theBuffer, sizeof(theBuffer), "v%s  %s", inVersion, theDateStr) ;
  FB_DrawString(sFrameBuffer, 80, 66, theBuffer, kColorBlack, 1) ;

  FB_DrawHLine(sFrameBuffer, 20, 82, 256, kColorBlack) ;

  FB_DrawString(sFrameBuffer, 52, 92, "(c) 2026 Mark Gavin", kColorBlack, 1) ;

  DrawFooter() ;
  FullRefreshScreen() ;
  sLastDrawnScreen = kScreenAbout ;
}

//----------------------------------------------
// Public: StatusDisplay_ShowRocketId
//----------------------------------------------
void StatusDisplay_ShowRocketId(
  uint8_t inRocketId,
  const char * inRocketName,
  bool inEditing)
{
  if (!sInitialized) return ;
  // Always redraw (editing changes state)

  FB_Clear(sFrameBuffer, kColorWhite) ;
  DrawHeader("ROCKET ID", NULL) ;

  if (inRocketName != NULL && inRocketName[0] != '\0')
  {
    FB_DrawString(sFrameBuffer, kCol1X, kLabel1Y, inRocketName, kColorBlack, 2) ;

    char theBuffer[16] ;
    snprintf(theBuffer, sizeof(theBuffer), "ID: %u", inRocketId) ;
    FB_DrawString(sFrameBuffer, kCol1X, kValue1Y + 8, theBuffer, kColorBlack, 2) ;
  }
  else
  {
    char theBuffer[8] ;
    snprintf(theBuffer, sizeof(theBuffer), "%u", inRocketId) ;
    FB_DrawString(sFrameBuffer, 120, 30, theBuffer, kColorBlack, 3) ;
  }

  FB_DrawString(sFrameBuffer, 100, kLabel2Y, "(0 - 15)", kColorBlack, 1) ;

  if (inEditing)
    FB_DrawString(sFrameBuffer, 80, kValue2Y, "[B] to change", kColorBlack, 1) ;
  else
    FB_DrawString(sFrameBuffer, 76, kValue2Y, "Press B to edit", kColorBlack, 1) ;

  DrawFooter() ;

  if (sLastDrawnScreen != kScreenRocketId)
  {
    FullRefreshScreen() ;
  }
  else
  {
    // Partial refresh content area on edits
    ContentAreaRefresh() ;
  }
  sLastDrawnScreen = kScreenRocketId ;
}

//----------------------------------------------
// Public: StatusDisplay_ShowPyro
// Pyro channel continuity status
//----------------------------------------------
void StatusDisplay_ShowPyro(
  float inPyro1Voltage,
  float inPyro2Voltage)
{
  if (!sInitialized) return ;

  bool theNeedsFull = (sLastDrawnScreen != kScreenPyro) ;

  if (theNeedsFull)
  {
    FB_Clear(sFrameBuffer, kColorWhite) ;
    DrawHeader("PYRO STATUS", NULL) ;
    DrawFooter() ;
  }
  else
  {
    FB_FillRect(sFrameBuffer, 0, kContentY, 296, kContentH, kColorWhite) ;
  }

  char theBuffer[32] ;

  // Row 1: Voltage readings
  FB_DrawString(sFrameBuffer, kCol1X, kLabel1Y, "DROGUE (CH1)", kColorBlack, 1) ;
  snprintf(theBuffer, sizeof(theBuffer), "%.2f", inPyro1Voltage) ;
  FB_DrawString(sFrameBuffer, kCol1X, kValue1Y, theBuffer, kColorBlack, kValueScale) ;
  FB_DrawString(sFrameBuffer, kCol1X + 72, kUnitRow1Y, "V", kColorBlack, 1) ;

  FB_DrawString(sFrameBuffer, kCol2X, kLabel1Y, "MAIN (CH2)", kColorBlack, 1) ;
  snprintf(theBuffer, sizeof(theBuffer), "%.2f", inPyro2Voltage) ;
  FB_DrawString(sFrameBuffer, kCol2X, kValue1Y, theBuffer, kColorBlack, kValueScale) ;
  FB_DrawString(sFrameBuffer, kCol2X + 72, kUnitRow1Y, "V", kColorBlack, 1) ;

  FB_DrawHLine(sFrameBuffer, 4, kSep1Y, 288, kColorBlack) ;

  // Row 2: Continuity status
  const char * theDrogueStatus = (inPyro1Voltage >= kPyroContinuityThresholdV) ? "CONNECTED" : "OPEN" ;
  const char * theMainStatus = (inPyro2Voltage >= kPyroContinuityThresholdV) ? "CONNECTED" : "OPEN" ;

  snprintf(theBuffer, sizeof(theBuffer), "DROGUE: %s", theDrogueStatus) ;
  FB_DrawString(sFrameBuffer, kCol1X, kValue2Y, theBuffer, kColorBlack, 1) ;

  snprintf(theBuffer, sizeof(theBuffer), "MAIN: %s", theMainStatus) ;
  FB_DrawString(sFrameBuffer, kCol2X, kValue2Y, theBuffer, kColorBlack, 1) ;

  FB_DrawHLine(sFrameBuffer, 4, kSep2Y, 288, kColorBlack) ;

  if (theNeedsFull)
  {
    FullRefreshScreen() ;
  }
  else
  {
    ContentAreaRefresh() ;
  }
  sLastDrawnScreen = kScreenPyro ;
}

//----------------------------------------------
// Stubs: Graphical modes not supported on eInk
//----------------------------------------------

void StatusDisplay_ShowImu(
  float inPitchDeg, float inRollDeg,
  float inAccelX, float inAccelY, float inAccelZ, float inAccelMag,
  float inGyroX, float inGyroY, float inGyroZ,
  float inHeadingDeg)
{
  (void)inPitchDeg ; (void)inRollDeg ;
  (void)inAccelX ; (void)inAccelY ; (void)inAccelZ ; (void)inAccelMag ;
  (void)inGyroX ; (void)inGyroY ; (void)inGyroZ ;
  (void)inHeadingDeg ;
}

void StatusDisplay_ShowSpin(float inSpinRate)
{
  (void)inSpinRate ;
}

void StatusDisplay_ShowCompass(
  float inHeadingDeg,
  float inMagX, float inMagY, float inMagZ)
{
  (void)inHeadingDeg ;
  (void)inMagX ; (void)inMagY ; (void)inMagZ ;
}

void StatusDisplay_ShowRates(
  const char * inBaroType,
  uint16_t inBaroHz, uint16_t inImuAccelHz, uint16_t inImuGyroHz,
  uint8_t inGpsHz, uint8_t inTelemetryHz, uint8_t inDisplayHz)
{
  (void)inBaroType ;
  (void)inBaroHz ; (void)inImuAccelHz ; (void)inImuGyroHz ;
  (void)inGpsHz ; (void)inTelemetryHz ; (void)inDisplayHz ;
}
