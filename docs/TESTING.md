# Rocket Avionics - Testing Procedures

## Overview

This document provides step-by-step procedures for testing the rocket avionics system before flight operations.

---

## Prerequisites

### Hardware Required
- [ ] Flight Computer: Feather RP2040 + RFM95 LoRa (Product 5714)
- [ ] Ground Gateway: Feather RP2040 + RFM95 LoRa (Product 5714)
- [ ] BMP390 Barometric Sensor (Product 4816) connected via STEMMA QT
- [ ] Adalogger FeatherWing with SD card inserted (Product 2922)
- [ ] FeatherWing OLED 128x64 (Product 4650)
- [ ] Quad Side-By-Side FeatherWing Kit (Product 4254)
- [ ] 2x USB-C cables
- [ ] FAT32 formatted microSD card (≤32GB recommended)

### Software Required
- [ ] Firmware files built and ready:
  - `firmware_flight/build/rocket_avionics_flight.uf2`
  - `firmware_gateway/build/rocket_gateway.uf2`
- [ ] Xojo IDE (for desktop app) or built application
- [ ] Serial terminal (screen, minicom, or PuTTY)

---

## Test 1: Firmware Flashing

### 1.1 Flash Flight Computer

1. Disconnect the flight computer from USB
2. Press and hold the **BOOTSEL** button on the Feather
3. While holding BOOTSEL, connect USB cable to computer
4. Release BOOTSEL - a drive named **RPI-RP2** should appear
5. Drag `rocket_avionics_flight.uf2` to the RPI-RP2 drive
6. The drive will disappear and the device will reboot
7. **Expected**: NeoPixel LED blinks green slowly (~1 Hz) indicating IDLE state

**Pass Criteria:**
- [ ] RPI-RP2 drive appeared
- [ ] UF2 file copied successfully
- [ ] Device rebooted automatically
- [ ] NeoPixel LED blinking green

### 1.2 Flash Ground Gateway

1. Repeat the same process with the gateway Feather
2. Drag `rocket_gateway.uf2` to the RPI-RP2 drive
3. **Expected**: Device reboots and appears as USB serial device

**Pass Criteria:**
- [ ] RPI-RP2 drive appeared
- [ ] UF2 file copied successfully
- [ ] Device rebooted automatically
- [ ] USB serial port appears (check with `ls /dev/cu.usbmodem*`)

---

## Test 2: Gateway Serial Communication

### 2.1 Connect to Gateway

```bash
# Find the serial port
ls /dev/cu.usbmodem*

# Connect (replace with actual port)
screen /dev/cu.usbmodem14101 115200
```

### 2.2 Verify Gateway Output

Wait 5 seconds. You should see periodic status messages:

```json
{"type":"link","connected":false}
```

This indicates the gateway is running but hasn't received any LoRa packets yet.

### 2.3 Send Test Commands

Type the following commands and press Enter:

```json
{"cmd":"status","id":1}
```

**Expected Response:**
```json
{"type":"ack","cmd":"status","id":1,"ok":true}
```

**Pass Criteria:**
- [ ] Gateway outputs link status messages
- [ ] Command acknowledgment received
- [ ] No error messages

*Exit screen: Press `Ctrl+A` then `K`, then `Y`*

---

## Test 3: BMP390 Sensor Verification

### 3.1 OLED Display Check

With the flight computer powered on:

1. Observe the OLED display
2. **Expected Display:**
   ```
   ROCKET AVIONICS
   State: IDLE
   Alt: 0.0 m
   Vel: 0.0 m/s
   Temp: 25.3 C
   ```

### 3.2 Altitude Response Test

1. Note the current altitude reading
2. Raise the flight computer approximately 1 meter
3. Wait 2-3 seconds for reading to stabilize
4. **Expected**: Altitude increases by approximately 1 meter

### 3.3 Pressure Seal Test

1. Cup your hands around the BMP390 sensor (don't touch it)
2. Breathe gently toward the sensor
3. **Expected**: Altitude reading changes (warm breath = pressure change)

**Pass Criteria:**
- [ ] OLED displays current state and readings
- [ ] Altitude responds to height changes
- [ ] Altitude responds to pressure changes
- [ ] Temperature reading is reasonable (15-35°C indoors)

---

## Test 4: LoRa Communication

### 4.1 Basic Link Test

**Setup:**
- Flight computer powered on (battery or USB)
- Gateway connected to computer via USB
- Serial terminal connected to gateway

**Procedure:**

1. Power on the flight computer
2. Wait for OLED to show "IDLE" state
3. Observe gateway serial output

**Expected Gateway Output:**
```json
{"type":"tel","seq":1,"t":1000,"alt":0.5,"vel":0.0,"state":"idle","rssi":-45,"snr":10}
{"type":"tel","seq":2,"t":1100,"alt":0.5,"vel":0.0,"state":"idle","rssi":-44,"snr":11}
```

**Pass Criteria:**
- [ ] Telemetry packets received at ~10 Hz
- [ ] RSSI value present (typically -30 to -90 dBm)
- [ ] SNR value present (typically 5 to 15 dB)
- [ ] Sequence numbers incrementing
- [ ] State shows "idle"

### 4.2 Arm/Disarm Test

1. In the serial terminal, send:
   ```json
   {"cmd":"arm","id":1}
   ```

2. **Expected Flight Computer Response:**
   - OLED shows "State: ARMED"
   - NeoPixel changes to yellow, blinks faster (~2 Hz)

3. **Expected Gateway Response:**
   ```json
   {"type":"ack","cmd":"arm","id":1,"ok":true}
   {"type":"tel","seq":123,"t":12300,"alt":0.5,"vel":0.0,"state":"armed",...}
   ```

4. Send disarm command:
   ```json
   {"cmd":"disarm","id":2}
   ```

5. **Expected**: Flight computer returns to IDLE state

**Pass Criteria:**
- [ ] Arm command acknowledged
- [ ] Flight computer enters ARMED state
- [ ] Telemetry shows "armed" state
- [ ] NeoPixel changes to yellow
- [ ] Disarm returns to IDLE

---

## Test 5: SD Card Logging

### 5.1 Verify SD Card Detection

1. Insert FAT32 formatted SD card into Adalogger
2. Power cycle the flight computer
3. Check OLED for SD card status indicator

### 5.2 Generate Test Data

1. Arm the flight computer
2. Wait 30 seconds
3. Disarm the flight computer

### 5.3 Verify Log Files

1. Power off the flight computer
2. Remove SD card and insert in computer
3. Navigate to `/flights/` directory
4. Open the most recent CSV file

**Expected CSV Format:**
```csv
# Rocket Flight Data
# Flight ID: abc123...
# Date: 2026-01-10T10:30:00
Time_ms,Altitude_m,Velocity_mps,Pressure_Pa,Temperature_C,State
0,0.52,0.00,101325.0,25.3,idle
10,0.53,0.01,101324.8,25.3,idle
...
```

**Pass Criteria:**
- [ ] SD card detected on boot
- [ ] Log file created in /flights/ directory
- [ ] CSV file contains valid header
- [ ] Data logged at ~100 Hz (10ms intervals)
- [ ] Values are reasonable

---

## Test 6: Range Test

### 6.1 Preparation

- Fully charged batteries in both units
- Clear line of sight test area
- Helper with the flight computer
- You at the gateway with laptop

### 6.2 Test Procedure

| Distance | Actions | Expected RSSI |
|----------|---------|---------------|
| 10m | Walk 10m away, verify telemetry | -40 to -50 dBm |
| 50m | Walk 50m away, verify telemetry | -55 to -65 dBm |
| 100m | Walk 100m away, verify telemetry | -65 to -75 dBm |
| 250m | Walk 250m away, verify telemetry | -75 to -85 dBm |
| 500m | Walk 500m away, verify telemetry | -85 to -95 dBm |

### 6.3 Link Loss Test

1. Continue walking until telemetry stops
2. Note the maximum distance achieved
3. Return toward gateway, verify link re-establishes

**Pass Criteria:**
- [ ] Reliable telemetry at 100m
- [ ] Telemetry received at 250m
- [ ] Maximum range documented
- [ ] Link re-establishes when returning

---

## Test 7: Swing Test (Simulated Flight)

### 7.1 Setup

1. Securely attach flight computer to a 2-3 meter rope/string
2. Ensure all connections are secure (use tape if needed)
3. Clear area of obstacles
4. Arm the flight computer

### 7.2 Test Procedure

1. Swing the flight computer in a vertical arc
2. Swing height: approximately 2-3 meters
3. Perform 5-10 swings
4. Observe telemetry on gateway

**Expected Behavior:**
- Altitude increases during upswing
- Velocity positive during upswing, negative during downswing
- State may transition to BOOST/COAST if thresholds exceeded

### 7.3 Verification

Check SD card log after test:
- Altitude curve should show oscillating pattern
- Velocity should oscillate between positive and negative
- Maximum altitude should match approximate swing height

**Pass Criteria:**
- [ ] Altitude tracks swing height
- [ ] Velocity shows correct direction
- [ ] State transitions occur appropriately
- [ ] No sensor glitches or dropouts

---

## Test 8: Full System Integration

### 8.1 Desktop App Test

1. Open the Rocket Avionics Desktop application
2. Select the gateway serial port from dropdown
3. Click "Connect"
4. Verify telemetry appears in the UI:
   - Altitude label updates
   - Velocity label updates
   - State label updates
   - RSSI/SNR displays
   - Chart shows altitude profile

### 8.2 Command Test via Desktop

1. Click "ARM" button
2. Confirm arm dialog
3. Verify flight computer arms
4. Click "DISARM" button
5. Verify flight computer disarms

**Pass Criteria:**
- [ ] Desktop app connects successfully
- [ ] Live telemetry displayed
- [ ] Altitude chart updates in real-time
- [ ] ARM command works
- [ ] DISARM command works
- [ ] State colors update correctly

---

## Test Results Summary

| Test | Result | Notes |
|------|--------|-------|
| 1.1 Flash Flight Computer | ☐ Pass ☐ Fail | |
| 1.2 Flash Ground Gateway | ☐ Pass ☐ Fail | |
| 2.1 Gateway Serial Connect | ☐ Pass ☐ Fail | |
| 2.2 Gateway Output | ☐ Pass ☐ Fail | |
| 2.3 Gateway Commands | ☐ Pass ☐ Fail | |
| 3.1 OLED Display | ☐ Pass ☐ Fail | |
| 3.2 Altitude Response | ☐ Pass ☐ Fail | |
| 3.3 Pressure Seal | ☐ Pass ☐ Fail | |
| 4.1 LoRa Basic Link | ☐ Pass ☐ Fail | RSSI: ___ dBm |
| 4.2 Arm/Disarm | ☐ Pass ☐ Fail | |
| 5.1 SD Card Detection | ☐ Pass ☐ Fail | |
| 5.2 Test Data Generation | ☐ Pass ☐ Fail | |
| 5.3 Log File Verification | ☐ Pass ☐ Fail | |
| 6. Range Test | ☐ Pass ☐ Fail | Max: ___ m |
| 7. Swing Test | ☐ Pass ☐ Fail | |
| 8.1 Desktop App | ☐ Pass ☐ Fail | |
| 8.2 Desktop Commands | ☐ Pass ☐ Fail | |

**Tested By:** _______________
**Date:** _______________
**Firmware Version:** _______________

---

## Troubleshooting

### No RPI-RP2 Drive Appears
- Try a different USB cable (data cable, not charge-only)
- Try a different USB port
- Hold BOOTSEL before plugging in, not after

### No Serial Port Appears
- Check USB cable
- Verify firmware was flashed successfully
- Try `ls /dev/cu.*` to see all serial devices

### No LoRa Telemetry Received
- Verify both devices have antennas attached (if external)
- Check that both are using same frequency (915 MHz)
- Move devices closer together for initial test
- Check RSSI - if below -100 dBm, signal is too weak

### OLED Not Displaying
- Check I2C connections (SDA/SCL)
- Verify OLED address is 0x3C
- Check for I2C bus conflicts

### SD Card Not Detected
- Ensure card is FAT32 formatted
- Card must be ≤32GB for FAT32
- Try reformatting the card
- Check SPI connections

### Altitude Reading Stuck at 0
- Check BMP390 I2C connection
- Verify sensor address (0x77)
- Look for I2C errors on OLED

---

## Pre-Flight Checklist

Before any flight operation:

- [ ] Flight computer battery fully charged
- [ ] SD card inserted and formatted
- [ ] All sensors responding correctly
- [ ] LoRa link established with gateway
- [ ] RSSI above -80 dBm at pad
- [ ] Ground station laptop charged
- [ ] Desktop app running and connected
- [ ] Arm command tested and verified
- [ ] Weather conditions acceptable
- [ ] Recovery system verified
- [ ] Range safety officer notified
