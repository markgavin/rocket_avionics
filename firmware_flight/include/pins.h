//----------------------------------------------
// Module: pins.h
// Description: GPIO pin assignments for Rocket
//   Avionics Flight Computer
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit Feather RP2040 with RFM95 LoRa 915MHz (5714)
//   - Adafruit BMP390 Barometric Sensor (4816)
//   - Adafruit Adalogger FeatherWing (SD + PCF8523 RTC) (2922)
//   - Adafruit FeatherWing OLED 128x64 (SSD1306) (4650)
//   - Quad Side-By-Side FeatherWing Kit (4254)
//
// Note: The Feather RP2040 RFM95 uses SPI1 (not SPI0!)
//       with different pins than generic Feather RP2040
//----------------------------------------------

#pragma once

#include "hardware/i2c.h"
#include "hardware/spi.h"

//----------------------------------------------
// I2C Bus (STEMMA QT + FeatherWing bus)
// Shared by: BMP390, PCF8523 RTC, SSD1306 OLED
//----------------------------------------------
#define kPinI2cSda          2   // GP2 - I2C SDA (Feather default)
#define kPinI2cScl          3   // GP3 - I2C SCL (Feather default)
#define kI2cPort            i2c1  // Feather uses I2C1 on these pins
#define kI2cBaudrate        400000  // 400 kHz

//----------------------------------------------
// I2C Device Addresses
//----------------------------------------------
#define kI2cAddrBMP390      0x77  // Barometric sensor (or 0x76 if SDO to GND)
#define kI2cAddrPCF8523     0x68  // RTC (Adalogger FeatherWing)
#define kI2cAddrSSD1306     0x3C  // OLED display (or 0x3D)

//----------------------------------------------
// SPI1 - Shared by LoRa Radio and SD Card
// Feather RP2040 RFM95 uses SPI1 on pins 8, 14, 15
// See: https://learn.adafruit.com/feather-rp2040-rfm95/pinouts
//----------------------------------------------
#define kPinSpiSck          14  // GP14 - SPI1 SCK
#define kPinSpiMosi         15  // GP15 - SPI1 TX/MOSI
#define kPinSpiMiso         8   // GP8 - SPI1 RX/MISO
#define kSpiPort            spi1
#define kSpiLoRaBaudrate    1000000   // 1 MHz for LoRa
#define kSpiSdBaudrate      12000000  // 12 MHz for SD card

//----------------------------------------------
// LoRa Radio (RFM95 built into Feather RP2040 RFM95)
//----------------------------------------------
#define kPinLoRaCs          16  // GP16 - RFM95 Chip Select
#define kPinLoRaReset       17  // GP17 - RFM95 Reset
#define kPinLoRaDio0        21  // GP21 - RFM95 DIO0 (RX Done interrupt)
#define kPinLoRaDio1        22  // GP22 - RFM95 DIO1 (optional)

//----------------------------------------------
// SD Card (Adalogger FeatherWing)
// Shares SPI1 with LoRa, different CS pin
//----------------------------------------------
#define kPinSdCs            10  // GP10 - SD Card Chip Select (D10)

//----------------------------------------------
// OLED FeatherWing Buttons
// Active LOW (internal pull-up, grounded when pressed)
//----------------------------------------------
#define kPinButtonA         9   // GP9 - Button A (left) - D9
#define kPinButtonB         6   // GP6 - Button B (middle) - D6
#define kPinButtonC         5   // GP5 - Button C (right) - D5

//----------------------------------------------
// Status LED (Built-in NeoPixel)
//----------------------------------------------
#define kPinNeoPixel        4   // GP4 - Built-in NeoPixel

//----------------------------------------------
// GPS (Adafruit Ultimate GPS FeatherWing - PA1616D)
// Uses UART0 on Feather serial pins (bottom of board)
//----------------------------------------------
#define kPinGpsTx           0   // GP0 - UART0 TX (GPS RX)
#define kPinGpsRx           1   // GP1 - UART0 RX (GPS TX)
#define kGpsUartPort        uart0
#define kGpsUartBaudrate    9600  // GPS default baud rate
// Note: GPS Enable pin is directly connected to GND on FeatherWing (always enabled)
// If using external enable, uncomment and set pin:
// #define kPinGpsEnable    11  // Optional GPS enable pin (active low)

//----------------------------------------------
// Future: Pyro Channels (for ejection charges)
// Reserve these pins for future use
//----------------------------------------------
#define kPinPyro1           24  // GP24 - Drogue deployment (future)
#define kPinPyro2           25  // GP25 - Main deployment (future)
#define kPinPyro1Continuity 26  // GP26/A0 - Drogue continuity ADC (future)
#define kPinPyro2Continuity 27  // GP27/A1 - Main continuity ADC (future)

//----------------------------------------------
// LoRa Radio Configuration
//----------------------------------------------
#define kLoRaFrequency      915000000   // 915 MHz ISM band (North America)
#define kLoRaSpreadFactor   7           // SF7 for fastest data rate
#define kLoRaBandwidth      125000      // 125 kHz bandwidth
#define kLoRaCodingRate     5           // 4/5 coding rate
#define kLoRaPreambleLen    8           // Preamble length
#define kLoRaTxPower        20          // 20 dBm (100 mW)
#define kLoRaSyncWord       0x14        // Private sync word

//----------------------------------------------
// Telemetry Timing
//----------------------------------------------
#define kTelemetryIntervalMs        100     // 10 Hz LoRa telemetry
#define kSdLogIntervalMs            10      // 100 Hz SD card logging
#define kDisplayUpdateIntervalMs    200     // 5 Hz display update
#define kSensorSampleIntervalMs     10      // 100 Hz sensor sampling

//----------------------------------------------
// Flight Detection Thresholds
//----------------------------------------------
#define kLaunchAltitudeThresholdM   10.0f   // Altitude to detect launch
#define kLaunchVelocityThresholdMps 10.0f   // Velocity to detect launch
#define kApogeeVelocityThresholdMps 2.0f    // Velocity threshold for apogee
#define kLandingVelocityThresholdMps 1.0f   // Velocity threshold for landing
#define kLandingStationarySeconds   5       // Seconds stationary to detect landing

//----------------------------------------------
// Data Collection Constants
//----------------------------------------------
#define kMaxSamples                 6000    // 10 minutes at 10 Hz
#define kMaxSdSamples               60000   // 10 minutes at 100 Hz

//----------------------------------------------
// Heartbeat LED Timing (NeoPixel blink rates)
//----------------------------------------------
#define kHeartbeatIdlePeriodMs      1000    // Slow pulse ~1 Hz (green)
#define kHeartbeatArmedPeriodMs     500     // Medium blink ~2 Hz (yellow)
#define kHeartbeatBoostPeriodMs     100     // Fast blink ~10 Hz (red)
#define kHeartbeatDescentPeriodMs   250     // Medium-fast ~4 Hz (blue)
#define kHeartbeatLandedPeriodMs    2000    // Very slow ~0.5 Hz (white)

//----------------------------------------------
// Button Timing
//----------------------------------------------
#define kButtonDebounceMs           50      // Debounce delay
#define kButtonLongPressMs          1000    // Long press threshold

//----------------------------------------------
// Display Constants (SSD1306 128x64)
//----------------------------------------------
#define kDisplayWidth               128
#define kDisplayHeight              64

//----------------------------------------------
// SD Card Logging
//----------------------------------------------
#define kSdLogDirectory             "/flights"
#define kSdLogFilePrefix            "flight_"
#define kSdLogFileExtension         ".csv"

//----------------------------------------------
// Communication Timeout
//----------------------------------------------
#define kLoRaTimeoutMs              5000    // 5 second LoRa timeout
#define kCommandTimeoutMs           30000   // 30 second command timeout

