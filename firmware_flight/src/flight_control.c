//----------------------------------------------
// Module: flight_control.c
// Description: Flight state machine and control
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "flight_control.h"
#include "pins.h"
#include "gps.h"

#include "pico/stdlib.h"

#include <string.h>
#include <math.h>

//----------------------------------------------
// Constants
//----------------------------------------------
#define kSeaLevelTempK          288.15f     // Standard temperature at sea level (K)
#define kTempLapseRate          0.0065f     // Temperature lapse rate (K/m)
#define kGasConstant            8.31447f    // Universal gas constant (J/(mol*K))
#define kMolarMass              0.0289644f  // Molar mass of air (kg/mol)
#define kGravity                9.80665f    // Gravitational acceleration (m/s^2)

// Velocity smoothing
#define kVelocitySmoothingAlpha 0.3f        // EMA smoothing factor

// Detection thresholds
#define kApogeeDescendCount     3           // Consecutive descending samples for apogee
#define kLandingStationaryCount 50          // 5 seconds at 10 Hz

//----------------------------------------------
// State Name Table
//----------------------------------------------
static const char * sStateNames[] = {
  "IDLE" ,
  "ARMED" ,
  "BOOST" ,
  "COAST" ,
  "APOGEE" ,
  "DESCENT" ,
  "LANDED" ,
  "COMPLETE"
} ;

//----------------------------------------------
// Function: FlightControl_CalculateAltitude
//----------------------------------------------
float FlightControl_CalculateAltitude(float inPressurePa, float inGroundPressurePa)
{
  // Barometric formula:
  // h = (T0 / L) * (1 - (P/P0)^((R*L)/(g*M)))
  //
  // Where:
  //   T0 = Sea level standard temperature (288.15 K)
  //   L  = Temperature lapse rate (0.0065 K/m)
  //   P  = Pressure at altitude
  //   P0 = Pressure at reference (ground level)
  //   R  = Gas constant (8.31447 J/(mol*K))
  //   g  = Gravity (9.80665 m/s^2)
  //   M  = Molar mass of air (0.0289644 kg/mol)

  if (inGroundPressurePa <= 0.0f || inPressurePa <= 0.0f)
  {
    return 0.0f ;
  }

  float theExponent = (kGasConstant * kTempLapseRate) / (kGravity * kMolarMass) ;
  float theRatio = inPressurePa / inGroundPressurePa ;
  float theAltitude = (kSeaLevelTempK / kTempLapseRate) * (1.0f - powf(theRatio, theExponent)) ;

  return theAltitude ;
}

//----------------------------------------------
// Internal: Calculate Velocity from Altitude Change
//----------------------------------------------
static float CalculateVelocity(
  float inCurrentAltitude,
  float inPreviousAltitude,
  float inPreviousVelocity,
  float inDeltaTimeS)
{
  if (inDeltaTimeS <= 0.0f)
  {
    return inPreviousVelocity ;
  }

  float theInstantVelocity = (inCurrentAltitude - inPreviousAltitude) / inDeltaTimeS ;

  // Apply exponential moving average for smoothing
  float theSmoothedVelocity = kVelocitySmoothingAlpha * theInstantVelocity +
                              (1.0f - kVelocitySmoothingAlpha) * inPreviousVelocity ;

  return theSmoothedVelocity ;
}

//----------------------------------------------
// Internal: CRC-8 Calculation
//----------------------------------------------
static uint8_t CalculateCrc8(const uint8_t * inData, size_t inLen)
{
  uint8_t theCrc = 0xFF ;

  for (size_t i = 0 ; i < inLen ; i++)
  {
    theCrc ^= inData[i] ;
    for (int j = 0 ; j < 8 ; j++)
    {
      if (theCrc & 0x80)
      {
        theCrc = (theCrc << 1) ^ 0x31 ;
      }
      else
      {
        theCrc <<= 1 ;
      }
    }
  }

  return theCrc ;
}

//----------------------------------------------
// Function: FlightControl_Init
//----------------------------------------------
void FlightControl_Init(
  FlightController * ioController,
  TelemetrySample * inSampleBuffer,
  uint32_t inMaxSamples)
{
  memset(ioController, 0, sizeof(FlightController)) ;

  ioController->pState = kFlightIdle ;
  ioController->pTelemetryEnabled = false ;
  ioController->pSdLoggingEnabled = false ;
  ioController->pSamples = inSampleBuffer ;
  ioController->pMaxSamples = inMaxSamples ;
  ioController->pSampleCount = 0 ;
}

//----------------------------------------------
// Function: FlightControl_UpdateSensors
//----------------------------------------------
void FlightControl_UpdateSensors(
  FlightController * ioController,
  float inPressurePa,
  float inTemperatureC,
  uint32_t inCurrentTimeMs)
{
  ioController->pCurrentPressurePa = inPressurePa ;
  ioController->pCurrentTemperatureC = inTemperatureC ;

  // In IDLE mode, use first valid pressure as temporary reference for display
  // This allows seeing altitude changes before arming
  float theReferencePressure = ioController->pGroundPressurePa ;
  if (theReferencePressure <= 0.0f && inPressurePa > 0.0f)
  {
    // Use first reading as temporary reference (will be properly set at arming)
    if (ioController->pState == kFlightIdle)
    {
      // Store as temporary reference for IDLE display only
      ioController->pGroundPressurePa = inPressurePa ;
      theReferencePressure = inPressurePa ;
    }
  }

  // Calculate altitude relative to ground
  if (theReferencePressure > 0.0f)
  {
    float thePreviousAltitude = ioController->pCurrentAltitudeM ;
    ioController->pCurrentAltitudeM = FlightControl_CalculateAltitude(
      inPressurePa, theReferencePressure) ;

    // Calculate velocity
    uint32_t theDeltaMs = inCurrentTimeMs - ioController->pLastSampleTimeMs ;
    if (theDeltaMs > 0 && ioController->pLastSampleTimeMs > 0)
    {
      float theDeltaS = (float)theDeltaMs / 1000.0f ;
      ioController->pCurrentVelocityMps = CalculateVelocity(
        ioController->pCurrentAltitudeM,
        thePreviousAltitude,
        ioController->pCurrentVelocityMps,
        theDeltaS) ;
    }

    ioController->pPreviousAltitudeM = thePreviousAltitude ;
  }
  else
  {
    ioController->pCurrentAltitudeM = 0.0f ;
    ioController->pCurrentVelocityMps = 0.0f ;
  }

  ioController->pLastSampleTimeMs = inCurrentTimeMs ;
}

//----------------------------------------------
// Function: FlightControl_Update
//----------------------------------------------
void FlightControl_Update(FlightController * ioController, uint32_t inCurrentTimeMs)
{
  FlightState thePreviousState = ioController->pState ;

  switch (ioController->pState)
  {
    case kFlightIdle:
      // Waiting for arm command
      break ;

    case kFlightArmed:
      // Check for launch detection
      if (ioController->pCurrentAltitudeM > kLaunchAltitudeThresholdM ||
          ioController->pCurrentVelocityMps > kLaunchVelocityThresholdMps)
      {
        // Launch detected!
        ioController->pState = kFlightBoost ;
        ioController->pLaunchTimeMs = inCurrentTimeMs ;
        ioController->pTelemetryEnabled = true ;
        ioController->pSdLoggingEnabled = true ;
        ioController->pSampleCount = 0 ;

        // Record launch conditions in results
        ioController->pResults.pTimestampUnix = inCurrentTimeMs / 1000 ;
        ioController->pResults.pGroundPressurePa = ioController->pGroundPressurePa ;
        ioController->pResults.pGroundTemperatureC = ioController->pGroundTemperatureC ;
      }
      break ;

    case kFlightBoost:
      // Check for burnout (velocity starts decreasing significantly)
      // Transition to coast when acceleration becomes negative
      // For now, just check if velocity is decreasing
      if (ioController->pCurrentVelocityMps < ioController->pResults.pMaxVelocityMps * 0.95f &&
          ioController->pCurrentAltitudeM > 20.0f)
      {
        ioController->pState = kFlightCoast ;
      }

      // Track max velocity
      if (ioController->pCurrentVelocityMps > ioController->pResults.pMaxVelocityMps)
      {
        ioController->pResults.pMaxVelocityMps = ioController->pCurrentVelocityMps ;
      }
      break ;

    case kFlightCoast:
      // Check for apogee (velocity crosses zero)
      if (ioController->pCurrentVelocityMps <= kApogeeVelocityThresholdMps)
      {
        ioController->pDescendingCount++ ;
        if (ioController->pDescendingCount >= kApogeeDescendCount)
        {
          ioController->pState = kFlightApogee ;
          ioController->pApogeeAltitudeM = ioController->pCurrentAltitudeM ;
          ioController->pApogeeTimeMs = inCurrentTimeMs - ioController->pLaunchTimeMs ;

          // Record apogee in results
          ioController->pResults.pMaxAltitudeM = ioController->pApogeeAltitudeM ;
          ioController->pResults.pApogeeTimeMs = ioController->pApogeeTimeMs ;
        }
      }
      else
      {
        ioController->pDescendingCount = 0 ;
      }

      // Track max altitude
      if (ioController->pCurrentAltitudeM > ioController->pResults.pMaxAltitudeM)
      {
        ioController->pResults.pMaxAltitudeM = ioController->pCurrentAltitudeM ;
      }
      break ;

    case kFlightApogee:
      // Immediate transition to descent
      ioController->pState = kFlightDescent ;
      break ;

    case kFlightDescent:
      // Check for landing (velocity near zero for extended period)
      if (fabsf(ioController->pCurrentVelocityMps) < kLandingVelocityThresholdMps &&
          ioController->pCurrentAltitudeM < 10.0f)
      {
        ioController->pStationaryCount++ ;
        if (ioController->pStationaryCount >= kLandingStationaryCount)
        {
          ioController->pState = kFlightLanded ;

          // Record flight time
          ioController->pResults.pFlightTimeMs = inCurrentTimeMs - ioController->pLaunchTimeMs ;
          ioController->pResults.pSampleCount = ioController->pSampleCount ;
        }
      }
      else
      {
        ioController->pStationaryCount = 0 ;
      }
      break ;

    case kFlightLanded:
      // Waiting for download command
      // Telemetry can continue at reduced rate
      break ;

    case kFlightComplete:
      // Flight data downloaded, ready for reset
      break ;
  }

  // Record telemetry sample if in flight
  if (ioController->pState >= kFlightBoost && ioController->pState <= kFlightLanded)
  {
    if (ioController->pSampleCount < ioController->pMaxSamples)
    {
      TelemetrySample * theSample = &ioController->pSamples[ioController->pSampleCount] ;
      theSample->pTimeMs = inCurrentTimeMs - ioController->pLaunchTimeMs ;
      theSample->pAltitudeM = ioController->pCurrentAltitudeM ;
      theSample->pVelocityMps = ioController->pCurrentVelocityMps ;
      theSample->pPressurePa = ioController->pCurrentPressurePa ;
      theSample->pTemperatureC = ioController->pCurrentTemperatureC ;
      theSample->pAccelX = 0 ;  // Future: accelerometer
      theSample->pAccelY = 0 ;
      theSample->pAccelZ = 0 ;
      ioController->pSampleCount++ ;
    }
  }

  // Log state transition
  if (thePreviousState != ioController->pState)
  {
    // State changed - could trigger events here
  }
}

//----------------------------------------------
// Function: FlightControl_Arm
//----------------------------------------------
FlightError FlightControl_Arm(FlightController * ioController)
{
  if (ioController->pState == kFlightArmed)
  {
    return kFlightErrorAlreadyArmed ;
  }

  if (ioController->pState != kFlightIdle && ioController->pState != kFlightComplete)
  {
    return kFlightErrorInFlight ;
  }

  // Record ground reference
  ioController->pGroundPressurePa = ioController->pCurrentPressurePa ;
  ioController->pGroundTemperatureC = ioController->pCurrentTemperatureC ;

  // Reset counters
  ioController->pSampleCount = 0 ;
  ioController->pDescendingCount = 0 ;
  ioController->pStationaryCount = 0 ;
  ioController->pCurrentAltitudeM = 0.0f ;
  ioController->pCurrentVelocityMps = 0.0f ;
  ioController->pTelemetrySequence = 0 ;

  // Clear results
  memset(&ioController->pResults, 0, sizeof(FlightResults)) ;

  ioController->pState = kFlightArmed ;
  return kFlightErrorNone ;
}

//----------------------------------------------
// Function: FlightControl_Disarm
//----------------------------------------------
FlightError FlightControl_Disarm(FlightController * ioController)
{
  if (ioController->pState == kFlightIdle)
  {
    return kFlightErrorNotArmed ;
  }

  // Can only disarm if armed (not during flight)
  if (ioController->pState != kFlightArmed)
  {
    return kFlightErrorInFlight ;
  }

  ioController->pState = kFlightIdle ;
  ioController->pTelemetryEnabled = false ;
  ioController->pSdLoggingEnabled = false ;

  return kFlightErrorNone ;
}

//----------------------------------------------
// Function: FlightControl_Reset
//----------------------------------------------
void FlightControl_Reset(FlightController * ioController)
{
  ioController->pState = kFlightIdle ;
  ioController->pTelemetryEnabled = false ;
  ioController->pSdLoggingEnabled = false ;
  ioController->pSampleCount = 0 ;
  ioController->pDescendingCount = 0 ;
  ioController->pStationaryCount = 0 ;
}

//----------------------------------------------
// Function: FlightControl_GetState
//----------------------------------------------
FlightState FlightControl_GetState(const FlightController * inController)
{
  return inController->pState ;
}

//----------------------------------------------
// Function: FlightControl_GetStateName
//----------------------------------------------
const char * FlightControl_GetStateName(FlightState inState)
{
  if (inState >= 0 && inState <= kFlightComplete)
  {
    return sStateNames[inState] ;
  }
  return "UNKNOWN" ;
}

//----------------------------------------------
// Function: FlightControl_GetResults
//----------------------------------------------
const FlightResults * FlightControl_GetResults(const FlightController * inController)
{
  return &inController->pResults ;
}

//----------------------------------------------
// Function: FlightControl_BuildTelemetryPacket
//----------------------------------------------
uint8_t FlightControl_BuildTelemetryPacket(
  const FlightController * inController,
  LoRaTelemetryPacket * outPacket)
{
  memset(outPacket, 0, sizeof(LoRaTelemetryPacket)) ;

  outPacket->pMagic = kLoRaMagic ;
  outPacket->pPacketType = kLoRaPacketTelemetry ;
  outPacket->pSequence = inController->pTelemetrySequence ;

  // Time since launch (or 0 if not launched)
  if (inController->pLaunchTimeMs > 0)
  {
    outPacket->pTimeMs = inController->pLastSampleTimeMs - inController->pLaunchTimeMs ;
  }
  else
  {
    outPacket->pTimeMs = 0 ;
  }

  // Convert to integer representations for compact transmission
  outPacket->pAltitudeCm = (int32_t)(inController->pCurrentAltitudeM * 100.0f) ;
  outPacket->pVelocityCmps = (int16_t)(inController->pCurrentVelocityMps * 100.0f) ;
  outPacket->pPressurePa = (uint32_t)inController->pCurrentPressurePa ;
  outPacket->pTemperatureC10 = (int16_t)(inController->pCurrentTemperatureC * 10.0f) ;

  // GPS data
  const GpsData * theGps = GPS_GetData() ;
  if (theGps != NULL)
  {
    // Convert to microdegrees (deg * 1e6) for transmission
    outPacket->pGpsLatitude = (int32_t)(theGps->pLatitude * 1000000.0f) ;
    outPacket->pGpsLongitude = (int32_t)(theGps->pLongitude * 1000000.0f) ;
    outPacket->pGpsSpeedCmps = (int16_t)(theGps->pSpeedMps * 100.0f) ;
    outPacket->pGpsHeadingDeg10 = (uint16_t)(theGps->pHeadingDeg * 10.0f) ;
    outPacket->pGpsSatellites = theGps->pSatellites ;
  }

  // Accelerometer data (future)
  outPacket->pAccelX = 0 ;
  outPacket->pAccelY = 0 ;
  outPacket->pAccelZ = 0 ;

  outPacket->pState = (uint8_t)inController->pState ;

  // Build status flags
  uint8_t theFlags = inController->pStatusFlags ;
  if (theGps != NULL && theGps->pValid)
  {
    theFlags |= kFlagGpsFix ;
  }
  outPacket->pFlags = theFlags ;

  // Calculate CRC (excluding CRC field itself)
  outPacket->pCrc = CalculateCrc8((const uint8_t *)outPacket, sizeof(LoRaTelemetryPacket) - 1) ;

  return sizeof(LoRaTelemetryPacket) ;
}

//----------------------------------------------
// Function: FlightControl_ShouldSendTelemetry
//----------------------------------------------
bool FlightControl_ShouldSendTelemetry(
  const FlightController * inController,
  uint32_t inCurrentTimeMs)
{
  // During active flight: send at full rate (10 Hz)
  if (inController->pState >= kFlightBoost && inController->pState <= kFlightDescent)
  {
    return (inCurrentTimeMs - inController->pLastTelemetryTimeMs) >= kTelemetryIntervalMs ;
  }

  // When armed: send at 1 Hz (ready for launch)
  if (inController->pState == kFlightArmed)
  {
    return (inCurrentTimeMs - inController->pLastTelemetryTimeMs) >= (kTelemetryIntervalMs * 10) ;
  }

  // In idle, landed, or complete: send heartbeat at 0.5 Hz (every 2 seconds)
  // This allows link verification before arming
  return (inCurrentTimeMs - inController->pLastTelemetryTimeMs) >= 2000 ;
}

//----------------------------------------------
// Function: FlightControl_MarkTelemetrySent
//----------------------------------------------
void FlightControl_MarkTelemetrySent(
  FlightController * ioController,
  uint32_t inCurrentTimeMs)
{
  ioController->pLastTelemetryTimeMs = inCurrentTimeMs ;
  ioController->pTelemetrySequence++ ;
}
