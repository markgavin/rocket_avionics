# Flash Storage Format

## Overview

Flight data is stored in the RP2040's onboard 8MB flash memory. The last 512KB is reserved for flight storage, organized into an index sector and 7 flight slots of 64KB each. Data is buffered in RAM during flight and written to flash after landing.

Source: `firmware_flight/src/flight_storage.c`, `firmware_flight/include/flight_storage.h`

## Memory Layout

```
Flash Address    Offset       Size    Contents
─────────────────────────────────────────────────────
0x10000000       0x000000     ~7.5MB  Firmware (program + data)
    ...
0x10780000       0x780000     64KB    Flight Slot 0
0x10790000       0x790000     64KB    Flight Slot 1
0x107A0000       0x7A0000     64KB    Flight Slot 2
0x107B0000       0x7B0000     64KB    Flight Slot 3
0x107C0000       0x7C0000     64KB    Flight Slot 4
0x107D0000       0x7D0000     64KB    Flight Slot 5
0x107E0000       0x7E0000     64KB    Flight Slot 6
0x107FE000       0x7FE000     4KB     Flight Index
0x107FF000       0x7FF000     4KB     Calibration / Settings
```

Total storage area: 512KB (`kFlightStorageSize = 0x80000`)

## Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `kFlashTotalSize` | 0x800000 (8MB) | Total flash on Feather RP2040 |
| `kFlightStorageSize` | 0x80000 (512KB) | Reserved for flight data |
| `kFlightStorageOffset` | 0x780000 | Start of flight storage |
| `kFlightSlotSize` | 0x10000 (64KB) | Size per flight slot |
| `kMaxStoredFlights` | 7 | Maximum stored flights |
| `kFlightIndexOffset` | 0x7FE000 | Index sector offset |
| `kCalibrationOffset` | 0x7FF000 | Settings/calibration sector |
| `FLASH_SECTOR_SIZE` | 4096 (4KB) | Flash erase granularity |
| `FLASH_PAGE_SIZE` | 256 | Flash program granularity |
| `kMaxSamplesPerFlight` | 1200 | Buffer limit (~120 seconds at 10 Hz) |

## Flight Index

The index sector tracks which slots are in use and the next flight ID.

```
Offset  Size    Field
──────────────────────────────────
0       4       Magic (0x58444E49 = "INDX")
4       4       Version (1)
8       4       Next flight ID (uint32)
12      7       Slot usage flags (1 byte per slot, 0=free, 1=used)
19      4       Checksum (sum of bytes 0-18)
```

The index is stored in the first 256-byte page of its 4KB sector. The entire sector is erased and rewritten on every update.

## Flight Header (80 bytes)

Each flight slot begins with a header in the first flash page (256 bytes):

```c
typedef struct __attribute__((packed))
{
  uint32_t pMagic ;             // 0x54484746 ("FGHT")
  uint32_t pVersion ;           // 1
  uint32_t pFlightId ;          // Sequential flight ID
  uint32_t pTimestamp ;         // Unix timestamp (if available)
  uint32_t pSampleCount ;      // Number of samples recorded

  float pMaxAltitudeM ;         // Peak altitude (meters AGL)
  float pMaxVelocityMps ;       // Peak velocity (m/s)
  uint32_t pApogeeTimeMs ;      // Time to apogee from launch
  uint32_t pFlightTimeMs ;      // Total flight duration (ms)
  float pGroundPressurePa ;     // Ground reference pressure

  int32_t pLaunchLatitude ;     // GPS latitude (microdegrees)
  int32_t pLaunchLongitude ;    // GPS longitude (microdegrees)

  uint8_t pReserved[20] ;       // Reserved for future use
  uint32_t pChecksum ;          // Sum of all preceding bytes
} FlightHeader ;                // 80 bytes
```

## Flight Sample (52 bytes)

Samples are stored sequentially starting at the second flash page (offset 256 within the slot):

```c
typedef struct __attribute__((packed))
{
  uint32_t pTimeMs ;            // Time since launch (ms)

  int32_t pAltitudeCm ;         // Altitude in centimeters
  int16_t pVelocityCmps ;       // Velocity in cm/s
  uint32_t pPressurePa ;        // Pressure in Pascals
  int16_t pTemperatureC10 ;     // Temperature * 10

  int32_t pGpsLatitude ;        // Latitude (microdegrees)
  int32_t pGpsLongitude ;       // Longitude (microdegrees)
  int16_t pGpsSpeedCmps ;       // Ground speed in cm/s
  uint16_t pGpsHeadingDeg10 ;   // Heading * 10
  uint8_t pGpsSatellites ;      // Satellite count

  int16_t pAccelX ;             // Accelerometer X (milli-g)
  int16_t pAccelY ;             // Accelerometer Y (milli-g)
  int16_t pAccelZ ;             // Accelerometer Z (milli-g)

  int16_t pGyroX ;              // Gyroscope X (0.1 dps)
  int16_t pGyroY ;              // Gyroscope Y (0.1 dps)
  int16_t pGyroZ ;              // Gyroscope Z (0.1 dps)

  int16_t pMagX ;               // Magnetometer X (milligauss)
  int16_t pMagY ;               // Magnetometer Y (milligauss)
  int16_t pMagZ ;               // Magnetometer Z (milligauss)

  uint8_t pState ;              // Flight state
} FlightSample ;                // 52 bytes
```

## Capacity

| Metric | Value |
|--------|-------|
| Bytes per sample | 52 |
| Samples per 64KB slot | ~1,230 (64,000 / 52) |
| RAM buffer limit | 1,200 samples |
| Max recording time | 120 seconds at 10 Hz |
| Typical 60s flight | 600 samples = ~31 KB |
| Typical 30s flight | 300 samples = ~16 KB |

## Recording Lifecycle

### 1. Start Recording (launch detected)

```
FlightStorage_StartFlight(groundPressure, launchLat, launchLon)
```

- Find first free slot in index
- Initialize header with flight ID, ground pressure, GPS launch coordinates
- Reset RAM sample buffer
- Returns flight ID (0 if storage full)

### 2. Log Samples (during flight, 10 Hz)

```
FlightStorage_LogSample(&sample)
```

- Copies sample to RAM buffer (`sSampleBuffer[]`)
- Increments sample count
- Returns false if buffer full (>1200 samples)
- No flash writes during flight (all in RAM)

### 3. End Recording (landing detected)

```
FlightStorage_EndFlight(maxAlt, maxVel, apogeeTime, flightTime)
```

- Updates header with flight results
- Calculates header checksum
- Erases required flash sectors (only sectors needed, not entire slot)
- Writes header to first page
- Writes samples page-by-page (256 bytes per page)
- Updates index: marks slot as used, increments next flight ID
- All flash operations done with interrupts disabled

## Flash Write Safety

Flash operations disable all interrupts to prevent corruption:

```c
uint32_t theInterrupts = save_and_disable_interrupts() ;
flash_range_erase(offset, size) ;
flash_range_program(offset, data, FLASH_PAGE_SIZE) ;
restore_interrupts(theInterrupts) ;
```

This means:
- No LoRa communication during flash writes
- No sensor reads during flash writes
- Writes only happen after landing, so flight safety is unaffected
- Erase is done in minimum sectors needed (4KB granularity)

## LoRa Download Protocol

Flight data can be downloaded wirelessly via LoRa commands:

### List Flights (`kCmdFlashList` = 0x20)

Response packet (`kLoRaPacketStorageList` = 0x06):
```
magic(1), type(1), count(1), then per flight:
  slot(1), flightId(4), maxAltCm(4), flightTimeMs(4), sampleCount(4)
```

### Read Flight Header (`kCmdFlashRead` = 0x21, startSample = 0xFFFFFFFF)

Response packet (`kLoRaPacketStorageData` = 0x07):
```
magic(1), type(1), slot(1), startSample=0xFFFFFFFF(4), headerData(80)
```

### Read Flight Samples (`kCmdFlashRead` = 0x21)

Request: `slot(1), startSample(4)`

Response packet: 3 samples per packet (52 bytes each = 156 bytes data):
```
magic(1), type(1), slot(1), startSample(4), totalSamples(4), count(1), samples(N*52)
```

### Delete Flight (`kCmdFlashDelete` = 0x22)

- Single slot: `slot(1)` - erases 64KB and updates index
- All flights: `slot = 0xFF` - erases all used slots

## Rocket ID and Name Storage

The calibration sector (0x7FF000) stores persistent settings via the `storage.c` module:
- Rocket ID (0-15): stored as single byte
- Rocket name: stored as null-terminated string (max `kRocketNameMaxLen`)
- Both persist across power cycles and firmware updates
