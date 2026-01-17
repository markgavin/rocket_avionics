# Rocket Flight Computer Carrier Board - Design Document

**Author:** Mark Gavin
**Created:** 2026-01-16
**Modified:** 2026-01-17
**Version:** 2.1

## Overview

This document describes the design of a custom carrier board for the Rocket Avionics Flight Computer. The carrier board hosts an Adafruit Feather RP2040 with RFM95 LoRa Radio (Product ID: 5714) via socket headers, along with sensors, pyro deployment channels, and supporting circuitry.

### Design Goals

- Use Feather RP2040 RFM95 as main module (reduces complexity)
- Socket-mount Feather for ease of development and debugging
- All sensors SMD-mounted for vibration resistance
- **Dual SPI buses** for optimal performance and isolation
- Dual pyro channels with continuity detection
- Fits in 54mm (2.1") rocket airframe
- Single 1S LiPo power (uses Feather's onboard charger)

### Target Dimensions

- Board size: ~50mm x 60mm
- Mounting: 4x M2.5 holes for tube rails

---

## Design Decisions

### Why Dual SPI Buses?

| Consideration | Single Bus | Dual Bus |
|---------------|------------|----------|
| Speed switching | Required (1-5 MHz) | Not needed |
| LoRa TX blocking | Blocks sensor reads | Sensors independent |
| Complexity | Simpler wiring | Slightly more pins |
| Reliability | Bus contention possible | Full isolation |

**Decision:** Use two SPI buses to isolate high-rate flight sensors from LoRa radio operations.

### Bus Assignment Rationale

| Bus | Devices | Speed | Why |
|-----|---------|-------|-----|
| **SPI1** | LoRa + BMP390 | 1 MHz | Low-rate devices; barometer data changes slowly |
| **SPI0** | ICM-42688 + ADXL375 | 5 MHz | High-rate flight sensors need fast, uninterrupted reads |

**Benefits:**
- LoRa transmissions don't block critical sensor reads
- IMU and high-G accel can sample at 1000+ Hz without interruption
- No speed switching overhead
- BMP390 doesn't need high speed (pressure changes slowly)

### SPI Device Compatibility

| Device | Max SPI Speed | SPI Mode | Bus Assignment |
|--------|---------------|----------|----------------|
| RFM95 LoRa | 10 MHz | Mode 0 | SPI1 (1 MHz) |
| BMP390 | 10 MHz | Mode 0 | SPI1 (1 MHz) |
| ICM-42688-P | 24 MHz | Mode 0/3 | SPI0 (5 MHz) |
| ADXL375 | 5 MHz | Mode 0/3 | SPI0 (5 MHz) |

All devices support **SPI Mode 0 (CPOL=0, CPHA=0)** - no conflicts.

---

## Feather RP2040 RFM95 Pin Allocation

### Pins Used Internally by Feather (DO NOT USE)

| GPIO | Function | Notes |
|------|----------|-------|
| GP8 | SPI1 MISO | LoRa radio + BMP390 |
| GP14 | SPI1 SCK | LoRa radio + BMP390 |
| GP15 | SPI1 MOSI | LoRa radio + BMP390 |
| GP16 | RFM95 CS | LoRa chip select |
| GP17 | RFM95 RST | LoRa reset |
| GP18 | RFM95 DIO5 | LoRa (internal) |
| GP19 | RFM95 DIO3 | LoRa (internal) |
| GP20 | RFM95 DIO4 | LoRa (internal) |
| GP21 | RFM95 DIO0 | LoRa RX done interrupt |
| GP22 | RFM95 DIO1 | LoRa (optional) |
| GP23 | RFM95 DIO2 | LoRa (internal) |

### Pins Available for Carrier Board

| GPIO | Feather Label | Carrier Function | Notes |
|------|---------------|------------------|-------|
| GP0 | TX | GPS RX | UART0 TX to GPS module RX |
| GP1 | RX | GPS TX | UART0 RX from GPS module TX |
| GP2 | SDA | SPI0 SCK | High-rate sensor bus clock |
| GP3 | SCL | SPI0 MOSI | High-rate sensor bus data out |
| GP4 | D4 | NeoPixel | Status LED (onboard) |
| GP5 | D5 | CS_BMP390 | SPI1 chip select |
| GP6 | D6 | CS_ICM42688 | SPI0 chip select |
| GP7 | BOOT | SPI0 MISO | High-rate sensor bus data in |
| GP8 | MISO | SPI1 MISO | LoRa + BMP390 bus |
| GP9 | D9 | CS_ADXL375 | SPI0 chip select |
| GP10 | D10 | Arm Switch | Input, active LOW |
| GP11 | D11 | Buzzer | PWM output for piezo |
| GP12 | D12 | *Available* | Future use |
| GP13 | D13 | *Available* | Future use |
| GP14 | SCK | SPI1 SCK | LoRa + BMP390 bus |
| GP15 | MOSI | SPI1 MOSI | LoRa + BMP390 bus |
| GP24 | D24 | Pyro 1 Fire | Drogue deployment MOSFET gate |
| GP25 | D25 | Pyro 2 Fire | Main deployment MOSFET gate |
| GP26 | A0 | Pyro 1 Continuity | ADC - e-match continuity sense |
| GP27 | A1 | Pyro 2 Continuity | ADC - e-match continuity sense |
| GP28 | A2 | Battery Voltage | ADC - voltage divider input |
| GP29 | A3 | Pyro 3 Continuity | ADC (optional) |

---

## Pin Summary Table

| GPIO | Function | Direction | Type | Notes |
|------|----------|-----------|------|-------|
| GP0 | GPS TX | Output | UART0 | To GPS RX |
| GP1 | GPS RX | Input | UART0 | From GPS TX |
| GP2 | SPI0 SCK | Output | SPI | High-rate sensors |
| GP3 | SPI0 MOSI | Output | SPI | High-rate sensors |
| GP4 | NeoPixel | Output | Digital | Status LED |
| GP5 | CS_BMP390 | Output | Digital | SPI1 chip select |
| GP6 | CS_ICM42688 | Output | Digital | SPI0 chip select |
| GP7 | SPI0 MISO | Input | SPI | High-rate sensors |
| GP8 | SPI1 MISO | Input | SPI | LoRa + BMP390 |
| GP9 | CS_ADXL375 | Output | Digital | SPI0 chip select |
| GP10 | Arm Switch | Input | Digital | Pull-up, active LOW |
| GP11 | Buzzer | Output | PWM | Piezo buzzer |
| GP14 | SPI1 SCK | Output | SPI | LoRa + BMP390 |
| GP15 | SPI1 MOSI | Output | SPI | LoRa + BMP390 |
| GP16 | CS_LoRa | Output | Digital | LoRa chip select |
| GP24 | Pyro 1 Fire | Output | Digital | Drogue MOSFET |
| GP25 | Pyro 2 Fire | Output | Digital | Main MOSFET |
| GP26 | Pyro 1 Cont | Input | ADC | Continuity sense |
| GP27 | Pyro 2 Cont | Input | ADC | Continuity sense |
| GP28 | VBAT Sense | Input | ADC | Battery voltage |
| GP29 | Pyro 3 Cont | Input | ADC | Optional |

---

## Dual SPI Bus Configuration

### Bus Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     DUAL SPI ARCHITECTURE                        │
│                                                                 │
│  SPI1 (1 MHz) - Low-Rate Bus                                   │
│  ════════════════════════════                                   │
│    GP14 (SCK)  ────┬──── RFM95 LoRa    (CS: GP16)              │
│    GP15 (MOSI) ────┤                                            │
│    GP8  (MISO) ────┴──── BMP390 Baro   (CS: GP5)               │
│                                                                 │
│                                                                 │
│  SPI0 (5 MHz) - High-Rate Sensor Bus                           │
│  ═══════════════════════════════════                           │
│    GP2 (SCK)  ─────┬──── ICM-42688 IMU  (CS: GP6)              │
│    GP3 (MOSI) ─────┤                                            │
│    GP7 (MISO) ─────┴──── ADXL375 Hi-G   (CS: GP9)              │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### SPI1 - LoRa + Barometer Bus

| Signal | GPIO | Devices |
|--------|------|---------|
| SCK | GP14 | RFM95, BMP390 |
| MOSI | GP15 | RFM95, BMP390 |
| MISO | GP8 | RFM95, BMP390 |
| CS_LoRa | GP16 | RFM95 |
| CS_BMP390 | GP5 | BMP390 |

**Speed:** 1 MHz (LoRa nominal speed)
**Mode:** Mode 0 (CPOL=0, CPHA=0)

### SPI0 - High-Rate Sensor Bus

| Signal | GPIO | Devices |
|--------|------|---------|
| SCK | GP2 | ICM-42688, ADXL375 |
| MOSI | GP3 | ICM-42688, ADXL375 |
| MISO | GP7 | ICM-42688, ADXL375 |
| CS_ICM42688 | GP6 | ICM-42688 |
| CS_ADXL375 | GP9 | ADXL375 |

**Speed:** 5 MHz (limited by ADXL375)
**Mode:** Mode 0 (CPOL=0, CPHA=0)

**Note:** GP7 is the BOOT pin on the Feather, but it functions normally as GPIO after boot. Safe to use for SPI0 MISO.

---

## Sensor SPI Connections

### BMP390 (Barometer) - SPI1

| BMP390 Pin | Connect To | Notes |
|------------|------------|-------|
| VDD | 3.3V | |
| GND | GND | |
| CSB | GP5 | Active LOW |
| SDI | GP15 (MOSI) | SPI1 data in |
| SDO | GP8 (MISO) | SPI1 data out |
| SCK | GP14 | SPI1 clock |

### ICM-42688-P (6-DOF IMU) - SPI0

| ICM-42688 Pin | Connect To | Notes |
|---------------|------------|-------|
| VDD | 3.3V | |
| VDDIO | 3.3V | |
| GND | GND | |
| CS | GP6 | Active LOW |
| SCLK | GP2 | SPI0 clock |
| SDI | GP3 (MOSI) | SPI0 data in |
| SDO | GP7 (MISO) | SPI0 data out |
| INT1 | *NC* | Optional interrupt |

### ADXL375 (High-G Accelerometer) - SPI0

| ADXL375 Pin | Connect To | Notes |
|-------------|------------|-------|
| VDD | 3.3V | |
| VS | 3.3V | |
| GND | GND | |
| CS | GP9 | Active LOW |
| SCLK | GP2 | SPI0 clock |
| MOSI | GP3 | SPI0 data in |
| MISO | GP7 | SPI0 data out |
| INT1 | *NC* | Optional interrupt |

---

## GPS Module (UART0)

| Signal | Feather Pin | GPS Module Pin |
|--------|-------------|----------------|
| TX | GP0 | RX |
| RX | GP1 | TX |
| VCC | 3.3V | VCC |
| GND | GND | GND |

**Recommended modules:** Quectel L76K, U-blox MAX-M10S, or Adafruit Ultimate GPS

**UART Configuration:** 9600 baud, 8N1

---

## Pyro Channel Circuit

### Schematic

```
                    ┌─────────────────────────────────────┐
                    │         Pyro Channel Circuit        │
                    │                                     │
  GP24 ────[1k]─────┤ Gate                               │
                    │   ┌─────┐                          │
                    │   │     │ Si2302                   │
            GND ────┤───┤ S   │ N-MOSFET                 │
                    │   │     │                          │
                    │   └──┬──┘                          │
                    │      │ Drain                       │
                    │      │                             │
  E-match (-)  ─────┼──────┘                             │
                    │                                     │
  E-match (+)  ─────┼──────────────── VBATT (3.7-4.2V)  │
                    │                                     │
                    │     Continuity Sensing:            │
                    │                                     │
  GP26 (ADC) ───────┼─────┬─────[10k]──── 3.3V          │
                    │     │                              │
                    │     └─────[10k]──── E-match (-)   │
                    │                                     │
                    └─────────────────────────────────────┘
```

### Pyro Channel Assignments

| Channel | Fire GPIO | Continuity ADC | Purpose |
|---------|-----------|----------------|---------|
| Pyro 1 | GP24 | GP26 (A0) | Drogue deployment |
| Pyro 2 | GP25 | GP27 (A1) | Main deployment |
| Pyro 3 | *N/A* | GP29 (A3) | Optional (continuity only) |

### Continuity ADC Values

| Condition | Voltage at ADC | ADC Reading (12-bit) |
|-----------|----------------|----------------------|
| Open circuit (no e-match) | ~1.65V | ~2048 |
| E-match connected (~2 ohm) | ~0.02V | ~25 |
| Short circuit | 0V | 0 |

### Safety Notes

- Pyro outputs are LOW by default (MOSFET off)
- Gate resistor (1k) limits current and provides ESD protection
- Always use arm switch to isolate pyro battery until ready
- Consider adding flyback diode across e-match terminals

---

## Arming Switch Circuit

```
  3.3V ────[10k]────┬──── GP10 (input, active LOW)
                    │
                    └──── Arm Switch ──── GND
```

| Switch State | GP10 Reading | System State |
|--------------|--------------|--------------|
| OPEN | HIGH | SAFE - Pyros disabled |
| CLOSED | LOW | ARMED - Pyros enabled |

---

## Battery Voltage Monitor

```
  VBATT ────[100k]────┬──── GP28 (A2, ADC)
                      │
                      └────[100k]──── GND
```

**Voltage calculation:**
```c
float vbat = adc_reading * 2.0f * 3.3f / 4096.0f;
```

| Battery State | VBATT | ADC Voltage | ADC Reading |
|---------------|-------|-------------|-------------|
| Full (4.2V) | 4.2V | 2.1V | ~2606 |
| Nominal (3.7V) | 3.7V | 1.85V | ~2296 |
| Low (3.3V) | 3.3V | 1.65V | ~2048 |
| Critical (3.0V) | 3.0V | 1.5V | ~1862 |

---

## Buzzer Circuit

```
  GP11 ────[100]────┬──── Piezo (+)
                    │
            GND ────┴──── Piezo (-)
```

Use PWM at 2-4kHz for audible tone. Useful for:
- Arming confirmation beeps
- Low battery warning
- Landing locator beacon

---

## Block Diagram

```
┌───────────────────────────────────────────────────────────────────────────┐
│                    CARRIER BOARD v2.1 (Dual SPI)                          │
│                                                                           │
│   ┌─────────────────────────────────────────────────────────────────┐    │
│   │                   FEATHER RP2040 RFM95                          │    │
│   │                   (Socket Mounted)                              │    │
│   │                                                                 │    │
│   │  USB  3V3  GND  GP0  GP1  GP2  GP3  GP4  GP5  GP6  GP7  GP9   │    │
│   │   │    │    │    │    │    │    │    │    │    │    │    │    │    │
│   │   │    │    │    │    │    │    │    │    │    │    │    │    │    │
│   │  GP8(MISO1) GP10 GP11 GP14(SCK1) GP15(MOSI1) GP16(CS_LoRa)   │    │
│   │    │         │    │      │          │           │             │    │
│   └────┼─────────┼────┼──────┼──────────┼───────────┼─────────────┘    │
│        │         │    │      │          │           │                   │
│        │         │    │      │          │           └──► LoRa Radio     │
│        │         │    │      │          │               (onboard)       │
│        │         │    │      │          │                               │
│   ┌────┴─────────┼────┼──────┴──────────┴───────────────────────┐      │
│   │   SPI1 Bus   │    │        (1 MHz - LoRa + Barometer)       │      │
│   │              │    │                                          │      │
│   │         ┌────────────┐                                       │      │
│   │         │   BMP390   │                                       │      │
│   │         │  Barometer │                                       │      │
│   │         │   CS: GP5  │                                       │      │
│   │         └────────────┘                                       │      │
│   └──────────────────────────────────────────────────────────────┘      │
│                  │    │                                                  │
│                  │    │   ARM Switch    Buzzer                          │
│                  │    │      GP10        GP11                           │
│                  │    │                                                  │
│   ┌──────────────┴────┴─────────────────────────────────────────┐      │
│   │              GP2  GP3  GP7                                   │      │
│   │   SPI0 Bus  (SCK)(MOSI)(MISO)   (5 MHz - Flight Sensors)    │      │
│   │                                                              │      │
│   │    ┌────────────┐         ┌────────────┐                    │      │
│   │    │ ICM-42688  │         │  ADXL375   │                    │      │
│   │    │  6-DOF IMU │         │   ±200g    │                    │      │
│   │    │   CS: GP6  │         │   CS: GP9  │                    │      │
│   │    └────────────┘         └────────────┘                    │      │
│   └──────────────────────────────────────────────────────────────┘      │
│                                                                          │
│                                                                          │
│   ┌─────────────┐    ┌─────────────────────┐    ┌─────────────┐        │
│   │  GPS Module │    │    Pyro Channels    │    │   Control   │        │
│   │             │    │                     │    │             │        │
│   │  ┌───────┐  │    │  ┌─────┐  ┌─────┐  │    │  ARM   GP10 │        │
│   │  │Quectel│  │    │  │Pyro1│  │Pyro2│  │    │  BZR   GP11 │        │
│   │  │ L76K  │  │    │  │GP24 │  │GP25 │  │    │  LED   GP4  │        │
│   │  └───┬───┘  │    │  │ADC26│  │ADC27│  │    │  VBAT  GP28 │        │
│   │      │      │    │  └──┬──┘  └──┬──┘  │    └─────────────┘        │
│   │   UART0     │    │     │        │     │                            │
│   │   GP0/GP1   │    │  ┌──┴────────┴──┐  │                            │
│   └─────────────┘    │  │ Screw Terms  │  │                            │
│                      │  │  E-matches   │  │                            │
│                      │  └──────────────┘  │                            │
│                      └─────────────────────┘                            │
│                                                                          │
│   ┌───────────────────────────────────────────────────────────────┐    │
│   │  POWER                                                         │    │
│   │                                                                │    │
│   │  LiPo 1S ──► Feather Charger ──► 3.3V Rail ──► All ICs       │    │
│   │  (JST-PH)    (onboard)                                         │    │
│   │                                                                │    │
│   └───────────────────────────────────────────────────────────────┘    │
│                                                                          │
│   Mounting: 4x M2.5 holes for 54mm (2.1") tube rails                    │
│   Dimensions: ~50mm x 60mm                                               │
└──────────────────────────────────────────────────────────────────────────┘
```

---

## Bill of Materials

### Main Components

| Ref | Part | Description | Package | Qty | Est. Price |
|-----|------|-------------|---------|-----|------------|
| U1 | BMP390 | Barometric pressure sensor | LGA-10 | 1 | $5.00 |
| U2 | ICM-42688-P | 6-DOF IMU (accel/gyro) | LGA-14 | 1 | $5.00 |
| U3 | ADXL375 | High-G accelerometer ±200g | LGA-14 | 1 | $15.00 |
| U4 | Quectel L76K | GPS module | Module | 1 | $10.00 |
| Q1, Q2 | Si2302 | N-MOSFET (pyro driver) | SOT-23 | 2 | $0.50 |

### Connectors

| Ref | Part | Description | Package | Qty | Est. Price |
|-----|------|-------------|---------|-----|------------|
| J1 | Female headers | 2x 16-pos for Feather | 2.54mm | 1 | $2.00 |
| J2, J3 | Screw terminal | 2-pos (pyro outputs) | 5.08mm | 2 | $1.00 |
| J4 | u.FL | GPS antenna connector | SMD | 1 | $0.50 |
| J5 | JST-PH 2-pin | LiPo battery (optional) | 2mm | 1 | $0.30 |

### Switches and Indicators

| Ref | Part | Description | Package | Qty | Est. Price |
|-----|------|-------------|---------|-----|------------|
| SW1 | SPST slide | Arm switch | Through-hole | 1 | $0.50 |
| BZ1 | Piezo buzzer | 12mm passive | Through-hole | 1 | $1.00 |

### Passive Components

| Ref | Value | Description | Package | Qty | Est. Price |
|-----|-------|-------------|---------|-----|------------|
| R1, R2 | 1k | Pyro gate resistors | 0402 | 2 | $0.05 |
| R3, R4, R5, R6 | 10k | Continuity dividers | 0402 | 4 | $0.10 |
| R7, R8 | 100k | VBAT divider | 0402 | 2 | $0.05 |
| R9 | 10k | Arm switch pull-up | 0402 | 1 | $0.02 |
| R10 | 100 | Buzzer series | 0402 | 1 | $0.02 |
| C1-C8 | 0.1uF | Decoupling caps | 0402 | 8 | $0.15 |
| C9-C12 | 10uF | Bulk caps | 0603 | 4 | $0.20 |

### PCB

| Item | Description | Qty | Est. Price |
|------|-------------|-----|------------|
| PCB | 4-layer, ENIG, 50x60mm | 5 | $15.00 ea |

### Cost Summary

| Category | Cost |
|----------|------|
| Carrier board components | ~$42 |
| PCB (each, qty 5) | ~$15 |
| Feather RP2040 RFM95 | $25 |
| **Total per unit** | **~$82** |

---

## PCB Design Notes

### Layer Stackup (4-layer)

1. **Top** - Signal + components
2. **Inner 1** - Ground plane (solid)
3. **Inner 2** - Power (3.3V)
4. **Bottom** - Signal + GPS antenna

### Layout Guidelines

1. **SPI0 routing** - Keep GP2, GP3, GP7 traces short and matched length
2. **SPI1 routing** - GP8, GP14, GP15 route to BMP390 (LoRa already connected)
3. **Chip selects** - Route CS lines away from clocks, add 10k pull-ups to 3.3V
4. **GPS antenna** - Keep clear of noisy signals, place u.FL near board edge
5. **Pyro traces** - Use wider traces (0.5mm+) for current handling
6. **Sensor placement** - Mount IMU and high-G accel near board center (center of rotation)
7. **Decoupling caps** - Place 0.1uF close to each IC power pin

### SPI Signal Integrity

- Keep SPI trace lengths under 50mm
- Use ground plane under SPI traces
- Add series termination resistors (33Ω) if needed for signal integrity
- Ensure CS lines have clean transitions (no ringing)

### GP7 (BOOT) Pin Note

GP7 is the BOOT pin on the Feather RP2040. It has an internal pull-up and is used during boot to enter USB bootloader mode. After boot, it functions as a normal GPIO and is safe to use for SPI0 MISO. The internal pull-up should not affect SPI operation.

### Mechanical

- 4x M2.5 mounting holes at corners
- Keep components away from mounting holes (3mm clearance)
- Consider conformal coating for moisture protection

---

## Firmware Pin Definitions

Update `firmware_flight/include/pins.h`:

```c
//----------------------------------------------
// Carrier Board Pin Assignments (Dual SPI Version)
// Hardware: Custom carrier board v2.1
//----------------------------------------------

//----------------------------------------------
// SPI1 Bus (LoRa + Barometer) - 1 MHz
//----------------------------------------------
#define kPinSpi1Sck         14  // GP14 - SPI1 SCK
#define kPinSpi1Mosi        15  // GP15 - SPI1 TX/MOSI
#define kPinSpi1Miso        8   // GP8 - SPI1 RX/MISO
#define kSpi1Port           spi1
#define kSpi1Baudrate       1000000  // 1 MHz

// SPI1 Chip Selects
#define kPinLoRaCs          16  // GP16 - RFM95 LoRa
#define kPinBMP390Cs        5   // GP5 - Barometer

//----------------------------------------------
// SPI0 Bus (High-Rate Sensors) - 5 MHz
//----------------------------------------------
#define kPinSpi0Sck         2   // GP2 - SPI0 SCK
#define kPinSpi0Mosi        3   // GP3 - SPI0 TX/MOSI
#define kPinSpi0Miso        7   // GP7 - SPI0 RX/MISO (BOOT pin)
#define kSpi0Port           spi0
#define kSpi0Baudrate       5000000  // 5 MHz (limited by ADXL375)

// SPI0 Chip Selects
#define kPinICM42688Cs      6   // GP6 - 6-DOF IMU
#define kPinADXL375Cs       9   // GP9 - High-G accelerometer

//----------------------------------------------
// GPS (UART0)
//----------------------------------------------
#define kPinGpsTx           0   // GP0 - UART0 TX (to GPS RX)
#define kPinGpsRx           1   // GP1 - UART0 RX (from GPS TX)
#define kGpsUartPort        uart0
#define kGpsUartBaudrate    9600

//----------------------------------------------
// Pyro Channels
//----------------------------------------------
#define kPinPyro1Fire       24  // GP24 - Drogue deployment
#define kPinPyro2Fire       25  // GP25 - Main deployment
#define kPinPyro1Cont       26  // GP26/A0 - Drogue continuity ADC
#define kPinPyro2Cont       27  // GP27/A1 - Main continuity ADC
#define kPinPyro3Cont       29  // GP29/A3 - Optional 3rd channel ADC

//----------------------------------------------
// Control Inputs
//----------------------------------------------
#define kPinArmSwitch       10  // GP10 - Arm switch (active LOW)

//----------------------------------------------
// Outputs
//----------------------------------------------
#define kPinBuzzer          11  // GP11 - Piezo buzzer (PWM)
#define kPinNeoPixel        4   // GP4 - Status LED (onboard)

//----------------------------------------------
// Analog Inputs
//----------------------------------------------
#define kPinVbatSense       28  // GP28/A2 - Battery voltage

//----------------------------------------------
// LoRa Radio Configuration
//----------------------------------------------
#define kLoRaFrequency      915000000   // 915 MHz ISM band
#define kLoRaSpreadFactor   7           // SF7 for fastest data rate
#define kLoRaBandwidth      125000      // 125 kHz bandwidth
#define kLoRaCodingRate     5           // 4/5 coding rate
#define kLoRaPreambleLen    8           // Preamble length
#define kLoRaTxPower        20          // 20 dBm (100 mW)
#define kLoRaSyncWord       0x14        // Private sync word

//----------------------------------------------
// Sensor Configuration
//----------------------------------------------
#define kICM42688AccelRange     16      // ±16g
#define kICM42688GyroRange      2000    // ±2000 dps
#define kICM42688SampleRate     1000    // 1 kHz ODR
#define kADXL375Range           200     // ±200g (fixed)
#define kADXL375SampleRate      3200    // Up to 3.2 kHz with SPI

//----------------------------------------------
// Timing
//----------------------------------------------
#define kTelemetryIntervalMs        100     // 10 Hz LoRa telemetry
#define kSensorSampleIntervalMs     1       // 1000 Hz sensor sampling
#define kBaroSampleIntervalMs       20      // 50 Hz barometer sampling
#define kDisplayUpdateIntervalMs    200     // 5 Hz display update
```

---

## SPI Driver Implementation

### Initialization

```c
#include "hardware/spi.h"

void InitSpiBuses(void) {
    //--- SPI1: LoRa + Barometer (1 MHz) ---
    spi_init(kSpi1Port, kSpi1Baudrate);
    gpio_set_function(kPinSpi1Sck, GPIO_FUNC_SPI);
    gpio_set_function(kPinSpi1Mosi, GPIO_FUNC_SPI);
    gpio_set_function(kPinSpi1Miso, GPIO_FUNC_SPI);

    // CS pins for SPI1
    gpio_init(kPinLoRaCs);
    gpio_set_dir(kPinLoRaCs, GPIO_OUT);
    gpio_put(kPinLoRaCs, 1);  // Deselect

    gpio_init(kPinBMP390Cs);
    gpio_set_dir(kPinBMP390Cs, GPIO_OUT);
    gpio_put(kPinBMP390Cs, 1);  // Deselect

    //--- SPI0: High-Rate Sensors (5 MHz) ---
    spi_init(kSpi0Port, kSpi0Baudrate);
    gpio_set_function(kPinSpi0Sck, GPIO_FUNC_SPI);
    gpio_set_function(kPinSpi0Mosi, GPIO_FUNC_SPI);
    gpio_set_function(kPinSpi0Miso, GPIO_FUNC_SPI);

    // CS pins for SPI0
    gpio_init(kPinICM42688Cs);
    gpio_set_dir(kPinICM42688Cs, GPIO_OUT);
    gpio_put(kPinICM42688Cs, 1);  // Deselect

    gpio_init(kPinADXL375Cs);
    gpio_set_dir(kPinADXL375Cs, GPIO_OUT);
    gpio_put(kPinADXL375Cs, 1);  // Deselect
}
```

### Transfer Functions

```c
// SPI1 transfer (LoRa + Barometer)
void Spi1Transfer(uint8_t csPin, uint8_t* txBuf, uint8_t* rxBuf, size_t len) {
    gpio_put(csPin, 0);  // Assert CS
    spi_write_read_blocking(kSpi1Port, txBuf, rxBuf, len);
    gpio_put(csPin, 1);  // Deassert CS
}

// SPI0 transfer (High-rate sensors)
void Spi0Transfer(uint8_t csPin, uint8_t* txBuf, uint8_t* rxBuf, size_t len) {
    gpio_put(csPin, 0);  // Assert CS
    spi_write_read_blocking(kSpi0Port, txBuf, rxBuf, len);
    gpio_put(csPin, 1);  // Deassert CS
}

// Convenience wrappers
void BMP390_SpiTransfer(uint8_t* tx, uint8_t* rx, size_t len) {
    Spi1Transfer(kPinBMP390Cs, tx, rx, len);
}

void ICM42688_SpiTransfer(uint8_t* tx, uint8_t* rx, size_t len) {
    Spi0Transfer(kPinICM42688Cs, tx, rx, len);
}

void ADXL375_SpiTransfer(uint8_t* tx, uint8_t* rx, size_t len) {
    Spi0Transfer(kPinADXL375Cs, tx, rx, len);
}
```

### Main Loop Example

```c
void FlightLoop(void) {
    uint32_t lastSensorRead = 0;
    uint32_t lastBaroRead = 0;
    uint32_t lastTelemetry = 0;

    while (true) {
        uint32_t now = time_us_32() / 1000;  // ms

        // High-rate sensor sampling (1000 Hz) - SPI0
        if (now - lastSensorRead >= kSensorSampleIntervalMs) {
            lastSensorRead = now;
            ICM42688_ReadAccelGyro(&imuData);   // SPI0
            ADXL375_ReadAccel(&highGData);      // SPI0
        }

        // Barometer sampling (50 Hz) - SPI1
        if (now - lastBaroRead >= kBaroSampleIntervalMs) {
            lastBaroRead = now;
            BMP390_ReadPressure(&baroData);     // SPI1
        }

        // LoRa telemetry (10 Hz) - SPI1
        if (now - lastTelemetry >= kTelemetryIntervalMs) {
            lastTelemetry = now;
            LoRa_SendTelemetry(&telemetry);     // SPI1
        }
    }
}
```

---

## References

- [Adafruit Feather RP2040 RFM95 Pinouts](https://learn.adafruit.com/feather-rp2040-rfm95/pinouts)
- [Adafruit Feather RP2040 RFM PCB Files (GitHub)](https://github.com/adafruit/Adafruit-Feather-RP2040-RFM-PCB)
- [BMP390 Datasheet](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp390-ds002.pdf)
- [ICM-42688-P Datasheet](https://invensense.tdk.com/download-pdf/icm-42688-p-datasheet/)
- [ADXL375 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL375.PDF)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-01-16 | Initial design document (I2C version) |
| 2.0 | 2026-01-17 | Changed to single SPI bus for all sensors |
| 2.1 | 2026-01-17 | Dual SPI architecture: SPI0 for high-rate sensors, SPI1 for LoRa + barometer |
