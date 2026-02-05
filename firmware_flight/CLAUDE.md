# Flight Computer Firmware (RP2040)

## Overview

Rocket flight computer firmware for Adafruit Feather RP2040 with RFM95 LoRa Radio. Handles flight detection, altitude tracking, parachute deployment, telemetry transmission, and flight data logging.

## Hardware

- **Board:** Adafruit Feather RP2040 with RFM95 LoRa Radio - 915MHz (Product ID: 5714)
- **Barometers:** BMP390 (Product ID: 4816) and/or BMP581 (Product ID: 5857) - auto-detected
- **IMU:** LSM6DSOX + LIS3MDL 9-DoF (Product ID: 4565) or ICM-20649
- **Sensor Fusion:** Complementary filter fuses baro + IMU for altitude/velocity (see `docs/SENSORS.md`)
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
│   ├── bmp390.h            # BMP390 barometric sensor driver
│   ├── bmp581.h            # BMP581 barometric sensor driver
│   ├── imu.h               # LSM6DSOX + LIS3MDL / ICM-20649 driver
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
│   ├── bmp390.c            # BMP390 barometer I2C driver
│   ├── bmp581.c            # BMP581 barometer I2C driver
│   ├── imu.c               # IMU I2C driver (LSM6DSOX + LIS3MDL / ICM-20649)
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

## Flashing

### Single Device
```bash
picotool load build/rocket_avionics_flight.uf2 -f
```

### Multiple Devices Connected
When multiple RP2040 devices are connected, specify the bus and address:
```bash
# List connected devices
picotool info

# Flash specific device (check bus/address from picotool info)
picotool load build/rocket_avionics_flight.uf2 -f --bus 0 --address 5
picotool load build/rocket_avionics_flight.uf2 -f --bus 0 --address 6
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

Binary packets transmitted at 10 Hz over LoRa, decoded to JSON by gateway:
```json
{
  "type": "tel",
  "id": 0,
  "state": "BOOST",
  "alt": 152.3,
  "vel": 45.2,
  "acc": 12.5,
  "max_alt": 152.3,
  "lat": 38.897,
  "lon": -77.036,
  "sats": 8,
  "pyro1": true,
  "pyro2": true
}
```

Key fields:
- `id`: Rocket ID (0-15) for multi-rocket tracking
- `sats`: Number of GPS satellites in view
- `lat`/`lon`: GPS coordinates (valid when `sats` > 0)

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
| Live | Flight state, altitude, velocity (home screen) |
| Sensors | Barometer, IMU readings |
| Pyro | Continuity status, arm state |
| Storage | Flight count, storage used |
| Rocket ID | Rocket name/ID (editable) |
| About | Version and build info |

## Button Navigation

The display has two buttons (A and B) for navigation:

### Button A - Cycle Screens
Cycles through display modes: Live → Sensors → Pyro → Storage → Rocket ID → About → Live...

### Button B - Context-Dependent
- **On Rocket ID screen:** Cycles rocket ID from 0-15
- **On all other screens:** Returns to Live (home) screen

## Rocket ID Configuration

Each flight computer has a unique rocket ID (0-15) for multi-rocket support:

- **Default:** ID 0
- **Editing:** Navigate to Rocket ID screen, press Button B to cycle through IDs
- **Storage:** Saved to flash memory, persists across power cycles
- **Display:** Shows on Rocket ID screen and in telemetry packets

The rocket ID is included in all telemetry packets, allowing the gateway and apps to track multiple rockets simultaneously.

## Coding Conventions

- Variables: `sStatic`, `inParam`, `theLocal`
- Constants: `kConstantName`
- Functions: `ModuleName_FunctionName()`
- Spaces before semicolons
- 2-space indentation
