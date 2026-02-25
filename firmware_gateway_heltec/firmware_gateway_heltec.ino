//----------------------------------------------
// Rocket Avionics Ground Gateway - Heltec Wireless Tracker
//
// Hardware: Heltec Wireless Tracker (ESP32-S3 + SX1262 + GPS)
//           - 0.96" TFT Display (ST7735, 160x80 pixels)
// Purpose: Bridge LoRa telemetry from flight computer to WiFi
//
// Author: Mark Gavin
// Created: 2026-01-15
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include <RadioLib.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <Preferences.h>
#include <TinyGPSPlus.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Wire.h>
#include "version.h"

//----------------------------------------------
// Pin Definitions for Heltec Wireless Tracker
//----------------------------------------------
// LoRa SX1262 pins (from Heltec schematic)
#define LORA_NSS    8
#define LORA_DIO1   14
#define LORA_RST    12
#define LORA_BUSY   13

// GPS UART
#define GPS_RX      33
#define GPS_TX      34
#define GPS_BAUD    115200  // UC6580 default baud rate

// TFT Display (ST7735 160x80)
#define TFT_MOSI    42
#define TFT_SCLK    41
#define TFT_CS      38
#define TFT_DC      40
#define TFT_RST     39
#define TFT_BL      21      // Backlight control
#define TFT_POWER   3       // VEXT power control (shared with GPS)

// Battery monitoring disabled - ADC pin undocumented and readings unstable
// #define BATTERY_ADC_PIN     4
// #define BATTERY_DIVIDER     6.4
// #define BATTERY_SAMPLES     10

// I2C for external barometer (BMP581 or BMP390)
#define I2C_SDA     5
#define I2C_SCL     6

// BMP581 Register Addresses (pre-compensated output)
#define BMP581_I2C_ADDR         0x47
#define BMP581_I2C_ADDR_ALT     0x46
#define BMP581_REG_CHIP_ID      0x01
#define BMP581_CHIP_ID          0x50
#define BMP581_REG_TEMP_DATA    0x1D
#define BMP581_REG_PRESS_DATA   0x20
#define BMP581_REG_STATUS       0x28
#define BMP581_REG_OSR_CONFIG   0x36
#define BMP581_REG_ODR_CONFIG   0x37
#define BMP581_REG_DSP_CONFIG   0x30
#define BMP581_REG_DSP_IIR      0x31
#define BMP581_REG_CMD          0x7E
#define BMP581_CMD_SOFT_RESET   0xB6

// BMP390 Register Addresses (requires calibration compensation)
#define BMP390_I2C_ADDR         0x77
#define BMP390_I2C_ADDR_ALT     0x76
#define BMP390_REG_CHIP_ID      0x00
#define BMP390_CHIP_ID          0x60
#define BMP390_REG_DATA         0x04
#define BMP390_REG_NVM_PAR      0x31
#define BMP390_REG_OSR          0x1C
#define BMP390_REG_ODR          0x1D
#define BMP390_REG_CONFIG       0x1F
#define BMP390_REG_PWR_CTRL     0x1B
#define BMP390_REG_CMD          0x7E

// Display dimensions
#define TFT_WIDTH   160
#define TFT_HEIGHT  80

// Backlight PWM (0-255, lower = dimmer = cooler)
#define TFT_BL_BRIGHTNESS  64    // 25% brightness to reduce heat

//----------------------------------------------
// Color Definitions (RGB565)
//----------------------------------------------
#define COLOR_BLACK       ST77XX_BLACK
#define COLOR_WHITE       ST77XX_WHITE
#define COLOR_GREEN       ST77XX_GREEN
#define COLOR_RED         ST77XX_RED
#define COLOR_YELLOW      ST77XX_YELLOW
#define COLOR_CYAN        ST77XX_CYAN
#define COLOR_BLUE        ST77XX_BLUE
#define COLOR_ORANGE      0xFD20
#define COLOR_DARK_GRAY   0x4208
#define COLOR_LIGHT_GRAY  0xC618

//----------------------------------------------
// LoRa Configuration (MUST match flight computer!)
//----------------------------------------------
#define LORA_FREQUENCY      915.0       // MHz
#define LORA_BANDWIDTH      125.0       // kHz
#define LORA_SPREAD_FACTOR  7
#define LORA_CODING_RATE    5           // 4/5
#define LORA_SYNC_WORD      0x14        // Private sync word
#define LORA_PREAMBLE_LEN   8
#define LORA_TX_POWER       14          // dBm (reduced from 20 to lower heat)

//----------------------------------------------
// WiFi Configuration
//----------------------------------------------
// Default credentials (used if no stored networks)
#define WIFI_DEFAULT_SSID       "AppligentGuestAirport"
#define WIFI_DEFAULT_PASSWORD   "qualitypdf"
#define WIFI_STA_TIMEOUT_MS     10000   // 10 seconds per network attempt

// AP mode (create hotspot) - fallback
#define WIFI_AP_SSID        "RocketGateway"
#define WIFI_AP_PASSWORD    ""          // Empty = open AP
#define WIFI_AP_CHANNEL     6

#define TCP_PORT        5000
#define MAX_CLIENTS     4

// Stored WiFi networks
#define MAX_WIFI_NETWORKS   4
#define WIFI_SSID_MAX_LEN   32
#define WIFI_PASS_MAX_LEN   64

typedef struct {
    char ssid[WIFI_SSID_MAX_LEN + 1];
    char password[WIFI_PASS_MAX_LEN + 1];
    uint8_t priority;       // Lower = higher priority (tried first)
    bool enabled;
} WiFiNetwork;

WiFiNetwork storedNetworks[MAX_WIFI_NETWORKS];
uint8_t storedNetworkCount = 0;
Preferences wifiPrefs;

//----------------------------------------------
// Global Objects
//----------------------------------------------
SX1262 radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
WiFiServer server(TCP_PORT);
WiFiClient clients[MAX_CLIENTS];

// TFT display using hardware SPI
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

//----------------------------------------------
// Binary Telemetry Packet (must match flight computer)
//----------------------------------------------
#define LORA_MAGIC              0xAF
#define LORA_PACKET_TELEMETRY   0x01
#define LORA_PACKET_SIZE        55
#define MAX_ROCKETS             15

typedef struct __attribute__((packed)) {
    uint8_t magic;
    uint8_t packetType;
    uint8_t rocketId;           // Rocket ID (0-15)
    uint16_t sequence;
    uint32_t timeMs;
    int32_t altitudeCm;
    int16_t velocityCmps;
    uint32_t pressurePa;
    int16_t temperatureC10;
    int32_t gpsLatitude;
    int32_t gpsLongitude;
    int16_t gpsSpeedCmps;
    uint16_t gpsHeadingDeg10;
    uint8_t gpsSatellites;
    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;
    int16_t magX;
    int16_t magY;
    int16_t magZ;
    uint8_t state;
    uint8_t flags;
    uint8_t crc;
} LoRaTelemetryPacket;

//----------------------------------------------
// Multi-Rocket Tracking
//----------------------------------------------
typedef struct {
    bool active;                // Has received data recently
    uint32_t lastUpdateMs;      // Last packet time
    float latitude;             // Last known latitude
    float longitude;            // Last known longitude
    float altitudeM;            // Last known altitude
    float distanceM;            // Distance from gateway
    uint8_t state;              // Flight state
    uint8_t satellites;         // GPS satellites
    int16_t rssi;               // Signal strength
} RocketData;

// Flight state names
const char* flightStateNames[] = {
    "IDLE", "ARMED", "BOOST", "COAST", "APOGEE", "DESCENT", "LANDED", "COMPLETE"
};

//----------------------------------------------
// State Variables
//----------------------------------------------
volatile bool loraPacketReceived = false;
String lastLoraPacket = "";
uint8_t lastLoraPacketBinary[256];
uint8_t lastLoraPacketLen = 0;
uint32_t loraPacketCount = 0;
uint32_t loraTxCount = 0;
uint32_t lastDisplayUpdate = 0;
uint32_t lastStatusPrint = 0;
int clientCount = 0;
bool wifiIsStationMode = false;         // true = station, false = AP
String wifiCurrentIP = "";
float lastRssi = 0;
float lastSnr = 0;

// Display cache (for partial updates)
bool displayNeedsFullRedraw = true;
uint32_t prevLoraPacketCount = 0;
int prevClientCount = -1;
String prevLastPacket = "";
bool prevGpsValid = false;
int prevGpsSats = -1;
double prevGpsLat = 0;
double prevGpsLon = 0;
uint32_t prevGpsChars = 0;
uint8_t prevGpsHour = 255;
uint8_t prevGpsMin = 255;
uint8_t prevGpsSec = 255;

// Runtime-adjustable settings
uint8_t currentBacklight = TFT_BL_BRIGHTNESS;  // 0-255
int8_t currentLoraTxPower = LORA_TX_POWER;     // 2-20 dBm for SX1262
int8_t currentWifiTxPower = 8;                 // WiFi power level (index into power table)

// Barometer state
bool baroOk = false;
bool baroBmp581 = false;          // true = BMP581, false = BMP390
uint8_t baroI2cAddr = 0;
float groundPressurePa = 0.0;
float groundTemperatureC = 0.0;
uint32_t lastBaroReadMs = 0;
#define BARO_READ_INTERVAL_MS 100   // 10 Hz

// BMP390 calibration data (only used if BMP390 detected)
struct {
    float T1, T2, T3;
    float P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11;
} bmp390Cal;

// Battery monitoring disabled - Wireless Tracker ADC pin undocumented
// float batteryVoltage = 0.0;
// uint32_t lastBatteryRead = 0;
// float prevBatteryVoltage = -1.0;

// Multi-rocket tracking
RocketData rockets[MAX_ROCKETS];
uint8_t activeRocketCount = 0;
uint8_t displayRocketIndex = 0;          // Which rocket to show on display
uint32_t lastDisplayCycleMs = 0;
#define DISPLAY_CYCLE_INTERVAL_MS 3000   // Cycle every 3 seconds
#define ROCKET_TIMEOUT_MS         10000  // Mark inactive after 10s no data

// Distance to current display rocket (for display)
float distanceToRocket = 0.0;    // meters
float prevDistanceToRocket = -1.0;
uint8_t prevDisplayRocketId = 255;
uint8_t prevRocketState = 255;

//----------------------------------------------
// LoRa Receive Callback (ISR)
//----------------------------------------------
void IRAM_ATTR onLoraReceive() {
    loraPacketReceived = true;
}

//----------------------------------------------
// Setup
//----------------------------------------------
void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n========================================");
    Serial.println("  ROCKET AVIONICS GATEWAY - HELTEC");
    Serial.printf("  %s\n", FIRMWARE_VERSION_STRING);
    Serial.printf("  %s %s\n", FIRMWARE_BUILD_DATE, FIRMWARE_BUILD_TIME);
    Serial.println("========================================\n");

    // Initialize display
    initDisplay();
    displaySplash();

    // Battery monitoring disabled - Wireless Tracker ADC pin undocumented

    // Initialize rocket tracking
    for (int i = 0; i < MAX_ROCKETS; i++) {
        rockets[i].active = false;
        rockets[i].lastUpdateMs = 0;
    }

    // Initialize GPS (also powers VEXT for GPS)
    initGPS();

    // Initialize barometer (optional external I2C sensor)
    displayStatus("Baro Init...", COLOR_YELLOW);
    baroOk = initBarometer();
    if (baroOk) {
        displayStatus(baroBmp581 ? "BMP581 OK" : "BMP390 OK", COLOR_GREEN);
    } else {
        displayStatus("No Baro (OK)", COLOR_DARK_GRAY);
    }
    delay(300);

    // Initialize LoRa
    displayStatus("LoRa Init...", COLOR_YELLOW);
    if (!initLoRa()) {
        displayStatus("LoRa FAILED!", COLOR_RED);
        Serial.println("ERROR: LoRa initialization failed!");
        while (true) delay(1000);
    }
    displayStatus("LoRa OK", COLOR_GREEN);
    delay(500);

    // Load WiFi networks from flash
    displayStatus("Loading WiFi...", COLOR_YELLOW);
    loadWifiNetworks();

    // Initialize WiFi (tries stored networks, falls back to AP)
    displayStatus("WiFi Init...", COLOR_YELLOW);
    initWiFi();
    displayStatus("WiFi OK", COLOR_GREEN);
    delay(500);

    // Start TCP server
    server.begin();

    // Initialize mDNS for network discovery
    initMDNS();

    // Initialize OTA updates
    initOTA();

    // Ready
    Serial.println("\n========================================");
    Serial.println("Gateway Ready!");
    Serial.printf("  WiFi Mode: %s\n", wifiIsStationMode ? "Station" : "AP");
    Serial.printf("  WiFi SSID: %s\n", wifiIsStationMode ? WiFi.SSID().c_str() : WIFI_AP_SSID);
    Serial.printf("  IP: %s\n", wifiCurrentIP.c_str());
    Serial.printf("  TCP Port: %d\n", TCP_PORT);
    Serial.printf("  LoRa Freq: %.1f MHz\n", LORA_FREQUENCY);
    if (baroOk) {
        Serial.printf("  Barometer: %s\n", baroBmp581 ? "BMP581" : "BMP390");
    } else {
        Serial.println("  Barometer: None (optional)");
    }
    Serial.println("========================================\n");

    // Initial display update
    updateDisplay();
}

//----------------------------------------------
// Main Loop
//----------------------------------------------
void loop() {
    // Handle OTA updates
    ArduinoOTA.handle();

    // Handle incoming LoRa packets
    handleLoRa();

    // Handle WiFi clients
    handleWiFiClients();

    // Read GPS
    readGPS();
    checkGpsHealth();

    // Read barometer
    readBarometer();

    // Update rocket tracking (mark inactive rockets, cycle display)
    updateRocketTracking();

    // Update display periodically (partial updates only redraw changed values)
    if (millis() - lastDisplayUpdate > 1000) {
        updateDisplay();
        lastDisplayUpdate = millis();
    }

    // Print status periodically
    if (millis() - lastStatusPrint > 10000) {
        printStatus();
        lastStatusPrint = millis();
    }

    // Small delay to reduce CPU heat - yield to other tasks
    delay(1);
}

//----------------------------------------------
// Initialize Display (ST7735 TFT)
//----------------------------------------------
void initDisplay() {
    // Enable VEXT power for display and GPS
    // VEXT controls power to both TFT and GPS module
    pinMode(TFT_POWER, OUTPUT);
    digitalWrite(TFT_POWER, HIGH);
    delay(200);  // Give GPS module time to power up properly

    // Enable backlight with PWM for brightness control
    // ESP32 Arduino 3.x uses simplified LEDC API
    ledcAttach(TFT_BL, 5000, 8);     // Pin, 5kHz frequency, 8-bit resolution
    ledcWrite(TFT_BL, TFT_BL_BRIGHTNESS); // Set brightness (0-255)

    // Initialize TFT
    // Use INITR_MINI160x80_PLUGIN for the 0.96" 160x80 display
    tft.initR(INITR_MINI160x80_PLUGIN);
    tft.setRotation(1);  // Landscape, USB port on left
    tft.fillScreen(COLOR_BLACK);

    Serial.printf("TFT Display initialized (160x80), backlight=%d/255\n", TFT_BL_BRIGHTNESS);
}

//----------------------------------------------
// Display Splash Screen
//----------------------------------------------
void displaySplash() {
    tft.fillScreen(COLOR_BLACK);

    // Title
    tft.setTextColor(COLOR_CYAN);
    tft.setTextSize(1);
    tft.setCursor(20, 10);
    tft.print("ROCKET GATEWAY");

    // Build number
    tft.setTextColor(COLOR_WHITE);
    tft.setCursor(40, 30);
    tft.print(FIRMWARE_VERSION_STRING);

    // Build date/time
    tft.setTextColor(COLOR_DARK_GRAY);
    tft.setCursor(15, 50);
    tft.printf("%s %s", FIRMWARE_BUILD_DATE, FIRMWARE_BUILD_TIME);

    tft.setCursor(40, 65);
    tft.print("Heltec Tracker");

    delay(1500);
}

//----------------------------------------------
// Display Status Message (during init)
//----------------------------------------------
void displayStatus(const char* message, uint16_t color) {
    tft.fillRect(0, 60, TFT_WIDTH, 20, COLOR_BLACK);
    tft.setTextColor(color);
    tft.setTextSize(1);

    // Center the text
    int16_t textWidth = strlen(message) * 6;
    int16_t x = (TFT_WIDTH - textWidth) / 2;
    tft.setCursor(x, 65);
    tft.print(message);
}

//----------------------------------------------
// Barometer I2C Helper Functions
//----------------------------------------------
bool baroReadRegister(uint8_t addr, uint8_t reg, uint8_t* value) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) return false;

    Wire.requestFrom(addr, (uint8_t)1);
    if (Wire.available()) {
        *value = Wire.read();
        return true;
    }
    return false;
}

bool baroReadRegisters(uint8_t addr, uint8_t reg, uint8_t* data, size_t len) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) return false;

    Wire.requestFrom(addr, (uint8_t)len);
    for (size_t i = 0; i < len && Wire.available(); i++) {
        data[i] = Wire.read();
    }
    return true;
}

bool baroWriteRegister(uint8_t addr, uint8_t reg, uint8_t value) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

//----------------------------------------------
// Initialize Barometer (BMP581 or BMP390)
//----------------------------------------------
bool initBarometer() {
    Wire.begin(I2C_SDA, I2C_SCL);
    Serial.printf("I2C initialized on SDA=%d, SCL=%d\n", I2C_SDA, I2C_SCL);

    // Try BMP581 first (better accuracy)
    uint8_t chipId = 0;

    // Try BMP581 at default address
    if (baroReadRegister(BMP581_I2C_ADDR, BMP581_REG_CHIP_ID, &chipId) && chipId == BMP581_CHIP_ID) {
        baroI2cAddr = BMP581_I2C_ADDR;
        baroBmp581 = true;
        Serial.printf("BMP581 found at 0x%02X\n", baroI2cAddr);
    }
    // Try BMP581 at alternate address
    else if (baroReadRegister(BMP581_I2C_ADDR_ALT, BMP581_REG_CHIP_ID, &chipId) && chipId == BMP581_CHIP_ID) {
        baroI2cAddr = BMP581_I2C_ADDR_ALT;
        baroBmp581 = true;
        Serial.printf("BMP581 found at 0x%02X\n", baroI2cAddr);
    }
    // Try BMP390 at default address
    else if (baroReadRegister(BMP390_I2C_ADDR, BMP390_REG_CHIP_ID, &chipId) && chipId == BMP390_CHIP_ID) {
        baroI2cAddr = BMP390_I2C_ADDR;
        baroBmp581 = false;
        Serial.printf("BMP390 found at 0x%02X\n", baroI2cAddr);
    }
    // Try BMP390 at alternate address
    else if (baroReadRegister(BMP390_I2C_ADDR_ALT, BMP390_REG_CHIP_ID, &chipId) && chipId == BMP390_CHIP_ID) {
        baroI2cAddr = BMP390_I2C_ADDR_ALT;
        baroBmp581 = false;
        Serial.printf("BMP390 found at 0x%02X\n", baroI2cAddr);
    }
    else {
        Serial.println("No barometer found (BMP581 or BMP390)");
        return false;
    }

    // Configure the sensor
    if (baroBmp581) {
        // BMP581 configuration
        // Soft reset
        baroWriteRegister(baroI2cAddr, BMP581_REG_CMD, BMP581_CMD_SOFT_RESET);
        delay(10);

        // Wait for ready
        for (int i = 0; i < 50; i++) {
            uint8_t status;
            if (baroReadRegister(baroI2cAddr, BMP581_REG_STATUS, &status)) {
                if ((status & 0x01) && !(status & 0x02)) break;
            }
            delay(1);
        }

        // OSR config: pressure 16x, temp 1x
        baroWriteRegister(baroI2cAddr, BMP581_REG_OSR_CONFIG, (0x00 << 3) | 0x04);

        // ODR config: 50Hz continuous mode
        baroWriteRegister(baroI2cAddr, BMP581_REG_ODR_CONFIG, 0x0F | (0x03 << 5));

        // DSP IIR: coefficient 3 for both
        baroWriteRegister(baroI2cAddr, BMP581_REG_DSP_IIR, (0x02 << 3) | 0x02);

        // DSP config: enable IIR
        baroWriteRegister(baroI2cAddr, BMP581_REG_DSP_CONFIG, 0x01 | (1 << 3) | (1 << 6));

        Serial.println("BMP581 configured");
    }
    else {
        // BMP390 configuration - read calibration data
        uint8_t calData[21];
        if (baroReadRegisters(baroI2cAddr, BMP390_REG_NVM_PAR, calData, 21)) {
            // Parse calibration coefficients (from datasheet)
            uint16_t T1_u = (uint16_t)calData[1] << 8 | calData[0];
            uint16_t T2_u = (uint16_t)calData[3] << 8 | calData[2];
            int8_t T3_s = (int8_t)calData[4];

            bmp390Cal.T1 = (float)T1_u / powf(2, -8);
            bmp390Cal.T2 = (float)T2_u / powf(2, 30);
            bmp390Cal.T3 = (float)T3_s / powf(2, 48);

            int16_t P1_s = (int16_t)((uint16_t)calData[6] << 8 | calData[5]);
            int16_t P2_s = (int16_t)((uint16_t)calData[8] << 8 | calData[7]);
            int8_t P3_s = (int8_t)calData[9];
            int8_t P4_s = (int8_t)calData[10];
            uint16_t P5_u = (uint16_t)calData[12] << 8 | calData[11];
            uint16_t P6_u = (uint16_t)calData[14] << 8 | calData[13];
            int8_t P7_s = (int8_t)calData[15];
            int8_t P8_s = (int8_t)calData[16];
            int16_t P9_s = (int16_t)((uint16_t)calData[18] << 8 | calData[17]);
            int8_t P10_s = (int8_t)calData[19];
            int8_t P11_s = (int8_t)calData[20];

            bmp390Cal.P1 = ((float)P1_s - powf(2, 14)) / powf(2, 20);
            bmp390Cal.P2 = ((float)P2_s - powf(2, 14)) / powf(2, 29);
            bmp390Cal.P3 = (float)P3_s / powf(2, 32);
            bmp390Cal.P4 = (float)P4_s / powf(2, 37);
            bmp390Cal.P5 = (float)P5_u / powf(2, -3);
            bmp390Cal.P6 = (float)P6_u / powf(2, 6);
            bmp390Cal.P7 = (float)P7_s / powf(2, 8);
            bmp390Cal.P8 = (float)P8_s / powf(2, 15);
            bmp390Cal.P9 = (float)P9_s / powf(2, 48);
            bmp390Cal.P10 = (float)P10_s / powf(2, 48);
            bmp390Cal.P11 = (float)P11_s / powf(2, 65);
        }

        // Configure OSR: pressure 8x, temp 2x
        baroWriteRegister(baroI2cAddr, BMP390_REG_OSR, (0x01 << 3) | 0x03);

        // Configure ODR: 50Hz
        baroWriteRegister(baroI2cAddr, BMP390_REG_ODR, 0x02);

        // Enable pressure and temperature, normal mode
        baroWriteRegister(baroI2cAddr, BMP390_REG_PWR_CTRL, 0x33);

        Serial.println("BMP390 configured with calibration");
    }

    return true;
}

//----------------------------------------------
// Read Barometer Data
//----------------------------------------------
void readBarometer() {
    if (!baroOk) return;

    if (millis() - lastBaroReadMs < BARO_READ_INTERVAL_MS) return;
    lastBaroReadMs = millis();

    if (baroBmp581) {
        // BMP581: pre-compensated output
        uint8_t tempData[3], pressData[3];

        if (!baroReadRegisters(baroI2cAddr, BMP581_REG_TEMP_DATA, tempData, 3)) return;
        if (!baroReadRegisters(baroI2cAddr, BMP581_REG_PRESS_DATA, pressData, 3)) return;

        // Parse 24-bit values
        int32_t rawTemp = (int32_t)tempData[0] | ((int32_t)tempData[1] << 8) | ((int32_t)tempData[2] << 16);
        if (rawTemp & 0x800000) rawTemp |= 0xFF000000;  // Sign extend

        uint32_t rawPress = (uint32_t)pressData[0] | ((uint32_t)pressData[1] << 8) | ((uint32_t)pressData[2] << 16);

        // BMP581 conversion (1/65536 °C per LSB, 1/64 Pa per LSB)
        groundTemperatureC = (float)rawTemp / 65536.0f;
        groundPressurePa = (float)rawPress / 64.0f;
    }
    else {
        // BMP390: requires calibration compensation
        uint8_t data[6];
        if (!baroReadRegisters(baroI2cAddr, BMP390_REG_DATA, data, 6)) return;

        uint32_t rawPress = (uint32_t)data[0] | ((uint32_t)data[1] << 8) | ((uint32_t)data[2] << 16);
        uint32_t rawTemp = (uint32_t)data[3] | ((uint32_t)data[4] << 8) | ((uint32_t)data[5] << 16);

        // Temperature compensation
        float pd1 = (float)rawTemp - bmp390Cal.T1;
        float pd2 = pd1 * bmp390Cal.T2;
        float tempComp = pd2 + (pd1 * pd1) * bmp390Cal.T3;

        // Pressure compensation
        float pd3 = (float)rawPress;
        float pd4 = pd3 - bmp390Cal.P5 - bmp390Cal.P6 * tempComp;
        float po1 = bmp390Cal.P9 * tempComp * tempComp;
        float po2 = po1 + bmp390Cal.P8 * tempComp + bmp390Cal.P7;
        float po3 = pd4 * (po2 + bmp390Cal.P4 * tempComp * tempComp * tempComp + tempComp * (bmp390Cal.P3 + tempComp * bmp390Cal.P2) + bmp390Cal.P1);
        float pressComp = po3 + (bmp390Cal.P10 + bmp390Cal.P11 * tempComp) * pd4 * pd4;

        groundTemperatureC = tempComp;
        groundPressurePa = pressComp;
    }
}

//----------------------------------------------
// Initialize GPS
//----------------------------------------------
void initGPS() {
    // VEXT must be HIGH for GPS (already set in initDisplay)
    // Give GPS module extra time to power up after VEXT enabled
    delay(100);

    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);
    Serial.println("GPS UART initialized");
    Serial.printf("  RX pin: %d, TX pin: %d, Baud: %d\n", GPS_RX, GPS_TX, GPS_BAUD);

    // Wait for GPS module to be ready after power-up
    delay(1000);

    // Flush any pending data
    while (gpsSerial.available()) {
        gpsSerial.read();
    }

    // Configure UC6580/UM600 GPS module
    Serial.println("Sending GPS configuration commands...");

    // Enable active antenna power (3.3V bias on antenna line)
    // Required for external amplified antennas
    // $CFGANT,1 = enable antenna power
    gpsSerial.println("$CFGANT,1*04");
    delay(100);

    // $CFGSYS - Configure GNSS systems
    // h11 = GPS+BDS+GALILEO+SBAS+QZSS (reported to work by Meshtastic users)
    gpsSerial.println("$CFGSYS,h11*61");
    delay(100);

    // $CFGMSG - Enable NMEA messages
    // Format: $CFGMSG,<msgType>,<rate>*checksum
    // Enable GGA at 1Hz
    gpsSerial.println("$CFGMSG,0,1,1*23");
    delay(50);
    // Enable RMC at 1Hz
    gpsSerial.println("$CFGMSG,4,1,1*27");
    delay(50);
    // Enable GSV at 1Hz (satellites in view)
    gpsSerial.println("$CFGMSG,3,1,1*26");
    delay(50);
    // Enable GSA at 1Hz (DOP and active satellites)
    gpsSerial.println("$CFGMSG,2,1,1*25");
    delay(50);

    Serial.println("GPS initialized with UC6580 configuration");
}

//----------------------------------------------
// Check GPS module presence (called periodically)
//----------------------------------------------
void checkGpsHealth() {
    // GPS health check - serial output only for local debugging
    static uint32_t lastCheck = 0;
    static uint32_t charsReceived = 0;

    if (millis() - lastCheck > 30000) {  // Every 30 seconds
        uint32_t newChars = gps.charsProcessed();

        if (newChars == charsReceived) {
            Serial.println("GPS WARNING: No data received - check VEXT power");
        }

        charsReceived = newChars;
        lastCheck = millis();
    }
}

//----------------------------------------------
// Initialize LoRa
//----------------------------------------------
bool initLoRa() {
    Serial.println("Initializing LoRa SX1262...");

    int state = radio.begin(
        LORA_FREQUENCY,
        LORA_BANDWIDTH,
        LORA_SPREAD_FACTOR,
        LORA_CODING_RATE,
        LORA_SYNC_WORD,
        LORA_TX_POWER,
        LORA_PREAMBLE_LEN
    );

    if (state != RADIOLIB_ERR_NONE) {
        Serial.printf("  LoRa begin failed: %d\n", state);
        return false;
    }

    // Set up receive callback
    radio.setDio1Action(onLoraReceive);

    // Start receiving
    state = radio.startReceive();
    if (state != RADIOLIB_ERR_NONE) {
        Serial.printf("  LoRa startReceive failed: %d\n", state);
        return false;
    }

    Serial.println("  LoRa initialized successfully");
    Serial.printf("  Frequency: %.1f MHz\n", LORA_FREQUENCY);
    Serial.printf("  Bandwidth: %.1f kHz\n", LORA_BANDWIDTH);
    Serial.printf("  SF: %d, CR: 4/%d\n", LORA_SPREAD_FACTOR, LORA_CODING_RATE);
    Serial.printf("  Sync Word: 0x%02X\n", LORA_SYNC_WORD);

    return true;
}

//----------------------------------------------
// Initialize WiFi (Station mode with AP fallback)
//----------------------------------------------
void initWiFi() {
    // Sort networks by priority before trying
    sortNetworksByPriority();

    // Try each stored network in priority order
    bool connected = false;

    if (storedNetworkCount > 0) {
        Serial.printf("Attempting WiFi Station mode (%d networks stored)\n", storedNetworkCount);
        WiFi.mode(WIFI_STA);
        WiFi.setTxPower(WIFI_POWER_8_5dBm);  // Reduce WiFi TX power to lower heat

        for (int i = 0; i < storedNetworkCount && !connected; i++) {
            if (!storedNetworks[i].enabled) {
                Serial.printf("  [%d] %s - skipped (disabled)\n", i, storedNetworks[i].ssid);
                continue;
            }

            Serial.printf("  [%d] Trying: %s\n", i, storedNetworks[i].ssid);
            displayStatus(storedNetworks[i].ssid, COLOR_YELLOW);

            WiFi.begin(storedNetworks[i].ssid, storedNetworks[i].password);

            uint32_t startTime = millis();
            while (WiFi.status() != WL_CONNECTED) {
                if (millis() - startTime > WIFI_STA_TIMEOUT_MS) {
                    Serial.println("      timeout");
                    WiFi.disconnect();
                    delay(100);
                    break;
                }
                delay(250);
                Serial.print(".");
            }

            if (WiFi.status() == WL_CONNECTED) {
                wifiIsStationMode = true;
                wifiCurrentIP = WiFi.localIP().toString();
                Serial.printf("\n  Connected to: %s\n", storedNetworks[i].ssid);
                Serial.printf("  IP: %s\n", wifiCurrentIP.c_str());
                connected = true;
            }
        }
    }

    // Fall back to AP mode if no station connection
    if (!connected) {
        Serial.println("Starting WiFi AP mode (fallback)...");

        WiFi.mode(WIFI_AP);
        WiFi.setTxPower(WIFI_POWER_8_5dBm);  // Reduce WiFi TX power to lower heat

        if (strlen(WIFI_AP_PASSWORD) >= 8) {
            WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD, WIFI_AP_CHANNEL);
        } else {
            WiFi.softAP(WIFI_AP_SSID, NULL, WIFI_AP_CHANNEL);
        }

        wifiIsStationMode = false;
        wifiCurrentIP = WiFi.softAPIP().toString();
        Serial.printf("  AP SSID: %s\n", WIFI_AP_SSID);
        Serial.printf("  AP IP: %s\n", wifiCurrentIP.c_str());
        Serial.printf("  Channel: %d\n", WIFI_AP_CHANNEL);
    }
}

//----------------------------------------------
// Initialize mDNS for Network Discovery
// Allows clients to find gateway at RocketGateway.local
//----------------------------------------------
void initMDNS() {
    const char* hostname = "RocketGateway";

    if (MDNS.begin(hostname)) {
        Serial.printf("mDNS started: %s.local\n", hostname);

        // Advertise TCP service for rocket telemetry
        MDNS.addService("_rocket", "_tcp", TCP_PORT);

        // Add TXT records with device info
        MDNS.addServiceTxt("_rocket", "_tcp", "version", FIRMWARE_VERSION_STRING);
        MDNS.addServiceTxt("_rocket", "_tcp", "type", "gateway");
        MDNS.addServiceTxt("_rocket", "_tcp", "board", "heltec");

        Serial.printf("  Service: _rocket._tcp.local port %d\n", TCP_PORT);
    } else {
        Serial.println("mDNS failed to start");
    }
}

//----------------------------------------------
// Initialize OTA (Over-The-Air) Updates
//----------------------------------------------
void initOTA() {
    // Set hostname for OTA
    ArduinoOTA.setHostname("RocketGateway");

    // Optional: Set password for OTA updates (uncomment to enable)
    // ArduinoOTA.setPassword("rocket");

    ArduinoOTA.onStart([]() {
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "firmware" : "filesystem";
        Serial.println("OTA Start: " + type);
        // Show OTA in progress on display
        tft.fillScreen(COLOR_BLACK);
        tft.setTextColor(COLOR_CYAN);
        tft.setTextSize(1);
        tft.setCursor(30, 30);
        tft.print("OTA UPDATE");
        tft.setCursor(35, 50);
        tft.print("in progress...");
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nOTA Complete!");
        tft.fillScreen(COLOR_BLACK);
        tft.setTextColor(COLOR_GREEN);
        tft.setCursor(30, 40);
        tft.print("OTA Complete!");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        int percent = (progress / (total / 100));
        Serial.printf("OTA Progress: %u%%\r", percent);
        // Update progress bar on display
        tft.fillRect(20, 60, 120, 10, COLOR_DARK_GRAY);
        tft.fillRect(20, 60, (percent * 120) / 100, 10, COLOR_GREEN);
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("OTA Error[%u]: ", error);
        String errMsg = "Unknown";
        if (error == OTA_AUTH_ERROR) errMsg = "Auth Failed";
        else if (error == OTA_BEGIN_ERROR) errMsg = "Begin Failed";
        else if (error == OTA_CONNECT_ERROR) errMsg = "Connect Failed";
        else if (error == OTA_RECEIVE_ERROR) errMsg = "Receive Failed";
        else if (error == OTA_END_ERROR) errMsg = "End Failed";
        Serial.println(errMsg);
        tft.fillScreen(COLOR_BLACK);
        tft.setTextColor(COLOR_RED);
        tft.setCursor(20, 40);
        tft.print("OTA Error:");
        tft.setCursor(20, 55);
        tft.print(errMsg);
    });

    ArduinoOTA.begin();
    Serial.println("OTA initialized");
}

//----------------------------------------------
// Handle LoRa Receive
//----------------------------------------------
void handleLoRa() {
    if (!loraPacketReceived) {
        return;
    }
    loraPacketReceived = false;

    // Read the packet as binary
    lastLoraPacketLen = radio.getPacketLength();
    int state = radio.readData(lastLoraPacketBinary, lastLoraPacketLen);

    if (state == RADIOLIB_ERR_NONE) {
        loraPacketCount++;

        lastRssi = radio.getRSSI();
        lastSnr = radio.getSNR();

        Serial.printf("LoRa RX [%d]: RSSI=%.1f SNR=%.1f len=%d\n",
                      loraPacketCount, lastRssi, lastSnr, lastLoraPacketLen);

        // Decode and forward to WiFi clients as JSON
        if (lastLoraPacketLen >= 2 && lastLoraPacketBinary[0] == LORA_MAGIC) {
            uint8_t packetType = lastLoraPacketBinary[1];

            switch (packetType) {
                case LORA_PACKET_TELEMETRY:  // 0x01
                    if (lastLoraPacketLen >= sizeof(LoRaTelemetryPacket)) {
                        forwardTelemetryAsJson();
                        // Send ACK back to flight computer with signal quality info
                        sendAckToFlightComputer();
                    } else {
                        forwardAsHex();
                    }
                    break;

                case 0x04:  // kLoRaPacketAck
                    forwardAckAsJson();
                    break;

                case 0x06:  // kLoRaPacketStorageList (flash_list)
                    forwardFlashListAsJson();
                    break;

                case 0x07:  // kLoRaPacketStorageData (flash_data/flash_header)
                    forwardFlashDataAsJson();
                    break;

                case 0x08:  // kLoRaPacketInfo (fc_info)
                    forwardDeviceInfoAsJson();
                    break;

                case 0x09:  // kLoRaPacketBaroCompare (sensor comparison)
                    forwardBaroCompareAsJson();
                    break;

                default:
                    // Unknown packet type - forward as hex for debugging
                    forwardAsHex();
                    break;
            }
        } else {
            // Not a recognized packet - forward as hex
            forwardAsHex();
        }

        // Update display preview
        lastLoraPacket = String(lastLoraPacketLen) + "B";

    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
        Serial.println("LoRa RX: CRC error");
    } else {
        Serial.printf("LoRa RX error: %d\n", state);
    }

    // Restart receiving
    radio.startReceive();
}

//----------------------------------------------
// Update Rocket Tracking (timeouts and display cycling)
//----------------------------------------------
void updateRocketTracking() {
    uint32_t now = millis();

    // Count active rockets and mark timed-out ones as inactive
    activeRocketCount = 0;
    for (int i = 0; i < MAX_ROCKETS; i++) {
        if (rockets[i].active) {
            if (now - rockets[i].lastUpdateMs > ROCKET_TIMEOUT_MS) {
                rockets[i].active = false;
            } else {
                activeRocketCount++;
            }
        }
    }

    // Cycle to next active rocket for display
    if (activeRocketCount > 1 && now - lastDisplayCycleMs > DISPLAY_CYCLE_INTERVAL_MS) {
        lastDisplayCycleMs = now;

        // Find next active rocket
        uint8_t startIdx = displayRocketIndex;
        do {
            displayRocketIndex = (displayRocketIndex + 1) % MAX_ROCKETS;
        } while (!rockets[displayRocketIndex].active && displayRocketIndex != startIdx);
    }

    // Update distance for display rocket
    if (displayRocketIndex < MAX_ROCKETS && rockets[displayRocketIndex].active) {
        if (gps.location.isValid()) {
            distanceToRocket = calculateDistance(
                gps.location.lat(), gps.location.lng(),
                rockets[displayRocketIndex].latitude,
                rockets[displayRocketIndex].longitude
            );
        } else {
            distanceToRocket = rockets[displayRocketIndex].distanceM;
        }
    }
}

//----------------------------------------------
// Calculate distance between two GPS coordinates (Haversine formula)
// Returns distance in meters
//----------------------------------------------
float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
    // Return 0 if either position is invalid
    if (lat1 == 0.0 && lon1 == 0.0) return 0.0;
    if (lat2 == 0.0 && lon2 == 0.0) return 0.0;

    const float R = 6371000.0;  // Earth radius in meters
    float dLat = radians(lat2 - lat1);
    float dLon = radians(lon2 - lon1);
    float a = sin(dLat/2) * sin(dLat/2) +
              cos(radians(lat1)) * cos(radians(lat2)) *
              sin(dLon/2) * sin(dLon/2);
    float c = 2 * atan2(sqrt(a), sqrt(1-a));
    return R * c;
}

//----------------------------------------------
// Send ACK to Flight Computer
// Sends signal quality info back so FC knows we received it
//----------------------------------------------
void sendAckToFlightComputer() {
    // ACK packet format: magic, type(0x04), rssi(2), snr(1)
    uint8_t ackPacket[5];
    ackPacket[0] = LORA_MAGIC;          // 0xAF
    ackPacket[1] = 0x04;                // kLoRaPacketAck

    // RSSI as int16 (little endian)
    int16_t rssi = (int16_t)lastRssi;
    ackPacket[2] = rssi & 0xFF;
    ackPacket[3] = (rssi >> 8) & 0xFF;

    // SNR as int8
    ackPacket[4] = (int8_t)lastSnr;

    // Send ACK via LoRa
    int state = radio.transmit(ackPacket, 5);
    if (state == RADIOLIB_ERR_NONE) {
        loraTxCount++;
        // Serial.printf("ACK TX: RSSI=%d SNR=%d\n", rssi, (int8_t)lastSnr);
    }

    // Restart receiving
    radio.startReceive();
}

//----------------------------------------------
// Forward Telemetry as JSON
//----------------------------------------------
void forwardTelemetryAsJson() {
    LoRaTelemetryPacket* pkt = (LoRaTelemetryPacket*)lastLoraPacketBinary;

    // Get rocket ID (clamp to valid range)
    uint8_t rocketId = pkt->rocketId;
    if (rocketId >= MAX_ROCKETS) rocketId = 0;

    // Extract GPS coordinates
    float rocketLat = pkt->gpsLatitude / 1000000.0;
    float rocketLon = pkt->gpsLongitude / 1000000.0;
    float rocketAlt = pkt->altitudeCm / 100.0;

    // Update rocket tracking data
    rockets[rocketId].active = true;
    rockets[rocketId].lastUpdateMs = millis();
    rockets[rocketId].latitude = rocketLat;
    rockets[rocketId].longitude = rocketLon;
    rockets[rocketId].altitudeM = rocketAlt;
    rockets[rocketId].state = pkt->state;
    rockets[rocketId].satellites = pkt->gpsSatellites;
    rockets[rocketId].rssi = (int16_t)lastRssi;

    // Calculate distance if gateway has GPS fix
    float rocketDist = 0.0;
    if (gps.location.isValid() && rocketLat != 0.0 && rocketLon != 0.0) {
        rocketDist = calculateDistance(
            gps.location.lat(), gps.location.lng(),
            rocketLat, rocketLon
        );
        rockets[rocketId].distanceM = rocketDist;
    }

    // If this is the first active rocket, make it the display rocket
    if (displayRocketIndex >= MAX_ROCKETS || !rockets[displayRocketIndex].active) {
        displayRocketIndex = rocketId;
        lastDisplayCycleMs = millis();
    }

    // Build JSON telemetry
    String json = "{\"type\":\"tel\"";
    json += ",\"id\":" + String(rocketId);
    json += ",\"seq\":" + String(pkt->sequence);
    json += ",\"time\":" + String(pkt->timeMs);
    json += ",\"alt\":" + String(rocketAlt, 2);
    json += ",\"vel\":" + String(pkt->velocityCmps / 100.0, 2);
    json += ",\"pres\":" + String(pkt->pressurePa);
    json += ",\"temp\":" + String(pkt->temperatureC10 / 10.0, 1);

    // GPS
    json += ",\"lat\":" + String(rocketLat, 6);
    json += ",\"lon\":" + String(rocketLon, 6);
    json += ",\"gspd\":" + String(pkt->gpsSpeedCmps / 100.0, 1);
    json += ",\"ghdg\":" + String(pkt->gpsHeadingDeg10 / 10.0, 1);
    json += ",\"sats\":" + String(pkt->gpsSatellites);

    // IMU - Accelerometer (convert from milli-g to g)
    json += ",\"ax\":" + String(pkt->accelX / 1000.0, 3);
    json += ",\"ay\":" + String(pkt->accelY / 1000.0, 3);
    json += ",\"az\":" + String(pkt->accelZ / 1000.0, 3);

    // IMU - Gyroscope (convert from 0.1 deg/s to deg/s)
    json += ",\"gx\":" + String(pkt->gyroX / 10.0, 1);
    json += ",\"gy\":" + String(pkt->gyroY / 10.0, 1);
    json += ",\"gz\":" + String(pkt->gyroZ / 10.0, 1);

    // IMU - Magnetometer (milligauss)
    json += ",\"mx\":" + String(pkt->magX);
    json += ",\"my\":" + String(pkt->magY);
    json += ",\"mz\":" + String(pkt->magZ);

    // State and flags
    uint8_t stateIdx = pkt->state;
    if (stateIdx > 7) stateIdx = 0;
    json += ",\"state\":\"" + String(flightStateNames[stateIdx]) + "\"";
    json += ",\"flags\":" + String(pkt->flags);

    // Add RSSI/SNR
    json += ",\"rssi\":" + String(lastRssi, 1);
    json += ",\"snr\":" + String(lastSnr, 1);

    // Add distance if valid
    if (rocketDist > 0) {
        json += ",\"dist\":" + String(rocketDist, 1);
    }

    json += "}";

    // Send to all clients
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i].connected()) {
            clients[i].println(json);
        }
    }

    Serial.println("TX JSON: " + json.substring(0, 80) + "...");
}

//----------------------------------------------
// Forward Baro Comparison as JSON
//----------------------------------------------
void forwardBaroCompareAsJson() {
    // Packet: magic(1), type(1), p390(4), t390(2), p581(4), t581(2) = 14 bytes
    if (lastLoraPacketLen < 14) {
        forwardAsHex();
        return;
    }

    uint32_t p390 = (uint32_t)lastLoraPacketBinary[2] |
                    ((uint32_t)lastLoraPacketBinary[3] << 8) |
                    ((uint32_t)lastLoraPacketBinary[4] << 16) |
                    ((uint32_t)lastLoraPacketBinary[5] << 24);

    // Check for diagnostic packet (p390 == 0xFFFFFFFF)
    if (p390 == 0xFFFFFFFF) {
        // Diagnostic: bytes 6=bmp390ok, 7=bmp581ok, 8=lastError, 9=i2cAddr, 10=chipId
        String json = "{\"type\":\"baro_diag\"";
        json += ",\"bmp390\":" + String(lastLoraPacketBinary[6]);
        json += ",\"bmp581\":" + String(lastLoraPacketBinary[7]);
        json += ",\"err\":" + String(lastLoraPacketBinary[8]);
        json += ",\"addr\":\"0x" + String(lastLoraPacketBinary[9], HEX) + "\"";
        json += ",\"chipId\":\"0x" + String(lastLoraPacketBinary[10], HEX) + "\"";
        json += ",\"errMsg\":\"";
        switch (lastLoraPacketBinary[8]) {
            case 0: json += "OK"; break;
            case 1: json += "I2C read failed"; break;
            case 2: json += "Wrong chip ID"; break;
            case 3: json += "Reset failed"; break;
            case 4: json += "Configure failed"; break;
            case 5: json += "SetMode failed"; break;
            case 6: json += "INT_SOURCE write failed"; break;
            case 7: json += "INT_CONFIG write failed"; break;
            default: json += "Unknown"; break;
        }
        json += "\"";
        json += ",\"rssi\":" + String(lastRssi, 1);
        json += ",\"snr\":" + String(lastSnr, 1) + "}";
        forwardToClients(json);
        return;
    }

    int16_t t390 = (int16_t)(lastLoraPacketBinary[6] | (lastLoraPacketBinary[7] << 8));

    uint32_t p581 = (uint32_t)lastLoraPacketBinary[8] |
                    ((uint32_t)lastLoraPacketBinary[9] << 8) |
                    ((uint32_t)lastLoraPacketBinary[10] << 16) |
                    ((uint32_t)lastLoraPacketBinary[11] << 24);
    int16_t t581 = (int16_t)(lastLoraPacketBinary[12] | (lastLoraPacketBinary[13] << 8));

    float dP = (float)p581 / 10.0 - (float)p390 / 10.0;
    float dT = (float)t581 / 100.0 - (float)t390 / 100.0;

    String json = "{\"type\":\"baro\"";
    json += ",\"p390\":" + String(p390 / 10.0, 1);
    json += ",\"t390\":" + String(t390 / 100.0, 2);
    json += ",\"p581\":" + String(p581 / 10.0, 1);
    json += ",\"t581\":" + String(t581 / 100.0, 2);
    json += ",\"dP\":" + String(dP, 1);
    json += ",\"dT\":" + String(dT, 2);
    json += ",\"rssi\":" + String(lastRssi, 1);
    json += ",\"snr\":" + String(lastSnr, 1);
    json += "}";

    forwardToClients(json);
}

//----------------------------------------------
// Forward Unknown Packet as Hex
//----------------------------------------------
void forwardAsHex() {
    String hex = "{\"type\":\"raw\",\"len\":" + String(lastLoraPacketLen);
    hex += ",\"hex\":\"";
    for (int i = 0; i < lastLoraPacketLen && i < 64; i++) {
        char buf[3];
        sprintf(buf, "%02X", lastLoraPacketBinary[i]);
        hex += buf;
    }
    hex += "\",\"rssi\":" + String(lastRssi, 1);
    hex += ",\"snr\":" + String(lastSnr, 1);
    hex += "}";

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i].connected()) {
            clients[i].println(hex);
        }
    }
}

//----------------------------------------------
// Forward ACK Packet as JSON
//----------------------------------------------
void forwardAckAsJson() {
    // Format: magic, type, cmdId, success
    if (lastLoraPacketLen < 4) {
        forwardAsHex();
        return;
    }

    uint8_t cmdId = lastLoraPacketBinary[2];
    bool success = lastLoraPacketBinary[3] != 0;

    String json = "{\"type\":\"ack\"";
    json += ",\"id\":" + String(cmdId);
    json += ",\"ok\":" + String(success ? "true" : "false");
    json += "}";

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i].connected()) {
            clients[i].println(json);
        }
    }
    Serial.println("Forwarded ack");
}

//----------------------------------------------
// Forward Device Info (fc_info) as JSON
//----------------------------------------------
void forwardDeviceInfoAsJson() {
    // Format: magic, type, versionLen, version[], buildLen, build[], flags, state, sampleCount(4),
    //         rocketId, nameLen, name[], baroTypeLen, baroType[], imuTypeLen, imuType[]
    if (lastLoraPacketLen < 5) {
        forwardAsHex();
        return;
    }

    int offset = 2;

    // Version string
    uint8_t versionLen = lastLoraPacketBinary[offset++];
    String version = "";
    for (int i = 0; i < versionLen && offset < lastLoraPacketLen; i++) {
        version += (char)lastLoraPacketBinary[offset++];
    }

    // Build string
    String build = "";
    if (offset < lastLoraPacketLen) {
        uint8_t buildLen = lastLoraPacketBinary[offset++];
        for (int i = 0; i < buildLen && offset < lastLoraPacketLen; i++) {
            build += (char)lastLoraPacketBinary[offset++];
        }
    }

    // Hardware flags
    uint8_t flags = 0;
    if (offset < lastLoraPacketLen) {
        flags = lastLoraPacketBinary[offset++];
    }

    // Flight state
    uint8_t state = 0;
    if (offset < lastLoraPacketLen) {
        state = lastLoraPacketBinary[offset++];
    }

    // Sample count
    uint32_t samples = 0;
    if (offset + 3 < lastLoraPacketLen) {
        samples = lastLoraPacketBinary[offset] |
                  (lastLoraPacketBinary[offset+1] << 8) |
                  (lastLoraPacketBinary[offset+2] << 16) |
                  (lastLoraPacketBinary[offset+3] << 24);
        offset += 4;
    }

    // Rocket ID (added in v2)
    uint8_t rocketId = 0;
    if (offset < lastLoraPacketLen) {
        rocketId = lastLoraPacketBinary[offset++];
    }

    // Rocket name (added in v2)
    String rocketName = "";
    if (offset < lastLoraPacketLen) {
        uint8_t nameLen = lastLoraPacketBinary[offset++];
        for (int i = 0; i < nameLen && offset < lastLoraPacketLen; i++) {
            rocketName += (char)lastLoraPacketBinary[offset++];
        }
    }

    // Barometer type string (added in v3 - Heltec flight computer)
    String baroType = "";
    if (offset < lastLoraPacketLen) {
        uint8_t baroTypeLen = lastLoraPacketBinary[offset++];
        for (int i = 0; i < baroTypeLen && offset < lastLoraPacketLen; i++) {
            baroType += (char)lastLoraPacketBinary[offset++];
        }
    }

    // IMU type string (added in v3 - Heltec flight computer)
    String imuType = "";
    if (offset < lastLoraPacketLen) {
        uint8_t imuTypeLen = lastLoraPacketBinary[offset++];
        for (int i = 0; i < imuTypeLen && offset < lastLoraPacketLen; i++) {
            imuType += (char)lastLoraPacketBinary[offset++];
        }
    }

    String json = "{\"type\":\"fc_info\"";
    json += ",\"version\":\"" + version + "\"";
    json += ",\"build\":\"" + build + "\"";
    json += ",\"bmp390\":" + String((flags & 0x01) ? "true" : "false");
    json += ",\"lora\":" + String((flags & 0x02) ? "true" : "false");
    json += ",\"imu\":" + String((flags & 0x04) ? "true" : "false");
    json += ",\"oled\":" + String((flags & 0x10) ? "true" : "false");
    json += ",\"gps\":" + String((flags & 0x20) ? "true" : "false");
    json += ",\"state\":\"" + String(flightStateNames[state > 7 ? 0 : state]) + "\"";
    json += ",\"samples\":" + String(samples);
    json += ",\"rocket_id\":" + String(rocketId);
    json += ",\"rocket_name\":\"" + rocketName + "\"";

    // Include sensor type strings if present (from Heltec flight computer)
    if (baroType.length() > 0) {
        json += ",\"baro_type\":\"" + baroType + "\"";
    }
    if (imuType.length() > 0) {
        json += ",\"imu_type\":\"" + imuType + "\"";
    }

    json += "}";

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i].connected()) {
            clients[i].println(json);
        }
    }
    Serial.println("Forwarded fc_info");
}

//----------------------------------------------
// Forward Flash List as JSON
//----------------------------------------------
void forwardFlashListAsJson() {
    // Format: magic, type, count, then for each: slot, flightId(4), altCm(4), timeMs(4), sampleCount(4)
    if (lastLoraPacketLen < 3) {
        forwardAsHex();
        return;
    }

    uint8_t flightCount = lastLoraPacketBinary[2];
    int offset = 3;

    String json = "{\"type\":\"flash_list\"";
    json += ",\"count\":" + String(flightCount);
    json += ",\"flights\":[";

    bool first = true;
    while (offset + 17 <= lastLoraPacketLen) {  // Each entry is 17 bytes
        uint8_t slot = lastLoraPacketBinary[offset++];

        uint32_t flightId = lastLoraPacketBinary[offset] |
                            (lastLoraPacketBinary[offset+1] << 8) |
                            (lastLoraPacketBinary[offset+2] << 16) |
                            (lastLoraPacketBinary[offset+3] << 24);
        offset += 4;

        int32_t altCm = lastLoraPacketBinary[offset] |
                        (lastLoraPacketBinary[offset+1] << 8) |
                        (lastLoraPacketBinary[offset+2] << 16) |
                        (lastLoraPacketBinary[offset+3] << 24);
        offset += 4;

        uint32_t timeMs = lastLoraPacketBinary[offset] |
                          (lastLoraPacketBinary[offset+1] << 8) |
                          (lastLoraPacketBinary[offset+2] << 16) |
                          (lastLoraPacketBinary[offset+3] << 24);
        offset += 4;

        uint32_t samples = lastLoraPacketBinary[offset] |
                           (lastLoraPacketBinary[offset+1] << 8) |
                           (lastLoraPacketBinary[offset+2] << 16) |
                           (lastLoraPacketBinary[offset+3] << 24);
        offset += 4;

        if (!first) json += ",";
        first = false;

        json += "{\"slot\":" + String(slot);
        json += ",\"id\":" + String(flightId);
        json += ",\"alt\":" + String(altCm / 100.0, 1);
        json += ",\"time\":" + String(timeMs);
        json += ",\"samples\":" + String(samples);
        json += "}";
    }

    json += "]}";

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i].connected()) {
            clients[i].println(json);
        }
    }
    Serial.println("Forwarded flash_list");
}

//----------------------------------------------
// Forward Flash Data/Header as JSON
//----------------------------------------------
void forwardFlashDataAsJson() {
    // Format: magic, type, slot, startSample(4), totalSamples(4), count, then sample data
    // If startSample == 0xFFFFFFFF, it's a header packet
    if (lastLoraPacketLen < 12) {
        forwardAsHex();
        return;
    }

    uint8_t slot = lastLoraPacketBinary[2];

    uint32_t startSample = lastLoraPacketBinary[3] |
                           (lastLoraPacketBinary[4] << 8) |
                           (lastLoraPacketBinary[5] << 16) |
                           (lastLoraPacketBinary[6] << 24);

    // Check if this is a header packet
    if (startSample == 0xFFFFFFFF) {
        // Header packet - forward the raw hex data for desktop to decode
        String json = "{\"type\":\"flash_header\"";
        json += ",\"slot\":" + String(slot);
        json += ",\"data\":\"";
        for (int i = 7; i < lastLoraPacketLen && i < 64; i++) {
            char buf[3];
            sprintf(buf, "%02X", lastLoraPacketBinary[i]);
            json += buf;
        }
        json += "\"}";

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] && clients[i].connected()) {
                clients[i].println(json);
            }
        }
        Serial.println("Forwarded flash_header");
        return;
    }

    // Data packet
    uint32_t totalSamples = lastLoraPacketBinary[7] |
                            (lastLoraPacketBinary[8] << 8) |
                            (lastLoraPacketBinary[9] << 16) |
                            (lastLoraPacketBinary[10] << 24);

    uint8_t sampleCount = lastLoraPacketBinary[11];

    // Forward sample data as hex (desktop will decode the FlightSample structs)
    String json = "{\"type\":\"flash_data\"";
    json += ",\"slot\":" + String(slot);
    json += ",\"start\":" + String(startSample);
    json += ",\"total\":" + String(totalSamples);
    json += ",\"count\":" + String(sampleCount);
    json += ",\"data\":\"";

    // Hex encode the sample data starting at offset 12
    for (int i = 12; i < lastLoraPacketLen; i++) {
        char buf[3];
        sprintf(buf, "%02X", lastLoraPacketBinary[i]);
        json += buf;
    }
    json += "\"}";

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i].connected()) {
            clients[i].println(json);
        }
    }
    Serial.println("Forwarded flash_data");
}

//----------------------------------------------
// Forward String to WiFi Clients
//----------------------------------------------
void forwardToClients(const String& data) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i].connected()) {
            clients[i].println(data);
        }
    }
}

//----------------------------------------------
// Handle WiFi Clients
//----------------------------------------------
void handleWiFiClients() {
    // Check for new connections
    WiFiClient newClient = server.available();
    if (newClient) {
        // Find empty slot
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!clients[i] || !clients[i].connected()) {
                clients[i] = newClient;
                Serial.printf("WiFi: Client %d connected from %s\n",
                              i, newClient.remoteIP().toString().c_str());

                // Send welcome message
                clients[i].println("{\"type\":\"link\",\"status\":\"wifi_connected\"}");
                break;
            }
        }
    }

    // Count connected clients and handle incoming data
    clientCount = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i].connected()) {
            clientCount++;

            // Check for incoming data from client
            while (clients[i].available()) {
                String line = clients[i].readStringUntil('\n');
                line.trim();
                if (line.length() > 0) {
                    handleClientCommand(i, line);
                }
            }
        }
    }
}

//----------------------------------------------
// Handle Command from WiFi Client
// Parses JSON commands and routes appropriately
//----------------------------------------------
void handleClientCommand(int clientIdx, const String& command) {
    Serial.printf("WiFi RX [%d]: %s\n", clientIdx, command.c_str());

    // Extract command type from JSON
    // Handle both "cmd":"value" and "cmd": "value" (with space)
    String cmd = "";
    int cmdStart = command.indexOf("\"cmd\":");
    if (cmdStart >= 0) {
        cmdStart += 6;  // Skip past "cmd":
        // Skip whitespace and opening quote
        while (cmdStart < command.length()) {
            char c = command.charAt(cmdStart);
            if (c == '"') {
                cmdStart++;
                break;
            } else if (c == ' ' || c == '\t') {
                cmdStart++;
            } else {
                break;
            }
        }
        int cmdEnd = command.indexOf("\"", cmdStart);
        if (cmdEnd > cmdStart) {
            cmd = command.substring(cmdStart, cmdEnd);
        }
    }

    Serial.printf("Parsed cmd: %s\n", cmd.c_str());

    //------------------------------------------
    // Gateway-local commands
    //------------------------------------------
    if (cmd == "gw_info") {
        sendGatewayInfo(clientIdx);
        return;
    }

    if (cmd == "status") {
        sendGatewayStatus(clientIdx);
        return;
    }

    if (cmd == "ping") {
        sendPingResponse(clientIdx);
        return;
    }

    if (cmd == "gw_get_settings") {
        sendGatewaySettings(clientIdx);
        return;
    }

    if (cmd == "gw_set") {
        applyGatewaySettings(clientIdx, command);
        return;
    }

    if (cmd == "rockets") {
        sendRocketList(clientIdx);
        return;
    }

    if (cmd == "wifi_status") {
        sendWifiStatus(clientIdx);
        return;
    }

    if (cmd == "gps_debug") {
        // Capture and send raw NMEA sentences for debugging
        sendGpsDebug(clientIdx);
        return;
    }

    // Send raw command to GPS and capture response
    // Format: {"cmd":"gps_cmd","data":"$CFGANT,1*04"}
    if (cmd == "gps_cmd") {
        int dataStart = command.indexOf("\"data\":\"");
        if (dataStart > 0) {
            dataStart += 8;
            int dataEnd = command.indexOf("\"", dataStart);
            if (dataEnd > dataStart) {
                String gpsCmd = command.substring(dataStart, dataEnd);
                Serial.printf("Sending GPS command: %s\n", gpsCmd.c_str());
                gpsSerial.println(gpsCmd);

                // Capture response for 1 second
                delay(100);
                String response = "{\"type\":\"gps_cmd_response\",\"sent\":\"" + gpsCmd + "\",\"response\":[";
                unsigned long start = millis();
                int count = 0;
                char nmea[128];
                int idx = 0;

                while (millis() - start < 1000) {
                    while (gpsSerial.available()) {
                        char c = gpsSerial.read();
                        gps.encode(c);

                        if (c == '$') {
                            idx = 0;
                        }
                        if (idx < 127) {
                            nmea[idx++] = c;
                        }
                        if (c == '\n' && idx > 1) {
                            nmea[idx-1] = '\0';  // Remove newline
                            if (nmea[0] == '$') {
                                if (count > 0) response += ",";
                                response += "\"";
                                response += nmea;
                                response += "\"";
                                count++;
                            }
                            idx = 0;
                        }
                    }
                    delay(1);
                }
                response += "]}";
                clients[clientIdx].println(response);
                return;
            }
        }
        clients[clientIdx].println("{\"type\":\"error\",\"msg\":\"Invalid gps_cmd format\"}");
        return;
    }

    if (cmd == "wifi_list") {
        sendWifiList(clientIdx);
        return;
    }

    if (cmd == "wifi_add") {
        handleWifiAdd(clientIdx, command);
        return;
    }

    if (cmd == "wifi_remove") {
        handleWifiRemove(clientIdx, command);
        return;
    }

    if (cmd == "wifi_save") {
        handleWifiSave(clientIdx);
        return;
    }

    if (cmd == "wifi_connect") {
        handleWifiConnect(clientIdx);
        return;
    }

    if (cmd == "wifi_scan") {
        handleWifiScan(clientIdx);
        return;
    }

    //------------------------------------------
    // Flight computer commands - convert to binary
    // All commands can include "rocket":N to target a specific rocket
    // Packet format: magic, type, targetRocketId, commandId, ...params
    //------------------------------------------
    int targetRocket = extractJsonInt(command, "rocket", -1);
    if (targetRocket < 0 || targetRocket >= MAX_ROCKETS) {
        Serial.printf("Command '%s' rejected: missing or invalid rocket ID\n", cmd.c_str());
        String err = "{\"type\":\"error\",\"code\":\"NO_ROCKET_ID\",\"message\":\"rocket ID required for " + cmd + "\"}";
        clients[clientIdx].println(err);
        return;
    }
    uint8_t loraPacket[32];
    uint8_t packetLen = 0;

    loraPacket[0] = LORA_MAGIC;
    loraPacket[1] = 0x03;  // kLoRaPacketCommand
    loraPacket[2] = (uint8_t)targetRocket;  // Target rocket ID

    if (cmd == "arm") {
        loraPacket[3] = 0x01;  // kCmdArm
        packetLen = 4;
    }
    else if (cmd == "disarm") {
        loraPacket[3] = 0x02;  // kCmdDisarm
        packetLen = 4;
    }
    else if (cmd == "reset") {
        loraPacket[3] = 0x04;  // kCmdReset
        packetLen = 4;
    }
    else if (cmd == "fc_info" || cmd == "info") {
        loraPacket[3] = 0x07;  // kCmdInfo
        packetLen = 4;
    }
    else if (cmd == "orientation_mode") {
        loraPacket[3] = 0x08;  // kCmdOrientationMode
        // Extract enabled parameter
        bool enabled = command.indexOf("\"enabled\":true") >= 0;
        loraPacket[4] = enabled ? 1 : 0;
        packetLen = 5;
    }
    else if (cmd == "set_rocket_name") {
        loraPacket[3] = 0x09;  // kCmdSetRocketName
        // Extract name parameter from JSON: {"cmd":"set_rocket_name","rocket":0,"name":"My Rocket"}
        String name = extractJsonString(command, "name");
        uint8_t nameLen = min((int)name.length(), 15);  // Max 15 chars + null
        memcpy(&loraPacket[4], name.c_str(), nameLen);
        loraPacket[4 + nameLen] = '\0';  // Null terminate
        packetLen = 5 + nameLen;  // magic + type + target + cmd + name + null
    }
    else if (cmd == "baro_compare") {
        loraPacket[3] = 0x0A;  // kCmdBaroCompare (toggle on/off)
        packetLen = 4;
    }
    else if (cmd == "flash_list") {
        loraPacket[3] = 0x20;  // kCmdFlashList
        packetLen = 4;
    }
    else if (cmd == "flash_read") {
        loraPacket[3] = 0x21;  // kCmdFlashRead
        // Extract slot and sample/header parameters
        int slot = extractJsonInt(command, "slot", 0);
        bool isHeader = command.indexOf("\"header\":true") >= 0;
        int sample = isHeader ? 0xFFFFFFFF : extractJsonInt(command, "sample", 0);
        loraPacket[4] = (uint8_t)slot;
        loraPacket[5] = sample & 0xFF;
        loraPacket[6] = (sample >> 8) & 0xFF;
        loraPacket[7] = (sample >> 16) & 0xFF;
        loraPacket[8] = (sample >> 24) & 0xFF;
        packetLen = 9;
    }
    else if (cmd == "flash_delete") {
        loraPacket[3] = 0x22;  // kCmdFlashDelete
        int slot = extractJsonInt(command, "slot", 255);
        loraPacket[4] = (uint8_t)slot;
        packetLen = 5;
    }
    else if (cmd == "download") {
        loraPacket[3] = 0x05;  // kCmdDownload
        packetLen = 4;
    }
    else if (cmd == "sd_list") {
        loraPacket[3] = 0x10;  // kCmdSdList
        packetLen = 4;
    }
    else if (cmd == "sd_read") {
        loraPacket[3] = 0x11;  // kCmdSdRead
        // Extract filename and offset
        String filename = extractJsonString(command, "file");
        int offset = extractJsonInt(command, "offset", 0);
        // Filename length + filename + offset (4 bytes)
        uint8_t fnLen = filename.length();
        if (fnLen > 32) fnLen = 32;  // Limit filename length
        loraPacket[4] = fnLen;
        for (int i = 0; i < fnLen; i++) {
            loraPacket[5 + i] = filename.charAt(i);
        }
        int offIdx = 5 + fnLen;
        loraPacket[offIdx] = offset & 0xFF;
        loraPacket[offIdx + 1] = (offset >> 8) & 0xFF;
        loraPacket[offIdx + 2] = (offset >> 16) & 0xFF;
        loraPacket[offIdx + 3] = (offset >> 24) & 0xFF;
        packetLen = offIdx + 4;
    }
    else if (cmd == "sd_delete") {
        loraPacket[3] = 0x12;  // kCmdSdDelete
        // Extract filename
        String filename = extractJsonString(command, "file");
        uint8_t fnLen = filename.length();
        if (fnLen > 32) fnLen = 32;
        loraPacket[4] = fnLen;
        for (int i = 0; i < fnLen; i++) {
            loraPacket[5 + i] = filename.charAt(i);
        }
        packetLen = 5 + fnLen;
    }
    else {
        Serial.printf("Unknown command: %s\n", cmd.c_str());
        // Send error response
        String err = "{\"type\":\"error\",\"code\":\"UNKNOWN_CMD\",\"message\":\"Unknown command: " + cmd + "\"}";
        clients[clientIdx].println(err);
        return;
    }

    // Send binary command via LoRa (retry up to 3 times for reliability)
    // Half-duplex LoRa can miss commands if flight computer is transmitting
    if (packetLen > 0) {
        bool sent = false;
        for (int attempt = 0; attempt < 3; attempt++) {
            int state = radio.transmit(loraPacket, packetLen);
            if (state == RADIOLIB_ERR_NONE) {
                loraTxCount++;
                sent = true;
                if (attempt == 0) {
                    Serial.printf("LoRa TX: cmd=%s len=%d OK\n", cmd.c_str(), packetLen);
                } else {
                    Serial.printf("LoRa TX: cmd=%s len=%d OK (attempt %d)\n", cmd.c_str(), packetLen, attempt + 1);
                }
            } else {
                Serial.printf("LoRa TX error: %d (attempt %d)\n", state, attempt + 1);
            }
            if (attempt < 2) {
                delay(50);  // Brief gap between retries
            }
        }
        if (!sent) {
            String err = "{\"type\":\"error\",\"code\":\"LORA_TX_FAIL\",\"message\":\"LoRa transmit failed after 3 attempts\"}";
            clients[clientIdx].println(err);
        }

        // Restart receiving after transmit
        radio.startReceive();
    }
}

//----------------------------------------------
// Extract integer from JSON string
//----------------------------------------------
int extractJsonInt(const String& json, const char* key, int defaultVal) {
    String searchKey = String("\"") + key + "\":";
    int keyStart = json.indexOf(searchKey);
    if (keyStart < 0) return defaultVal;

    keyStart += searchKey.length();
    // Skip whitespace
    while (keyStart < json.length() && json.charAt(keyStart) == ' ') keyStart++;

    // Read number
    String numStr = "";
    while (keyStart < json.length()) {
        char c = json.charAt(keyStart);
        if (c >= '0' && c <= '9') {
            numStr += c;
            keyStart++;
        } else if (c == '-' && numStr.length() == 0) {
            numStr += c;
            keyStart++;
        } else {
            break;
        }
    }

    if (numStr.length() == 0) return defaultVal;
    return numStr.toInt();
}

//----------------------------------------------
// Extract string from JSON
//----------------------------------------------
String extractJsonString(const String& json, const char* key) {
    String searchKey = String("\"") + key + "\":";
    int keyStart = json.indexOf(searchKey);
    if (keyStart < 0) return "";

    keyStart += searchKey.length();
    // Skip whitespace
    while (keyStart < json.length() && json.charAt(keyStart) == ' ') keyStart++;

    // Check for opening quote
    if (keyStart >= json.length() || json.charAt(keyStart) != '"') return "";
    keyStart++;  // Skip opening quote

    // Find closing quote
    int keyEnd = json.indexOf("\"", keyStart);
    if (keyEnd < 0) return "";

    return json.substring(keyStart, keyEnd);
}

//----------------------------------------------
// Load WiFi Networks from Flash
//----------------------------------------------
void loadWifiNetworks() {
    wifiPrefs.begin("wifi", true);  // Read-only

    storedNetworkCount = wifiPrefs.getUChar("count", 0);
    if (storedNetworkCount > MAX_WIFI_NETWORKS) {
        storedNetworkCount = MAX_WIFI_NETWORKS;
    }

    Serial.printf("Loading %d stored WiFi networks\n", storedNetworkCount);

    for (int i = 0; i < storedNetworkCount; i++) {
        String keyPrefix = "n" + String(i);
        String ssid = wifiPrefs.getString((keyPrefix + "s").c_str(), "");
        String pass = wifiPrefs.getString((keyPrefix + "p").c_str(), "");
        uint8_t priority = wifiPrefs.getUChar((keyPrefix + "r").c_str(), 100);
        bool enabled = wifiPrefs.getBool((keyPrefix + "e").c_str(), true);

        strncpy(storedNetworks[i].ssid, ssid.c_str(), WIFI_SSID_MAX_LEN);
        storedNetworks[i].ssid[WIFI_SSID_MAX_LEN] = '\0';
        strncpy(storedNetworks[i].password, pass.c_str(), WIFI_PASS_MAX_LEN);
        storedNetworks[i].password[WIFI_PASS_MAX_LEN] = '\0';
        storedNetworks[i].priority = priority;
        storedNetworks[i].enabled = enabled;

        Serial.printf("  [%d] SSID: %s, Priority: %d, Enabled: %s\n",
                      i, storedNetworks[i].ssid, priority, enabled ? "yes" : "no");
    }

    wifiPrefs.end();

    // If no stored networks, add the default
    if (storedNetworkCount == 0 && strlen(WIFI_DEFAULT_SSID) > 0) {
        Serial.println("No stored networks - adding default");
        strncpy(storedNetworks[0].ssid, WIFI_DEFAULT_SSID, WIFI_SSID_MAX_LEN);
        strncpy(storedNetworks[0].password, WIFI_DEFAULT_PASSWORD, WIFI_PASS_MAX_LEN);
        storedNetworks[0].priority = 0;
        storedNetworks[0].enabled = true;
        storedNetworkCount = 1;
    }
}

//----------------------------------------------
// Save WiFi Networks to Flash
//----------------------------------------------
void saveWifiNetworks() {
    wifiPrefs.begin("wifi", false);  // Read-write

    wifiPrefs.putUChar("count", storedNetworkCount);

    for (int i = 0; i < storedNetworkCount; i++) {
        String keyPrefix = "n" + String(i);
        wifiPrefs.putString((keyPrefix + "s").c_str(), storedNetworks[i].ssid);
        wifiPrefs.putString((keyPrefix + "p").c_str(), storedNetworks[i].password);
        wifiPrefs.putUChar((keyPrefix + "r").c_str(), storedNetworks[i].priority);
        wifiPrefs.putBool((keyPrefix + "e").c_str(), storedNetworks[i].enabled);
    }

    // Clear any extra slots
    for (int i = storedNetworkCount; i < MAX_WIFI_NETWORKS; i++) {
        String keyPrefix = "n" + String(i);
        wifiPrefs.remove((keyPrefix + "s").c_str());
        wifiPrefs.remove((keyPrefix + "p").c_str());
        wifiPrefs.remove((keyPrefix + "r").c_str());
        wifiPrefs.remove((keyPrefix + "e").c_str());
    }

    wifiPrefs.end();
    Serial.printf("Saved %d WiFi networks to flash\n", storedNetworkCount);
}

//----------------------------------------------
// Sort Networks by Priority (bubble sort)
//----------------------------------------------
void sortNetworksByPriority() {
    for (int i = 0; i < storedNetworkCount - 1; i++) {
        for (int j = 0; j < storedNetworkCount - i - 1; j++) {
            if (storedNetworks[j].priority > storedNetworks[j + 1].priority) {
                WiFiNetwork temp = storedNetworks[j];
                storedNetworks[j] = storedNetworks[j + 1];
                storedNetworks[j + 1] = temp;
            }
        }
    }
}

//----------------------------------------------
// Send WiFi Status Response
//----------------------------------------------
void sendWifiStatus(int clientIdx) {
    String response = "{\"type\":\"wifi_status\"";
    response += ",\"mode\":\"" + String(wifiIsStationMode ? "station" : "ap") + "\"";

    if (wifiIsStationMode) {
        response += ",\"ssid\":\"" + String(WiFi.SSID()) + "\"";
        response += ",\"ip\":\"" + wifiCurrentIP + "\"";
        response += ",\"rssi\":" + String(WiFi.RSSI());
        response += ",\"connected\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false");
    } else {
        response += ",\"ssid\":\"" + String(WIFI_AP_SSID) + "\"";
        response += ",\"ip\":\"" + wifiCurrentIP + "\"";
        response += ",\"rssi\":0";
        response += ",\"connected\":" + String(clientCount > 0 ? "true" : "false");
    }

    response += "}";
    clients[clientIdx].println(response);
    Serial.println("Sent wifi_status");
}

//----------------------------------------------
// Send WiFi Network List Response
//----------------------------------------------
void sendWifiList(int clientIdx) {
    String response = "{\"type\":\"wifi_list\"";
    response += ",\"count\":" + String(storedNetworkCount);
    response += ",\"networks\":[";

    for (int i = 0; i < storedNetworkCount; i++) {
        if (i > 0) response += ",";
        response += "{\"ssid\":\"" + String(storedNetworks[i].ssid) + "\"";
        response += ",\"priority\":" + String(storedNetworks[i].priority);
        response += ",\"enabled\":" + String(storedNetworks[i].enabled ? "true" : "false");
        response += "}";
    }

    response += "]}";
    clients[clientIdx].println(response);
    Serial.println("Sent wifi_list");
}

//----------------------------------------------
// Handle WiFi Add Network Command
//----------------------------------------------
void handleWifiAdd(int clientIdx, const String& command) {
    if (storedNetworkCount >= MAX_WIFI_NETWORKS) {
        clients[clientIdx].println("{\"type\":\"ack\",\"ok\":false,\"error\":\"Max networks reached\"}");
        return;
    }

    String ssid = extractJsonString(command, "ssid");
    String password = extractJsonString(command, "password");
    int priority = extractJsonInt(command, "priority", 100);

    if (ssid.length() == 0) {
        clients[clientIdx].println("{\"type\":\"ack\",\"ok\":false,\"error\":\"SSID required\"}");
        return;
    }

    // Add the network
    int idx = storedNetworkCount;
    strncpy(storedNetworks[idx].ssid, ssid.c_str(), WIFI_SSID_MAX_LEN);
    storedNetworks[idx].ssid[WIFI_SSID_MAX_LEN] = '\0';
    strncpy(storedNetworks[idx].password, password.c_str(), WIFI_PASS_MAX_LEN);
    storedNetworks[idx].password[WIFI_PASS_MAX_LEN] = '\0';
    storedNetworks[idx].priority = (uint8_t)priority;
    storedNetworks[idx].enabled = true;
    storedNetworkCount++;

    // Sort by priority
    sortNetworksByPriority();

    Serial.printf("Added WiFi network: %s (priority %d)\n", ssid.c_str(), priority);
    clients[clientIdx].println("{\"type\":\"ack\",\"ok\":true}");
}

//----------------------------------------------
// Handle WiFi Remove Network Command
//----------------------------------------------
void handleWifiRemove(int clientIdx, const String& command) {
    int index = extractJsonInt(command, "index", -1);

    if (index < 0 || index >= storedNetworkCount) {
        clients[clientIdx].println("{\"type\":\"ack\",\"ok\":false,\"error\":\"Invalid index\"}");
        return;
    }

    Serial.printf("Removing WiFi network: %s\n", storedNetworks[index].ssid);

    // Shift remaining networks down
    for (int i = index; i < storedNetworkCount - 1; i++) {
        storedNetworks[i] = storedNetworks[i + 1];
    }
    storedNetworkCount--;

    clients[clientIdx].println("{\"type\":\"ack\",\"ok\":true}");
}

//----------------------------------------------
// Handle WiFi Save Command
//----------------------------------------------
void handleWifiSave(int clientIdx) {
    saveWifiNetworks();
    clients[clientIdx].println("{\"type\":\"ack\",\"ok\":true,\"message\":\"Saved to flash\"}");
}

//----------------------------------------------
// Handle WiFi Scan Command
//----------------------------------------------
void handleWifiScan(int clientIdx) {
    Serial.println("Scanning WiFi networks...");

    // Send immediate ack that scan is starting
    clients[clientIdx].println("{\"type\":\"wifi_scan_start\"}");

    // Perform scan (this blocks for a few seconds)
    int numNetworks = WiFi.scanNetworks(false, false, false, 300);  // active scan, 300ms per channel

    // Deduplicate by SSID, keeping strongest signal
    // Use simple array to track unique SSIDs
    String uniqueSsids[20];
    int uniqueRssi[20];
    String uniqueSecurity[20];
    int uniqueChannel[20];
    int uniqueCount = 0;

    for (int i = 0; i < numNetworks && uniqueCount < 20; i++) {
        String ssid = WiFi.SSID(i);
        if (ssid.length() == 0) continue;  // Skip hidden networks

        int rssi = WiFi.RSSI(i);

        // Check if we already have this SSID
        bool found = false;
        for (int j = 0; j < uniqueCount; j++) {
            if (uniqueSsids[j] == ssid) {
                // Keep the stronger signal
                if (rssi > uniqueRssi[j]) {
                    uniqueRssi[j] = rssi;
                }
                found = true;
                break;
            }
        }

        if (!found) {
            uniqueSsids[uniqueCount] = ssid;
            uniqueRssi[uniqueCount] = rssi;
            uniqueChannel[uniqueCount] = WiFi.channel(i);

            // Encryption type
            switch (WiFi.encryptionType(i)) {
                case WIFI_AUTH_OPEN:            uniqueSecurity[uniqueCount] = "open"; break;
                case WIFI_AUTH_WEP:             uniqueSecurity[uniqueCount] = "WEP"; break;
                case WIFI_AUTH_WPA_PSK:         uniqueSecurity[uniqueCount] = "WPA"; break;
                case WIFI_AUTH_WPA2_PSK:        uniqueSecurity[uniqueCount] = "WPA2"; break;
                case WIFI_AUTH_WPA_WPA2_PSK:    uniqueSecurity[uniqueCount] = "WPA/WPA2"; break;
                case WIFI_AUTH_WPA3_PSK:        uniqueSecurity[uniqueCount] = "WPA3"; break;
                case WIFI_AUTH_WPA2_WPA3_PSK:   uniqueSecurity[uniqueCount] = "WPA2/WPA3"; break;
                default:                        uniqueSecurity[uniqueCount] = "unknown"; break;
            }
            uniqueCount++;
        }
    }

    // Build response with deduplicated networks
    String response = "{\"type\":\"wifi_scan\"";
    response += ",\"count\":" + String(uniqueCount);
    response += ",\"networks\":[";

    for (int i = 0; i < uniqueCount; i++) {
        if (i > 0) response += ",";
        response += "{\"ssid\":\"" + uniqueSsids[i] + "\"";
        response += ",\"rssi\":" + String(uniqueRssi[i]);
        response += ",\"security\":\"" + uniqueSecurity[i] + "\"";
        response += ",\"channel\":" + String(uniqueChannel[i]);
        response += "}";
    }

    response += "]}";

    // Clean up scan results
    WiFi.scanDelete();

    clients[clientIdx].println(response);
    Serial.printf("Scan complete: %d networks found (%d unique)\n", numNetworks, uniqueCount);
}

//----------------------------------------------
// Handle WiFi Connect Command (try to connect now)
//----------------------------------------------
void handleWifiConnect(int clientIdx) {
    clients[clientIdx].println("{\"type\":\"ack\",\"ok\":true,\"message\":\"Reconnecting...\"}");

    // Disconnect and reconnect
    WiFi.disconnect();
    delay(100);

    // Try stored networks
    bool connected = false;
    for (int i = 0; i < storedNetworkCount && !connected; i++) {
        if (!storedNetworks[i].enabled) continue;

        Serial.printf("Trying WiFi: %s\n", storedNetworks[i].ssid);
        displayStatus(storedNetworks[i].ssid, COLOR_YELLOW);

        WiFi.begin(storedNetworks[i].ssid, storedNetworks[i].password);

        uint32_t startTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_STA_TIMEOUT_MS) {
            delay(100);
        }

        if (WiFi.status() == WL_CONNECTED) {
            wifiIsStationMode = true;
            wifiCurrentIP = WiFi.localIP().toString();
            Serial.printf("Connected to %s, IP: %s\n", storedNetworks[i].ssid, wifiCurrentIP.c_str());
            connected = true;
        }
    }

    if (!connected) {
        // Fall back to AP mode
        Serial.println("All networks failed, starting AP mode");
        WiFi.mode(WIFI_AP);
        WiFi.softAP(WIFI_AP_SSID, strlen(WIFI_AP_PASSWORD) >= 8 ? WIFI_AP_PASSWORD : NULL, WIFI_AP_CHANNEL);
        wifiIsStationMode = false;
        wifiCurrentIP = WiFi.softAPIP().toString();
    }

    displayNeedsFullRedraw = true;
}

//----------------------------------------------
// Send Gateway Status Response
//----------------------------------------------
void sendGatewayStatus(int clientIdx) {
    String response = "{\"type\":\"status\"";
    response += ",\"connected\":" + String(loraPacketCount > 0 ? "true" : "false");
    response += ",\"rx\":" + String(loraPacketCount);
    response += ",\"tx\":" + String(loraTxCount);
    response += ",\"rssi\":" + String((int)lastRssi);
    response += ",\"snr\":" + String((int)lastSnr);
    response += ",\"clients\":" + String(clientCount);
    response += ",\"uptime\":" + String(millis() / 1000);
    response += "}";

    clients[clientIdx].println(response);
    Serial.println("Sent gateway status");
}

//----------------------------------------------
// Send Ping Response
//----------------------------------------------
void sendPingResponse(int clientIdx) {
    String response = "{\"type\":\"ack\",\"id\":0,\"ok\":true}";
    clients[clientIdx].println(response);
    Serial.println("Sent ping response");
}

//----------------------------------------------
// Send Gateway Settings
//----------------------------------------------
void sendGatewaySettings(int clientIdx) {
    String response = "{\"type\":\"gw_settings\"";
    response += ",\"backlight\":" + String(currentBacklight);
    response += ",\"backlight_max\":255";
    response += ",\"lora_tx_power\":" + String(currentLoraTxPower);
    response += ",\"lora_tx_power_min\":2";
    response += ",\"lora_tx_power_max\":20";
    response += ",\"wifi_tx_power\":" + String(currentWifiTxPower);
    response += ",\"wifi_tx_power_min\":0";
    response += ",\"wifi_tx_power_max\":20";
    response += "}";

    clients[clientIdx].println(response);
    Serial.println("Sent gateway settings");
}

//----------------------------------------------
// Apply Gateway Settings
//----------------------------------------------
void applyGatewaySettings(int clientIdx, const String& command) {
    bool changed = false;
    String changes = "";

    // Check for backlight setting
    if (command.indexOf("\"backlight\"") >= 0) {
        int newBacklight = extractJsonInt(command, "backlight", -1);
        if (newBacklight >= 0 && newBacklight <= 255) {
            currentBacklight = newBacklight;
            ledcWrite(TFT_BL, currentBacklight);
            changes += "backlight=" + String(currentBacklight) + " ";
            changed = true;
        }
    }

    // Check for LoRa TX power setting
    if (command.indexOf("\"lora_tx_power\"") >= 0) {
        int newPower = extractJsonInt(command, "lora_tx_power", -1);
        if (newPower >= 2 && newPower <= 20) {
            currentLoraTxPower = newPower;
            radio.setOutputPower(currentLoraTxPower);
            changes += "lora_tx=" + String(currentLoraTxPower) + "dBm ";
            changed = true;
        }
    }

    // Check for WiFi TX power setting
    if (command.indexOf("\"wifi_tx_power\"") >= 0) {
        int newPower = extractJsonInt(command, "wifi_tx_power", -1);
        if (newPower >= 0 && newPower <= 20) {
            currentWifiTxPower = newPower;
            // WiFi power is set using predefined levels
            // Map 0-20 to actual dBm values
            wifi_power_t wifiPower;
            if (newPower <= 5) wifiPower = WIFI_POWER_5dBm;
            else if (newPower <= 7) wifiPower = WIFI_POWER_7dBm;
            else if (newPower <= 8) wifiPower = WIFI_POWER_8_5dBm;
            else if (newPower <= 11) wifiPower = WIFI_POWER_11dBm;
            else if (newPower <= 13) wifiPower = WIFI_POWER_13dBm;
            else if (newPower <= 15) wifiPower = WIFI_POWER_15dBm;
            else if (newPower <= 17) wifiPower = WIFI_POWER_17dBm;
            else if (newPower <= 18) wifiPower = WIFI_POWER_18_5dBm;
            else if (newPower <= 19) wifiPower = WIFI_POWER_19dBm;
            else wifiPower = WIFI_POWER_19_5dBm;
            WiFi.setTxPower(wifiPower);
            changes += "wifi_tx=" + String(currentWifiTxPower) + "dBm ";
            changed = true;
        }
    }

    // Send response
    String response;
    if (changed) {
        response = "{\"type\":\"ack\",\"ok\":true,\"changes\":\"" + changes + "\"}";
        Serial.printf("Applied settings: %s\n", changes.c_str());
    } else {
        response = "{\"type\":\"ack\",\"ok\":false,\"error\":\"No valid settings provided\"}";
    }
    clients[clientIdx].println(response);
}

//----------------------------------------------
// Send Rocket List Response
//----------------------------------------------
void sendRocketList(int clientIdx) {
    String response = "{\"type\":\"rockets\"";
    response += ",\"count\":" + String(activeRocketCount);
    response += ",\"rockets\":[";

    bool first = true;
    for (int i = 0; i < MAX_ROCKETS; i++) {
        if (rockets[i].active) {
            if (!first) response += ",";
            first = false;

            response += "{\"id\":" + String(i);
            response += ",\"lat\":" + String(rockets[i].latitude, 6);
            response += ",\"lon\":" + String(rockets[i].longitude, 6);
            response += ",\"alt\":" + String(rockets[i].altitudeM, 1);
            response += ",\"dist\":" + String(rockets[i].distanceM, 1);

            uint8_t st = rockets[i].state;
            if (st > 7) st = 0;
            response += ",\"state\":\"" + String(flightStateNames[st]) + "\"";
            response += ",\"sats\":" + String(rockets[i].satellites);
            response += ",\"rssi\":" + String(rockets[i].rssi);
            response += ",\"age\":" + String((millis() - rockets[i].lastUpdateMs) / 1000);
            response += "}";
        }
    }

    response += "]}";
    clients[clientIdx].println(response);
    Serial.println("Sent rocket list");
}

//----------------------------------------------
// Send GPS Debug Info
// Captures raw NMEA sentences for debugging
//----------------------------------------------
void sendGpsDebug(int clientIdx) {
    String response = "{\"type\":\"gps_debug\"";

    // TinyGPS++ statistics
    response += ",\"chars\":" + String(gps.charsProcessed());
    response += ",\"sentences\":" + String(gps.sentencesWithFix());
    response += ",\"passed\":" + String(gps.passedChecksum());
    response += ",\"failed\":" + String(gps.failedChecksum());

    // Satellite info
    response += ",\"sats\":" + String(gps.satellites.value());
    response += ",\"sats_valid\":" + String(gps.satellites.isValid() ? "true" : "false");
    response += ",\"sats_age\":" + String(gps.satellites.age());

    // Location info
    response += ",\"loc_valid\":" + String(gps.location.isValid() ? "true" : "false");
    response += ",\"loc_age\":" + String(gps.location.age());

    // HDOP
    response += ",\"hdop\":" + String(gps.hdop.value());
    response += ",\"hdop_valid\":" + String(gps.hdop.isValid() ? "true" : "false");

    // Time info
    response += ",\"time_valid\":" + String(gps.time.isValid() ? "true" : "false");
    response += ",\"date_valid\":" + String(gps.date.isValid() ? "true" : "false");

    if (gps.time.isValid()) {
        char timeStr[16];
        snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d",
                 gps.time.hour(), gps.time.minute(), gps.time.second());
        response += ",\"time\":\"" + String(timeStr) + "\"";
    }

    if (gps.date.isValid()) {
        char dateStr[16];
        snprintf(dateStr, sizeof(dateStr), "%04d-%02d-%02d",
                 gps.date.year(), gps.date.month(), gps.date.day());
        response += ",\"date\":\"" + String(dateStr) + "\"";
    }

    response += "}";
    clients[clientIdx].println(response);

    // Now capture raw NMEA for 2 seconds
    clients[clientIdx].println("{\"type\":\"nmea_capture\",\"duration_ms\":2000}");

    char nmeaBuffer[128];
    uint8_t nmeaIdx = 0;
    uint32_t startTime = millis();
    int sentenceCount = 0;

    while (millis() - startTime < 2000 && sentenceCount < 20) {
        while (gpsSerial.available()) {
            char c = gpsSerial.read();
            gps.encode(c);  // Keep TinyGPS++ updated

            if (c == '$') {
                nmeaIdx = 0;
            }
            if (nmeaIdx < sizeof(nmeaBuffer) - 1) {
                nmeaBuffer[nmeaIdx++] = c;
            }
            if (c == '\n') {
                nmeaBuffer[nmeaIdx] = '\0';
                // Remove CR/LF
                if (nmeaIdx > 0 && nmeaBuffer[nmeaIdx-1] == '\n') nmeaBuffer[nmeaIdx-1] = '\0';
                if (nmeaIdx > 1 && nmeaBuffer[nmeaIdx-2] == '\r') nmeaBuffer[nmeaIdx-2] = '\0';

                // Send the sentence
                String nmea = "{\"type\":\"nmea\",\"data\":\"" + String(nmeaBuffer) + "\"}";
                clients[clientIdx].println(nmea);
                sentenceCount++;
                nmeaIdx = 0;
            }
        }
        delay(1);
    }

    clients[clientIdx].println("{\"type\":\"nmea_capture_end\",\"count\":" + String(sentenceCount) + "}");
    Serial.printf("GPS debug sent: %d NMEA sentences\n", sentenceCount);
}

//----------------------------------------------
// Send Gateway Info Response
// Field names must match what desktop app expects
//----------------------------------------------
void sendGatewayInfo(int clientIdx) {
    String response = "{\"type\":\"gw_info\"";

    // Version and build info
    response += ",\"version\":\"" + String(FIRMWARE_VERSION_STRING) + "\"";
    response += ",\"build\":\"" + String(__DATE__) + " " + String(__TIME__) + "\"";
    response += ",\"hostname\":\"RocketGateway.local\"";

    // Hardware status booleans
    response += ",\"lora\":true";           // LoRa is working if we got here
    response += ",\"baro\":" + String(baroOk ? "true" : "false");
    if (baroOk) {
        response += ",\"baro_type\":\"" + String(baroBmp581 ? "BMP581" : "BMP390") + "\"";
    } else {
        response += ",\"baro_type\":\"None\"";
    }
    response += ",\"ota\":true";            // OTA updates supported
    // GPS present if we've received any data from it
    response += ",\"gps\":" + String(gps.charsProcessed() > 0 ? "true" : "false");
    response += ",\"display\":true";        // TFT display is working

    // Connection status
    response += ",\"connected\":" + String(loraPacketCount > 0 ? "true" : "false");
    response += ",\"rx\":" + String(loraPacketCount);
    response += ",\"tx\":" + String(loraTxCount);
    response += ",\"rssi\":" + String((int)lastRssi);
    response += ",\"snr\":" + String((int)lastSnr);

    // Ground reference from barometer
    response += ",\"ground_pres\":" + String((int)groundPressurePa);
    response += ",\"ground_temp\":" + String(groundTemperatureC, 1);

    // GPS data
    response += ",\"gps_fix\":" + String(gps.location.isValid() ? "true" : "false");
    if (gps.location.isValid()) {
        response += ",\"gps_lat\":" + String(gps.location.lat(), 6);
        response += ",\"gps_lon\":" + String(gps.location.lng(), 6);
    } else {
        response += ",\"gps_lat\":0";
        response += ",\"gps_lon\":0";
    }
    response += ",\"gps_sats\":" + String(gps.satellites.value());
    response += ",\"gps_chars\":" + String(gps.charsProcessed());
    response += ",\"gps_passed\":" + String(gps.passedChecksum());
    response += ",\"gps_failed\":" + String(gps.failedChecksum());

    response += "}";

    clients[clientIdx].println(response);
    Serial.println("Sent gateway info");
}

//----------------------------------------------
// Read GPS Data
//----------------------------------------------
void readGPS() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }
}

//----------------------------------------------
// Update Display (Main runtime display with partial updates)
//----------------------------------------------
void updateDisplay() {
    // Full redraw only on first call or when forced
    if (displayNeedsFullRedraw) {
        tft.fillScreen(COLOR_BLACK);

        // Row 0: Header with WiFi mode (static content)
        tft.setTextSize(1);
        tft.setTextColor(COLOR_CYAN);
        tft.setCursor(2, 2);
        tft.print("GATEWAY");

        // WiFi mode indicator (right side)
        tft.setTextColor(wifiIsStationMode ? COLOR_GREEN : COLOR_ORANGE);
        tft.setCursor(100, 2);
        tft.print(wifiIsStationMode ? "[STA]" : "[AP]");

        // Horizontal line
        tft.drawFastHLine(0, 12, TFT_WIDTH, COLOR_DARK_GRAY);

        // Row 1: IP:port (static)
        tft.setTextColor(COLOR_WHITE);
        tft.setCursor(2, 16);
        tft.printf("%s:%d", wifiCurrentIP.c_str(), TCP_PORT);

        // Reset all prev values to force updates
        prevClientCount = -1;
        prevLoraPacketCount = 0xFFFFFFFF;
        prevLastPacket = "";
        prevGpsValid = !gps.location.isValid();
        prevGpsSats = -1;
        prevGpsHour = 255;

        displayNeedsFullRedraw = false;
    }

    // Row 1: Client count (only if changed)
    if (clientCount != prevClientCount) {
        tft.fillRect(130, 16, 30, 10, COLOR_BLACK);
        tft.setTextColor(clientCount > 0 ? COLOR_GREEN : COLOR_DARK_GRAY);
        tft.setCursor(130, 16);
        tft.printf("C:%d", clientCount);
        prevClientCount = clientCount;
    }

    // Row 2: LoRa stats (only if changed)
    if (loraPacketCount != prevLoraPacketCount) {
        tft.fillRect(2, 28, 156, 10, COLOR_BLACK);
        tft.setTextColor(COLOR_YELLOW);
        tft.setCursor(2, 28);
        tft.printf("LoRa:%lu", loraPacketCount);

        if (loraPacketCount > 0) {
            tft.setTextColor(COLOR_WHITE);
            tft.setCursor(70, 28);
            tft.printf("%.0fdB", lastRssi);
        }
        prevLoraPacketCount = loraPacketCount;
    }

    // Row 3: GPS status (only if changed)
    bool gpsValid = gps.location.isValid();
    int gpsSats = gps.satellites.value();
    double gpsLat = gps.location.lat();
    double gpsLon = gps.location.lng();
    uint32_t gpsChars = gps.charsProcessed();

    // Update if status changes, or if no fix and chars processed increases (show progress)
    bool gpsRowNeedsUpdate = (gpsValid != prevGpsValid || gpsSats != prevGpsSats ||
        (gpsValid && (gpsLat != prevGpsLat || gpsLon != prevGpsLon)) ||
        (!gpsValid && gpsChars != prevGpsChars));

    if (gpsRowNeedsUpdate) {
        tft.fillRect(2, 40, 156, 10, COLOR_BLACK);
        tft.setCursor(2, 40);
        if (gpsValid) {
            tft.setTextColor(COLOR_GREEN);
            tft.printf("%.4f,%.4f", gpsLat, gpsLon);
        } else {
            // Show more detail when no fix
            uint32_t charsProc = gps.charsProcessed();
            if (charsProc == 0) {
                tft.setTextColor(COLOR_RED);
                tft.print("GPS: no data");
            } else if (gps.satellites.age() > 5000) {
                tft.setTextColor(COLOR_YELLOW);
                tft.printf("GPS: wait... %luc", charsProc / 100);
            } else {
                tft.setTextColor(COLOR_DARK_GRAY);
                tft.printf("GPS: %d sats", gpsSats);
            }
        }
        prevGpsValid = gpsValid;
        prevGpsSats = gpsSats;
        prevGpsLat = gpsLat;
        prevGpsLon = gpsLon;
        prevGpsChars = gpsChars;
    }

    // Row 4: Rocket info (ID, distance, state)
    uint8_t currentRocketState = (displayRocketIndex < MAX_ROCKETS && rockets[displayRocketIndex].active) ? rockets[displayRocketIndex].state : 255;
    if (distanceToRocket != prevDistanceToRocket || displayRocketIndex != prevDisplayRocketId || currentRocketState != prevRocketState) {
        tft.fillRect(2, 52, 156, 10, COLOR_BLACK);
        tft.setCursor(2, 52);

        if (activeRocketCount > 0 && displayRocketIndex < MAX_ROCKETS && rockets[displayRocketIndex].active) {
            // Show rocket ID (and count if multiple rockets)
            tft.setTextColor(activeRocketCount > 1 ? COLOR_ORANGE : COLOR_CYAN);
            tft.printf("R%d", displayRocketIndex);
            if (activeRocketCount > 1) {
                tft.setTextColor(COLOR_DARK_GRAY);
                tft.printf("/%d", activeRocketCount);
            }
            tft.print(" ");

            // Show distance or GPS status
            RocketData* r = &rockets[displayRocketIndex];
            bool rocketHasGps = (r->latitude != 0.0 || r->longitude != 0.0);
            bool gatewayHasGps = gps.location.isValid();

            if (distanceToRocket > 0) {
                // Have distance - show it
                tft.setTextColor(COLOR_CYAN);
                if (distanceToRocket >= 1000) {
                    tft.printf("%.1fkm", distanceToRocket / 1000.0);
                } else {
                    tft.printf("%.0fm", distanceToRocket);
                }
            } else if (rocketHasGps && !gatewayHasGps) {
                // Rocket has GPS but gateway doesn't
                tft.setTextColor(COLOR_YELLOW);
                tft.printf("%dsat", r->satellites);
            } else if (!rocketHasGps && r->satellites > 0) {
                // Rocket acquiring GPS
                tft.setTextColor(COLOR_DARK_GRAY);
                tft.printf("%dsat", r->satellites);
            } else {
                // No GPS data
                tft.setTextColor(COLOR_DARK_GRAY);
                tft.print("noGPS");
            }

            // Show state
            uint8_t st = r->state;
            if (st <= 7) {
                tft.setTextColor(COLOR_WHITE);
                tft.printf(" %s", flightStateNames[st]);
            }
        } else if (loraPacketCount > 0) {
            tft.setTextColor(COLOR_DARK_GRAY);
            tft.print("Rocket: waiting GPS...");
        } else {
            tft.setTextColor(COLOR_DARK_GRAY);
            tft.print("Rocket: no data");
        }
        prevDistanceToRocket = distanceToRocket;
        prevDisplayRocketId = displayRocketIndex;
        prevRocketState = currentRocketState;
    }

    // Row 5: Time (only if changed - update once per second)
    if (gps.time.isValid()) {
        uint8_t h = gps.time.hour();
        uint8_t m = gps.time.minute();
        uint8_t s = gps.time.second();
        if (h != prevGpsHour || m != prevGpsMin || s != prevGpsSec) {
            tft.fillRect(2, 66, 100, 10, COLOR_BLACK);
            tft.setTextColor(COLOR_DARK_GRAY);
            tft.setCursor(2, 66);
            tft.printf("%02d:%02d:%02d UTC", h, m, s);
            prevGpsHour = h;
            prevGpsMin = m;
            prevGpsSec = s;
        }
    }
}

//----------------------------------------------
// Print Status to Serial
//----------------------------------------------
void printStatus() {
    Serial.println("\n--- Gateway Status ---");
    Serial.printf("  WiFi: %s (%s)\n", wifiCurrentIP.c_str(), wifiIsStationMode ? "STA" : "AP");
    Serial.printf("  LoRa packets: %d\n", loraPacketCount);
    Serial.printf("  WiFi clients: %d\n", clientCount);

    if (gps.location.isValid()) {
        Serial.printf("  GPS: %.6f, %.6f (%.1fm)\n",
                      gps.location.lat(),
                      gps.location.lng(),
                      gps.altitude.meters());
        Serial.printf("  Satellites: %d\n", gps.satellites.value());
    } else {
        Serial.printf("  GPS: No fix (%d sats)\n", gps.satellites.value());
    }
    Serial.println("----------------------\n");
}
