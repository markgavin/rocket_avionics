# Heltec Flight Computer Firmware (ESP32-S3)

## Overview

Rocket flight computer firmware for Heltec Wireless Tracker. A lower-cost alternative to the RP2040 flight computer with built-in GPS and display.

## Hardware

- **Board:** Heltec Wireless Tracker (ESP32-S3 + SX1262 LoRa + GPS)
- **Barometer:** Adafruit BMP390 (external, I2C)
- **IMU:** Auto-detected, supports either:
  - **ICM-20649** (recommended) - ±30g, ±4000dps - better for high-g rockets
  - **LSM6DSOX + LIS3MDL** - ±16g, ±2000dps - includes magnetometer
- **Display:** Built-in 0.96" TFT (ST7735, 160x80)
- **GPS:** Built-in UC6580 module
- **LoRa:** Built-in SX1262 (915 MHz)

## Pin Configuration

### External I2C Sensors (solder wires to edge connector)

| Function | GPIO | Notes |
|----------|------|-------|
| I2C SDA | 5 | BMP390 + IMU |
| I2C SCL | 6 | BMP390 + IMU |

### Pyro Channels

| Function | GPIO | Notes |
|----------|------|-------|
| Pyro 1 Fire | 7 | Drogue MOSFET gate |
| Pyro 2 Fire | 45 | Main MOSFET gate |
| Pyro 1 Continuity | 2 | ADC input |
| Pyro 2 Continuity | 4 | ADC input |
| Arm Switch | 46 | Active low, internal pullup |

### Built-in Peripherals

| Function | GPIO | Notes |
|----------|------|-------|
| LoRa NSS | 8 | SX1262 chip select |
| LoRa DIO1 | 14 | Interrupt |
| LoRa RST | 12 | Reset |
| LoRa BUSY | 13 | Busy signal |
| GPS RX | 33 | UC6580 UART |
| GPS TX | 34 | UC6580 UART |
| TFT CS | 38 | Display |
| TFT DC | 40 | Display |
| TFT RST | 39 | Display |
| TFT BL | 21 | Backlight PWM |
| VEXT | 3 | Power control for GPS/display |

## Project Structure

```
firmware_flight_heltec/
├── firmware_flight_heltec.ino  # Main sketch (all code)
├── version.h                   # Version information
├── build_number.h              # Auto-generated build number
├── build_number.txt            # Persistent counter
├── increment_build.sh          # Build number script
├── CLAUDE.md                   # This file
└── README.md                   # Setup instructions
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
   - **Adafruit GFX Library**
   - **Adafruit ST7735 and ST7789 Library**

4. Select board: **Tools → Board → Heltec Wireless Tracker**

5. Upload speed: 921600

## Flight States

```
IDLE → ARMED → BOOST → COAST → APOGEE → DROGUE → MAIN → LANDED
```

| State | Entry Condition | Actions |
|-------|-----------------|---------|
| IDLE | Power on | Wait for ARM command |
| ARMED | ARM command received | Detect launch |
| BOOST | Acceleration > 2g or altitude > 10m | Motor burning |
| COAST | Acceleration drops | Coasting to apogee |
| APOGEE | Velocity crosses zero | Fire drogue (Pyro 1) |
| DROGUE | Drogue deployed | Descending on drogue |
| MAIN | Below 150m altitude | Fire main (Pyro 2) |
| LANDED | Velocity < 2m/s for 5s | Beacon mode |

## Telemetry Format

55-byte binary packet at 10Hz:

| Field | Offset | Size | Description |
|-------|--------|------|-------------|
| magic | 0 | 1 | 0xAA |
| type | 1 | 1 | 0x01 = telemetry |
| rocketId | 2 | 1 | Rocket ID (0-15) |
| sequence | 3 | 2 | Packet counter |
| state | 5 | 1 | Flight state |
| altitudeCm | 6 | 4 | Altitude in cm |
| velocityCmS | 10 | 2 | Velocity in cm/s |
| accelMg | 12 | 2 | Acceleration in milli-g |
| maxAltCm | 14 | 4 | Max altitude in cm |
| latitude | 18 | 4 | Lat * 1e7 |
| longitude | 22 | 4 | Lon * 1e7 |
| gpsSats | 26 | 1 | Satellites |
| gpsValid | 27 | 1 | GPS fix valid |
| gyroX/Y/Z | 28 | 6 | Gyro data |
| magX/Y/Z | 34 | 6 | Magnetometer |
| pyro1Cont | 40 | 1 | Pyro 1 continuity |
| pyro2Cont | 41 | 1 | Pyro 2 continuity |
| armed | 42 | 1 | Arm switch state |
| tempC10 | 43 | 2 | Temperature * 10 |
| battMv | 45 | 2 | Battery mV |
| checksum | 47 | 1 | XOR checksum |

## LoRa Configuration

Must match gateway settings:

```cpp
#define LORA_FREQUENCY      915.0   // MHz
#define LORA_BANDWIDTH      125.0   // kHz
#define LORA_SPREAD_FACTOR  7
#define LORA_CODING_RATE    5       // 4/5
#define LORA_SYNC_WORD      0x14    // Private sync word
```

## Commands (via LoRa)

| Command | ID | Description |
|---------|-----|-------------|
| ARM | 0x01 | Transition IDLE → ARMED |
| DISARM | 0x02 | Transition ARMED → IDLE |
| SET_ID | 0x10 | Set rocket ID (0-15) |

## Display Content

```
┌─────────────────────────┐
│ STATE: IDLE          R0│  State + Rocket ID
│ ALT: 0.0m  VEL: 0.0m/s │  Altitude + Velocity
│ MAX: 0.0m  ACC: 1.0g   │  Max altitude + Accel
│ GPS: 0 sats  NO FIX    │  GPS status
│ P1:OK P2:OK ARM:NO     │  Pyro + Arm status
│ TX:123  T:25.0C        │  LoRa count + Temp
└─────────────────────────┘
```

## Configuration

Settings stored in ESP32 NVS flash:

- **rocketId** (0-15): Unique rocket identifier
- **rocketName** (16 chars): Display name

Configure via:
1. LoRa command from gateway
2. Serial commands (future)
3. WiFi AP mode (future)

## Cost Comparison

| Component | RP2040 Flight | Heltec Flight |
|-----------|---------------|---------------|
| MCU + LoRa | $24.95 | $26.90 (includes GPS, display) |
| BMP390 | $12.50 | $12.50 |
| IMU | $19.95 | $19.95 |
| Display | $14.95 | Built-in |
| GPS | $29.95 | Built-in |
| Stacking | $8.50 | N/A |
| **Total** | **~$111** | **~$59** |

**Savings: ~$52 per flight computer (47%)**

## Troubleshooting

### Sensors not detected
- Check I2C wiring (SDA to GPIO5, SCL to GPIO6)
- Verify 3.3V power to sensors
- Check I2C addresses (BMP390: 0x77, LSM6DSOX: 0x6A, LIS3MDL: 0x1E)

### No GPS fix
- Built-in GPS antenna is weak
- Move outdoors with clear sky view
- Wait 1-2 minutes for cold start

### LoRa not transmitting
- Check antenna connection
- Verify frequency matches gateway (915 MHz)
- Check sync word matches (0x14)

### Pyro not firing
- Check arm switch is closed (GPIO46 LOW)
- Verify continuity (ADC reads > 1000)
- Check MOSFET wiring

## Safety Notes

1. **Never connect e-matches until ready to fly**
2. **Arm switch must be OFF during assembly**
3. **Test continuity detection before each flight**
4. **Verify GPS fix before arming**
5. **Keep battery charged (3.7V nominal)**

## Claude Code Notes

**WARNING: Do not attempt to read serial output from this board using cat, screen, or similar commands.** The USB serial connection on ESP32-S3 can cause sessions to hang.

For debugging:
- Use the TFT display to verify operation
- Use Arduino IDE Serial Monitor
- Connect via WiFi TCP (if implemented)
