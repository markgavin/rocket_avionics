//----------------------------------------------
// Rocket Avionics Ground Gateway - Heltec Wireless Tracker
//
// Hardware: Heltec Wireless Tracker (ESP32-S3 + SX1262 + GPS)
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
#include "HT_SSD1306Wire.h"

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

// Display
#define DISPLAY_SDA 17
#define DISPLAY_SCL 18
#define DISPLAY_RST 21

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
#define WIFI_STA_SSID       ""          // Your home/field WiFi SSID
#define WIFI_STA_PASSWORD   ""          // Your home/field WiFi password
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
SSD1306Wire display(0x3C, DISPLAY_SDA, DISPLAY_SCL, GEOMETRY_128_64, DISPLAY_RST);

//----------------------------------------------
// State Variables
//----------------------------------------------
volatile bool loraPacketReceived = false;
String lastLoraPacket = "";
uint32_t loraPacketCount = 0;
uint32_t lastDisplayUpdate = 0;
uint32_t lastStatusPrint = 0;
int clientCount = 0;
bool wifiIsStationMode = false;         // true = station, false = AP
String wifiCurrentIP = "";

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
    Serial.println("Rocket Avionics Gateway - Heltec Wireless Tracker");
    Serial.println("========================================\n");

    // Initialize display
    initDisplay();
    displayStatus("Initializing...", "", "");

    // Initialize GPS
    initGPS();

    // Initialize LoRa
    if (!initLoRa()) {
        displayStatus("LoRa FAILED!", "", "");
        Serial.println("ERROR: LoRa initialization failed!");
        while (true) delay(1000);
    }

    // Initialize WiFi AP
    initWiFi();

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

    displayStatus("Ready", wifiCurrentIP.c_str(), "Waiting...");
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

    // Update display periodically
    if (millis() - lastDisplayUpdate > 500) {
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
// Initialize Display
//----------------------------------------------
void initDisplay() {
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    display.clear();
    display.drawString(0, 0, "Rocket Gateway");
    display.display();
    Serial.println("Display initialized");
}

//----------------------------------------------
// Initialize GPS
//----------------------------------------------
void initGPS() {
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
        displayStatus("Connecting WiFi", WIFI_STA_SSID, "Please wait...");

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
    displayStatus("Starting AP", WIFI_AP_SSID, "");

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

    // Read the packet
    String packet;
    int state = radio.readData(packet);

    if (state == RADIOLIB_ERR_NONE) {
        loraPacketCount++;
        lastLoraPacket = packet;

        float rssi = radio.getRSSI();
        float snr = radio.getSNR();

        Serial.printf("LoRa RX [%d]: RSSI=%.1f SNR=%.1f len=%d\n",
                      loraPacketCount, rssi, snr, packet.length());

        // Forward to all connected WiFi clients
        forwardToClients(packet);

    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
        Serial.println("LoRa RX: CRC error");
    } else {
        Serial.printf("LoRa RX error: %d\n", state);
    }

    // Restart receiving
    radio.startReceive();
}

//----------------------------------------------
// Forward Data to WiFi Clients
//----------------------------------------------
void forwardToClients(const String& data) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i].connected()) {
            clients[i].print(data);
            clients[i].print('\n');
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
    int state = radio.transmit(command);
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
    response += "\"fw_ver\":\"2.0.0\",";
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
// Update Display
//----------------------------------------------
void updateDisplay() {
    display.clear();

    // Title with WiFi mode indicator
    display.setFont(ArialMT_Plain_10);
    String titleStr = wifiIsStationMode ? "Gateway [STA]" : "Gateway [AP]";
    display.drawString(0, 0, titleStr);

    // WiFi status - show IP and client count
    String wifiStr = wifiCurrentIP + " (" + String(clientCount) + ")";
    display.drawString(0, 12, wifiStr);

    // LoRa status
    String loraStr = "LoRa: " + String(loraPacketCount) + " pkts";
    display.drawString(0, 24, loraStr);

    // GPS status
    String gpsStr = "GPS: ";
    if (gps.location.isValid()) {
        gpsStr += String(gps.location.lat(), 4) + "," + String(gps.location.lng(), 4);
    } else {
        gpsStr += "No fix (" + String(gps.satellites.value()) + " sats)";
    }
    display.drawString(0, 36, gpsStr);

    // Last packet info (truncated)
    if (lastLoraPacket.length() > 0) {
        String pktStr = lastLoraPacket.substring(0, 20);
        if (lastLoraPacket.length() > 20) pktStr += "...";
        display.drawString(0, 48, pktStr);
    }

    display.display();
}

//----------------------------------------------
// Display Status Message
//----------------------------------------------
void displayStatus(const char* line1, const char* line2, const char* line3) {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, line1);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 20, line2);
    display.drawString(0, 35, line3);
    display.display();
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
