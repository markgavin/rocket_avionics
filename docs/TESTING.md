# Rocket Avionics - Testing Procedures

## Overview

This document provides step-by-step procedures for testing the rocket avionics system before flight operations.

---

## Prerequisites

### Hardware Required
- [ ] Flight Computer: Feather RP2040 + RFM95 LoRa (Product 5714)
- [ ] Ground Gateway: Feather RP2040 + RFM95 LoRa (Product 5714)
- [ ] BMP390 Barometric Sensor (Product 4816) for flight computer via STEMMA QT
- [ ] BMP390 Barometric Sensor (Product 4816) for gateway via STEMMA QT
- [ ] GPS Module (UART, 9600 baud) connected to flight computer
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
   GPS: No Fix (0 sat)
   ```

### 3.2 Pressure Value Check

1. Connect to gateway serial port
2. Observe telemetry JSON output
3. **Expected Values:**
   - `pres`: ~101,000 Pa at sea level (varies with weather/elevation)
   - `gpres`: Similar to `pres` (gateway ground pressure)
   - `temp`: 15-35°C (indoor temperature)

**Warning:** If pressure reads ~85,000 Pa instead of ~101,000 Pa, the BMP390 may not be initializing correctly. Power cycle and check I2C connections.

### 3.3 Altitude Response Test

1. Note the current altitude reading
2. Raise the flight computer approximately 1 meter above the gateway
3. Wait 2-3 seconds for reading to stabilize
4. **Expected**:
   - `alt` changes slightly (absolute altitude)
   - `dalt` increases by approximately 1 meter (differential altitude)

### 3.4 Differential Altitude Test

1. Place flight computer and gateway at the same height
2. Observe `dalt` field in telemetry
3. **Expected**: `dalt` should be near 0.0 m (±0.5m)
4. Raise flight computer 2 meters above gateway
5. **Expected**: `dalt` should read approximately 2.0 m

### 3.5 Pressure Seal Test

1. Cup your hands around the BMP390 sensor (don't touch it)
2. Breathe gently toward the sensor
3. **Expected**: Altitude reading changes (warm breath = pressure change)

**Pass Criteria:**
- [ ] OLED displays current state and readings
- [ ] Pressure values are ~101,000 Pa at sea level
- [ ] Both flight and gateway BMP390 sensors working
- [ ] Differential altitude (`dalt`) responds to height changes
- [ ] Temperature reading is reasonable (15-35°C indoors)

---

## Test 4: GPS Verification

### 4.1 Indoor GPS Test (No Fix Expected)

1. Power on flight computer indoors
2. Observe OLED display
3. **Expected**: "GPS: No Fix (0 sat)" or similar
4. Check telemetry JSON: `"gps":false`, `"sat":0`

### 4.2 Outdoor GPS Test

1. Take flight computer outside with clear sky view
2. Wait 30-60 seconds for GPS cold start
3. **Expected**:
   - OLED shows satellite count increasing
   - Eventually shows "GPS: Fix (N sat)" where N >= 3
4. Check telemetry JSON:
   - `"gps":true`
   - `"sat":` 3 or higher
   - `"lat":` valid latitude (e.g., 39.938)
   - `"lon":` valid longitude (e.g., -75.271)
   - `"gspd":` ground speed in m/s
   - `"hdg":` heading in degrees

### 4.3 GPS Movement Test

1. Walk with flight computer (GPS must have fix)
2. Observe telemetry JSON
3. **Expected**:
   - `gspd` shows walking speed (~1-2 m/s)
   - `hdg` shows direction of travel
   - `lat`/`lon` update as you move

**Pass Criteria:**
- [ ] GPS shows no fix indoors (expected)
- [ ] GPS acquires fix outdoors within 60 seconds
- [ ] Satellite count is 3 or more with fix
- [ ] Coordinates are valid for your location
- [ ] Ground speed responds to movement
- [ ] Heading responds to direction change

---

## Test 5: LoRa Communication

### 5.1 Basic Link Test

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

### 5.2 Arm/Disarm Test

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

## Test 6: SD Card Logging

### 6.1 Verify SD Card Detection

1. Insert FAT32 formatted SD card into Adalogger
2. Power cycle the flight computer
3. Check OLED for SD card status indicator

### 6.2 Generate Test Data

1. Arm the flight computer
2. Wait 30 seconds
3. Disarm the flight computer

### 6.3 Verify Log Files

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

## Test 7: Range Test

### 7.1 Preparation

- Fully charged batteries in both units
- Clear line of sight test area
- Helper with the flight computer
- You at the gateway with laptop

### 8.2 Test Procedure

| Distance | Actions | Expected RSSI |
|----------|---------|---------------|
| 10m | Walk 10m away, verify telemetry | -40 to -50 dBm |
| 50m | Walk 50m away, verify telemetry | -55 to -65 dBm |
| 100m | Walk 100m away, verify telemetry | -65 to -75 dBm |
| 250m | Walk 250m away, verify telemetry | -75 to -85 dBm |
| 500m | Walk 500m away, verify telemetry | -85 to -95 dBm |

### 7.3 Link Loss Test

1. Continue walking until telemetry stops
2. Note the maximum distance achieved
3. Return toward gateway, verify link re-establishes

**Pass Criteria:**
- [ ] Reliable telemetry at 100m
- [ ] Telemetry received at 250m
- [ ] Maximum range documented
- [ ] Link re-establishes when returning

---

## Test 8: Swing Test (Simulated Flight)

### 8.1 Setup

1. Securely attach flight computer to a 2-3 meter rope/string
2. Ensure all connections are secure (use tape if needed)
3. Clear area of obstacles
4. Arm the flight computer

### 8.2 Test Procedure

1. Swing the flight computer in a vertical arc
2. Swing height: approximately 2-3 meters
3. Perform 5-10 swings
4. Observe telemetry on gateway

**Expected Behavior:**
- Altitude increases during upswing
- Velocity positive during upswing, negative during downswing
- State may transition to BOOST/COAST if thresholds exceeded

### 8.3 Verification

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

## Test 9: Full System Integration

### 9.1 Desktop App Test

1. Open the Rocket Avionics Desktop application
2. Select the gateway serial port from dropdown
3. Click "Connect"
4. Verify telemetry appears in the UI:
   - Altitude label updates
   - Velocity label updates
   - State label updates
   - RSSI/SNR displays
   - Chart shows altitude profile

### 9.2 Command Test via Desktop

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
| 3.2 Pressure Values | ☐ Pass ☐ Fail | Flight: ___ Pa, Gateway: ___ Pa |
| 3.3 Altitude Response | ☐ Pass ☐ Fail | |
| 3.4 Differential Altitude | ☐ Pass ☐ Fail | |
| 3.5 Pressure Seal | ☐ Pass ☐ Fail | |
| 4.1 GPS Indoor (No Fix) | ☐ Pass ☐ Fail | |
| 4.2 GPS Outdoor (Fix) | ☐ Pass ☐ Fail | Satellites: ___ |
| 4.3 GPS Movement | ☐ Pass ☐ Fail | |
| 5.1 LoRa Basic Link | ☐ Pass ☐ Fail | RSSI: ___ dBm |
| 5.2 Arm/Disarm | ☐ Pass ☐ Fail | |
| 6.1 SD Card Detection | ☐ Pass ☐ Fail | |
| 6.2 Test Data Generation | ☐ Pass ☐ Fail | |
| 6.3 Log File Verification | ☐ Pass ☐ Fail | |
| 7. Range Test | ☐ Pass ☐ Fail | Max: ___ m |
| 8. Swing Test | ☐ Pass ☐ Fail | |
| 9.1 Desktop App | ☐ Pass ☐ Fail | |
| 9.2 Desktop Commands | ☐ Pass ☐ Fail | |

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
- [ ] BMP390 sensors responding (both flight and gateway)
- [ ] Pressure readings ~101,000 Pa at sea level
- [ ] GPS has fix with 3+ satellites
- [ ] GPS coordinates match launch site location
- [ ] LoRa link established with gateway
- [ ] RSSI above -80 dBm at pad
- [ ] Differential altitude near 0m with devices level
- [ ] Ground station laptop charged
- [ ] Desktop app running and connected
- [ ] Arm command tested and verified
- [ ] Weather conditions acceptable
- [ ] Recovery system verified
- [ ] Range safety officer notified
