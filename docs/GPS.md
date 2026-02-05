# GPS Integration

## Overview

The flight computer uses an Adafruit Ultimate GPS FeatherWing (PA1616D module) connected via UART for position tracking, ground speed, heading, and UTC time. GPS data is included in telemetry packets and flash-stored flight recordings.

Source: `firmware_flight/src/gps.c`, `firmware_flight/include/gps.h`

## Hardware

| Parameter | Value |
|-----------|-------|
| Module | PA1616D (MediaTek MT3339) |
| Product | Adafruit Ultimate GPS FeatherWing (PID 3133) |
| Interface | UART0 at 9600 baud, 8N1 |
| TX Pin | GP0 (UART0 TX -> GPS RX) |
| RX Pin | GP1 (UART0 RX -> GPS TX) |
| Update Rate | 1 Hz (configurable up to 10 Hz) |
| Channels | 22 tracking, 66 searching |
| Sensitivity | -165 dBm |
| Antenna | Built-in patch antenna (external SMA optional) |
| Enable | Directly wired to GND (always on) |

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
