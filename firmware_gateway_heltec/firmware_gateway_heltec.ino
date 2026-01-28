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
#include <TinyGPSPlus.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
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
#define GPS_BAUD    9600

// TFT Display (ST7735 160x80)
#define TFT_MOSI    42
#define TFT_SCLK    41
#define TFT_CS      38
#define TFT_DC      40
#define TFT_RST     39
#define TFT_BL      21      // Backlight control
#define TFT_POWER   3       // VEXT power control (shared with GPS)

// Battery voltage ADC
#define BATTERY_ADC_PIN     37      // GPIO37 for battery voltage divider
#define BATTERY_SAMPLES     10      // Average multiple samples for stability

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
// Station mode (connect to existing network)
#define WIFI_STA_SSID       "AppligentGuestAirport"
#define WIFI_STA_PASSWORD   "qualitypdf"
#define WIFI_STA_TIMEOUT_MS 10000       // 10 seconds to connect before fallback

// AP mode (create hotspot)
#define WIFI_AP_SSID        "RocketGateway"
#define WIFI_AP_PASSWORD    ""          // Empty = open AP
#define WIFI_AP_CHANNEL     6

#define TCP_PORT        5000
#define MAX_CLIENTS     4

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
#define LORA_PACKET_SIZE        54

typedef struct __attribute__((packed)) {
    uint8_t magic;
    uint8_t packetType;
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
uint8_t prevGpsHour = 255;
uint8_t prevGpsMin = 255;
uint8_t prevGpsSec = 255;

// Runtime-adjustable settings
uint8_t currentBacklight = TFT_BL_BRIGHTNESS;  // 0-255
int8_t currentLoraTxPower = LORA_TX_POWER;     // 2-20 dBm for SX1262
int8_t currentWifiTxPower = 8;                 // WiFi power level (index into power table)

// Battery monitoring
float batteryVoltage = 0.0;
uint32_t lastBatteryRead = 0;
float prevBatteryVoltage = -1.0;

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
    Serial.printf("  Version %s\n", FIRMWARE_VERSION_STRING);
    Serial.printf("  Build: %s %s\n", FIRMWARE_BUILD_DATE, FIRMWARE_BUILD_TIME);
    Serial.println("========================================\n");

    // Initialize display
    initDisplay();
    displaySplash();

    // Initialize battery ADC
    pinMode(BATTERY_ADC_PIN, INPUT);
    analogSetAttenuation(ADC_11db);  // Full range 0-3.3V
    readBatteryVoltage();  // Initial reading
    Serial.printf("Battery: %.2fV (%d%%)\n", batteryVoltage, getBatteryPercent());

    // Initialize GPS (also powers VEXT for GPS)
    initGPS();

    // Initialize LoRa
    displayStatus("LoRa Init...", COLOR_YELLOW);
    if (!initLoRa()) {
        displayStatus("LoRa FAILED!", COLOR_RED);
        Serial.println("ERROR: LoRa initialization failed!");
        while (true) delay(1000);
    }
    displayStatus("LoRa OK", COLOR_GREEN);
    delay(500);

    // Initialize WiFi AP
    displayStatus("WiFi Init...", COLOR_YELLOW);
    initWiFi();
    displayStatus("WiFi OK", COLOR_GREEN);
    delay(500);

    // Start TCP server
    server.begin();

    // Ready
    Serial.println("\n========================================");
    Serial.println("Gateway Ready!");
    Serial.printf("  WiFi Mode: %s\n", wifiIsStationMode ? "Station" : "AP");
    Serial.printf("  WiFi SSID: %s\n", wifiIsStationMode ? WIFI_STA_SSID : WIFI_AP_SSID);
    Serial.printf("  IP: %s\n", wifiCurrentIP.c_str());
    Serial.printf("  TCP Port: %d\n", TCP_PORT);
    Serial.printf("  LoRa Freq: %.1f MHz\n", LORA_FREQUENCY);
    Serial.println("========================================\n");

    // Initial display update
    updateDisplay();
}

//----------------------------------------------
// Main Loop
//----------------------------------------------
void loop() {
    // Handle incoming LoRa packets
    handleLoRa();

    // Handle WiFi clients
    handleWiFiClients();

    // Read GPS
    readGPS();

    // Read battery voltage (has internal rate limiting)
    readBatteryVoltage();

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
    pinMode(TFT_POWER, OUTPUT);
    digitalWrite(TFT_POWER, HIGH);
    delay(50);

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

    // Version
    tft.setTextColor(COLOR_WHITE);
    tft.setCursor(55, 30);
    tft.printf("v%s", FIRMWARE_VERSION_STRING);

    // Build info
    tft.setTextColor(COLOR_DARK_GRAY);
    tft.setCursor(25, 50);
    tft.print(FIRMWARE_BUILD_DATE);

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
// Initialize GPS
//----------------------------------------------
void initGPS() {
    // VEXT must be HIGH for GPS (already set in initDisplay)
    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);
    Serial.println("GPS UART initialized");
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
    // Try station mode first if SSID is configured
    if (strlen(WIFI_STA_SSID) > 0) {
        Serial.println("Attempting WiFi Station mode...");
        Serial.printf("  Connecting to: %s\n", WIFI_STA_SSID);

        WiFi.mode(WIFI_STA);
        WiFi.setTxPower(WIFI_POWER_8_5dBm);  // Reduce WiFi TX power to lower heat
        WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASSWORD);

        uint32_t startTime = millis();
        while (WiFi.status() != WL_CONNECTED) {
            if (millis() - startTime > WIFI_STA_TIMEOUT_MS) {
                Serial.println("  Station mode failed - timeout");
                break;
            }
            delay(500);
            Serial.print(".");
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
            wifiIsStationMode = true;
            wifiCurrentIP = WiFi.localIP().toString();
            Serial.println("  Station mode connected!");
            Serial.printf("  IP: %s\n", wifiCurrentIP.c_str());
            return;
        }
    }

    // Fall back to AP mode
    Serial.println("Starting WiFi AP mode...");

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
// Forward Telemetry as JSON
//----------------------------------------------
void forwardTelemetryAsJson() {
    LoRaTelemetryPacket* pkt = (LoRaTelemetryPacket*)lastLoraPacketBinary;

    // Build JSON telemetry
    String json = "{\"type\":\"tel\"";
    json += ",\"seq\":" + String(pkt->sequence);
    json += ",\"time\":" + String(pkt->timeMs);
    json += ",\"alt\":" + String(pkt->altitudeCm / 100.0, 2);
    json += ",\"vel\":" + String(pkt->velocityCmps / 100.0, 2);
    json += ",\"pres\":" + String(pkt->pressurePa);
    json += ",\"temp\":" + String(pkt->temperatureC10 / 10.0, 1);

    // GPS
    json += ",\"lat\":" + String(pkt->gpsLatitude / 1000000.0, 6);
    json += ",\"lon\":" + String(pkt->gpsLongitude / 1000000.0, 6);
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
    // Format: magic, type, versionLen, version[], buildLen, build[], flags, state, sampleCount(4)
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

    //------------------------------------------
    // Flight computer commands - convert to binary
    //------------------------------------------
    uint8_t loraPacket[32];
    uint8_t packetLen = 0;

    loraPacket[0] = LORA_MAGIC;
    loraPacket[1] = 0x03;  // kLoRaPacketCommand

    if (cmd == "arm") {
        loraPacket[2] = 0x01;  // kCmdArm
        packetLen = 3;
    }
    else if (cmd == "disarm") {
        loraPacket[2] = 0x02;  // kCmdDisarm
        packetLen = 3;
    }
    else if (cmd == "reset") {
        loraPacket[2] = 0x04;  // kCmdReset
        packetLen = 3;
    }
    else if (cmd == "fc_info" || cmd == "info") {
        loraPacket[2] = 0x07;  // kCmdInfo
        packetLen = 3;
    }
    else if (cmd == "orientation_mode") {
        loraPacket[2] = 0x08;  // kCmdOrientationMode
        // Extract enabled parameter
        bool enabled = command.indexOf("\"enabled\":true") >= 0;
        loraPacket[3] = enabled ? 1 : 0;
        packetLen = 4;
    }
    else if (cmd == "flash_list") {
        loraPacket[2] = 0x20;  // kCmdFlashList
        packetLen = 3;
    }
    else if (cmd == "flash_read") {
        loraPacket[2] = 0x21;  // kCmdFlashRead
        // Extract slot and sample/header parameters
        int slot = extractJsonInt(command, "slot", 0);
        bool isHeader = command.indexOf("\"header\":true") >= 0;
        int sample = isHeader ? 0xFFFFFFFF : extractJsonInt(command, "sample", 0);
        loraPacket[3] = (uint8_t)slot;
        loraPacket[4] = sample & 0xFF;
        loraPacket[5] = (sample >> 8) & 0xFF;
        loraPacket[6] = (sample >> 16) & 0xFF;
        loraPacket[7] = (sample >> 24) & 0xFF;
        packetLen = 8;
    }
    else if (cmd == "flash_delete") {
        loraPacket[2] = 0x22;  // kCmdFlashDelete
        int slot = extractJsonInt(command, "slot", 255);
        loraPacket[3] = (uint8_t)slot;
        packetLen = 4;
    }
    else if (cmd == "download") {
        loraPacket[2] = 0x05;  // kCmdDownload
        packetLen = 3;
    }
    else if (cmd == "sd_list") {
        loraPacket[2] = 0x10;  // kCmdSdList
        packetLen = 3;
    }
    else if (cmd == "sd_read") {
        loraPacket[2] = 0x11;  // kCmdSdRead
        // Extract filename and offset
        String filename = extractJsonString(command, "file");
        int offset = extractJsonInt(command, "offset", 0);
        // Filename length + filename + offset (4 bytes)
        uint8_t fnLen = filename.length();
        if (fnLen > 32) fnLen = 32;  // Limit filename length
        loraPacket[3] = fnLen;
        for (int i = 0; i < fnLen; i++) {
            loraPacket[4 + i] = filename.charAt(i);
        }
        int offIdx = 4 + fnLen;
        loraPacket[offIdx] = offset & 0xFF;
        loraPacket[offIdx + 1] = (offset >> 8) & 0xFF;
        loraPacket[offIdx + 2] = (offset >> 16) & 0xFF;
        loraPacket[offIdx + 3] = (offset >> 24) & 0xFF;
        packetLen = offIdx + 4;
    }
    else if (cmd == "sd_delete") {
        loraPacket[2] = 0x12;  // kCmdSdDelete
        // Extract filename
        String filename = extractJsonString(command, "file");
        uint8_t fnLen = filename.length();
        if (fnLen > 32) fnLen = 32;
        loraPacket[3] = fnLen;
        for (int i = 0; i < fnLen; i++) {
            loraPacket[4 + i] = filename.charAt(i);
        }
        packetLen = 4 + fnLen;
    }
    else {
        Serial.printf("Unknown command: %s\n", cmd.c_str());
        // Send error response
        String err = "{\"type\":\"error\",\"code\":\"UNKNOWN_CMD\",\"message\":\"Unknown command: " + cmd + "\"}";
        clients[clientIdx].println(err);
        return;
    }

    // Send binary command via LoRa
    if (packetLen > 0) {
        int state = radio.transmit(loraPacket, packetLen);
        if (state == RADIOLIB_ERR_NONE) {
            loraTxCount++;
            Serial.printf("LoRa TX: cmd=%s len=%d OK\n", cmd.c_str(), packetLen);
        } else {
            Serial.printf("LoRa TX error: %d\n", state);
            String err = "{\"type\":\"error\",\"code\":\"LORA_TX_FAIL\",\"message\":\"LoRa transmit failed\"}";
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
    response += ",\"battery_v\":" + String(batteryVoltage, 2);
    response += ",\"battery_pct\":" + String(getBatteryPercent());
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
// Send Gateway Info Response
// Field names must match what desktop app expects
//----------------------------------------------
void sendGatewayInfo(int clientIdx) {
    String response = "{\"type\":\"gw_info\"";

    // Version and build info
    response += ",\"version\":\"" + String(FIRMWARE_VERSION_STRING) + "\"";
    response += ",\"build\":\"" + String(__DATE__) + " " + String(__TIME__) + "\"";

    // Hardware status booleans
    response += ",\"lora\":true";           // LoRa is working if we got here
    response += ",\"bmp390\":false";        // Heltec doesn't have BMP390
    response += ",\"gps\":" + String(gps.satellites.value() > 0 ? "true" : "false");
    response += ",\"display\":true";        // TFT display is working

    // Connection status
    response += ",\"connected\":" + String(loraPacketCount > 0 ? "true" : "false");
    response += ",\"rx\":" + String(loraPacketCount);
    response += ",\"tx\":" + String(loraTxCount);
    response += ",\"rssi\":" + String((int)lastRssi);
    response += ",\"snr\":" + String((int)lastSnr);

    // Ground reference (not applicable for Heltec gateway)
    response += ",\"ground_pres\":0";
    response += ",\"ground_temp\":0";

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

    // Battery status
    response += ",\"battery_v\":" + String(batteryVoltage, 2);
    response += ",\"battery_pct\":" + String(getBatteryPercent());

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
// Read Battery Voltage
// Heltec Wireless Tracker uses GPIO37 with voltage divider
//----------------------------------------------
void readBatteryVoltage() {
    // Only read every 2 seconds to reduce overhead
    if (millis() - lastBatteryRead < 2000 && lastBatteryRead > 0) {
        return;
    }
    lastBatteryRead = millis();

    // Average multiple samples for stability
    uint32_t sum = 0;
    for (int i = 0; i < BATTERY_SAMPLES; i++) {
        sum += analogRead(BATTERY_ADC_PIN);
        delayMicroseconds(100);
    }
    uint32_t avgReading = sum / BATTERY_SAMPLES;

    // Convert to voltage
    // ESP32-S3 ADC: 12-bit (0-4095), reference 3.3V
    // Heltec uses 2:1 voltage divider, so multiply by 2
    // Actual calibration: reading * (3.3 / 4095) * 2
    batteryVoltage = (avgReading * 3.3 * 2.0) / 4095.0;

    // Clamp to reasonable LiPo range
    if (batteryVoltage < 2.5) batteryVoltage = 0.0;  // No battery connected
    if (batteryVoltage > 4.5) batteryVoltage = 4.5;  // Clamp max
}

//----------------------------------------------
// Get Battery Percentage (approximate)
//----------------------------------------------
int getBatteryPercent() {
    // LiPo voltage to percentage (approximate)
    // 4.2V = 100%, 3.0V = 0%
    if (batteryVoltage >= 4.2) return 100;
    if (batteryVoltage <= 3.0) return 0;
    return (int)((batteryVoltage - 3.0) / 1.2 * 100);
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

        // Row 1: IP (static)
        tft.setTextColor(COLOR_WHITE);
        tft.setCursor(2, 16);
        tft.print(wifiCurrentIP);

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

    if (gpsValid != prevGpsValid || gpsSats != prevGpsSats ||
        (gpsValid && (gpsLat != prevGpsLat || gpsLon != prevGpsLon))) {
        tft.fillRect(2, 40, 156, 10, COLOR_BLACK);
        tft.setCursor(2, 40);
        if (gpsValid) {
            tft.setTextColor(COLOR_GREEN);
            tft.printf("%.4f,%.4f", gpsLat, gpsLon);
        } else {
            tft.setTextColor(COLOR_DARK_GRAY);
            tft.printf("GPS: %d sats", gpsSats);
        }
        prevGpsValid = gpsValid;
        prevGpsSats = gpsSats;
        prevGpsLat = gpsLat;
        prevGpsLon = gpsLon;
    }

    // Row 4: Last packet preview (only if changed)
    if (lastLoraPacket != prevLastPacket) {
        tft.fillRect(2, 52, 156, 10, COLOR_BLACK);
        tft.setTextColor(COLOR_LIGHT_GRAY);
        tft.setCursor(2, 52);
        if (lastLoraPacket.length() > 0) {
            String preview = lastLoraPacket.substring(0, 25);
            if (lastLoraPacket.length() > 25) preview += "..";
            tft.print(preview);
        } else {
            tft.print("Waiting for data...");
        }
        prevLastPacket = lastLoraPacket;
    }

    // Row 5: Time and Battery (only if changed)
    bool timeUpdated = false;
    if (gps.time.isValid()) {
        uint8_t h = gps.time.hour();
        uint8_t m = gps.time.minute();
        uint8_t s = gps.time.second();
        if (h != prevGpsHour || m != prevGpsMin || s != prevGpsSec) {
            tft.fillRect(2, 66, 70, 10, COLOR_BLACK);
            tft.setTextColor(COLOR_DARK_GRAY);
            tft.setCursor(2, 66);
            tft.printf("%02d:%02d:%02d", h, m, s);
            prevGpsHour = h;
            prevGpsMin = m;
            prevGpsSec = s;
            timeUpdated = true;
        }
    }

    // Battery voltage (right side of Row 5)
    if (batteryVoltage != prevBatteryVoltage || timeUpdated) {
        tft.fillRect(100, 66, 60, 10, COLOR_BLACK);
        if (batteryVoltage > 0.0) {
            // Color based on charge level
            uint16_t batColor;
            if (batteryVoltage >= 3.8) batColor = COLOR_GREEN;
            else if (batteryVoltage >= 3.5) batColor = COLOR_YELLOW;
            else batColor = COLOR_RED;

            tft.setTextColor(batColor);
            tft.setCursor(100, 66);
            tft.printf("%.2fV %d%%", batteryVoltage, getBatteryPercent());
        }
        prevBatteryVoltage = batteryVoltage;
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
