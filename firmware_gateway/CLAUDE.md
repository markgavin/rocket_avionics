# Gateway Firmware (RP2040)

## Overview

Ground station gateway firmware for Adafruit Feather RP2040 with RFM95 LoRa Radio. Bridges LoRa telemetry from flight computer to USB serial (and optionally WiFi).

## Hardware

- **Board:** Adafruit Feather RP2040 with RFM95 LoRa Radio - 915MHz (Product ID: 5714)
- **Display:** Adafruit FeatherWing OLED 128x64 SSD1306/SH1107 (Product ID: 4650)
- **Barometer:** Adafruit BMP390 (Product ID: 4816) - ground reference pressure
- **GPS:** Adafruit Ultimate GPS FeatherWing (optional)
- **WiFi:** Adafruit AirLift FeatherWing (optional, disabled by default)

## Project Structure

```
firmware_gateway/
├── include/
│   ├── pins.h              # GPIO pin assignments
│   ├── version.h           # Version declarations
│   ├── lora_radio.h        # RFM95 LoRa driver
│   ├── gateway_protocol.h  # Telemetry parsing/formatting
│   ├── gateway_display.h   # OLED display management
│   ├── bmp390.h            # Barometric sensor driver
│   ├── ssd1306.h           # OLED display driver
│   ├── gps.h               # GPS NMEA parser
│   ├── neopixel.h          # WS2812 LED driver
│   ├── wifi_nina.h         # AirLift WiFi driver
│   └── wifi_config.h       # WiFi configuration
├── src/
│   ├── main.c              # Main application loop
│   ├── version.c           # Build timestamp (force-rebuilt)
│   ├── lora_radio.c        # RFM95 SPI communication
│   ├── gateway_protocol.c  # JSON telemetry handling
│   ├── gateway_display.c   # Display screens and updates
│   ├── bmp390.c            # BMP390 I2C driver
│   ├── ssd1306.c           # SSD1306 I2C driver
│   ├── gps.c               # NMEA sentence parsing
│   ├── neopixel.c          # NeoPixel PIO driver
│   ├── wifi_nina.c         # NINA SPI protocol
│   └── wifi_config.c       # WiFi AP/station management
├── build/                  # CMake build directory
└── CMakeLists.txt          # Build configuration
```

## Build

```bash
mkdir -p build && cd build
cmake ..
make -j4
# Output: build/rocket_gateway.uf2
```

## Configuration

### pins.h Key Settings

```c
#define kEnableGps      1   // Set to 0 if GPS not installed
#define kEnableWifi     0   // Set to 1 to enable AirLift WiFi
```

### LoRa Settings (must match flight computer)

```c
#define kLoRaFrequency      915000000   // 915 MHz
#define kLoRaSpreadFactor   7           // SF7
#define kLoRaBandwidth      125000      // 125 kHz
#define kLoRaSyncWord       0x14        // Private sync word
```

## Main Loop Flow

1. Initialize hardware (SPI, I2C, UART, display, LoRa, GPS)
2. Enter receive mode on LoRa radio
3. Main loop:
   - Check for LoRa packets → parse and output JSON to USB
   - Check USB input → forward commands to flight computer via LoRa
   - Update display (5 Hz)
   - Read barometer (ground reference)
   - Process GPS if enabled
   - Update NeoPixel status LED

## Display Modes

Navigate with OLED FeatherWing buttons (A/B/C):

| Mode | Content |
|------|---------|
| Status | Connection state, packet count, RSSI |
| Telemetry | Altitude, velocity, flight state |
| GPS | Latitude, longitude, satellites |
| Barometer | Ground pressure and temperature |
| About | Version and build info |

## NeoPixel Status Colors

| Color | Meaning |
|-------|---------|
| Blue slow blink | Searching for flight computer |
| Green solid | Connected, receiving telemetry |
| Green fast blink | Receiving packets |
| Red | Error condition |

## JSON Output Format

Telemetry forwarded to USB:
```json
{"type":"tel","alt":152.3,"vel":45.2,"state":"BOOST","lat":38.897,"lon":-77.036}
```

Status message:
```json
{"type":"status","version":"1.0.0","build":"Jan 22 2026 10:30:15","lora":true}
```

## Coding Conventions

- Variables: `sStatic`, `inParam`, `theLocal`
- Constants: `kConstantName`
- Functions: `ModuleName_FunctionName()`
- Spaces before semicolons
- 2-space indentation
