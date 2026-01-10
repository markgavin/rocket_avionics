//----------------------------------------------
// Module: pins.h
// Description: GPIO pin assignments for Rocket
//   Avionics Ground Gateway
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit Feather RP2040 with RFM95 LoRa 915MHz (5714)
//   - Adafruit TFT FeatherWing 3.5" 480x320 (5872)
//
// Note: The Feather RP2040 RFM95 uses SPI1 (not SPI0!)
//       with different pins than generic Feather RP2040
//----------------------------------------------

#pragma once

#include "hardware/spi.h"

//----------------------------------------------
// SPI1 - Shared by LoRa Radio and TFT Display
// Feather RP2040 RFM95 uses SPI1 on pins 8, 14, 15
//----------------------------------------------
#define kPinSpiSck          14  // GP14 - SPI1 SCK
#define kPinSpiMosi         15  // GP15 - SPI1 TX/MOSI
#define kPinSpiMiso         8   // GP8 - SPI1 RX/MISO
#define kSpiPort            spi1
#define kSpiLoRaBaudrate    1000000   // 1 MHz for LoRa
#define kSpiTftBaudrate     32000000  // 32 MHz for TFT (faster updates)

//----------------------------------------------
// LoRa Radio (RFM95 built into Feather RP2040 RFM95)
//----------------------------------------------
#define kPinLoRaCs          16  // GP16 - RFM95 Chip Select
#define kPinLoRaReset       17  // GP17 - RFM95 Reset
#define kPinLoRaDio0        21  // GP21 - RFM95 DIO0 (RX Done interrupt)
#define kPinLoRaDio1        22  // GP22 - RFM95 DIO1 (optional)

//----------------------------------------------
// TFT Display (HX8357D on FeatherWing)
// Shares SPI1 bus with LoRa, different CS/DC pins
//----------------------------------------------
#define kPinTftCs           9   // GP9 - TFT Chip Select (D9)
#define kPinTftDc           10  // GP10 - TFT Data/Command (D10)
#define kPinTftRst          (-1) // No reset pin (use software reset)

//----------------------------------------------
// Status LED
//----------------------------------------------
#define kPinNeoPixel        4   // GP4 - Built-in NeoPixel
#define kPinLed             13  // GP13 - Built-in red LED (accent LED)

//----------------------------------------------
// Boot Button
//----------------------------------------------
#define kPinBootButton      7   // GP7 - Boot button

//----------------------------------------------
// LoRa Radio Configuration
// Must match flight computer settings!
//----------------------------------------------
#define kLoRaFrequency      915000000   // 915 MHz ISM band (North America)
#define kLoRaSpreadFactor   7           // SF7 for fastest data rate
#define kLoRaBandwidth      125000      // 125 kHz bandwidth
#define kLoRaCodingRate     5           // 4/5 coding rate
#define kLoRaPreambleLen    8           // Preamble length
#define kLoRaTxPower        20          // 20 dBm (100 mW)
#define kLoRaSyncWord       0x14        // Private sync word (must match!)

//----------------------------------------------
// USB Serial Settings
//----------------------------------------------
#define kUsbBaudrate        115200
#define kUsbTimeoutMs       100

//----------------------------------------------
// Protocol Constants
//----------------------------------------------
#define kJsonBufferSize     256
#define kLoRaPacketMaxSize  64

