# Flight Computer Parts List

Complete bill of materials for building the rocket avionics flight computer.

## Core Components

| Component | Adafruit Product | Price | Link |
|-----------|------------------|-------|------|
| Feather RP2040 with RFM95 LoRa Radio (915MHz) | PID 5714 | $24.95 | [adafruit.com/product/5714](https://www.adafruit.com/product/5714) |
| BMP390 Barometric Pressure Sensor (STEMMA QT) | PID 4816 | $12.50 | [adafruit.com/product/4816](https://www.adafruit.com/product/4816) |
| LSM6DSOX + LIS3MDL 9-DoF IMU FeatherWing | PID 4565 | $19.95 | [adafruit.com/product/4565](https://www.adafruit.com/product/4565) |
| FeatherWing OLED 128x64 Display | PID 4650 | $14.95 | [adafruit.com/product/4650](https://www.adafruit.com/product/4650) |
| GPS Module (PA1010D Mini GPS) | PID 4415 | $29.95 | [adafruit.com/product/4415](https://www.adafruit.com/product/4415) |
| FeatherWing Tripler Mini Kit | PID 3417 | $8.50 | [adafruit.com/product/3417](https://www.adafruit.com/product/3417) |
| STEMMA QT Cable (100mm) | PID 4210 | $0.95 | [adafruit.com/product/4210](https://www.adafruit.com/product/4210) |
| LiPo Battery 3.7V 500mAh | PID 258 | $7.95 | [adafruit.com/product/258](https://www.adafruit.com/product/258) |

**Core Components Subtotal:** ~$120

## Pyro Channel Components

For dual parachute deployment (drogue at apogee, main at altitude).

| Component | Description | Qty | Est. Price |
|-----------|-------------|-----|------------|
| IRLZ44N N-Channel MOSFET | Logic-level MOSFET for pyro firing | 2 | $2.00 |
| 1kΩ Resistor (1/4W) | MOSFET gate resistor | 2 | $0.20 |
| 10kΩ Resistor (1/4W) | Gate pull-down resistor | 2 | $0.20 |
| 2-Position Screw Terminal (5mm) | E-match connection | 2 | $1.50 |
| SPST Toggle Switch | Arm safety switch | 1 | $2.00 |
| 2N2222 NPN Transistor (optional) | Gate driver for reliable switching | 2 | $0.50 |

**Pyro Components Subtotal:** ~$7

## Optional Components

| Component | Description | Price |
|-----------|-------------|-------|
| LiPo Battery 1200mAh (PID 258) | Longer flight time | $9.95 |
| FeatherWing Doubler (PID 2890) | Alternative to Tripler | $7.50 |
| uFL to SMA Adapter | External antenna option | $3.95 |
| 915MHz Antenna (SMA) | Higher gain antenna | $7.95 |

## Tools Required

- Soldering iron and solder
- Wire strippers
- Heat shrink tubing
- Multimeter (for continuity testing)
- Small Phillips screwdriver

## Assembly Overview

### Stacking Order (bottom to top)

1. **FeatherWing Tripler** - Base mounting board
2. **Feather RP2040 + RFM95** - Main MCU with LoRa radio
3. **LSM6DSOX + LIS3MDL IMU** - 9-DoF motion sensor
4. **FeatherWing OLED** - Display with buttons (on top for visibility)

### Connections

| Component | Connection | Pins |
|-----------|------------|------|
| BMP390 | STEMMA QT (I2C) | SDA/SCL via cable |
| GPS | UART0 | GP0 (TX), GP1 (RX) |
| Pyro 1 (Drogue) | GPIO | GP24 (fire), GP26 (continuity) |
| Pyro 2 (Main) | GPIO | GP25 (fire), GP27 (continuity) |
| Arm Switch | GPIO | GP10 (active low) |
| OLED Button A | GPIO | GP9 |
| OLED Button B | GPIO | GP6 |
| OLED Button C | GPIO | GP5 |

### Pyro Channel Wiring

```
Battery (+) ──────┬──────────────────┐
                  │                  │
              [E-Match]          [E-Match]
                  │                  │
                  ├── Pyro 1 Term ───┤
                  │                  │
              [MOSFET D]         [MOSFET D]
              [IRLZ44N]          [IRLZ44N]
              [MOSFET S]         [MOSFET S]
                  │                  │
                  └────── GND ───────┘

GP24 ──[1kΩ]──┬── MOSFET Gate (Pyro 1)
              │
           [10kΩ]
              │
             GND

GP25 ──[1kΩ]──┬── MOSFET Gate (Pyro 2)
              │
           [10kΩ]
              │
             GND
```

### Continuity Detection

The continuity ADC inputs (GP26, GP27) detect whether e-matches are connected:
- Voltage divider with e-match resistance
- ADC reads ~1.5V when e-match connected
- ADC reads 0V when open circuit

## Safety Notes

1. **Never connect e-matches until ready to fly**
2. **Arm switch must be OFF during assembly and transport**
3. **Test continuity detection before each flight**
4. **Use fresh e-matches for each flight**
5. **Keep battery charged (3.7V nominal, 4.2V full)**

## Estimated Total Cost

| Category | Cost |
|----------|------|
| Core Components | ~$120 |
| Pyro Components | ~$7 |
| **Total** | **~$127** |

*Prices as of January 2026. Does not include shipping or tax.*

## Where to Buy

- **Adafruit:** [adafruit.com](https://www.adafruit.com) - Primary source for Feather ecosystem
- **DigiKey:** [digikey.com](https://www.digikey.com) - MOSFETs, resistors, terminals
- **Mouser:** [mouser.com](https://www.mouser.com) - Alternative for electronic components
- **Amazon:** Backup for common components, batteries
