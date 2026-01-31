# iOS Application (Xojo)

## Overview

iOS companion app for rocket telemetry monitoring and rocket recovery. Connects to the gateway via WiFi to receive real-time telemetry and provides navigation to the landed rocket using iPhone GPS.

## Requirements

- **IDE:** Xojo 2023r4 or later
- **Project File:** `RocketAvionicsIOS.xojo_project`
- **Deployment:** iOS 15.0 or later

## Project Structure

```
ios_app/
├── RocketAvionicsIOS.xojo_project  # Main project file
├── App.xojo_code                   # Application class
│
├── Screens/
│   ├── Screen_Main.xojo_screen     # Main telemetry display
│   ├── Screen_Settings.xojo_screen # Connection settings
│   ├── Screen_Recovery.xojo_screen # Rocket recovery navigation
│   └── Screen_FlashStorage.xojo_screen # Flash download
│
├── Classes/
│   ├── FlightConnection.xojo_code  # TCP connection manager
│   ├── FlightData.xojo_code        # Flight data model
│   ├── TelemetrySample.xojo_code   # Telemetry data point
│   ├── RocketLocation.xojo_code    # Persistent rocket position
│   └── AltitudeChartView.xojo_code # Altitude graph view
│
├── Modules/
│   ├── Module_Location.xojo_module # iPhone GPS access
│   └── Module_Navigation.xojo_module # Distance/bearing math
│
└── icons/                          # App icons
```

## Key Features

### Real-Time Telemetry
- Connect to gateway via WiFi TCP
- Display altitude, velocity, flight state
- Live altitude chart

### Rocket Recovery
- Persists last known rocket GPS coordinates
- Uses iPhone GPS for current position
- Calculates distance and bearing to rocket
- Works offline (uses cached rocket position)
- "Open in Maps" for turn-by-turn navigation

### Flash Download
- List flights stored on flight computer
- Download flight data over LoRa
- Export to CSV

## Key Classes

### FlightConnection
TCP client for gateway communication:
- Connect to gateway IP:port
- Parse JSON telemetry
- Send commands to flight computer
- Flash storage commands

### TelemetrySample
Telemetry data point with GPS:
```xojo
pRocketId As Integer        // Rocket ID (0-15)
pAltitudeM As Double
pVelocityMps As Double
pState As String
pGpsLatitude As Double
pGpsLongitude As Double
pGpsFix As Boolean
pGpsSatellites As Integer
```

### RocketLocation
Persistent last known rocket position:
- Supports multiple rockets via `pRocketId` (0-15)
- Saved to iOS preferences with ID-based keys
- Survives app restart
- Used for offline recovery navigation
- Load with `RocketLocation.Load(inRocketId)` for specific rocket

### Module_Location
iPhone GPS wrapper:
```xojo
Shared Sub StartUpdating()
Shared Sub StopUpdating()
Shared Event LocationUpdated(lat As Double, lon As Double)
```

### Module_Navigation
Distance/bearing calculations:
```xojo
Function CalculateDistanceM(lat1, lon1, lat2, lon2) As Double
  // Haversine formula

Function CalculateBearingDeg(lat1, lon1, lat2, lon2) As Double
  // Returns 0-360 degrees (0=North)
```

## Screens

### Screen_Main
Primary telemetry display:
- Connection status
- Current altitude, velocity
- Flight state
- Max altitude
- Navigation to Recovery screen

### Screen_Settings
Connection configuration:
- Gateway IP address (default: 192.168.4.1)
- TCP port (default: 5000)
- Connect/disconnect button

### Screen_Recovery
Rocket recovery navigation:
- Compass showing direction to rocket (adjusts with phone orientation)
- Distance to rocket in meters or feet
- Rocket GPS coordinates
- Phone GPS coordinates
- Data age indicator (for offline mode)
- Multi-rocket selector (switch between rockets)
- "Open in Maps" button for turn-by-turn navigation

### Screen_FlashStorage
Flight data download:
- List of stored flights
- Download selected flight
- Progress indicator
- Export/share options

## Connection Protocol

### TCP Connection
- Default host: `192.168.4.1` (Heltec AP mode) or `RocketGateway.local` (mDNS)
- Default port: `5000`
- Format: JSON lines (newline-delimited)

**Note:** When the Heltec gateway is in AP mode (creating its own hotspot), use `192.168.4.1`. When connected to an existing WiFi network (station mode), use `RocketGateway.local` or the assigned IP address.

### Telemetry Message
```json
{
  "type": "tel",
  "id": 0,
  "alt": 152.3,
  "vel": 45.2,
  "state": "BOOST",
  "lat": 38.897,
  "lon": -77.036,
  "sats": 8,
  "dist": 245.5
}
```

### Rockets List Request/Response
```json
// Request
{"cmd":"rockets"}

// Response
{
  "type": "rockets",
  "count": 2,
  "rockets": [
    {"id": 0, "lat": 38.897, "lon": -77.036, "alt": 152.3, "dist": 245.5, "state": "LANDED", "sats": 8, "age": 5},
    {"id": 1, "lat": 38.898, "lon": -77.035, "alt": 0.0, "dist": 100.2, "state": "IDLE", "sats": 6, "age": 2}
  ]
}
```

### Flash Commands
```json
// Request flight list
{"cmd":"flash_list"}

// Download flight
{"cmd":"flash_read","slot":0,"sample":0}

// Delete flight
{"cmd":"flash_delete","slot":0}
```

## iOS Permissions

Required in Info.plist:
```xml
<key>NSLocationWhenInUseUsageDescription</key>
<string>Used to navigate to your rocket's landing location</string>
```

## Coding Conventions

### Xojo Naming
- Properties: `pPropertyName`
- Methods: `MethodName()`
- Parameters: `inParamName`
- Local variables: `theVarName`
- Modules: `Module_Name`
- Screens: `Screen_Name`

## Building

1. Open `RocketAvionicsIOS.xojo_project` in Xojo IDE
2. Configure iOS signing (Apple Developer account required)
3. Build → Build Application
4. Deploy to device via Xcode or direct install

## Multi-Rocket Support

The app supports tracking multiple rockets (IDs 0-15) simultaneously:

- **Rocket List:** Main screen shows all active rockets
- **Individual Recovery:** Each rocket's position is cached separately
- **Rocket Selector:** Switch between rockets in Recovery screen
- **Persistent Storage:** Each rocket's last known position saved by ID

## Offline Recovery Mode

The app caches the last known rocket position:

1. While connected, each GPS-valid telemetry sample updates `RocketLocation`
2. Position is saved to preferences immediately
3. When connection lost, cached position remains available
4. User can navigate to rocket using iPhone GPS
5. "Data age" indicator shows how old the cached position is

This allows recovery even if:
- Rocket lands out of LoRa range
- Gateway battery dies
- WiFi connection is lost
