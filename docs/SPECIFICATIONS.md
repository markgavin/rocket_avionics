# System Specifications

## Flight Computer

| Parameter | Value |
|-----------|-------|
| MCU | RP2040 (Dual Cortex-M0+, 133 MHz) |
| Board | Adafruit Feather RP2040 with RFM95 LoRa (PID 5714) |
| Flash | 8 MB (QSPI) |
| RAM | 264 KB |
| Main Loop Rate | 1 kHz (1 ms period) |
| Supply Voltage | 3.3V (USB or LiPo via onboard regulator) |

## Sensor Specifications

### BMP390 Barometer (Primary)

| Parameter | Value |
|-----------|-------|
| I2C Address | 0x77 |
| Pressure Range | 300-1250 hPa |
| Pressure Accuracy | +/- 0.5 hPa |
| Pressure Resolution | 0.016 Pa (24-bit) |
| Temperature Range | -40 to +85 C |
| Temperature Accuracy | +/- 0.5 C |
| Sample Rate | 100 Hz (configured: 50 Hz ODR, oversampled to 100 Hz read) |
| Oversampling | Pressure 8x, Temperature 2x |
| IIR Filter | Coefficient 3 |
| Measured Noise | ~0.05m altitude std (at ground level) |

### BMP581 Barometer (Secondary)

| Parameter | Value |
|-----------|-------|
| I2C Address | 0x47 (default) or 0x46 (alt) |
| Chip ID | 0x50 (BMP580 variant) |
| Pressure Range | 300-1250 hPa |
| Output Format | Pre-compensated (P/64 Pa, T/65536 C) |
| Measured Offset vs BMP390 | +8 Pa pressure, -0.33 C temperature |
| Measured Drift | None detected over 2+ minutes |

### ICM-20649 IMU (Primary)

| Parameter | Value |
|-----------|-------|
| I2C Address | 0x68 (default) or 0x69 (alt) |
| WHO_AM_I | 0xE1 |
| Accel Range | +/- 4g, 8g, 16g, 30g (configured: 8g) |
| Accel Sensitivity | 4096 LSB/g at 8g |
| Accel DLPF | ~50 Hz 3dB bandwidth |
| Accel Sample Rate | 1.125 kHz native, read at 100 Hz |
| Gyro Range | +/- 500, 1000, 2000, 4000 dps (configured: 1000 dps) |
| Gyro Sensitivity | 32.8 LSB/dps at 1000 dps |
| Gyro DLPF | ~51 Hz 3dB bandwidth |
| Gyro Sample Rate | 1.1 kHz native, read at 100 Hz |
| Advantage | 30g max range for high-G flights |

### LSM6DSOX IMU (Fallback)

| Parameter | Value |
|-----------|-------|
| I2C Address | 0x6A |
| WHO_AM_I | 0x6C |
| Accel Range | +/- 2g, 4g, 8g, 16g (configured: 8g) |
| Accel Sensitivity | 0.244 mg/LSB at 8g |
| Accel ODR | 416 Hz |
| Gyro Range | +/- 125, 250, 500, 1000, 2000 dps (configured: 1000 dps) |
| Gyro Sensitivity | 35 mdps/LSB at 1000 dps |
| Gyro ODR | 416 Hz |
| Advantage | Higher precision at lower ranges |

### LIS3MDL Magnetometer

| Parameter | Value |
|-----------|-------|
| I2C Address | 0x1E (default) or 0x1C (alt) |
| WHO_AM_I | 0x3D |
| Range | +/- 4, 8, 12, 16 gauss (configured: 4 gauss) |
| Sensitivity | 6842 LSB/gauss at 4 gauss |
| ODR | 80 Hz (ultra-high performance mode) |
| Performance Mode | Ultra-high (XY + Z axes) |

### PA1616D GPS

| Parameter | Value |
|-----------|-------|
| Interface | UART0, 9600 baud |
| Update Rate | 1 Hz |
| Channels | 22 tracking, 66 searching |
| Sensitivity | -165 dBm tracking |
| Position Accuracy | 3.0m CEP |
| TTFF (Cold) | ~30 seconds |
| TTFF (Hot) | ~1 second |

## Communication

### LoRa Radio (RFM95W)

| Parameter | Value |
|-----------|-------|
| Frequency | 915 MHz (ISM band, North America) |
| Spreading Factor | SF7 |
| Bandwidth | 125 kHz |
| Coding Rate | 4/5 |
| TX Power | 20 dBm (100 mW) |
| Sync Word | 0x14 |
| Preamble | 8 symbols |
| Data Rate | ~5.5 kbps (SF7, 125 kHz) |
| Range | ~1 km line of sight (typical) |
| SPI Bus | SPI1 at 1 MHz |

### Telemetry Packet

| Parameter | Value |
|-----------|-------|
| Packet Size | 55 bytes (LoRaTelemetryPacket) |
| Magic Byte | 0xAF |
| CRC | CRC-8 (polynomial 0x31, init 0xFF) |
| Max Payload | ~255 bytes (LoRa limit) |

## Timing

| Parameter | Value | Rate |
|-----------|-------|------|
| Main Loop | 1 ms | 1 kHz |
| Sensor Sampling | 10 ms | 100 Hz |
| IMU Sampling | 10 ms | 100 Hz |
| Telemetry TX (flight) | 100 ms | 10 Hz |
| Telemetry TX (idle) | 2000 ms | 0.5 Hz |
| Telemetry TX (armed) | 500 ms | 2 Hz |
| Display Update | 200 ms | 5 Hz |
| GPS Update | 1000 ms | 1 Hz |
| Flash Logging | 100 ms | 10 Hz |
| LoRa Timeout | 5000 ms | - |
| Orientation Mode Timeout | 30000 ms | - |

## Flash Storage

| Parameter | Value |
|-----------|-------|
| Total Flash | 8 MB |
| Storage Reserved | 512 KB |
| Flight Slots | 7 |
| Slot Size | 64 KB |
| Sample Size | 52 bytes |
| Max Samples/Flight | 1,200 |
| Max Recording Time | 120 seconds |
| Header Size | 80 bytes |
| Index Magic | 0x58444E49 ("INDX") |
| Flight Magic | 0x54484746 ("FGHT") |

## Display

| Parameter | Value |
|-----------|-------|
| Type | SSD1306 OLED |
| Resolution | 128 x 64 pixels |
| Interface | I2C at 0x3C |
| Update Rate | 5 Hz |
| Screens | 12 modes (Live, Device Info, Flight Stats, LoRa Status, Sensors, GPS, IMU, Spin, Compass, Rates, Rocket ID, About) |

## Buttons

| Button | GPIO | Short Press | Location |
|--------|------|-------------|----------|
| A | GP9 | Previous display mode | Left |
| B | GP6 | Home / Edit Rocket ID | Middle |
| C | GP5 | Next display mode | Right |

| Parameter | Value |
|-----------|-------|
| Debounce | 50 ms |
| Long Press | 1000 ms |
| Pull-up | Internal (active low) |

## LED Patterns (NeoPixel)

| State | Color | Period |
|-------|-------|--------|
| IDLE | Green | 1000 ms (1 Hz) |
| ARMED | Yellow | 500 ms (2 Hz) |
| BOOST | Red | 100 ms (10 Hz) |
| DESCENT | Blue | 250 ms (4 Hz) |
| LANDED | White | 2000 ms (0.5 Hz) |

## Sensor Fusion

| Parameter | Value |
|-----------|-------|
| Filter Type | Complementary (2nd order) |
| Crossover Frequency | ~0.5 Hz |
| Altitude Gain (Ka) | 6.0 |
| Velocity Gain (Kv) | 10.0 |
| Bias Learning Rate (Kb) | 1.0 |
| Max Integration dt | 50 ms (clamp) |
| Altitude Smoothing Alpha | 0.1 (~2 Hz cutoff at 100 Hz) |
| Velocity Smoothing Alpha | 0.15 (baro-only fallback) |
| Vertical Axis | Z (accelZ - 1g) |

## I2C Bus

| Parameter | Value |
|-----------|-------|
| Port | I2C1 |
| SDA | GP2 |
| SCL | GP3 |
| Speed | 400 kHz |
| Pull-ups | Internal GPIO pull-ups |

### I2C Device Map

| Address | Device | Function |
|---------|--------|----------|
| 0x3C | SSD1306 | OLED display |
| 0x47 | BMP581 | Barometer (secondary) |
| 0x68 | ICM-20649 | Accel/gyro (primary IMU) |
| 0x6A | LSM6DSOX | Accel/gyro (fallback IMU) |
| 0x77 | BMP390 | Barometer (primary) |
| 0x1E | LIS3MDL | Magnetometer |

## Pyro Channels (Future)

| Channel | Fire GPIO | Continuity GPIO | Purpose |
|---------|-----------|-----------------|---------|
| Pyro 1 | GP24 | GP26 (ADC) | Drogue deployment |
| Pyro 2 | GP25 | GP27 (ADC) | Main deployment |

## Multi-Rocket Support

| Parameter | Value |
|-----------|-------|
| Max Rockets | 16 (IDs 0-15) |
| Broadcast ID | 0xFF |
| ID Storage | Flash (persists across reboots) |
| Name Storage | Flash (null-terminated, configurable via LoRa) |
