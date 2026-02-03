# Heltec Flight Computer Parts List

Lower-cost flight computer using Heltec Wireless Tracker with external sensors.

## Core Components

| Component | Description | Price | Link |
|-----------|-------------|-------|------|
| Heltec Wireless Tracker | ESP32-S3 + SX1262 + GPS + Display | $26.90 | [heltec.org](https://heltec.org/project/wireless-tracker/) |
| **Adafruit BMP581** | High-accuracy barometer ±3.3cm (Recommended) | $9.95 | [adafruit.com/product/5857](https://www.adafruit.com/product/5857) |
| **Adafruit ICM-20649** | Wide-range IMU ±30g, ±4000dps (Recommended) | $14.95 | [adafruit.com/product/4464](https://www.adafruit.com/product/4464) |
| LiPo Battery 3.7V 500mAh | Power source | $7.95 | [adafruit.com/product/258](https://www.adafruit.com/product/258) |

**Core Components Subtotal:** ~$60

## Why BMP581?

The BMP581 is the recommended barometer for rocket flight computers:

| Spec | BMP581 | BMP390 |
|------|--------|--------|
| Accuracy | **±0.4 Pa (±3.3cm)** | ±3 Pa (±25cm) |
| Max ODR | **240 Hz** | 200 Hz |
| Noise | **1 cm** | 10 cm |
| Price | **$9.95** | $12.50 |

**Benefits:**
- **7.5x better altitude accuracy** - Critical for precise apogee detection
- **Lower noise** - Cleaner velocity calculations for deployment timing
- **Faster sampling** - Better response during rapid altitude changes
- **Cheaper** - Saves $2.55 per flight computer
- **Newer technology** - Capacitive sensing vs piezo-resistive

### Alternative: BMP390

If you already have BMP390 sensors:
- [Adafruit BMP390](https://www.adafruit.com/product/4816) - $12.50
- Firmware auto-detects and supports both sensors
- Still good performance for most rockets

## Why ICM-20649?

The ICM-20649 is the recommended IMU for rocket flight computers:

| Spec | ICM-20649 | LSM6DSOX |
|------|-----------|----------|
| Accelerometer | **±30g** | ±16g |
| Gyroscope | **±4000 dps** | ±2000 dps |
| Price | **$14.95** | $19.95 |
| Magnetometer | No | Yes (with LIS3MDL) |

**Benefits:**
- **Won't clip during boost** - High-thrust motors (E-I class) can produce 20-30g
- **Handles fast spins** - If fins are canted or rocket tumbles
- **Cheaper** - Saves $5 per flight computer
- **Magnetometer not needed** - Parachute deployment is altitude-based, not heading-based

### Alternative: LSM6DSOX + LIS3MDL

If you need magnetometer data for heading/orientation:
- [LSM6DSOX + LIS3MDL 9-DoF](https://www.adafruit.com/product/4517) - $19.95
- Only use for low-power motors (A-D class) where g-forces stay under 16g

## Pyro Channel Components

| Component | Description | Qty | Est. Price |
|-----------|-------------|-----|------------|
| IRLZ44N N-Channel MOSFET | Logic-level MOSFET for pyro firing | 2 | $2.00 |
| 1kΩ Resistor (1/4W) | MOSFET gate resistor | 2 | $0.20 |
| 10kΩ Resistor (1/4W) | Gate pull-down resistor | 2 | $0.20 |
| 2-Position Screw Terminal (5mm) | E-match connection | 2 | $1.50 |
| SPST Toggle Switch | Arm safety switch | 1 | $2.00 |

**Pyro Components Subtotal:** ~$6

## Wiring Supplies

| Component | Description | Est. Price |
|-----------|-------------|------------|
| 26 AWG silicone wire | I2C and pyro connections | $5.00 |
| Heat shrink tubing | Wire protection | $3.00 |
| Solder | For Heltec edge connections | - |

**Wiring Subtotal:** ~$8

## Total Cost

| Category | Cost |
|----------|------|
| Core Components (BMP581 + ICM-20649) | ~$60 |
| Pyro Components | ~$6 |
| Wiring Supplies | ~$8 |
| **Total** | **~$74** |

## Cost Comparison

| Item | RP2040 Flight Computer | Heltec Flight Computer | Savings |
|------|------------------------|------------------------|---------|
| MCU + LoRa | $24.95 | $26.90 | - |
| GPS | $29.95 | Built-in | $29.95 |
| Display | $14.95 | Built-in | $14.95 |
| IMU | $19.95 | $14.95 (ICM-20649) | $5.00 |
| Barometer | $12.50 | $9.95 (BMP581) | $2.55 |
| Stacking board | $8.50 | N/A | $8.50 |
| Battery | $7.95 | $7.95 | - |
| Pyro | $6.00 | $6.00 | - |
| Wiring | $0 | $8.00 | -$8.00 |
| **Total** | **~$125** | **~$74** | **~$51 (41%)** |

## Wiring Diagram

```
Heltec Wireless Tracker (edge pads)
    │
    ├── GPIO 5 (SDA) ────┬──── BMP581 SDA
    │                    └──── ICM-20649 SDA
    │
    ├── GPIO 6 (SCL) ────┬──── BMP581 SCL
    │                    └──── ICM-20649 SCL
    │
    ├── GPIO 7 ──[1kΩ]──┬── MOSFET 1 Gate (Drogue)
    │                   │
    │                [10kΩ]
    │                   │
    │                  GND
    │
    ├── GPIO 45 ──[1kΩ]──┬── MOSFET 2 Gate (Main)
    │                    │
    │                 [10kΩ]
    │                    │
    │                   GND
    │
    ├── GPIO 2 ──── Pyro 1 Continuity Divider
    │
    ├── GPIO 4 ──── Pyro 2 Continuity Divider
    │
    ├── GPIO 46 ──── Arm Switch ──── GND
    │
    ├── 3.3V ────┬──── BMP581 VIN
    │            └──── ICM-20649 VIN
    │
    └── GND ─────┬──── BMP581 GND
                 └──── ICM-20649 GND
```

## Sensor I2C Addresses

Recommended sensors:
| Sensor | Address | Function |
|--------|---------|----------|
| BMP581 | 0x47 | Barometric pressure (±3.3cm accuracy) |
| ICM-20649 | 0x68 | Accelerometer + Gyroscope (±30g, ±4000dps) |

Alternative sensors (auto-detected):
| Sensor | Address | Function |
|--------|---------|----------|
| BMP390 | 0x77 | Barometric pressure (±25cm accuracy) |
| LSM6DSOX | 0x6A | Accelerometer + Gyroscope (±16g, ±2000dps) |
| LIS3MDL | 0x1E | Magnetometer |

All sensors share the same I2C bus (GPIO 5/6). Firmware auto-detects which sensors are connected.

## Assembly Notes

1. **Solder I2C wires to Heltec edge pads** (GPIO 5, 6, 3.3V, GND)
2. **Connect BMP581 via STEMMA QT** or solder directly
3. **Connect ICM-20649 via STEMMA QT** or solder directly
4. **Wire pyro channels** to GPIO 7, 45, 2, 4, 46
5. **Secure with heat shrink** and cable ties

## Form Factor

The Heltec is very compact (~50x25mm). The external sensors add bulk but the total package is still smaller than the Feather stack.

Suggested mounting:
- Heltec board oriented with display facing out
- BMP581 mounted nearby with port exposed to air
- ICM-20649 mounted rigidly to sense rocket motion
- Pyro terminals accessible for e-match connection

## Where to Buy

### Heltec
- **Heltec Official:** [heltec.org](https://heltec.org)
- **AliExpress:** Search "Heltec Wireless Tracker" (~$20-25)
- **Amazon:** Higher price but faster shipping

### Adafruit Components
- **Adafruit:** [adafruit.com](https://www.adafruit.com)

### Electronic Components
- **DigiKey:** [digikey.com](https://www.digikey.com)
- **Mouser:** [mouser.com](https://www.mouser.com)
