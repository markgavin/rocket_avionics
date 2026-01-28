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
- **Sensors:** BMP390 barometer, LSM6DSOX+LIS3MDL 9-DoF IMU
- **Display:** SSD1306 128x64 OLED FeatherWing
- **Features:** Dual pyro channels, GPS, flash storage for flight data

### Ground Gateway (RP2040)
- **MCU:** Adafruit Feather RP2040 with RFM95 LoRa Radio (915MHz)
- **Display:** SSD1306 128x64 OLED FeatherWing
- **Optional:** GPS FeatherWing, AirLift WiFi FeatherWing

### Ground Gateway (Heltec)
- **Board:** Heltec Wireless Tracker (ESP32-S3 + SX1262 + GPS)
- **Features:** Built-in display, WiFi AP/Station mode, GPS

## Communication Protocol

- **LoRa:** 915 MHz, SF7, 125 kHz bandwidth, sync word 0x14
- **Telemetry:** JSON format over LoRa at 10 Hz
- **Commands:** JSON format from gateway to flight computer

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
# Build gateway firmware
cd firmware_gateway/build && cmake .. && make -j4

# Build flight firmware
cd firmware_flight/build && cmake .. && make -j4

# USB serial console
tools/usb_console.sh
```

## Documentation

- `docs/software_versioning.md` - Version management system
- `docs/flight_computer_carrier_board_design.md` - Custom PCB design
- `docs/conversation_archive_*.md` - Development session archives
