# Rocket Avionics Gateway - Heltec Wireless Tracker

Ground station gateway using Heltec Wireless Tracker (ESP32-S3 + SX1262 + GPS).

## Hardware Required

- Heltec Wireless Tracker (915MHz version)
- 915MHz LoRa antenna (U.FL/IPEX connector)
- GPS/GNSS antenna (U.FL/IPEX connector, active recommended)
- Optional: 3.7V LiPo battery with SH1.25 connector

## Arduino IDE Setup

### 1. Install Heltec Board Support

In Arduino IDE, go to **File > Preferences** and add this URL to "Additional Board Manager URLs":
```
https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/0.0.9/package_heltec_esp32_index.json
```

Then go to **Tools > Board > Board Manager**, search for "Heltec" and install **Heltec ESP32 Series Dev-boards**.

### 2. Install Required Libraries

Go to **Tools > Manage Libraries** and install:
- **RadioLib** by Jan Gromes (for SX1262 LoRa)
- **TinyGPSPlus** by Mikal Hart (for GPS parsing)

The display library is included with Heltec board support.

### 3. Select Board and Port

- **Board**: Heltec Wireless Tracker
- **Port**: Select the USB serial port
- **Upload Speed**: 921600

### 4. Upload Firmware

Open `firmware_gateway_heltec.ino` and click Upload.

## Configuration

LoRa settings in the firmware MUST match your flight computer:

```cpp
#define LORA_FREQUENCY      915.0       // MHz
#define LORA_BANDWIDTH      125.0       // kHz
#define LORA_SPREAD_FACTOR  7
#define LORA_CODING_RATE    5           // 4/5
#define LORA_SYNC_WORD      0x14        // Private sync word
```

WiFi settings (fallback mode - tries station first, then AP):
```cpp
// Station mode - connect to existing network
#define WIFI_STA_SSID       "YourHomeWiFi"    // Leave empty to skip station mode
#define WIFI_STA_PASSWORD   "YourPassword"
#define WIFI_STA_TIMEOUT_MS 10000             // 10 second timeout

// AP mode - create hotspot (fallback)
#define WIFI_AP_SSID        "RocketGateway"
#define WIFI_AP_PASSWORD    ""                // Empty = open AP
#define TCP_PORT            5000
```

## Usage

### WiFi Fallback Mode

The gateway uses a fallback WiFi strategy:

1. **Station mode first**: If `WIFI_STA_SSID` is configured, it tries to connect to your existing WiFi network
2. **AP fallback**: If station mode fails (or isn't configured), it creates the "RocketGateway" hotspot

### Connecting

**If using Station mode** (connected to your home/field network):
1. Power on the gateway
2. Check serial output or display for the assigned IP address
3. Connect to TCP port 5000 at that IP

**If using AP mode** (hotspot):
1. Power on the gateway
2. Connect your device to WiFi network "RocketGateway"
3. Connect to TCP port 5000 (IP: 192.168.4.1)

### Data Flow

- LoRa telemetry from flight computer is forwarded to all connected TCP clients
- Commands sent from TCP clients are forwarded via LoRa to flight computer

## Display

The LCD shows:
- Line 1: "Rocket Gateway"
- Line 2: WiFi SSID and client count
- Line 3: LoRa packet count
- Line 4: GPS status
- Line 5: Last packet preview

## Troubleshooting

### No LoRa packets received
- Check antenna connection
- Verify frequency/SF/BW match flight computer
- Check sync word matches (0x14)

### WiFi not visible
- Check if board is powered
- Try resetting the board

### GPS no fix
- Ensure GPS antenna is connected
- Move to area with clear sky view
- Wait 1-2 minutes for cold start fix
