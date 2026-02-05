# Rocket Avionics Project

## Overview

A complete rocket avionics system for model/high-power rocketry including flight computer, ground gateway, and companion apps for telemetry monitoring and rocket recovery.

## Project Structure

```
rocket_avionics/
├── firmware_flight/        # Flight computer firmware (RP2040)
├── firmware_gateway/       # Ground gateway firmware (RP2040)
├── firmware_gateway_heltec/# Ground gateway firmware (Heltec ESP32-S3)
├── desktop_app/            # macOS/Windows desktop app (Xojo)
├── ios_app/                # iOS companion app (Xojo)
├── hardware/               # PCB designs and schematics
├── docs/                   # Documentation
├── artwork/                # Icons and graphics
├── tools/                  # Utility scripts
└── sdk -> ../test_stand/sdk  # Pico SDK symlink
```

## Hardware Components

### Flight Computer
- **MCU:** Adafruit Feather RP2040 with RFM95 LoRa Radio (915MHz)
- **Barometers:** BMP390 (0x77) and/or BMP581 (0x47) -- auto-detected at startup
- **IMU:** LSM6DSOX+LIS3MDL 9-DoF (or ICM-20649 for high-G flights)
- **Display:** SSD1306 128x64 OLED FeatherWing
- **Features:** Dual pyro channels, GPS, flash storage for flight data
- **Sensor Fusion:** Complementary filter fuses barometer + IMU for altitude/velocity (see `docs/SENSORS.md`)

### Ground Gateway (RP2040)
- **MCU:** Adafruit Feather RP2040 with RFM95 LoRa Radio (915MHz)
- **Display:** SSD1306 128x64 OLED FeatherWing
- **Optional:** GPS FeatherWing, AirLift WiFi FeatherWing

### Ground Gateway (Heltec) - Recommended
- **Board:** Heltec Wireless Tracker (ESP32-S3 + SX1262 + GPS)
- **Features:** Built-in display, WiFi AP/Station mode, GPS, OTA updates
- **Network:** mDNS discovery (`RocketGateway.local`), TCP port 5000

## Communication Protocol

- **LoRa:** 915 MHz, SF7, 125 kHz bandwidth, sync word 0x14
- **Telemetry:** Binary packet with JSON forwarding at 10 Hz
- **Commands:** JSON from apps to gateway, binary LoRa to flight computer
- **Multi-Rocket:** Up to 15 flight computers with unique IDs (0-15)

### Packet Format (v2.0)
```
Telemetry: magic(1), type(1), rocketId(1), seq(2), data...
Command:   magic(1), type(1), targetRocketId(1), cmdId(1), params...
```

### Multi-Rocket Support
- Each flight computer has a unique rocket ID (0-15)
- Rocket ID is editable via display Button B on the Rocket ID screen
- Rocket ID stored in flash, persists across reboots
- Gateway tracks all active rockets and cycles display
- Apps can request rocket list: `{"cmd":"rockets"}`
- Commands target specific rocket: `{"cmd":"arm","rocket":0}`

### Flight Computer Display Navigation
- **Button A:** Cycle through display screens
- **Button B:**
  - On Rocket ID screen: Cycle rocket ID (0-15)
  - On all other screens: Home (return to Live screen)

## Build Requirements

### Firmware (RP2040)
- Raspberry Pi Pico SDK (set `PICO_SDK_PATH` environment variable)
- CMake 3.13+
- ARM GCC toolchain

### Firmware (Heltec)
- Arduino IDE with Heltec ESP32 board support
- RadioLib library
- TinyGPSPlus library

### Desktop/iOS Apps
- Xojo IDE (2023r4 or later)

## Key Files

| File | Purpose |
|------|---------|
| `firmware_*/include/version.h` | Version numbers |
| `firmware_*/include/pins.h` | GPIO pin assignments |
| `docs/software_versioning.md` | Versioning system documentation |
| `docs/flight_computer_carrier_board_design.md` | Custom PCB design |

## Coding Conventions

### C Firmware
- Hungarian notation for variables: `sStatic`, `inParam`, `theLocal`
- Constants: `kConstantName`
- Functions: `Module_FunctionName()`
- 2-space indentation
- Spaces before semicolons: `statement ;`

### Xojo Apps
- Properties: `pPropertyName`
- Methods: `MethodName()`
- Modules: `Module_Name`

## Version Management

Each component has independent versioning:
- Version string in `version.h`: `FIRMWARE_VERSION_STRING`
- Build timestamps auto-update via CMake force-rebuild
- See `docs/software_versioning.md` for details

## Quick Commands

```bash
# Build flight firmware
cd firmware_flight/build && cmake .. && make -j4

# Build RP2040 gateway firmware
cd firmware_gateway/build && cmake .. && make -j4

# Build Heltec gateway firmware
cd firmware_gateway_heltec
./increment_build.sh
arduino-cli compile --fqbn "Heltec-esp32:esp32:heltec_wireless_tracker" .

# Flash Heltec gateway via OTA (over WiFi)
python3 ~/Library/Arduino15/packages/Heltec-esp32/hardware/esp32/3.0.3/tools/espota.py \
  -i RocketGateway.local -p 3232 -f firmware_gateway_heltec.ino.bin

# Connect to Heltec gateway
nc RocketGateway.local 5000

# Query gateway info
echo '{"cmd":"gw_info"}' | nc RocketGateway.local 5000

# USB serial console
tools/usb_console.sh
```

## Documentation

- `docs/SENSORS.md` - Sensor fusion: altitude and velocity calculation (barometer + IMU complementary filter)
- `docs/HARDWARE.md` - Hardware guide, BOM, pin assignments, wiring
- `docs/PROTOCOL.md` - Communication protocol (LoRa packets, JSON, commands)
- `docs/TESTING.md` - Testing procedures
- `docs/software_versioning.md` - Version management system
- `docs/flight_computer_carrier_board_design.md` - Custom PCB design
- `docs/conversation_archive_*.md` - Development session archives
- `firmware_gateway_heltec/CLAUDE.md` - Heltec gateway details (OTA, mDNS, GPS)
- `firmware_flight/CLAUDE.md` - Flight computer firmware details
- `desktop_app/CLAUDE.md` - Desktop application details
- `ios_app/CLAUDE.md` - iOS app details (recovery, multi-rocket)
