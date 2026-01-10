//----------------------------------------------
// Module: flight_control.h
// Description: Flight state machine and control
//   for rocket avionics system
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// Flight States
//----------------------------------------------
typedef enum
{
  kFlightIdle = 0 ,       // Pre-flight, on pad
  kFlightArmed ,          // Armed, waiting for launch detect
  kFlightBoost ,          // Motor burning, accelerating
  kFlightCoast ,          // Coast phase, decelerating upward
  kFlightApogee ,         // At/near apogee
  kFlightDescent ,        // Descending (under chute or ballistic)
  kFlightLanded ,         // On ground, flight complete
  kFlightComplete         // Flight data downloaded, ready for reset
} FlightState ;

//----------------------------------------------
// Flight Error Codes
//----------------------------------------------
typedef enum
{
  kFlightErrorNone = 0 ,
  kFlightErrorSensorFail ,
  kFlightErrorNoGround ,
  kFlightErrorLoRaFail ,
  kFlightErrorSdFail ,
  kFlightErrorAlreadyArmed ,
  kFlightErrorNotArmed ,
  kFlightErrorInFlight
} FlightError ;

//----------------------------------------------
// Telemetry Sample (recorded at sensor rate)
//----------------------------------------------
typedef struct
{
  uint32_t pTimeMs ;              // Time since launch (ms)
  float pAltitudeM ;              // Altitude AGL (meters)
  float pVelocityMps ;            // Vertical velocity (m/s, positive = up)
  float pPressurePa ;             // Raw pressure (Pascals)
  float pTemperatureC ;           // Temperature (Celsius)
  int16_t pAccelX ;               // Accelerometer X (raw, future)
  int16_t pAccelY ;               // Accelerometer Y (raw, future)
  int16_t pAccelZ ;               // Accelerometer Z (raw, future)
} TelemetrySample ;

//----------------------------------------------
// Flight Results Summary
//----------------------------------------------
typedef struct
{
  uint32_t pFlightId ;            // Unique flight identifier
  uint32_t pTimestampUnix ;       // Launch time (Unix timestamp)
  uint32_t pSampleCount ;         // Total samples recorded
  float pMaxAltitudeM ;           // Peak altitude AGL
  uint32_t pApogeeTimeMs ;        // Time to apogee from launch
  float pMaxVelocityMps ;         // Maximum velocity
  float pMaxAccelG ;              // Maximum acceleration (future)
  uint32_t pFlightTimeMs ;        // Total flight duration
  float pGroundPressurePa ;       // Ground level pressure at arming
  float pGroundTemperatureC ;     // Ground temperature at arming
} FlightResults ;

//----------------------------------------------
// LoRa Telemetry Packet (binary, 28 bytes)
// Sent at 10 Hz during flight
//----------------------------------------------
typedef struct __attribute__((packed))
{
  uint8_t pMagic ;                // 0xAF (Avionics Flight)
  uint8_t pPacketType ;           // 0x01 = telemetry
  uint16_t pSequence ;            // Packet sequence number
  uint32_t pTimeMs ;              // Mission time (ms)
  int32_t pAltitudeCm ;           // Altitude in centimeters
  int16_t pVelocityCmps ;         // Velocity in cm/s
  uint32_t pPressurePa ;          // Pressure (Pa)
  int16_t pTemperatureC10 ;       // Temperature * 10 (0.1C resolution)
  int16_t pAccelX ;               // Accelerometer X (raw)
  int16_t pAccelY ;               // Accelerometer Y
  int16_t pAccelZ ;               // Accelerometer Z
  uint8_t pState ;                // Flight state enum
  uint8_t pFlags ;                // Status flags
  uint8_t pCrc ;                  // CRC-8
} LoRaTelemetryPacket ;

// Telemetry magic byte
#define kLoRaMagic              0xAF

// Packet types
#define kLoRaPacketTelemetry    0x01
#define kLoRaPacketStatus       0x02
#define kLoRaPacketCommand      0x03
#define kLoRaPacketAck          0x04
#define kLoRaPacketData         0x05

// Flags byte bit definitions
#define kFlagPyro1Continuity    0x01
#define kFlagPyro2Continuity    0x02
#define kFlagSdLogging          0x04
#define kFlagLowBattery         0x08
#define kFlagGpsLock            0x10
#define kFlagSensorOk           0x20

//----------------------------------------------
// Flight Controller State
//----------------------------------------------
typedef struct
{
  FlightState pState ;
  bool pTelemetryEnabled ;        // LoRa transmission active
  bool pSdLoggingEnabled ;        // SD card logging active

  // Sensor data (current)
  float pCurrentAltitudeM ;
  float pCurrentVelocityMps ;
  float pCurrentPressurePa ;
  float pCurrentTemperatureC ;

  // Ground reference (set at arming)
  float pGroundPressurePa ;
  float pGroundTemperatureC ;

  // Data collection
  TelemetrySample * pSamples ;    // Pointer to sample buffer
  uint32_t pMaxSamples ;          // Maximum samples (buffer size)
  uint32_t pSampleCount ;         // Current sample count
  uint32_t pLaunchTimeMs ;        // System time at launch detection
  uint32_t pLastSampleTimeMs ;    // Last sample time

  // LoRa telemetry
  uint16_t pTelemetrySequence ;   // Packet sequence counter
  uint32_t pLastTelemetryTimeMs ; // Last telemetry send time

  // Apogee detection
  float pApogeeAltitudeM ;        // Recorded apogee altitude
  uint32_t pApogeeTimeMs ;        // Time of apogee
  uint8_t pDescendingCount ;      // Consecutive descending samples

  // Landing detection
  float pPreviousAltitudeM ;      // Previous altitude for velocity
  uint8_t pStationaryCount ;      // Consecutive stationary samples

  // Results
  FlightResults pResults ;

  // Status flags
  uint8_t pStatusFlags ;
} FlightController ;

//----------------------------------------------
// Function: FlightControl_Init
// Purpose: Initialize the flight controller
// Parameters:
//   ioController - Controller to initialize
//   inSampleBuffer - Buffer for telemetry samples
//   inMaxSamples - Maximum number of samples
//----------------------------------------------
void FlightControl_Init(
  FlightController * ioController,
  TelemetrySample * inSampleBuffer,
  uint32_t inMaxSamples) ;

//----------------------------------------------
// Function: FlightControl_Update
// Purpose: Update flight controller (call every loop)
// Parameters:
//   ioController - Controller to update
//   inCurrentTimeMs - Current system time
//----------------------------------------------
void FlightControl_Update(
  FlightController * ioController,
  uint32_t inCurrentTimeMs) ;

//----------------------------------------------
// Function: FlightControl_UpdateSensors
// Purpose: Update sensor readings
// Parameters:
//   ioController - Controller
//   inPressurePa - Current pressure reading
//   inTemperatureC - Current temperature reading
//   inCurrentTimeMs - Current time
//----------------------------------------------
void FlightControl_UpdateSensors(
  FlightController * ioController,
  float inPressurePa,
  float inTemperatureC,
  uint32_t inCurrentTimeMs) ;

//----------------------------------------------
// Function: FlightControl_Arm
// Purpose: Arm the flight computer
// Returns: Error code
//----------------------------------------------
FlightError FlightControl_Arm(FlightController * ioController) ;

//----------------------------------------------
// Function: FlightControl_Disarm
// Purpose: Disarm the flight computer
// Returns: Error code
//----------------------------------------------
FlightError FlightControl_Disarm(FlightController * ioController) ;

//----------------------------------------------
// Function: FlightControl_Reset
// Purpose: Reset to idle state
//----------------------------------------------
void FlightControl_Reset(FlightController * ioController) ;

//----------------------------------------------
// Function: FlightControl_GetState
// Purpose: Get current flight state
//----------------------------------------------
FlightState FlightControl_GetState(const FlightController * inController) ;

//----------------------------------------------
// Function: FlightControl_GetStateName
// Purpose: Get string name of flight state
//----------------------------------------------
const char * FlightControl_GetStateName(FlightState inState) ;

//----------------------------------------------
// Function: FlightControl_GetResults
// Purpose: Get flight results (after landing)
//----------------------------------------------
const FlightResults * FlightControl_GetResults(
  const FlightController * inController) ;

//----------------------------------------------
// Function: FlightControl_BuildTelemetryPacket
// Purpose: Build a LoRa telemetry packet
// Parameters:
//   inController - Controller
//   outPacket - Packet to fill
// Returns: Packet size in bytes
//----------------------------------------------
uint8_t FlightControl_BuildTelemetryPacket(
  const FlightController * inController,
  LoRaTelemetryPacket * outPacket) ;

//----------------------------------------------
// Function: FlightControl_ShouldSendTelemetry
// Purpose: Check if telemetry should be sent
// Parameters:
//   inController - Controller
//   inCurrentTimeMs - Current time
// Returns: true if telemetry should be sent
//----------------------------------------------
bool FlightControl_ShouldSendTelemetry(
  const FlightController * inController,
  uint32_t inCurrentTimeMs) ;

//----------------------------------------------
// Function: FlightControl_MarkTelemetrySent
// Purpose: Mark telemetry as sent (update timestamp)
// Parameters:
//   ioController - Controller
//   inCurrentTimeMs - Current time
//----------------------------------------------
void FlightControl_MarkTelemetrySent(
  FlightController * ioController,
  uint32_t inCurrentTimeMs) ;

//----------------------------------------------
// Function: FlightControl_CalculateAltitude
// Purpose: Calculate altitude from pressure
// Parameters:
//   inPressurePa - Current pressure
//   inGroundPressurePa - Reference ground pressure
// Returns: Altitude in meters AGL
//----------------------------------------------
float FlightControl_CalculateAltitude(
  float inPressurePa,
  float inGroundPressurePa) ;
