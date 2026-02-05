# Field Operations Guide

## Pre-Flight Checklist

### Equipment

- [ ] Flight computer (charged LiPo battery)
- [ ] Ground gateway (Heltec Wireless Tracker or RP2040)
- [ ] Phone/tablet with companion app
- [ ] USB cable (for emergency re-flash)
- [ ] Laptop (optional, for desktop app)

### Setup Sequence

1. **Power on gateway first** - allows it to establish WiFi AP and start listening
2. **Power on flight computer** - watch for startup sequence:
   - NeoPixel flashes during init
   - OLED shows splash screen (2 seconds)
   - OLED shows device info (2 seconds)
   - OLED shows live screen
3. **Verify sensor status** on Device Info screen:
   - Baro: BMP390 (primary) or BMP581
   - LoRa: OK
   - IMU: OK
   - GPS: OK (may take 30+ seconds for first fix)
4. **Connect app to gateway** via WiFi (`RocketGateway` AP or local network)
5. **Verify telemetry** - confirm data appears in app at 0.5 Hz
6. **Check GPS fix** - wait for satellite count > 3 on GPS screen

### Rocket ID

If flying multiple rockets:
1. Navigate to Rocket ID screen (Button C to cycle)
2. Press Button B to cycle through IDs (0-15)
3. ID is saved to flash automatically
4. Verify correct ID appears in telemetry

## Arming Procedure

1. **Verify IDLE state** on OLED display
2. **Send arm command** from app or gateway CLI:
   ```
   {"cmd":"arm","rocket":0}
   ```
3. **Confirm ARMED state:**
   - OLED shows "ARMED" with ground pressure
   - NeoPixel blinks yellow at 2 Hz
   - Telemetry rate increases to 2 Hz
4. **Ground reference is set** - do not move rocket after arming

### Important Notes
- Ground pressure reference is captured at arm time
- The complementary filter's accelerometer bias (learned during idle) is preserved
- Altitude and velocity counters are zeroed
- Arm only works from IDLE or COMPLETE states
- Disarm only works from ARMED state (cannot disarm during flight)

## Launch Detection

Launch is automatically detected when either:
- Barometric altitude exceeds 10 meters AGL, OR
- Vertical velocity exceeds 10 m/s

Upon detection:
- State transitions to BOOST
- NeoPixel blinks red at 10 Hz
- Telemetry increases to 10 Hz
- Flash recording starts (GPS launch coordinates saved)
- Mission timer begins

## During Flight

No operator action required. The state machine handles all transitions:

```
BOOST -> COAST -> APOGEE -> DESCENT -> LANDED
```

Telemetry streams at 10 Hz with full sensor data (baro, IMU, GPS).

## Post-Flight

### Recovery

1. **Check last known GPS coordinates** in app
2. **NeoPixel blinks white** at 0.5 Hz (LANDED state)
3. **Telemetry beacons** at 1 Hz for tracking
4. **Retrieve rocket**

### Data Download

Flight data can be downloaded via LoRa:

1. **List stored flights:**
   ```
   {"cmd":"flash_list"}
   ```
2. **Download flight header:**
   ```
   {"cmd":"flash_read","slot":0,"start":4294967295}
   ```
3. **Download flight samples:**
   ```
   {"cmd":"flash_read","slot":0,"start":0}
   ```
   Samples are sent 3 per packet. Request successive chunks by incrementing start index.

### Reset for Next Flight

1. **Send reset command:**
   ```
   {"cmd":"reset","rocket":0}
   ```
2. **Verify IDLE state** on display
3. **Optionally delete flight data** to free storage:
   ```
   {"cmd":"flash_delete","slot":0}
   ```
   Or delete all:
   ```
   {"cmd":"flash_delete","slot":255}
   ```

## Disarming

If launch is cancelled while ARMED:

```
{"cmd":"disarm","rocket":0}
```

Returns to IDLE state. Cannot disarm during active flight.

## Orientation Testing

For pre-flight orientation verification:

1. **Enable orientation mode:**
   ```
   {"cmd":"orientation","enable":true}
   ```
2. Telemetry increases to 10 Hz in IDLE state
3. Verify IMU axes are correct (pitch, roll, heading)
4. Mode auto-disables after 30 seconds

## Troubleshooting

### No Telemetry

- Check gateway is powered and WiFi connected
- Verify LoRa frequency match (915 MHz)
- Check rocket ID matches app target
- Verify LoRa shows "OK" on device info screen

### GPS No Fix

- Ensure clear sky view (not indoors)
- Wait up to 60 seconds for cold start
- Check satellite count on GPS screen (need >= 4 for fix)
- GPS fix timeout is 3 seconds; coordinates go stale after timeout

### Sensors Not Detected

- Power cycle the flight computer
- Check I2C connections (STEMMA QT cables, FeatherWing seating)
- Verify on Device Info screen which sensors are OK

### Flash Storage Full

- Maximum 7 flights stored
- Delete old flights via `flash_delete` command
- Each flight uses one 64KB slot regardless of duration

### Gateway IP Changed

After gateway reboot, its IP may change:
```bash
ping RocketGateway.local
```
Use the new IP for OTA flashing or direct TCP connections.

## Safety Reminders

- Always arm last, just before launch
- Verify IDLE state before handling the rocket
- Disarm if launch is scrubbed
- Do not rely solely on GPS for recovery - note launch location visually
- The flight computer has no hardware arm switch in the current build; arming is software-only via LoRa command
- Pyro channels are reserved but not yet implemented
