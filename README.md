# Rocket Avionics Flight Computer

A complete model rocket avionics system with real-time LoRa telemetry, dual parachute deployment, GPS tracking, and multi-rocket support. Includes flight computer, ground gateway, and desktop/iOS companion apps.

## Overview

The Rocket Avionics system provides comprehensive flight monitoring and recovery capabilities for model and high-power rockets. Real-time telemetry is transmitted via LoRa radio to a ground station, with GPS coordinates enabling rocket recovery after landing.

### Key Features

- **Multi-rocket support** - Track up to 16 rockets simultaneously (IDs 0-15)
- **10 Hz telemetry** via LoRa radio (915 MHz)
- **GPS tracking** with position saved for offline recovery
- **Dual parachute deployment** - Drogue at apogee, main at configurable altitude
- **9-DoF IMU** - Accelerometer, gyroscope, and magnetometer
- **Barometric altitude** using BMP390 precision sensor
- **WiFi connectivity** - Connect apps directly to gateway over WiFi
- **OTA updates** - Update Heltec gateway firmware over WiFi
- **mDNS discovery** - Find gateway at `RocketGateway.local`
- **Cross-platform apps** for macOS, Windows, and iOS
- **Rocket recovery navigation** - iOS app guides you to landed rocket

## System Components

### 1a. Flight Computer - RP2040 (`/firmware_flight/`)
RP2040-based flight computer with Feather ecosystem.

- **Feather RP2040 + RFM95** LoRa radio (915 MHz)
- **BMP390** barometric altitude sensor
- **LSM6DSOX + LIS3MDL** 9-DoF IMU
- **GPS module** for position tracking
- **OLED display** with button navigation
- **Dual pyro channels** with continuity detection
- **~$125 total cost**

### 1b. Flight Computer - Heltec (Lower Cost) (`/firmware_flight_heltec/`)
ESP32-S3 based flight computer with built-in GPS and display.

- **Heltec Wireless Tracker** (ESP32-S3 + SX1262 + GPS + Display)
- **BMP390** barometric altitude (external I2C)
- **LSM6DSOX + LIS3MDL** 9-DoF IMU (external I2C)
- **Built-in GPS** and TFT display
- **Dual pyro channels** with continuity detection
- **~$81 total cost (35% savings)**

### 2. Ground Gateway - Heltec (Recommended) (`/firmware_gateway_heltec/`)
ESP32-S3 based gateway with built-in WiFi, GPS, and display.

- **Heltec Wireless Tracker** (ESP32-S3 + SX1262 + GPS)
- **WiFi AP/Station mode** - Create hotspot or join existing network
- **mDNS** - Discover at `RocketGateway.local`
- **OTA updates** - Flash new firmware over WiFi
- **Built-in display** - Shows rocket status, GPS, packet count
- **TCP server** on port 5000 for app connections

### 3. Ground Gateway - RP2040 (`/firmware_gateway/`)
Alternative gateway using RP2040 with optional WiFi.

- **Feather RP2040 + RFM95** LoRa radio
- **USB serial** output for desktop connection
- **Optional AirLift** WiFi FeatherWing

### 4. Desktop Application (`/desktop_app/`)
Xojo application for macOS and Windows.

- Real-time altitude/velocity charts
- Multi-rocket tracking with rocket list
- Rename rockets with custom names
- Flight data download from flash storage
- Gateway status and configuration
- Flight history database
- Data export (CSV)

### 5. iOS Application (`/ios_app/`)
Xojo mobile app for iPhone.

- Live telemetry display
- **Rocket recovery navigation** - Compass and distance to landed rocket
- Uses iPhone GPS to guide you to rocket location
- Works offline with cached rocket coordinates
- Multi-rocket support with individual recovery

## Hardware

### Flight Computer (RP2040 - Full Featured)
| Component | Description |
|-----------|-------------|
| Adafruit Feather RP2040 + RFM95 | MCU with 915 MHz LoRa radio |
| Adafruit BMP390 | Barometric pressure/altitude |
| Adafruit LSM6DSOX + LIS3MDL | 9-DoF IMU FeatherWing |
| Adafruit FeatherWing OLED 128x64 | Status display with buttons |
| GPS Module | UART GPS for position tracking |

### Flight Computer (Heltec - Lower Cost)
| Component | Description |
|-----------|-------------|
| Heltec Wireless Tracker | ESP32-S3 + SX1262 + GPS + Display |
| Adafruit BMP390 | Barometric pressure (I2C) |
| Adafruit LSM6DSOX + LIS3MDL | 9-DoF IMU (I2C) |

### Ground Gateway (Heltec - Recommended)
| Component | Description |
|-----------|-------------|
| Heltec Wireless Tracker | ESP32-S3 + SX1262 LoRa + GPS + Display |

### Ground Gateway (RP2040 - Alternative)
| Component | Description |
|-----------|-------------|
| Adafruit Feather RP2040 + RFM95 | MCU with 915 MHz LoRa radio |
| Adafruit AirLift FeatherWing | Optional WiFi (ESP32) |

## Quick Start

### Flight Computer
```bash
cd firmware_flight
mkdir -p build && cd build
cmake .. && make -j4
picotool load rocket_avionics_flight.uf2 -f
```

### Heltec Gateway
```bash
cd firmware_gateway_heltec
./increment_build.sh
# Use Arduino IDE or arduino-cli to compile and upload
```

### OTA Update (Heltec Gateway)
```bash
python3 ~/Library/Arduino15/packages/Heltec-esp32/hardware/esp32/3.0.3/tools/espota.py \
  -i RocketGateway.local -p 3232 -f firmware_gateway_heltec.ino.bin
```

### Connect to Gateway
```bash
# Via mDNS hostname
nc RocketGateway.local 5000

# Query gateway info
echo '{"cmd":"gw_info"}' | nc RocketGateway.local 5000
```

## Flight Computer Display

The OLED display has two buttons for navigation:

| Button | Action |
|--------|--------|
| **A** | Cycle through screens: Live → Sensors → Pyro → Storage → Rocket ID → About |
| **B** | On Rocket ID screen: Change ID (0-15). On other screens: Return to Live |

## Multi-Rocket Support

Each flight computer has a unique rocket ID (0-15):

1. Navigate to **Rocket ID** screen (Button A)
2. Press **Button B** to cycle through IDs
3. ID is saved to flash and persists across reboots
4. All telemetry packets include the rocket ID
5. Gateway and apps track all rockets independently

## Flight States

| State | Description | Trigger |
|-------|-------------|---------|
| IDLE | On pad, pre-flight | Power on |
| ARMED | Ready for launch | ARM command |
| BOOST | Motor burning | Altitude >10m or velocity >10m/s |
| COAST | Coasting to apogee | Acceleration drops |
| APOGEE | Peak altitude | Velocity crosses zero (fires drogue) |
| DROGUE | Descending on drogue | Drogue deployed |
| MAIN | Main chute altitude | Below configured altitude (fires main) |
| LANDED | On ground | Stationary for 5 seconds |

## Pyro Channels

| Channel | Purpose | Fire Condition |
|---------|---------|----------------|
| Pyro 1 | Drogue parachute | At apogee (velocity = 0) |
| Pyro 2 | Main parachute | Below main deploy altitude |

**Safety:** Arm switch must be closed to enable pyro firing.

## Directory Structure

```
rocket_avionics/
├── firmware_flight/          # Flight computer (RP2040, C)
├── firmware_flight_heltec/   # Flight computer (Heltec, Arduino)
├── firmware_gateway/         # RP2040 gateway (C)
├── firmware_gateway_heltec/  # Heltec gateway (Arduino)
├── desktop_app/              # Desktop app (Xojo)
├── ios_app/                  # iOS app (Xojo)
├── hardware/                 # PCB designs
├── docs/                     # Documentation
└── tools/                    # Utility scripts
```

## Documentation

- `CLAUDE.md` - Project overview and coding conventions
- `firmware_flight/CLAUDE.md` - RP2040 flight computer details
- `firmware_flight_heltec/CLAUDE.md` - Heltec flight computer details
- `firmware_gateway_heltec/CLAUDE.md` - Heltec gateway (OTA, mDNS, GPS)
- `desktop_app/CLAUDE.md` - Desktop application
- `ios_app/CLAUDE.md` - iOS app (recovery, multi-rocket)
- `docs/flight_computer_parts_list.md` - RP2040 flight computer BOM
- `docs/heltec_flight_computer_parts_list.md` - Heltec flight computer BOM
- `docs/gateway_parts_list.md` - Gateway options BOM

## License

Copyright (c) 2025-2026 by Mark Gavin. All Rights Reserved.

Proprietary software - not for distribution.
