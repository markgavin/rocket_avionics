# Heltec Gateway Firmware (ESP32-S3)

## Overview

Ground station gateway firmware for Heltec Wireless Tracker. Bridges LoRa telemetry from flight computer to WiFi TCP clients with built-in GPS for ground station location.

## Hardware

- **Board:** Heltec Wireless Tracker (ESP32-S3 + SX1262 LoRa + GPS)
- **LoRa:** SX1262 915MHz with onboard PCB antenna
- **Display:** Built-in 0.96" TFT (ST7735, 160x80 pixels, color)
- **GPS:** Built-in UC6580 GPS module with ceramic chip antenna
- **WiFi:** Built-in ESP32-S3 WiFi

## Antennas

The Wireless Tracker has **built-in antennas** - no external antennas required:

| Antenna | Type | Notes |
|---------|------|-------|
| **LoRa** | Onboard spring/wire antenna | Ready to use |
| **GPS** | Ceramic chip antenna on PCB | Needs clear sky view for fix |

For extended LoRa range, you can optionally add an external 915MHz antenna via the U.FL connector (if present on your board version).

## Arduino IDE Setup

### 1. Install Heltec Board Support

Add this URL to **File > Preferences > Additional Board Manager URLs**:
```
https://resource.heltec.cn/download/package_heltec_esp32_index.json
```

Install **Heltec ESP32 Series Dev-boards** from Board Manager.

### 2. Install Required Libraries

Go to **Tools > Manage Libraries** and install:
- **RadioLib** by Jan Gromes
- **TinyGPSPlus** by Mikal Hart
- **Adafruit GFX Library** by Adafruit
- **Adafruit ST7735 and ST7789 Library** by Adafruit

### 3. Select Board and Port

- **Board**: Tools → Board → Heltec Wireless Tracker
- **Port**: Select the USB serial port
- **Upload Speed**: 921600

### 4. Upload Firmware

Open `firmware_gateway_heltec.ino` and click Upload.

**If upload fails**, enter bootloader mode manually:
1. Hold **USER** button
2. Press **RESET** button
3. Release RESET, then release USER
4. Try upload again

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

1. **Station mode first:** If `WIFI_STA_SSID` is configured, attempts to connect to existing WiFi
2. **AP fallback:** If station mode fails (or SSID empty), creates "RocketGateway" hotspot
3. Display shows current mode ([STA] or [AP]) and IP address

### Connecting

**Station mode:**
- Check display for assigned IP
- Connect to port 5000 at that IP

**AP mode:**
- Connect to WiFi "RocketGateway"
- Connect to `192.168.4.1:5000`

## Pin Definitions

```cpp
// LoRa SX1262
#define LORA_NSS    8
#define LORA_DIO1   14
#define LORA_RST    12
#define LORA_BUSY   13

// GPS UART
#define GPS_RX      33
#define GPS_TX      34

// TFT Display (ST7735 160x80)
#define TFT_MOSI    42
#define TFT_SCLK    41
#define TFT_CS      38
#define TFT_DC      40
#define TFT_RST     39
#define TFT_BL      21      // Backlight
#define TFT_POWER   3       // VEXT (shared with GPS)
```

## Display Layout

The 160x80 color TFT shows:

```
┌────────────────────────────┐
│ GATEWAY            [AP]    │
├────────────────────────────┤
│ 192.168.4.1          C:1   │
│ LoRa:1234      -45dB       │
│ 38.8970,-77.0360           │
│ {"type":"tel"...           │
│ 12:34:56 UTC               │
└────────────────────────────┘
```

- Row 0: Gateway title + WiFi mode (cyan/orange)
- Row 1: IP address + client count
- Row 2: LoRa packet count + RSSI
- Row 3: GPS coordinates (green when valid)
- Row 4: Last packet preview
- Row 5: UTC time from GPS

## Data Flow

```
Flight Computer → LoRa → Heltec → WiFi TCP → Desktop/iOS App
                                    ↓
                                 Display
```

## Troubleshooting

### Upload fails
- Enter bootloader mode manually (USER + RESET)
- Try lower upload speed (460800)

### No LoRa packets received
- Verify frequency/SF/BW match flight computer
- Check sync word matches (0x14)
- Move closer to transmitter for testing

### WiFi not visible
- Check serial output for errors
- Try resetting the board

### GPS no fix
- Move to area with clear sky view
- Wait 1-2 minutes for cold start fix
- Keep the PCB oriented with clear view up

### Display blank
- VEXT (GPIO3) must be HIGH for display power
- Backlight (GPIO21) must be HIGH

## Hardware Versions

| Version | GPS Power Pin | Notes |
|---------|---------------|-------|
| V1.0 | GPIO37 | Original |
| V1.1+ | GPIO3 | Current, firmware configured for this |
