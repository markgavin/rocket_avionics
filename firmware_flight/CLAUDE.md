# Flight Computer Firmware (RP2040)

## Overview

Rocket flight computer firmware for Adafruit Feather RP2040 with RFM95 LoRa Radio. Handles flight detection, altitude tracking, parachute deployment, telemetry transmission, and flight data logging.

## Hardware

- **Board:** Adafruit Feather RP2040 with RFM95 LoRa Radio - 915MHz (Product ID: 5714)
- **Barometer:** Adafruit BMP390 (Product ID: 4816) - altitude measurement
- **IMU:** Adafruit LSM6DSOX + LIS3MDL 9-DoF IMU FeatherWing (Product ID: 4565)
- **Display:** Adafruit FeatherWing OLED 128x64 (Product ID: 4650)
- **GPS:** Optional GPS module on UART0

## Project Structure

```
firmware_flight/
├── include/
│   ├── pins.h              # GPIO pin assignments
│   ├── version.h           # Version declarations
│   ├── flight_control.h    # Flight state machine
│   ├── lora_radio.h        # RFM95 LoRa driver
│   ├── bmp390.h            # Barometric sensor driver
│   ├── imu.h               # LSM6DSOX + LIS3MDL driver
│   ├── storage.h           # Flash storage low-level
│   ├── flight_storage.h    # Flight data management
│   ├── status_display.h    # OLED display
│   ├── ssd1306.h           # Display driver
│   ├── gps.h               # GPS NMEA parser
│   └── heartbeat_led.h     # Status LED patterns
├── src/
│   ├── main.c              # Main application
│   ├── version.c           # Build timestamp (force-rebuilt)
│   ├── flight_control.c    # State machine logic
│   ├── lora_radio.c        # LoRa communication
│   ├── bmp390.c            # Barometer I2C driver
│   ├── imu.c               # IMU I2C driver
│   ├── storage.c           # Flash read/write
│   ├── flight_storage.c    # Flight log management
│   ├── status_display.c    # Display screens
│   ├── ssd1306.c           # SSD1306 driver
│   ├── gps.c               # NMEA parsing
│   ├── heartbeat_led.c     # LED patterns
│   ├── base64.c            # Base64 encoding
│   └── ws2812.pio          # NeoPixel PIO program
├── build/                  # CMake build directory
└── CMakeLists.txt          # Build configuration
```

## Build

```bash
mkdir -p build && cd build
cmake ..
make -j4
# Output: build/rocket_avionics_flight.uf2
```

## Flight State Machine

```
PAD → BOOST → COAST → APOGEE → DROGUE → MAIN → LANDED
```

| State | Entry Condition | Actions |
|-------|-----------------|---------|
| PAD | Power on | Calibrate sensors, wait for launch |
| BOOST | Acceleration > threshold | Record data at high rate |
| COAST | Acceleration drops | Continue recording |
| APOGEE | Velocity crosses zero | Fire drogue pyro |
| DROGUE | Drogue deployed | Descend under drogue |
| MAIN | Below main altitude | Fire main pyro |
| LANDED | Low velocity, stable | Stop recording, beacon |

## Pyro Channels

| Channel | GPIO Fire | GPIO Continuity | Purpose |
|---------|-----------|-----------------|---------|
| Pyro 1 | GP24 | GP26 (ADC) | Drogue deployment |
| Pyro 2 | GP25 | GP27 (ADC) | Main deployment |

**Safety:** Arm switch (GP10) must be closed to enable pyro firing.

## Telemetry Format

JSON transmitted at 10 Hz over LoRa:
```json
{
  "type": "tel",
  "state": "BOOST",
  "alt": 152.3,
  "vel": 45.2,
  "acc": 12.5,
  "max_alt": 152.3,
  "lat": 38.897,
  "lon": -77.036,
  "pyro1": true,
  "pyro2": true
}
```

## Flash Storage

- Flight data stored in RP2040 flash (after program space)
- Multiple flight slots
- Binary format for efficiency
- Downloadable via LoRa commands

### Storage Commands

| Command | Description |
|---------|-------------|
| `flash_list` | List stored flights |
| `flash_read` | Download flight data |
| `flash_delete` | Delete flight |
| `flash_delete_all` | Clear all flights |

## Configuration

### pins.h Key Settings

```c
#define kPinPyro1Fire       24  // Drogue MOSFET gate
#define kPinPyro2Fire       25  // Main MOSFET gate
#define kPinArmSwitch       10  // Arm switch input
#define kMainDeployAltitude 300 // Main chute altitude (meters)
```

### LoRa Settings (must match gateway)

```c
#define kLoRaFrequency      915000000
#define kLoRaSpreadFactor   7
#define kLoRaBandwidth      125000
#define kLoRaSyncWord       0x14
```

## Display Modes

| Mode | Content |
|------|---------|
| Status | Flight state, altitude, velocity |
| Sensors | Barometer, IMU readings |
| Pyro | Continuity status, arm state |
| Storage | Flight count, storage used |
| About | Version and build info |

## Coding Conventions

- Variables: `sStatic`, `inParam`, `theLocal`
- Constants: `kConstantName`
- Functions: `ModuleName_FunctionName()`
- Spaces before semicolons
- 2-space indentation
