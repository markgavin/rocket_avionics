# Heltec Flight Computer

Rocket flight computer firmware for Heltec Wireless Tracker with external BMP390 and IMU sensors.

## Features

- Real-time telemetry at 10Hz via LoRa
- GPS tracking with built-in module
- 9-DoF IMU (accelerometer, gyroscope, magnetometer)
- Barometric altitude measurement
- Dual parachute deployment (drogue + main)
- View-only TFT display
- ~47% cost savings vs RP2040 version

## Hardware Required

| Component | Description | Est. Price |
|-----------|-------------|------------|
| Heltec Wireless Tracker | ESP32-S3 + SX1262 + GPS + Display | $27 |
| Adafruit BMP390 | Barometric sensor (STEMMA QT) | $12.50 |
| Adafruit LSM6DSOX + LIS3MDL | 9-DoF IMU FeatherWing | $19.95 |
| LiPo Battery 3.7V | 500-1000mAh | $8 |
| **Total** | | **~$67** |

Plus pyro channel components (~$7).

## Wiring

### I2C Sensors

Solder wires from Heltec edge connector to sensors:

| Heltec GPIO | Connect To |
|-------------|------------|
| GPIO 5 | SDA (BMP390 + IMU) |
| GPIO 6 | SCL (BMP390 + IMU) |
| 3.3V | VIN (sensors) |
| GND | GND (sensors) |

### Pyro Channels

| Heltec GPIO | Connect To |
|-------------|------------|
| GPIO 7 | Pyro 1 MOSFET gate (drogue) |
| GPIO 45 | Pyro 2 MOSFET gate (main) |
| GPIO 2 | Pyro 1 continuity divider |
| GPIO 4 | Pyro 2 continuity divider |
| GPIO 46 | Arm switch (to GND when armed) |

## Arduino IDE Setup

1. **Add Heltec board URL** to File → Preferences → Additional Board Manager URLs:
   ```
   https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/releases/download/0.0.9/package_heltec_esp32_index.json
   ```

2. **Install board**: Tools → Board → Board Manager → Search "Heltec ESP32" → Install

3. **Install libraries** via Library Manager:
   - RadioLib
   - TinyGPSPlus
   - Adafruit GFX Library
   - Adafruit ST7735 and ST7789 Library

4. **Select board**: Tools → Board → Heltec ESP32 → Heltec Wireless Tracker

5. **Upload speed**: 921600

## Building and Flashing

```bash
# Increment build number
./increment_build.sh

# Compile and upload via Arduino IDE
# Or use arduino-cli:
arduino-cli compile --fqbn "Heltec-esp32:esp32:heltec_wireless_tracker" .
arduino-cli upload --fqbn "Heltec-esp32:esp32:heltec_wireless_tracker" -p /dev/cu.usbserial-* .
```

## Flight Procedure

### Pre-Flight

1. Power on flight computer
2. Verify display shows "IDLE" state
3. Wait for GPS lock (green "FIX" on display)
4. Verify pyro continuity (P1:OK, P2:OK)
5. Close arm switch
6. Verify ARM:YES on display
7. Send ARM command from ground station

### Launch

1. Flight computer automatically detects launch (>2g accel or >10m altitude)
2. Real-time telemetry transmitted to gateway
3. Apogee detected → drogue deployed
4. Main altitude reached → main deployed
5. Landing detected → beacon mode

### Post-Flight

1. Open arm switch first!
2. Download flight data via LoRa
3. Power off

## Display Screens

The display shows (no buttons, view-only):

- **Line 1**: Flight state + Rocket ID
- **Line 2**: Altitude + Velocity
- **Line 3**: Max altitude + Acceleration
- **Line 4**: GPS status
- **Line 5**: Pyro continuity + Arm status
- **Line 6**: TX count + Temperature

## Troubleshooting

**Sensors not found**
- Check solder connections on GPIO 5 and 6
- Verify 3.3V power to sensors
- Run I2C scanner sketch to find addresses

**No GPS fix**
- Go outdoors with clear sky view
- Wait 1-2 minutes for cold start
- Built-in antenna is weak but works outdoors

**LoRa not working**
- Check antenna is connected
- Verify gateway is running
- Check sync word matches (0x14)

## License

Copyright (c) 2025-2026 by Mark Gavin. All Rights Reserved.
