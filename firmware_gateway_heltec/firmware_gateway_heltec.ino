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

// Display dimensions
#define TFT_WIDTH   160
#define TFT_HEIGHT  80

// Backlight PWM (0-255, lower = dimmer = cooler)
#define TFT_BL_BRIGHTNESS  128   // 50% brightness to reduce heat

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
#define LORA_TX_POWER       20          // dBm

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
        if (lastLoraPacketLen >= 4 && lastLoraPacketBinary[0] == LORA_MAGIC) {
            if (lastLoraPacketBinary[1] == LORA_PACKET_TELEMETRY &&
                lastLoraPacketLen >= sizeof(LoRaTelemetryPacket)) {
                forwardTelemetryAsJson();
            } else {
                // Unknown packet type - forward as hex for debugging
                forwardAsHex();
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
//----------------------------------------------
void handleClientCommand(int clientIdx, const String& command) {
    Serial.printf("WiFi RX [%d]: %s\n", clientIdx, command.c_str());

    // Check if it's a gateway info request
    if (command.indexOf("\"cmd\":\"gw_info\"") >= 0) {
        sendGatewayInfo(clientIdx);
        return;
    }

    // Otherwise, forward to LoRa (to flight computer)
    String txData = command;  // RadioLib needs non-const
    int state = radio.transmit(txData);
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println("LoRa TX: OK");
    } else {
        Serial.printf("LoRa TX error: %d\n", state);
    }

    // Restart receiving after transmit
    radio.startReceive();
}

//----------------------------------------------
// Send Gateway Info Response
//----------------------------------------------
void sendGatewayInfo(int clientIdx) {
    String response = "{\"type\":\"gw_info\",";
    response += "\"fw_ver\":\"" + String(FIRMWARE_VERSION_STRING) + "\",";
    response += "\"hw\":\"Heltec Wireless Tracker\",";
    response += "\"lora_freq\":" + String(LORA_FREQUENCY, 1) + ",";
    response += "\"lora_sf\":" + String(LORA_SPREAD_FACTOR) + ",";
    response += "\"lora_bw\":" + String(LORA_BANDWIDTH, 0) + ",";
    response += "\"wifi_mode\":\"" + String(wifiIsStationMode ? "station" : "ap") + "\",";
    response += "\"wifi_ssid\":\"" + String(wifiIsStationMode ? WIFI_STA_SSID : WIFI_AP_SSID) + "\",";
    response += "\"wifi_ip\":\"" + wifiCurrentIP + "\",";
    response += "\"gps_fix\":" + String(gps.location.isValid() ? "true" : "false");

    if (gps.location.isValid()) {
        response += ",\"gps_lat\":" + String(gps.location.lat(), 6);
        response += ",\"gps_lon\":" + String(gps.location.lng(), 6);
        response += ",\"gps_alt\":" + String(gps.altitude.meters(), 1);
        response += ",\"gps_sats\":" + String(gps.satellites.value());
    }

    response += ",\"lora_pkts\":" + String(loraPacketCount);
    response += ",\"clients\":" + String(clientCount);
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
