//----------------------------------------------
// Module: pins.h
// Description: GPIO pin assignments for Rocket
//   Avionics Ground Gateway
// Author: Mark Gavin
// Created: 2026-01-10
// Modified: 2026-01-13 (Switched to OLED display)
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit Feather RP2040 with RFM95 LoRa 915MHz (5714)
//   - Adafruit FeatherWing OLED 128x64 (SSD1306/SH1107) (4650)
//   - Adafruit BMP390 Barometric Sensor (4816)
//   - Quad Side-By-Side FeatherWing Kit (4254)
//
// Note: The Feather RP2040 RFM95 uses SPI1 (not SPI0!)
//       with different pins than generic Feather RP2040
//----------------------------------------------

#pragma once

#include "hardware/spi.h"
#include "hardware/i2c.h"

//----------------------------------------------
// I2C Bus (STEMMA QT + FeatherWing bus)
// Shared by: BMP390, SSD1306 OLED
//----------------------------------------------
#define kPinI2cSda          2   // GP2 - I2C SDA (Feather default)
#define kPinI2cScl          3   // GP3 - I2C SCL (Feather default)
#define kI2cPort            i2c1  // Feather uses I2C1 on these pins
#define kI2cBaudrate        400000  // 400 kHz

//----------------------------------------------
// I2C Device Addresses
//----------------------------------------------
#define kI2cAddrBMP390      0x77  // Barometric sensor (or 0x76 if SDO to GND)
#define kI2cAddrSSD1306     0x3C  // OLED display (or 0x3D)

//----------------------------------------------
// SPI1 - LoRa Radio
// Feather RP2040 RFM95 uses SPI1 on pins 8, 14, 15
//----------------------------------------------
#define kPinSpiSck          14  // GP14 - SPI1 SCK
#define kPinSpiMosi         15  // GP15 - SPI1 TX/MOSI
#define kPinSpiMiso         8   // GP8 - SPI1 RX/MISO
#define kSpiPort            spi1
#define kSpiLoRaBaudrate    1000000   // 1 MHz for LoRa

//----------------------------------------------
// LoRa Radio (RFM95 built into Feather RP2040 RFM95)
//----------------------------------------------
#define kPinLoRaCs          16  // GP16 - RFM95 Chip Select
#define kPinLoRaReset       17  // GP17 - RFM95 Reset
#define kPinLoRaDio0        21  // GP21 - RFM95 DIO0 (RX Done interrupt)
#define kPinLoRaDio1        22  // GP22 - RFM95 DIO1 (optional)

//----------------------------------------------
// OLED FeatherWing Buttons
// Active LOW (internal pull-up, grounded when pressed)
//----------------------------------------------
#define kPinButtonA         9   // GP9 - Button A (left) - D9
#define kPinButtonB         6   // GP6 - Button B (middle) - D6
#define kPinButtonC         5   // GP5 - Button C (right) - D5

//----------------------------------------------
// Status LEDs
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
// GPS (Adafruit Ultimate GPS FeatherWing - PA1616D)
// Uses UART0 on Feather serial pins
// Set kEnableGps to 0 if GPS FeatherWing is not installed
//----------------------------------------------
#define kEnableGps          0   // Set to 1 if GPS FeatherWing is installed
#define kPinGpsTx           0   // GP0 - UART0 TX (GPS RX)
#define kPinGpsRx           1   // GP1 - UART0 RX (GPS TX)
#define kGpsUartPort        uart0
#define kGpsUartBaudrate    9600  // GPS default baud rate

//----------------------------------------------
// AirLift FeatherWing (ESP32 WiFi Co-processor)
// Uses SPI0 for communication
// Set kEnableWifi to 0 if AirLift is not installed
//
// Note: Default AirLift CS pin (13) conflicts with red LED.
//   Using default AirLift pins - red LED disabled when WiFi enabled.
//----------------------------------------------
#define kEnableWifi         0   // Disabled - using Heltec Wireless Tracker instead

// SPI1 pins for AirLift WiFi (shared bus with LoRa radio)
// On Feather RP2040 RFM95, the FeatherWing SPI header uses SPI1, NOT SPI0!
// The AirLift and LoRa radio share the SPI bus with different CS pins.
#define kPinWifiSck         14  // GP14 - SPI1 SCK (same as LoRa)
#define kPinWifiMosi        15  // GP15 - SPI1 TX/MOSI (same as LoRa)
#define kPinWifiMiso        8   // GP8 - SPI1 RX/MISO (same as LoRa)
#define kSpiWifiPort        spi1  // Use SPI1 (shared with LoRa)
#define kSpiWifiBaudrate    8000000  // 8 MHz (NINA firmware max)

// ESP32 control pins (AirLift FeatherWing defaults)
#define kPinWifiCs          13  // GP13 - ESP32 Chip Select (D13)
#define kPinWifiBusy        11  // GP11 - ESP32 Busy/Ready (D11)
#define kPinWifiReset       12  // GP12 - ESP32 Reset (D12)

// WiFi AP Configuration
#define kWifiSsid           "RocketGateway"
#define kWifiPassword       ""  // Empty = open AP (for testing)
#define kWifiChannel        6
#define kWifiServerPort     5000

//----------------------------------------------
// USB Serial Settings
//----------------------------------------------
#define kUsbBaudrate        115200
#define kUsbTimeoutMs       100

//----------------------------------------------
// Protocol Constants
//----------------------------------------------
#define kJsonBufferSize     512
#define kLoRaPacketMaxSize  64

//----------------------------------------------
// Display Constants (SSD1306/SH1107 128x64)
//----------------------------------------------
#define kDisplayWidth               128
#define kDisplayHeight              64

//----------------------------------------------
// Button Timing
//----------------------------------------------
#define kButtonDebounceMs           50      // Debounce delay
#define kButtonLongPressMs          1000    // Long press threshold

//----------------------------------------------
// Display Update Timing
//----------------------------------------------
#define kDisplayUpdateIntervalMs    200     // 5 Hz display update

