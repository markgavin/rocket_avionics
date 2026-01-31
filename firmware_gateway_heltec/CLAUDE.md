# Heltec Gateway Firmware (ESP32-S3)

## Overview

Ground station gateway firmware for Heltec Wireless Tracker. An alternative to the RP2040 gateway with built-in WiFi, GPS, and display. Bridges LoRa telemetry to WiFi TCP clients.

## Hardware

- **Board:** Heltec Wireless Tracker (ESP32-S3 + SX1262 LoRa + GPS)
- **LoRa:** SX1262 915MHz
- **Display:** Built-in 0.96" OLED (SSD1306)
- **GPS:** Built-in UC6580 GPS module
- **WiFi:** Built-in ESP32-S3 WiFi

## Project Structure

```
firmware_gateway_heltec/
├── firmware_gateway_heltec.ino  # Main Arduino sketch
├── version.h                    # Version information
├── build_number.h               # Auto-generated build number
├── build_number.txt             # Persistent build counter
├── increment_build.sh           # Script to increment build number
└── README.md                    # Setup instructions
```

## Build Requirements

### Arduino IDE Setup

1. Add Heltec board URL to Preferences:
   ```
   https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/0.0.9/package_heltec_esp32_index.json
   ```

2. Install board: **Tools → Board → Board Manager → Heltec ESP32 Series Dev-boards**

3. Install libraries via Library Manager:
   - **RadioLib** by Jan Gromes
   - **TinyGPSPlus** by Mikal Hart

4. Select board: **Tools → Board → Heltec Wireless Tracker**

5. Upload speed: 921600

## Configuration

### LoRa Settings (must match flight computer)

```cpp
#define LORA_FREQUENCY      915.0       // MHz
#define LORA_BANDWIDTH      125.0       // kHz
#define LORA_SPREAD_FACTOR  7
#define LORA_CODING_RATE    5           // 4/5
#define LORA_SYNC_WORD      0x14        // Private sync word
```

### WiFi Settings

```cpp
// Station mode (connect to existing network)
#define WIFI_STA_SSID       "YourHomeWiFi"
#define WIFI_STA_PASSWORD   "YourPassword"
#define WIFI_STA_TIMEOUT_MS 10000

// AP mode (create hotspot) - fallback
#define WIFI_AP_SSID        "RocketGateway"
#define WIFI_AP_PASSWORD    ""          // Empty = open
#define TCP_PORT            5000
```

## WiFi Fallback Mode

1. **Station mode first:** If `WIFI_STA_SSID` is configured, attempts to connect to existing WiFi network
2. **AP fallback:** If station mode fails (or SSID empty), creates "RocketGateway" hotspot
3. Display shows current mode and IP address

### Connecting

**Station mode:**
- Check serial output or display for assigned IP
- Connect to `TCP_PORT` (5000) at that IP

**AP mode:**
- Connect device to "RocketGateway" WiFi
- Connect to `192.168.4.1:5000`

## Pin Definitions

```cpp
// LoRa SX1262
#define LORA_NSS    8
#define LORA_DIO1   14
#define LORA_RST    12
#define LORA_BUSY   13

// GPS UART (UC6580 module)
#define GPS_RX      33
#define GPS_TX      34
#define GPS_BAUD    115200  // UC6580 default baud rate

// Display I2C
#define DISPLAY_SDA 17
#define DISPLAY_SCL 18
#define DISPLAY_RST 21
```

## Data Flow

```
Flight Computer → LoRa → Heltec → WiFi TCP → Desktop/iOS App
                                    ↓
                                  Display
```

1. LoRa packet received from flight computer
2. JSON forwarded to all connected TCP clients
3. Display updated with packet info
4. Commands from TCP clients forwarded via LoRa

## Display Content

```
┌─────────────────────────┐
│ Rocket Gateway v1.0.0   │
│ WiFi: RocketGateway (1) │
│ LoRa: 1234 pkts         │
│ GPS: 38.897°N 77.036°W  │
│ Last: {"type":"tel"...  │
└─────────────────────────┘
```

- Line 1: Version
- Line 2: WiFi SSID and client count
- Line 3: LoRa packet count
- Line 4: GPS coordinates (gateway location)
- Line 5: Last packet preview

## Serial Output

115200 baud, same JSON as TCP output:
```
========================================
  ROCKET AVIONICS GATEWAY - HELTEC
  Version 1.0.0
  Build: Jan 22 2026 10:30:15
========================================

Gateway Ready!
  WiFi Mode: AP
  WiFi SSID: RocketGateway
  IP: 192.168.4.1
  TCP Port: 5000
  LoRa Freq: 915.0 MHz
```

## Network Discovery (mDNS)

The gateway advertises itself via mDNS for easy network discovery:

- **Hostname:** `RocketGateway.local`
- **Service:** `_rocket._tcp` on port 5000
- **TXT Records:** version, type=gateway, board=heltec

### Connecting via mDNS

Instead of using an IP address, connect using the hostname:

```bash
# Ping the gateway
ping RocketGateway.local

# Connect via netcat
nc RocketGateway.local 5000

# Query gateway info
echo '{"cmd":"gw_info"}' | nc RocketGateway.local 5000
```

Works on macOS, iOS, and Linux (with Avahi). Windows requires Bonjour.

## OTA (Over-The-Air) Updates

The gateway supports wireless firmware updates via ArduinoOTA.

### Prerequisites

- Gateway must be connected to WiFi (Station mode)
- Computer must be on the same network
- First OTA-enabled firmware must be flashed via USB

### Update Procedure

1. **Increment build number and compile:**
   ```bash
   cd firmware_gateway_heltec
   ./increment_build.sh
   arduino-cli compile --fqbn "Heltec-esp32:esp32:heltec_wireless_tracker" \
     --output-dir /tmp/heltec_build .
   ```

2. **Upload via OTA:**
   ```bash
   python3 ~/Library/Arduino15/packages/Heltec-esp32/hardware/esp32/3.0.3/tools/espota.py \
     -i RocketGateway.local -p 3232 \
     -f /tmp/heltec_build/firmware_gateway_heltec.ino.bin
   ```

   Or using IP address:
   ```bash
   python3 .../espota.py -i 192.168.x.x -p 3232 -f /tmp/heltec_build/firmware_gateway_heltec.ino.bin
   ```

3. **Verify update:**
   ```bash
   echo '{"cmd":"gw_info"}' | nc RocketGateway.local 5000 | grep version
   ```

### OTA Display Feedback

During OTA update, the gateway display shows:
- "OTA UPDATE in progress..." with progress bar
- "OTA Complete!" on success
- Error message on failure

### OTA Security

OTA is currently open (no password). To add password protection, uncomment in `initOTA()`:
```cpp
ArduinoOTA.setPassword("your_password");
```

## Build Numbers

Firmware uses auto-incrementing build numbers instead of semantic versioning:

- **Format:** "Build N" (e.g., "Build 14")
- **Storage:** `build_number.txt` (persistent counter)
- **Header:** `build_number.h` (auto-generated)

### Incrementing Build Number

Run before each build:
```bash
./increment_build.sh
```

This updates both `build_number.txt` and `build_number.h`.

## Troubleshooting

### No LoRa packets received
- Check antenna connection (U.FL connector)
- Verify frequency/SF/BW match flight computer
- Check sync word matches (0x14)

### WiFi not visible
- Check if board is powered
- Try resetting the board
- Check serial output for errors

### GPS no fix
- The UC6580 GPS has a weak ceramic antenna
- Requires clear sky view (won't work indoors)
- Move gateway outside or to window with direct sky view
- Wait 1-2 minutes for cold start fix
- Flight computer GPS modules are more sensitive

### OTA update fails
- Verify gateway is on same network as computer
- Check gateway IP with `ping RocketGateway.local`
- Ensure port 3232 is not blocked by firewall
- Try using IP address instead of hostname
- Power cycle gateway and retry

## Claude Code Notes

**WARNING: Do not attempt to read serial output from this board using cat, screen, or similar commands.** The USB serial connection on the Heltec Wireless Tracker causes Claude Code sessions to hang indefinitely when attempting to read from it.

For debugging:
- Use the TFT display to verify operation
- Connect via WiFi TCP and send `status` or `gw_info` commands
- Use the Arduino IDE Serial Monitor if manual serial debugging is needed

## Differences from RP2040 Gateway

| Feature | RP2040 Gateway | Heltec Gateway |
|---------|----------------|----------------|
| LoRa chip | RFM95 (SX1276) | SX1262 |
| WiFi | Optional AirLift | Built-in |
| GPS | Optional FeatherWing | Built-in (UC6580) |
| Display | FeatherWing OLED | Built-in TFT |
| Build system | CMake/Pico SDK | Arduino IDE |
| Primary output | USB Serial | WiFi TCP |
| OTA updates | Not supported | Supported |
| mDNS | Not supported | RocketGateway.local |
