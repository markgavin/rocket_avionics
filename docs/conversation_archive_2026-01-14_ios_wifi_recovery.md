# Conversation Archive: iOS WiFi Connection & Rocket Recovery

**Date:** 2026-01-14
**Topic:** Adding WiFi AP mode to gateway and rocket recovery navigation to iOS app

---

## Overview

Implemented direct WiFi connectivity between the iOS app and gateway, plus iPhone GPS integration for locating landed rockets when walking to recover them.

---

## User Requirements

1. iOS app connects to gateway via WiFi (no desktop bridge needed)
2. Download flight data from flash storage on flight computer
3. Use iPhone's built-in GPS to navigate to rocket's last known position
4. Retain rocket GPS coordinates when WiFi connection is lost (offline recovery)
5. Support case where gateway is stationary, rocket has landed, and user walks to recover it

---

## Hardware Configuration

**AirLift FeatherWing (ESP32 via SPI0)**
- User confirmed they have this hardware
- Uses NINA firmware for WiFi operations
- Pin assignments moved from defaults to avoid OLED button conflicts:
  - GP10 = ESP32 Chip Select (was D13)
  - GP11 = ESP32 Busy/Ready (was D6)
  - GP12 = ESP32 Reset (was D5)
  - SPI0 (GP18=SCK, GP19=MOSI, GP20=MISO)
  - 8 MHz SPI clock

**WiFi Configuration**
- SSID: "RocketGateway"
- Password: "rocket2026"
- Channel: 6
- Server Port: 8080
- AP IP: 192.168.4.1 (standard ESP32 AP mode address)

---

## Files Created

### Gateway Firmware

| File | Purpose |
|------|---------|
| `firmware_gateway/include/wifi_nina.h` | NINA SPI protocol definitions, command constants |
| `firmware_gateway/src/wifi_nina.c` | Full AirLift driver (~400 lines): SPI comms, AP mode, TCP server |

### iOS App

| File | Purpose |
|------|---------|
| `ios_app/Module_Navigation.xojo_module` | Haversine distance/bearing calculations |
| `ios_app/Module_Location.xojo_module` | iPhone GPS wrapper using MobileLocation |
| `ios_app/RocketLocation.xojo_code` | Persistent rocket position storage |
| `ios_app/Screen_Recovery.xojo_screen` | Recovery navigation UI with compass |
| `ios_app/Screen_FlashStorage.xojo_screen` | Flight data download UI |

---

## Files Modified

### Gateway Firmware

| File | Changes |
|------|---------|
| `firmware_gateway/include/pins.h` | Added WiFi pin definitions and configuration |
| `firmware_gateway/CMakeLists.txt` | Added wifi_nina.c to build |
| `firmware_gateway/src/main.c` | WiFi init, TCP polling, dual output (USB+WiFi), command routing |

### iOS App

| File | Changes |
|------|---------|
| `ios_app/TelemetrySample.xojo_code` | Added GPS properties |
| `ios_app/FlightConnection.xojo_code` | Added GPS parsing, flash commands/events |
| `ios_app/App.xojo_code` | Added pRocketLocation, location service init/shutdown |
| `ios_app/Screen_Settings.xojo_screen` | Updated IP hint to 192.168.4.1 |
| `ios_app/RocketAvionicsIOS.xojo_project` | Added new files to project |

---

## Key Implementation Details

### NINA SPI Protocol (wifi_nina.c)

```c
// Command frame format:
// START_CMD (0xE0) | CMD | NUM_PARAMS | [PARAM_LEN | PARAM_DATA]... | END_CMD (0xEE)

// Key functions:
bool WiFi_Init(void);                    // Reset ESP32, init SPI
bool WiFi_StartAP(void);                 // Create hotspot
bool WiFi_StartServer(uint16_t port);    // TCP server
bool WiFi_HasClient(void);               // Client connected?
int WiFi_Read(char* buf, int maxLen);    // Read from TCP
int WiFi_Write(const char* data, int len); // Write to TCP
```

### Dual Output System (main.c)

```c
#if kEnableWifi
  static void OutputToAll(const char * inJson);
  #define OUTPUT_JSON(json) OutputToAll(json)
#else
  #define OUTPUT_JSON(json) do { printf("%s", json); stdio_flush(); } while(0)
#endif
```

### GPS Properties Added to TelemetrySample

```xojo
pGpsLatitude As Double = 0.0
pGpsLongitude As Double = 0.0
pGpsSpeedMps As Double = 0.0
pGpsHeadingDeg As Double = 0.0
pGpsSatellites As Integer = 0
pGpsFix As Boolean = False
```

### Haversine Distance Calculation (Module_Navigation)

```xojo
Function CalculateDistanceM(inLat1, inLon1, inLat2, inLon2) As Double
  // Great-circle distance using Haversine formula
  Const kEarthRadiusM = 6371000.0
  Const kDegToRad = 0.01745329252
  // ... formula implementation
  Return kEarthRadiusM * theC
End Function
```

### RocketLocation Persistence

- Uses iOS Preferences API for storage
- Keys: rocket_lat, rocket_lon, rocket_alt, rocket_fix, rocket_sats, rocket_state, rocket_timestamp
- Load() is shared/static function
- Save() persists immediately when GPS data received
- GetAgeString() returns human-readable age ("5 sec ago", "2 min ago", etc.)

### Screen_Recovery Features

- Large compass canvas showing bearing arrow to rocket
- Real-time distance and bearing display
- Rocket coordinates (lat/lon with N/S/E/W formatting)
- Rocket info (altitude, satellites, flight state)
- Data age indicator
- Phone GPS coordinates
- "Open in Maps" button launches Apple Maps with directions

### Flash Download Protocol

Commands added to FlightConnection:
- `SendFlashList()` - Request list of stored flights
- `SendFlashRead(slot, sample)` - Request data chunk
- `SendFlashDelete(slot)` - Delete stored flight

Events:
- `FlashListReceived(count, flights())` - List of Dictionary with slot, id, altitude, time_ms, samples
- `FlashDataReceived(slot, start, total, data)` - Data chunk for progressive download

---

## JSON Message Types

### Telemetry (type="tel")
```json
{
  "type": "tel",
  "t": 12345,
  "alt": 1234.5,
  "vel": 45.6,
  "lat": 38.897957,
  "lon": -77.036560,
  "gspd": 2.5,
  "hdg": 180.0,
  "sat": 8,
  "gps": true,
  "state": "LANDED"
}
```

### Flash List (type="flash_list")
```json
{
  "type": "flash_list",
  "count": 2,
  "flights": [
    {"slot": 0, "id": 1, "alt": 1234.5, "time": 45600, "samples": 1200},
    {"slot": 1, "id": 2, "alt": 987.3, "time": 32100, "samples": 850}
  ]
}
```

### Flash Data (type="flash_data")
```json
{
  "type": "flash_data",
  "slot": 0,
  "start": 0,
  "total": 1200,
  "data": "[{\"t\":0,\"alt\":0.0,...}, ...]"
}
```

---

## Testing Checklist

1. [ ] iPhone connects to "RocketGateway" WiFi hotspot
2. [ ] App connects to 192.168.4.1:8080
3. [ ] Telemetry with GPS coordinates displays in app
4. [ ] Rocket GPS persists after app kill/restart
5. [ ] Recovery screen shows distance/bearing
6. [ ] Distance updates as user walks
7. [ ] "Open in Maps" launches Apple Maps correctly
8. [ ] Offline mode works (shows persisted rocket location)
9. [ ] Flash list downloads and displays flights
10. [ ] Flight data downloads with progress bar
11. [ ] CSV export/share works

---

## Notes

- Gateway firmware builds successfully with WiFi code
- iOS app requires `NSLocationWhenInUseUsageDescription` in Info.plist for GPS access
- MobileLocation requires user authorization before providing coordinates
- Recovery navigation works offline using persisted rocket coordinates
- CSV export uses MobileShare for iOS share sheet integration
