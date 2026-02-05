# Rocket Avionics Hardware Guide

## Overview

The Rocket Avionics system consists of two main hardware assemblies:

1. **Flight Computer** - Mounted in the rocket
2. **Ground Gateway** - Connected to desktop computer via USB

Both use the same base board (Adafruit Feather RP2040 with RFM95 LoRa) for simplified development and interchangeable parts.

---

## Flight Computer Bill of Materials

| Component | Product ID | Quantity | Purpose |
|-----------|------------|----------|---------|
| Feather RP2040 + RFM95 LoRa 915MHz | 5714 | 1 | Main MCU + LoRa radio |
| BMP390 Barometric Sensor | 4816 | 1 | Altitude measurement (primary) |
| BMP581 Barometric Sensor | 5857 | 1 | Altitude measurement (secondary, higher accuracy) |
| LSM6DSOX + LIS3MDL 9-DoF IMU FeatherWing | 4565 | 1 | Accelerometer, gyroscope, magnetometer |
| GPS Module (UART, 9600 baud) | varies | 1 | Position tracking |
| FeatherWing OLED 128x64 | 4650 | 1 | Status display |
| Quad Side-By-Side FeatherWing Kit | 4254 | 1 | Mounting all boards |
| LiPo Battery 3.7V 500mAh+ | varies | 1 | Power |

### Sensors

The flight computer supports two barometric pressure sensors and two IMU variants:

**Barometers** (either or both may be installed):
- **BMP390** (I2C 0x77/0x76) -- Requires host-side compensation using 14 NVM calibration coefficients. Accuracy +/-3 Pa.
- **BMP581** (I2C 0x47/0x46) -- Pre-compensated output, no calibration needed. Accuracy +/-0.4 Pa. Chip may report ID 0x50 (BMP580 variant) or 0x51 (BMP581).

**IMUs** (one installed):
- **LSM6DSOX + LIS3MDL** -- 6-axis accel/gyro + 3-axis mag. +/-8g accel, +/-1000 dps gyro.
- **ICM-20649** -- 6-axis accel/gyro. +/-30g accel, +/-4000 dps gyro. Higher range for high-G flights.

The barometer and IMU data are fused using a complementary filter to produce altitude and velocity estimates. See [SENSORS.md](SENSORS.md) for details on the sensor fusion algorithm.

---

## Ground Gateway Bill of Materials

| Component | Product ID | Quantity | Purpose |
|-----------|------------|----------|---------|
| Feather RP2040 + RFM95 LoRa 915MHz | 5714 | 1 | Gateway MCU + LoRa radio |
| BMP390 Barometric Sensor | 4816 | 1 | Ground pressure reference |
| USB-C Cable | - | 1 | Connection to computer |

The gateway BMP390 provides ground-level pressure readings, enabling differential altitude calculation (height of flight computer above gateway) which is more accurate than absolute altitude from a single sensor.

---

## Pin Assignments

### Feather RP2040 + RFM95 LoRa

The RFM95 LoRa radio is built into the Feather and uses dedicated pins.

#### LoRa Radio (SPI0)

| Function | GPIO | Notes |
|----------|------|-------|
| SPI SCK | GP18 | SPI0 Clock |
| SPI MOSI | GP19 | SPI0 TX |
| SPI MISO | GP20 | SPI0 RX |
| LoRa CS | GP7 | Chip Select |
| LoRa Reset | GP11 | Radio Reset |
| LoRa DIO0 | GP6 | Interrupt |

#### I2C Bus (STEMMA QT)

| Function | GPIO | Notes |
|----------|------|-------|
| I2C SDA | GP2 | I2C1 Data |
| I2C SCL | GP3 | I2C1 Clock |

I2C Devices:
- BMP390 Barometer: 0x77 (or 0x76)
- BMP581 Barometer: 0x47 (or 0x46)
- LSM6DSOX Accel/Gyro: 0x6A (or 0x6B)
- LIS3MDL Magnetometer: 0x1C (or 0x1E)
- ICM-20649 Accel/Gyro: 0x68 (or 0x69)
- SSD1306 OLED: 0x3C

#### GPS Module (UART1) - Flight Computer Only

| Function | GPIO | Notes |
|----------|------|-------|
| UART TX | GP4 | To GPS RX |
| UART RX | GP5 | From GPS TX |

GPS Configuration:
- Baud Rate: 9600
- Protocol: NMEA 0183
- Sentences: GPRMC, GPGGA
- Update Rate: 1 Hz typical

#### SD Card (Adalogger FeatherWing)

| Function | GPIO | Notes |
|----------|------|-------|
| SD CS | GP10 | Chip Select |
| SD SCK | GP18 | Shared with LoRa |
| SD MOSI | GP19 | Shared with LoRa |
| SD MISO | GP20 | Shared with LoRa |

**Note:** SD card shares SPI0 with LoRa radio. Only one device can be active at a time - use CS pins to select.

#### OLED FeatherWing Buttons

| Button | GPIO | Function |
|--------|------|----------|
| A (Left) | GP9 | Cycle display modes |
| B (Middle) | GP8 | Arm/Disarm toggle |
| C (Right) | GP5 | Reserved |

Buttons are active LOW with internal pull-ups.

#### Status LED

| Function | GPIO | Notes |
|----------|------|-------|
| NeoPixel | GP16 | Built-in WS2812 |

#### Future Pyro Channels

| Function | GPIO | Notes |
|----------|------|-------|
| Pyro 1 | GP24 | Drogue (reserved) |
| Pyro 2 | GP25 | Main (reserved) |
| Pyro 1 Cont | GP26/A0 | Continuity ADC |
| Pyro 2 Cont | GP27/A1 | Continuity ADC |

---

## Assembly Instructions

### Flight Computer Assembly

1. **Install Headers on FeatherWing Boards**
   - Solder female headers to OLED FeatherWing
   - Solder female headers to Adalogger FeatherWing
   - Solder male headers to Quad Side-By-Side kit plates

2. **Assemble Quad FeatherWing**
   - Connect the four plates using standoffs
   - Install the Feather RP2040 + RFM95 in one slot
   - Install OLED FeatherWing in adjacent slot
   - Install Adalogger FeatherWing in another slot
   - Fourth slot available for future expansion (IMU, GPS)

3. **Connect BMP390 Sensor**
   - Use STEMMA QT cable from Feather to BMP390
   - BMP390 can be mounted externally for better pressure readings

4. **Connect Battery**
   - Use JST connector on Feather for LiPo battery
   - Battery should be 500mAh or larger for adequate flight time

### Ground Gateway Assembly

1. Simply connect the Feather RP2040 + RFM95 to computer via USB-C
2. No additional components required for basic operation
3. Optional: Add antenna extension for better range

---

## LoRa Antenna

Both flight computer and ground gateway use 915 MHz antennas.

### Antenna Options

1. **Wire Antenna** (included with Feather)
   - 1/4 wave: 82mm length
   - Simple, lightweight, included

2. **External Antenna**
   - SMA or u.FL connector (may require modification)
   - Better range and pattern
   - Recommended for ground station

### Antenna Placement

- **Flight Computer**: Mount antenna vertically if possible
- **Ground Station**: Use external antenna on tripod for best reception

---

## Power Requirements

### Flight Computer

| Component | Current (typical) | Current (peak) |
|-----------|-------------------|----------------|
| RP2040 | 25 mA | 100 mA |
| RFM95 LoRa TX | - | 120 mA |
| RFM95 LoRa RX | 12 mA | - |
| BMP390 | 0.7 mA | 3.4 mA |
| BMP581 | 0.5 mA | 2.5 mA |
| LSM6DSOX + LIS3MDL | 1.5 mA | 3.0 mA |
| OLED Display | 10 mA | 20 mA |
| GPS | 25 mA | 50 mA |
| **Total** | ~75 mA | ~300 mA |

With a 500 mAh battery, expect 6-8 hours of operation in idle mode, or several hours of active flight operations.

### Ground Gateway

Powered via USB from computer - no battery required.

---

## I2C Bus Scan

### Flight Computer

Expected devices on I2C bus (full sensor configuration):

```
   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- 1C -- -- --  <- LIS3MDL Mag
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- 3C -- -- --  <- OLED
40: -- -- -- -- -- -- -- 47 -- -- -- -- -- -- -- --  <- BMP581
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- 6A -- -- -- -- --  <- LSM6DSOX
70: -- -- -- -- -- -- -- 77 -- -- -- -- -- -- -- --  <- BMP390
```

Note: Not all devices may be present. The firmware auto-detects available sensors at startup and uses whatever is installed.

### Ground Gateway

Expected devices on I2C bus:

```
   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- 77 -- -- -- -- -- -- -- --  <- BMP390
```

---

## Wiring Diagram

### Flight Computer

```
                    ┌──────────────────────────────┐
                    │      Quad FeatherWing        │
  ┌─────────────────┼──────────────────────────────┼──────────┐
  │                 │                              │          │
  │  ┌──────────┐   │  ┌──────────┐  ┌──────────┐  │          │
  │  │ Feather  │   │  │   OLED   │  │  9-DoF   │  │          │
  │  │  RP2040  │   │  │FeatherW  │  │   IMU    │  │          │
  │  │ + RFM95  │   │  │          │  │FeatherW  │  │          │
  │  │          │   │  │[Display] │  │ LSM6DSOX │  │          │
  │  │  [LoRa]  │   │  │  A  B    │  │ LIS3MDL  │  │          │
  │  └────┬─────┘   │  └──────────┘  └──────────┘  │          │
  │       │         │                              │          │
  │       │ STEMMA QT (I2C daisy chain)            │          │
  │       │         │                              │          │
  │  ┌────┴─────┐   │  ┌──────────┐  ┌──────────┐  │          │
  │  │  BMP390  │   │  │  BMP581  │  │   GPS    │──┼─ UART1  │
  │  │ Baromet  │   │  │ Baromet  │  │  Module  │  │  GP4/5  │
  │  │  0x77    │   │  │  0x47    │  │          │  │          │
  │  └──────────┘   │  └──────────┘  └──────────┘  │          │
  └─────────────────┴──────────────────────────────┴──────────┘

  All I2C sensors connect via STEMMA QT daisy-chain on GP2 (SDA) / GP3 (SCL)
```

### Ground Gateway

```
  ┌─────────────────┐
  │ Feather RP2040  │
  │    + RFM95      │
  │                 │
  │    [LoRa]       │
  │                 │
  └────────┬────────┘
           │ STEMMA QT
           │
     ┌─────┴─────┐
     │  BMP390   │
     │ Baromet.  │
     │  Sensor   │
     └───────────┘
```

---

## Troubleshooting

### No Display on OLED

1. Check I2C connections (GP2/GP3)
2. Verify OLED address (0x3C or 0x3D)
3. Check that OLED FeatherWing is fully seated

### No Sensor Readings

1. Check STEMMA QT cable connection
2. Verify BMP390 address (0x77 or 0x76)
3. Run I2C scan to detect devices

### No LoRa Communication

1. Verify both units are set to 915 MHz
2. Check antenna connections
3. Ensure ground gateway is powered
4. Check for LoRa CS pin conflicts

### SD Card Not Working

1. Ensure SD card is FAT32 formatted
2. Check SD card is fully inserted
3. Verify CS pin (GP10) is correct
4. Note: Cannot use SD and LoRa simultaneously

### Altitude Reading Stuck or Wrong

1. Check BMP390 I2C connection via STEMMA QT
2. Verify BMP390 address (0x77 or 0x76)
3. BMP390 uses forced mode - readings update on each sample
4. At sea level, expect ~101,000 Pa pressure
5. If readings are ~85,000 Pa, sensor may not be initializing correctly

### GPS Not Getting Fix

1. GPS requires clear sky view - won't work indoors
2. Wait 30-60 seconds for initial fix (cold start)
3. Check UART connections (GP4 TX to GPS RX, GP5 RX from GPS TX)
4. Verify GPS module baud rate is 9600
5. Verify GPS module power (3.3V from Feather)
6. Check for valid NMEA sentences (GPRMC, GPGGA)

### Gateway Ground Pressure Not Working

1. Ensure BMP390 is connected to gateway via STEMMA QT
2. Check I2C address 0x77
3. Gateway firmware must be rebuilt with BMP390 support
4. Ground pressure enables differential altitude calculation
