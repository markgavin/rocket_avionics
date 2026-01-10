# Rocket Avionics Flight Computer

A model rocket flight avionics system with real-time LoRa telemetry, consisting of an onboard flight computer, ground station gateway, and desktop/iOS applications.

## Overview

The Rocket Avionics system captures flight telemetry data including altitude, velocity, and acceleration during model rocket flights. Real-time data is transmitted via LoRa radio to a ground station for live monitoring, while high-resolution data is logged to an SD card for post-flight analysis.

### Key Features

- **10 Hz telemetry** transmitted via LoRa radio (915 MHz)
- **100 Hz data logging** to SD card with RTC timestamps
- **Barometric altitude** using BMP390 precision sensor
- **GPS tracking** with position, speed, and heading
- **Differential altitude** using ground reference BMP390
- **Real-time ground station** display via LoRa gateway
- **Cross-platform apps** for macOS, Windows, Linux, and iOS
- **Flight state machine** with automatic phase detection
- **Data export** to CSV and JSON formats

## System Components

### 1. Flight Computer Firmware (`/firmware_flight/`)
C firmware for Adafruit Feather RP2040 with RFM95 LoRa Radio.

- BMP390 barometric pressure/altitude sensor
- GPS module for position tracking
- RFM95 LoRa radio for telemetry transmission
- 128x64 OLED display for status
- SD card logging with RTC timestamps
- Automatic flight phase detection

### 2. Ground Gateway Firmware (`/firmware_gateway/`)
C firmware for Adafruit Feather RP2040 with RFM95 LoRa Radio.

- LoRa to USB bridge
- BMP390 for ground pressure reference
- Calculates differential altitude (flight vs ground)
- Receives telemetry from flight computer
- Forwards to desktop application via USB serial
- Relays commands from desktop to flight computer

### 3. Desktop Application (`/desktop_app/`)
Xojo desktop application for macOS, Windows, and Linux.

- Real-time altitude/velocity visualization
- Flight control and monitoring
- Flight history database
- Data export (CSV, JSON)

### 4. iOS Application (`/ios_app/`)
Xojo mobile application for iPhone and iPad.

- Live telemetry display
- Flight history
- Post-flight analysis

## Hardware Components

### Flight Computer
| Component | Product ID | Function |
|-----------|------------|----------|
| Feather RP2040 + RFM95 LoRa 915MHz | 5714 | MCU + LoRa radio |
| BMP390 Barometric Sensor | 4816 | Altitude via STEMMA QT |
| GPS Module (UART) | varies | Position tracking |
| Adalogger FeatherWing | 2922 | RTC + SD card |
| FeatherWing OLED 128x64 | 4650 | Status display |
| Quad Side-By-Side FeatherWing Kit | 4254 | Mounting |

### Ground Station
| Component | Product ID | Function |
|-----------|------------|----------|
| Feather RP2040 + RFM95 LoRa 915MHz | 5714 | Gateway MCU + radio |
| BMP390 Barometric Sensor | 4816 | Ground pressure reference |

## Quick Start

### 1. Build Flight Firmware
```bash
cd firmware_flight
mkdir build && cd build
cmake ..
make -j4
```

### 2. Flash Firmware
Copy `rocket_avionics_flight.uf2` to the Feather in bootloader mode.

### 3. Build Gateway Firmware
```bash
cd firmware_gateway
mkdir build && cd build
cmake ..
make -j4
```

### 4. Flash Gateway
Copy `rocket_avionics_gateway.uf2` to the gateway Feather.

### 5. Connect Desktop Application
- Plug gateway into USB
- Open desktop application
- Select serial port
- Connect

## Flight Procedure

### Pre-Flight
1. Power on flight computer
2. Wait for GPS lock / sensor initialization
3. Verify telemetry reception on ground station
4. Press **ARM** when ready

### Launch
1. Flight computer automatically detects launch
2. Real-time telemetry displayed on ground station
3. Apogee automatically detected
4. Landing detected when stationary

### Post-Flight
1. Connect to flight computer (or download via LoRa)
2. Download high-resolution SD card data
3. Analyze flight profile in desktop app

## Flight States

| State | Description | Transition |
|-------|-------------|------------|
| IDLE | Pre-flight, on pad | ARM command |
| ARMED | Ready for launch | Altitude >10m or velocity >10m/s |
| BOOST | Motor burning | Acceleration <0 |
| COAST | Coasting to apogee | Velocity crosses zero |
| APOGEE | Peak altitude | Immediate |
| DESCENT | Falling under chute | Stationary for 5 seconds |
| LANDED | On ground | Download command |
| COMPLETE | Data downloaded | Reset |

## Directory Structure

```
rocket_avionics/
├── firmware_flight/        # Flight computer (C, Pico SDK)
│   ├── src/                # Source files
│   ├── include/            # Header files
│   └── CMakeLists.txt
├── firmware_gateway/       # Ground gateway (C, Pico SDK)
│   ├── src/
│   ├── include/
│   └── CMakeLists.txt
├── desktop_app/            # Xojo desktop application
├── ios_app/                # Xojo iOS application
├── docs/                   # Documentation
│   ├── PROTOCOL.md
│   ├── HARDWARE.md
│   └── ARCHITECTURE.md
└── sdk/                    # Symlink to Pico SDK
```

## Documentation

- [Hardware Setup](docs/HARDWARE.md)
- [Protocol Specification](docs/PROTOCOL.md)
- [Architecture Overview](docs/ARCHITECTURE.md)

## Future Enhancements

- Accelerometer integration (LSM6DS3 or ADXL345)
- Pyro channel control for ejection charges
- Dual deployment (drogue at apogee, main at altitude)
- Bluetooth LE gateway option for iOS direct connection

## License

Copyright (c) 2025-2026 by Mark Gavin. All Rights Reserved.

Proprietary software - not for distribution.
