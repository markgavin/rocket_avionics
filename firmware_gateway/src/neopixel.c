//----------------------------------------------
// Module: neopixel.c
// Description: NeoPixel (WS2812) driver for RP2040
//   Uses PIO for precise timing
// Author: Mark Gavin
// Created: 2026-01-14
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "neopixel.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

//----------------------------------------------
// WS2812 PIO Program
// Inline assembly for precise timing
//----------------------------------------------

// WS2812 timing (in cycles at 800kHz):
//   T0H = 0.4us = 0.32 cycles
//   T1H = 0.8us = 0.64 cycles
//   T0L = 0.85us = 0.68 cycles
//   T1L = 0.45us = 0.36 cycles
//   Reset >= 50us

#define WS2812_T1 3
#define WS2812_T2 3
#define WS2812_T3 4

static const uint16_t sWs2812Program[] = {
    //     .wrap_target
    0x6221, //  0: out    x, 1            side 0 [2]
    0x1123, //  1: jmp    !x, 3           side 1 [1]
    0x1400, //  2: jmp    0               side 1 [4]
    0xa442, //  3: nop                    side 0 [4]
    //     .wrap
} ;

static const struct pio_program sWs2812PioProgram = {
    .instructions = sWs2812Program,
    .length = 4,
    .origin = -1,
} ;

//----------------------------------------------
// Module State
//----------------------------------------------
static PIO sPio = pio0 ;
static uint sStateMachine = 0 ;
static bool sInitialized = false ;

//----------------------------------------------
// Function: NeoPixel_Init
//----------------------------------------------
bool NeoPixel_Init(uint8_t inPin)
{
    if (sInitialized)
    {
        return true ;
    }

    // Find a free state machine
    int theSm = pio_claim_unused_sm(sPio, false) ;
    if (theSm < 0)
    {
        // Try pio1
        sPio = pio1 ;
        theSm = pio_claim_unused_sm(sPio, false) ;
        if (theSm < 0)
        {
            return false ;
        }
    }
    sStateMachine = (uint)theSm ;

    // Add program to PIO
    uint theOffset = pio_add_program(sPio, &sWs2812PioProgram) ;

    // Configure state machine
    pio_sm_config theConfig = pio_get_default_sm_config() ;

    sm_config_set_wrap(&theConfig, theOffset, theOffset + 3) ;
    sm_config_set_sideset(&theConfig, 1, false, false) ;
    sm_config_set_sideset_pins(&theConfig, inPin) ;
    sm_config_set_out_shift(&theConfig, false, true, 24) ;  // Shift left, autopull at 24 bits
    sm_config_set_fifo_join(&theConfig, PIO_FIFO_JOIN_TX) ;

    // Calculate clock divider for 800kHz bit rate
    // Each bit takes T1+T2+T3 = 10 cycles
    float theClockDiv = (float)clock_get_hz(clk_sys) / (800000.0f * 10.0f) ;
    sm_config_set_clkdiv(&theConfig, theClockDiv) ;

    // Initialize pin
    pio_gpio_init(sPio, inPin) ;
    pio_sm_set_consecutive_pindirs(sPio, sStateMachine, inPin, 1, true) ;

    // Initialize and enable state machine
    pio_sm_init(sPio, sStateMachine, theOffset, &theConfig) ;
    pio_sm_set_enabled(sPio, sStateMachine, true) ;

    sInitialized = true ;

    // Start with LED off
    NeoPixel_Off() ;

    return true ;
}

//----------------------------------------------
// Function: NeoPixel_SetColor
//----------------------------------------------
void NeoPixel_SetColor(uint32_t inColor)
{
    if (!sInitialized)
    {
        return ;
    }

    // WS2812 expects GRB order, shift left by 8 to align for 24-bit transfer
    pio_sm_put_blocking(sPio, sStateMachine, inColor << 8) ;
}

//----------------------------------------------
// Function: NeoPixel_SetRGB
//----------------------------------------------
void NeoPixel_SetRGB(uint8_t inRed, uint8_t inGreen, uint8_t inBlue)
{
    // Convert to GRB format
    uint32_t theColor = ((uint32_t)inGreen << 16) | ((uint32_t)inRed << 8) | inBlue ;
    NeoPixel_SetColor(theColor) ;
}

//----------------------------------------------
// Function: NeoPixel_Off
//----------------------------------------------
void NeoPixel_Off(void)
{
    NeoPixel_SetColor(NEOPIXEL_OFF) ;
}

//----------------------------------------------
// Function: NeoPixel_RGB
//----------------------------------------------
uint32_t NeoPixel_RGB(uint8_t inRed, uint8_t inGreen, uint8_t inBlue, uint8_t inBrightness)
{
    // Scale by brightness
    uint32_t theRed = (inRed * inBrightness) / 255 ;
    uint32_t theGreen = (inGreen * inBrightness) / 255 ;
    uint32_t theBlue = (inBlue * inBrightness) / 255 ;

    // Return GRB format
    return (theGreen << 16) | (theRed << 8) | theBlue ;
}
