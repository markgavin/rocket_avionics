//----------------------------------------------
// Module: heartbeat_led.c
// Description: NeoPixel RGB LED heartbeat indicator
//   using PIO for precise WS2812 timing
//   (Flight Avionics version)
// Author: Mark Gavin
// Created: 2025-12-19
// Modified: 2026-01-10 (Rocket Avionics)
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "heartbeat_led.h"
#include "pins.h"
#include "flight_control.h"

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// Generated PIO header from ws2812.pio
#include "ws2812.pio.h"

#include <stdio.h>

#define printf(...) ((void)0)

//----------------------------------------------
// Module State
//----------------------------------------------
static PIO sPio = NULL ;
static uint sStateMachine = 0 ;
static bool sInitialized = false ;
static uint32_t sLastUpdateMs = 0 ;
static bool sLedOn = false ;
static bool sTestMode = false ;       // When true, use test color instead of state color
static uint32_t sTestColor = 0 ;      // Color to display in test mode

//----------------------------------------------
// Internal: Send Color to LED
//----------------------------------------------
static void SendColor(uint32_t inColor)
{
  if (!sInitialized)
  {
    return ;
  }

  // WS2812 expects GRB order, MSB first
  // Shift left by 8 to align 24 bits for PIO
  uint32_t theData = inColor << 8 ;

  // Non-blocking: skip if FIFO is full (avoids infinite hang).
  // Rate-limited by blink period, so FIFO is always empty in practice.
  if (!pio_sm_is_tx_fifo_full(sPio, sStateMachine))
  {
    pio_sm_put(sPio, sStateMachine, theData) ;
  }

  // Small delay to ensure PIO processes the data
  busy_wait_us_32(100) ;
}

//----------------------------------------------
// Function: HeartbeatLED_Init
//----------------------------------------------
bool HeartbeatLED_Init(uint8_t inPin)
{
  printf("Initializing heartbeat LED on GPIO%d...\n", inPin) ;

  // Use PIO0 (Feather doesn't have WiFi, so no conflict)
  sPio = pio0 ;
  int theSm = pio_claim_unused_sm(sPio, false) ;

  if (theSm < 0)
  {
    // Fall back to PIO1 if PIO0 is full
    sPio = pio1 ;
    theSm = pio_claim_unused_sm(sPio, false) ;

    if (theSm < 0)
    {
      printf("ERROR: No PIO state machine available\n") ;
      return false ;
    }
  }

  sStateMachine = (uint)theSm ;
  printf("  Using PIO%d SM%d\n", sPio == pio0 ? 0 : 1, sStateMachine) ;

  // Check if we can add the program
  if (!pio_can_add_program(sPio, &ws2812_program))
  {
    printf("ERROR: Cannot add PIO program\n") ;
    pio_sm_unclaim(sPio, sStateMachine) ;
    return false ;
  }

  // Add the program and get the offset
  uint theOffset = pio_add_program(sPio, &ws2812_program) ;
  printf("  Program loaded at offset %d\n", theOffset) ;

  // Initialize the state machine with the SDK-generated function
  // Parameters: PIO, state machine, offset, pin, frequency (800kHz), RGBW mode (false for RGB)
  ws2812_program_init(sPio, sStateMachine, theOffset, inPin, 800000, false) ;

  sInitialized = true ;
  sLastUpdateMs = 0 ;
  sLedOn = false ;

  // Test flash: Red -> Green -> Blue (5% brightness)
  printf("  Testing LED...\n") ;
  pio_sm_put_blocking(sPio, sStateMachine, kLedColorRed << 8) ;
  sleep_ms(300) ;
  pio_sm_put_blocking(sPio, sStateMachine, kLedColorGreen << 8) ;
  sleep_ms(300) ;
  pio_sm_put_blocking(sPio, sStateMachine, kLedColorBlue << 8) ;
  sleep_ms(300) ;
  pio_sm_put_blocking(sPio, sStateMachine, kLedColorOff << 8) ;

  printf("Heartbeat LED initialized (PIO%d SM%d)\n",
         sPio == pio0 ? 0 : 1, sStateMachine) ;
  return true ;
}

//----------------------------------------------
// Function: HeartbeatLED_Update
//----------------------------------------------
void HeartbeatLED_Update(
  FlightState inState ,
  uint32_t inCurrentTimeMs)
{
  if (!sInitialized)
  {
    return ;
  }

  // Determine blink period and colors based on state (or test mode)
  uint32_t thePeriodMs ;
  uint32_t theBrightColor ;
  uint32_t theDimColor ;

  if (sTestMode)
  {
    // In test mode, use the test color with the appropriate state period
    theBrightColor = sTestColor ;

    // Calculate dim version of test color (divide each component by ~4)
    uint32_t theG = (sTestColor >> 16) & 0xFF ;
    uint32_t theR = (sTestColor >> 8) & 0xFF ;
    uint32_t theB = sTestColor & 0xFF ;
    theDimColor = ((theG / 4) << 16) | ((theR / 4) << 8) | (theB / 4) ;

    // Match the blink period to the color's state
    if (sTestColor == kLedColorYellow)
    {
      thePeriodMs = kHeartbeatArmedPeriodMs ;
    }
    else if (sTestColor == kLedColorRed)
    {
      thePeriodMs = kHeartbeatBoostPeriodMs ;
    }
    else if (sTestColor == kLedColorBlue)
    {
      thePeriodMs = kHeartbeatDescentPeriodMs ;
    }
    else
    {
      thePeriodMs = kHeartbeatIdlePeriodMs ;
    }
  }
  else
  {
    switch (inState)
    {
      case kFlightArmed:
        thePeriodMs = kHeartbeatArmedPeriodMs ;
        theBrightColor = kLedColorYellow ;
        theDimColor = kLedColorYellowDim ;
        break ;

      case kFlightBoost:
        thePeriodMs = kHeartbeatBoostPeriodMs ;
        theBrightColor = kLedColorRed ;
        theDimColor = kLedColorRedDim ;
        break ;

      case kFlightCoast:
      case kFlightApogee:
        thePeriodMs = kHeartbeatDescentPeriodMs ;
        theBrightColor = kLedColorOrange ;
        theDimColor = kLedColorRedDim ;
        break ;

      case kFlightDescent:
        thePeriodMs = kHeartbeatDescentPeriodMs ;
        theBrightColor = kLedColorBlue ;
        theDimColor = kLedColorBlueDim ;
        break ;

      case kFlightLanded:
      case kFlightComplete:
        thePeriodMs = kHeartbeatLandedPeriodMs ;
        theBrightColor = kLedColorCyan ;
        theDimColor = kLedColorBlueDim ;
        break ;

      case kFlightIdle:
      default:
        thePeriodMs = kHeartbeatIdlePeriodMs ;
        theBrightColor = kLedColorGreen ;
        theDimColor = kLedColorGreenDim ;
        break ;
    }
  }

  // Check if it's time to toggle
  if ((inCurrentTimeMs - sLastUpdateMs) >= (thePeriodMs / 2))
  {
    sLastUpdateMs = inCurrentTimeMs ;
    sLedOn = !sLedOn ;

    // Send the appropriate color (bright or dim for pulsing effect)
    uint32_t theColor = sLedOn ? theBrightColor : theDimColor ;
    SendColor(theColor) ;
  }
}

//----------------------------------------------
// Function: HeartbeatLED_SetColor
//----------------------------------------------
void HeartbeatLED_SetColor(uint32_t inColor)
{
  SendColor(inColor) ;
}

//----------------------------------------------
// Function: HeartbeatLED_Off
//----------------------------------------------
void HeartbeatLED_Off(void)
{
  SendColor(kLedColorOff) ;
}

//----------------------------------------------
// Function: HeartbeatLED_SetTestMode
// Purpose: Enable/disable test mode for manual LED control
// Parameters:
//   inEnabled - true to enter test mode, false to resume normal
//   inColor - color to display (only used if inEnabled is true)
//----------------------------------------------
void HeartbeatLED_SetTestMode(bool inEnabled, uint32_t inColor)
{
  sTestMode = inEnabled ;
  sTestColor = inColor ;

  // Reset timing so pulsing starts immediately
  sLastUpdateMs = 0 ;

  // When disabling test mode, the next HeartbeatLED_Update call
  // will set the appropriate color based on current state
}
