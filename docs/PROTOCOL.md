# Rocket Avionics Communication Protocol

## Overview

The Rocket Avionics system uses a two-layer communication protocol:

1. **LoRa Layer** (Flight Computer ↔ Gateway): Binary packets for efficiency
2. **USB Layer** (Gateway ↔ Desktop): JSON messages for compatibility

```
┌──────────────┐      LoRa       ┌─────────────┐      USB        ┌─────────────┐
│    Flight    │◄──────────────► │   Ground    │◄───────────────►│   Desktop   │
│   Computer   │   915 MHz       │   Gateway   │   JSON/Serial   │     App     │
│              │   Binary        │             │                 │             │
└──────────────┘                 └─────────────┘                 └─────────────┘
```

---

## LoRa Protocol (Binary)

### Configuration

| Parameter | Value | Notes |
|-----------|-------|-------|
| Frequency | 915 MHz | North America ISM band |
| Spreading Factor | SF7 | Fastest data rate |
| Bandwidth | 125 kHz | Standard |
| Coding Rate | 4/5 | Good error correction |
| Sync Word | 0x14 | Private network |
| TX Power | 20 dBm | Maximum legal power |

### Packet Format

All LoRa packets share a common header:

```c
struct LoRaPacketHeader {
    uint8_t  magic;      // 0xAF (Avionics Flight)
    uint8_t  type;       // Packet type
    uint16_t sequence;   // Sequence number
};
```

### Packet Types

| Type | Value | Direction | Description |
|------|-------|-----------|-------------|
| Telemetry | 0x01 | Flight → Ground | Real-time flight data |
| Status | 0x02 | Flight → Ground | System status response |
| Command | 0x03 | Ground → Flight | Control commands |
| Ack | 0x04 | Flight → Ground | Command acknowledgment |
| Data | 0x05 | Flight → Ground | Bulk data transfer |
| Info | 0x06 | Flight → Ground | Device info response |
| Flash List | 0x07 | Flight → Ground | Stored flight list |
| Flash Data | 0x08 | Flight → Ground | Flight data download |
| Baro Compare | 0x09 | Flight → Ground | Dual barometer comparison (debug) |

### Telemetry Packet (42 bytes)

Sent at 10 Hz during flight.

```c
struct LoRaTelemetryPacket {
    uint8_t  magic;           // 0xAF
    uint8_t  type;            // 0x01
    uint16_t sequence;        // Packet counter
    uint32_t time_ms;         // Mission time (ms)
    int32_t  altitude_cm;     // Altitude (centimeters)
    int16_t  velocity_cmps;   // Velocity (cm/s)
    uint32_t pressure_pa;     // Pressure (Pascals)
    int16_t  temp_c10;        // Temperature * 10
    int16_t  accel_x;         // Accelerometer X
    int16_t  accel_y;         // Accelerometer Y
    int16_t  accel_z;         // Accelerometer Z
    int32_t  latitude;        // Latitude * 1e6 (microdegrees)
    int32_t  longitude;       // Longitude * 1e6 (microdegrees)
    uint16_t ground_speed;    // Ground speed (cm/s)
    uint16_t heading;         // Heading * 10 (decidegrees)
    uint8_t  satellites;      // GPS satellite count
    uint8_t  state;           // Flight state
    uint8_t  flags;           // Status flags
    uint8_t  crc;             // CRC-8 checksum
};
```

### Status Flags

| Bit | Name | Description |
|-----|------|-------------|
| 0 | PYRO1_CONT | Pyro channel 1 continuity OK |
| 1 | PYRO2_CONT | Pyro channel 2 continuity OK |
| 2 | SD_LOGGING | SD card logging active |
| 3 | LOW_BATTERY | Battery voltage low |
| 4 | GPS_LOCK | GPS has valid fix |
| 5 | SENSOR_OK | All sensors operational |

### Flight States

| Value | State | Description |
|-------|-------|-------------|
| 0 | IDLE | Pre-flight, on pad |
| 1 | ARMED | Armed, awaiting launch |
| 2 | BOOST | Motor burning |
| 3 | COAST | Coasting to apogee |
| 4 | APOGEE | At peak altitude |
| 5 | DESCENT | Descending |
| 6 | LANDED | On ground |
| 7 | COMPLETE | Data downloaded |

### Command Packet (8 bytes)

```c
struct LoRaCommandPacket {
    uint8_t  magic;           // 0xAF
    uint8_t  type;            // 0x03
    uint16_t sequence;        // Command ID
    uint8_t  command;         // Command code
    uint8_t  param1;          // Parameter 1
    uint8_t  param2;          // Parameter 2
    uint8_t  crc;             // CRC-8
};
```

### Command Codes

| Code | Command | Parameters | Description |
|------|---------|------------|-------------|
| 0x01 | ARM | - | Arm flight computer |
| 0x02 | DISARM | - | Disarm flight computer |
| 0x03 | STATUS | - | Request status |
| 0x04 | RESET | - | Reset to idle |
| 0x05 | DOWNLOAD | - | Request data download |
| 0x06 | PING | - | Ping for connectivity check |
| 0x07 | INFO | - | Request device info (version, sensors, state) |
| 0x08 | ORIENT_MODE | 1 byte | Enable/disable orientation test mode |
| 0x09 | SET_NAME | string | Set rocket name (null-terminated) |
| 0x0A | BARO_COMPARE | - | Toggle baro comparison stream (debug) |
| 0x10 | SD_LIST | - | List SD card flights |
| 0x11 | SD_READ | - | Read SD card flight |
| 0x12 | SD_DELETE | - | Delete SD card flight |
| 0x20 | FLASH_LIST | - | List flash-stored flights |
| 0x21 | FLASH_READ | flight# | Read flash flight data |
| 0x22 | FLASH_DELETE | flight# | Delete flash flight |

---

## USB/JSON Protocol

The gateway converts LoRa binary packets to JSON for the desktop application.

### Message Format

All messages are single-line JSON terminated with newline (`\n`).

```json
{"type":"message_type","field1":"value1","field2":123}
```

### Telemetry Message

Sent to desktop at 10 Hz during flight.

```json
{
    "type": "tel",
    "seq": 1234,
    "t": 5000,
    "alt": 152.5,
    "dalt": 152.3,
    "vel": 45.2,
    "pres": 99500,
    "gpres": 101325,
    "galt": 0.2,
    "temp": 22.5,
    "lat": 39.938126,
    "lon": -75.271606,
    "gspd": 0.5,
    "hdg": 180.0,
    "sat": 8,
    "gps": true,
    "state": "boost",
    "flags": 16,
    "rssi": -45,
    "snr": 10
}
```

| Field | Type | Description |
|-------|------|-------------|
| type | string | Message type ("tel" for telemetry) |
| seq | int | Sequence number |
| t | int | Mission time (milliseconds) |
| alt | float | Altitude above sea level (meters) |
| dalt | float | Differential altitude - flight above gateway (meters) |
| vel | float | Vertical velocity (m/s) |
| pres | int | Flight computer pressure (Pascals) |
| gpres | int | Gateway ground pressure (Pascals) |
| galt | float | Gateway altitude above sea level (meters) |
| temp | float | Temperature (Celsius) |
| lat | float | GPS latitude (degrees) |
| lon | float | GPS longitude (degrees) |
| gspd | float | GPS ground speed (m/s) |
| hdg | float | GPS heading (degrees) |
| sat | int | GPS satellite count |
| gps | bool | GPS has valid fix |
| state | string | Flight state |
| flags | int | Status flags bitmask |
| rssi | int | LoRa signal strength (dBm) |
| snr | int | LoRa signal-to-noise ratio (dB) |

### Status Message

Response to status request or periodic update.

```json
{
    "type": "status",
    "id": 1,
    "state": "armed",
    "alt_m": 0.5,
    "ground_pres_pa": 101325,
    "battery_v": 4.1,
    "sd_ok": true,
    "samples": 0,
    "rssi": -45,
    "firmware": "1.0.0"
}
```

### Gateway Status Message

Status of the ground gateway itself.

```json
{
    "type": "gateway_status",
    "connected": true,
    "flight_rssi": -55,
    "packets_received": 1234,
    "packets_lost": 5,
    "firmware": "1.0.0"
}
```

### Command Messages (Desktop → Gateway)

#### Arm Command
```json
{"cmd": "arm", "id": 1}
```

#### Disarm Command
```json
{"cmd": "disarm", "id": 2}
```

#### Status Request
```json
{"cmd": "status", "id": 3}
```

#### Reset Command
```json
{"cmd": "reset", "id": 4}
```

#### Download Request
```json
{"cmd": "download", "id": 5}
```

### Command Response

```json
{
    "type": "response",
    "id": 1,
    "success": true,
    "message": "Armed successfully"
}
```

### Error Response

```json
{
    "type": "error",
    "id": 1,
    "code": "NOT_READY",
    "message": "Cannot arm: sensors not ready"
}
```

### Flight Data (Chunked Download)

Flight data is transferred in chunks after landing.

#### Data Request
```json
{"cmd": "download", "id": 10, "chunk": 0}
```

#### Data Response
```json
{
    "type": "flight_data",
    "id": 10,
    "chunk": 0,
    "total_chunks": 12,
    "flight_id": "20260110_143052",
    "samples": [
        {"t_ms": 0, "alt_m": 0.0, "vel_mps": 0.0, "pres_pa": 101325},
        {"t_ms": 100, "alt_m": 5.2, "vel_mps": 52.0, "pres_pa": 101260},
        {"t_ms": 200, "alt_m": 15.8, "vel_mps": 98.5, "pres_pa": 101150}
    ]
}
```

### Flight Summary

Sent after flight completion.

```json
{
    "type": "flight_summary",
    "flight_id": "20260110_143052",
    "timestamp": "2026-01-10T14:30:52Z",
    "max_altitude_m": 285.4,
    "apogee_time_ms": 8500,
    "max_velocity_mps": 125.3,
    "flight_time_ms": 42000,
    "sample_count": 420,
    "ground_pressure_pa": 101325
}
```

---

## Error Codes

| Code | Description |
|------|-------------|
| NONE | No error |
| NOT_READY | System not ready |
| ALREADY_ARMED | Already in armed state |
| NOT_ARMED | Not in armed state |
| IN_FLIGHT | Cannot perform action during flight |
| SENSOR_FAIL | Sensor failure |
| LORA_FAIL | LoRa communication failure |
| SD_FAIL | SD card failure |
| INVALID_CMD | Invalid command |
| TIMEOUT | Operation timed out |

---

## Timing

| Parameter | Value | Notes |
|-----------|-------|-------|
| Telemetry Rate | 10 Hz | During flight |
| Status Poll | 1 Hz | Pre-flight |
| Command Timeout | 2 seconds | Retry if no ack |
| Download Timeout | 5 seconds | Per chunk |

---

## CRC-8 Calculation

Simple CRC-8 for packet validation.

```c
uint8_t crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0xFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc <<= 1;
        }
    }
    return crc;
}
```

---

## State Transitions

```
                    ┌───────────────┐
                    │     IDLE      │◄────────────────┐
                    └───────┬───────┘                 │
                            │ arm                     │
                            ▼                         │
                    ┌───────────────┐                 │
                    │    ARMED      │─── disarm ──────┤
                    └───────┬───────┘                 │
                            │ launch detect           │
                            ▼                         │
                    ┌───────────────┐                 │
                    │    BOOST      │                 │
                    └───────┬───────┘                 │
                            │ burnout                 │
                            ▼                         │
                    ┌───────────────┐                 │
                    │    COAST      │                 │
                    └───────┬───────┘                 │
                            │ velocity ≤ 0            │
                            ▼                         │
                    ┌───────────────┐                 │
                    │    APOGEE     │                 │
                    └───────┬───────┘                 │
                            │ immediate               │
                            ▼                         │
                    ┌───────────────┐                 │
                    │   DESCENT     │                 │
                    └───────┬───────┘                 │
                            │ stationary              │
                            ▼                         │
                    ┌───────────────┐                 │
                    │    LANDED     │                 │
                    └───────┬───────┘                 │
                            │ download complete       │
                            ▼                         │
                    ┌───────────────┐                 │
                    │   COMPLETE    │─── reset ───────┘
                    └───────────────┘
```
