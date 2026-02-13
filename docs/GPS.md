# GPS Integration

## Overview

The flight computer uses an Adafruit Ultimate GPS module connected via UART for position tracking, ground speed, heading, and UTC time. GPS data is included in telemetry packets and flash-stored flight recordings.

Source: `firmware_flight/src/gps.c`, `firmware_flight/include/gps.h`

## Supported Modules

Two Adafruit Ultimate GPS modules are supported:

| | FeatherWing (PID 3133) | Breakout (PID 5440) |
|---|---|---|
| Chipset | MT3339 | MTK3333 |
| GNSS | GPS only | GPS + GLONASS |
| Channels | 22 tracking / 66 searching | 33 tracking / 99 searching |
| Sensitivity | -165 dBm | -165 dBm |
| Accuracy | < 3 m | < 3 m |
| Antenna | Built-in patch | Built-in patch + uFL for external |
| Enable Pin | Hardwired always-on | EN pin (active high, internal pull-up) |
| PPS | Available | Available |
| Backup Battery | CR1220 footprint | CR1220 footprint |
| NMEA Prefix | `$GP` | `$GN` (multi-GNSS) or `$GP` |

**Recommended:** The Breakout (PID 5440) is preferred for rocket flights because:
- GPS + GLONASS provides faster fix acquisition and more satellites
- uFL connector allows an external active antenna for better reception inside a rocket airframe
- CR1220 backup battery enables warm starts (faster fix after power cycle)

The NMEA parser handles both `$GP` and `$GN` prefixes — it matches sentence types at offset +3 (e.g., `GGA`, `RMC`), so the talker ID is ignored.

## Hardware Interface

| Parameter | Value |
|-----------|-------|
| Interface | UART0 at 9600 baud, 8N1 |
| TX Pin | GP0 (UART0 TX → GPS RX) |
| RX Pin | GP1 (UART0 RX → GPS TX) |
| Logic Level | 3.3V (5V-safe input) |
| Power | 3.0–5.5V, ~29 mA tracking / 34 mA acquisition |
| Update Rate | 1 Hz (configurable up to 10 Hz) |

## Wiring: GPS Breakout (PID 5440) to Feather RP2040 RFM95

```
Adafruit Ultimate GPS              Feather RP2040
Breakout (PID 5440)                + RFM95 LoRa
┌──────────────────┐               ┌──────────────────┐
│                  │               │                  │
│  VIN ────────────────────────────── 3V3              │
│                  │               │                  │
│  GND ────────────────────────────── GND              │
│                  │               │                  │
│  TX  ────────────────────────────── GP1 (UART0 RX)   │
│                  │               │                  │
│  RX  ────────────────────────────── GP0 (UART0 TX)   │
│                  │               │                  │
│  EN  ── n/c (internal pull-up)   │                  │
│  FIX ── n/c (LED on breakout)    │                  │
│  PPS ── n/c (no free GPIO)       │                  │
│                  │               │                  │
└──────────────────┘               └──────────────────┘

4 wires: VIN, GND, TX, RX
```

**Notes:**
- **VIN** accepts 3.0–5.5V. Use Feather 3V3 pin (regulated) or USB pin (5V when USB connected).
- **TX/RX crossover:** GPS TX connects to Feather RX (GP1), GPS RX connects to Feather TX (GP0).
- **EN pin:** Leave unconnected — internal pull-up keeps GPS always enabled. On a carrier board with a free GPIO, EN can be driven LOW to put the GPS in standby for power savings.
- **PPS pin:** 1 pulse-per-second output for precision timing. Not used by current firmware (no free GPIO on the Feather). Can be connected on a carrier board for future use.
- **External antenna:** Connect via the uFL connector on the breakout. An active antenna is recommended for flights inside composite or fiberglass airframes.

## NMEA Sentence Parsing

The driver parses two NMEA sentence types:

### GGA - Position and Fix Quality

```
$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,47.0,M,,*47
```

Extracted fields:
- UTC time (hour, minute, second, milliseconds)
- Latitude and longitude (DDDMM.MMMM format, converted to decimal degrees)
- Fix quality (0=none, 1=GPS, 2=DGPS)
- Number of satellites in use
- HDOP (horizontal dilution of precision)
- Altitude above mean sea level

### RMC - Speed, Heading, and Date

```
$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
```

Extracted fields:
- Active/void status flag
- Ground speed (knots, converted to m/s using factor 0.514444)
- Track heading (degrees true)
- Date (day, month, year)

### Sentence Filtering

At initialization, the driver sends PMTK commands to configure the GPS:
- `PMTK220,1000` - Set 1 Hz update rate
- `PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0` - Output only GGA and RMC sentences

This reduces UART traffic and processing overhead.

## Coordinate Conversion

NMEA coordinates are in `DDDMM.MMMM` format (degrees and decimal minutes). The driver converts to decimal degrees:

```
decimal_degrees = degrees + (minutes / 60.0)
```

South latitudes and West longitudes are negated.

For telemetry transmission and flash storage, coordinates are stored as microdegrees (degrees * 1,000,000) in int32 format, providing ~0.11m resolution.

## Fix Validity and Timeout

The GPS data includes validity tracking:

| Field | Description |
|-------|-------------|
| `pValid` | true when fix quality >= GPS fix |
| `pLastFixTimeMs` | System timestamp of last valid GGA sentence |
| `pAge` | Milliseconds since last valid fix |

A fix is invalidated if the age exceeds `GPS_FIX_TIMEOUT_MS` (3 seconds). This prevents stale coordinates from being used if the GPS loses satellite lock.

## Data Structure

```c
typedef struct
{
  float pLatitude ;         // Decimal degrees (+ = North)
  float pLongitude ;        // Decimal degrees (+ = East)
  float pAltitudeMsl ;      // Altitude MSL (meters)

  float pSpeedKnots ;       // Ground speed (knots)
  float pSpeedMps ;         // Ground speed (m/s)
  float pHeadingDeg ;       // Track heading (0-360)

  GpsFixQuality pFixQuality ;
  uint8_t pSatellites ;
  float pHdop ;

  uint8_t pHour, pMinute, pSecond ;
  uint16_t pMilliseconds ;
  uint8_t pDay, pMonth ;
  uint16_t pYear ;

  bool pValid ;
  uint32_t pLastFixTimeMs ;
  uint32_t pAge ;
} GpsData ;
```

## Fix Quality Levels

| Value | Name | Description |
|-------|------|-------------|
| 0 | None | No fix |
| 1 | GPS | Standard GPS fix |
| 2 | DGPS | Differential GPS |
| 3 | PPS | Precise Positioning Service |
| 4 | RTK | Real-Time Kinematic |
| 5 | Float RTK | Float RTK solution |
| 6 | Estimated | Dead reckoning |
| 7 | Manual | Manual input |
| 8 | Simulation | Simulation mode |

## Integration with Flight System

### Telemetry
GPS data is included in every telemetry packet:
- Latitude/longitude as microdegrees (int32)
- Ground speed as cm/s (int16)
- Heading as degrees * 10 (uint16)
- Satellite count (uint8)
- GPS fix flag in status flags byte (bit 4)

### Flash Storage
GPS data recorded with each flight sample at 10 Hz:
- Launch coordinates stored in flight header (set at BOOST entry)
- Per-sample GPS: lat, lon, speed, heading, satellite count

### Display
GPS status shown on multiple OLED screens:
- Live screen: satellite count and fix indicator
- GPS Status screen: coordinates, speed, heading, satellite count, fix quality

### Main Loop
GPS is updated at 100 Hz in the main sensor read loop:
```c
if (sGpsOk)
{
  GPS_Update(theCurrentMs) ;
}
```

The `GPS_Update()` function reads all available UART bytes (non-blocking), buffers partial sentences, and processes complete NMEA sentences. At 9600 baud with 1 Hz updates, each update produces ~160 bytes of NMEA data.

## Checksum Verification

Every NMEA sentence includes an XOR checksum after the `*` delimiter. The driver verifies this checksum before processing:

```c
uint8_t theChecksum = 0 ;
// XOR all characters between $ and *
while (*ptr && *ptr != '*')
  theChecksum ^= *ptr++ ;
```

Sentences with invalid checksums are silently dropped.

## Sending Commands

The driver can send PMTK commands to the GPS module:

```c
GPS_SendCommand("PMTK220,500") ;  // Set 2 Hz update rate
```

The function automatically wraps the command with `$`, calculates the XOR checksum, and appends `*XX\r\n`.
