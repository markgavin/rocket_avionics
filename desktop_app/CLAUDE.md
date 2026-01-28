# Desktop Application (Xojo)

## Overview

macOS/Windows desktop application for rocket telemetry monitoring, flight data analysis, and device configuration. Built with Xojo for cross-platform compatibility.

## Requirements

- **IDE:** Xojo 2023r4 or later
- **Project File:** `RocketAvionicsDesktop.xojo_project`

## Project Structure

```
desktop_app/
├── RocketAvionicsDesktop.xojo_project  # Main project file
├── App.xojo_code                       # Application class
├── MenuBar_Main.xojo_menu              # Menu bar definition
│
├── Windows/
│   ├── Window_Main.xojo_window         # Main telemetry display
│   ├── Window_Console.xojo_window      # Raw JSON console
│   ├── Window_DeviceInfo.xojo_window   # Device status/info
│   ├── Window_DeviceStorage.xojo_window# Flash storage management
│   ├── Window_History.xojo_window      # Flight history browser
│   ├── Window_FlightCompare.xojo_window# Compare multiple flights
│   ├── Window_Orientation.xojo_window  # 3D orientation view
│   ├── Window_WifiConfig.xojo_window   # Gateway WiFi settings
│   └── Window_Splash.xojo_window       # Startup splash screen
│
├── Dialogs/
│   ├── Dialog_About.xojo_window        # About dialog
│   └── Dialog_Preferences.xojo_window  # App preferences
│
├── Classes/
│   ├── FlightConnection.xojo_code      # Serial/TCP connection manager
│   ├── FlightData.xojo_code            # Flight data model
│   ├── FlightDatabase.xojo_code        # SQLite flight storage
│   ├── TelemetrySample.xojo_code       # Single telemetry point
│   └── AltitudeChartCanvas.xojo_code   # Altitude graph canvas
│
├── Modules/
│   ├── Module_Preferences.xojo_code    # User preferences
│   ├── Module_PrintReport.xojo_code    # Flight report printing
│   └── Module_WindowSettings.xojo_code # Window position persistence
│
├── Resources/
│   ├── icons/                          # Application icons
│   ├── Help.html                       # Help documentation
│   └── RocketAvionicsDesktop.xojo_resources  # Embedded resources
│
└── Builds - RocketAvionicsDesktop/     # Build output
```

## Key Classes

### FlightConnection
Manages serial port and TCP connections to the gateway:
- Auto-detection of USB serial ports
- TCP client for WiFi gateway connection
- JSON parsing of telemetry messages
- Command sending to flight computer

### FlightData
In-memory flight data storage:
- Array of TelemetrySample objects
- Statistics (max altitude, max velocity, flight duration)
- Export to CSV

### TelemetrySample
Single telemetry data point:
- Timestamp
- Altitude, velocity, acceleration
- GPS coordinates
- Flight state
- Pyro continuity status

### FlightDatabase
SQLite database for flight history:
- Store completed flights
- Query and filter flights
- Export flight data

## Windows

### Window_Main
Primary telemetry display:
- Real-time altitude graph
- Current telemetry values
- Flight state indicator
- Connection status

### Window_DeviceStorage
Flash storage management:
- List flights stored on device
- Download flight data
- Delete flights

### Window_Console
Debug console:
- Raw JSON messages
- Send manual commands
- Connection diagnostics

## Connection Protocol

### Serial Connection
- Baud rate: 115200
- Format: JSON lines (newline-delimited)

### TCP Connection
- Default port: 5000 (Heltec) or 8080 (AirLift)
- Format: JSON lines

### Message Types

```xojo
// Telemetry from flight computer
{"type":"tel","alt":152.3,"vel":45.2,"state":"BOOST"}

// Status from gateway
{"type":"status","version":"1.0.0","lora":true}

// Flash list response
{"type":"flash_list","flights":[{"slot":0,"samples":1234}]}
```

## Preferences

Stored in user preferences:
- Serial port selection
- TCP host/port for WiFi connection
- Units (metric/imperial)
- Chart settings
- Window positions

## Coding Conventions

### Xojo Naming
- Properties: `pPropertyName`
- Methods: `MethodName()`
- Parameters: `inParamName`
- Local variables: `theVarName`
- Modules: `Module_Name`
- Windows: `Window_Name`
- Dialogs: `Dialog_Name`

### Event Handlers
```xojo
Sub ButtonConnect_Pressed()
  // Handle button press
End Sub
```

## Building

1. Open `RocketAvionicsDesktop.xojo_project` in Xojo IDE
2. Select target platform (macOS, Windows)
3. Build → Build Application
4. Output in `Builds - RocketAvionicsDesktop/`

## Menu Structure

```
File
├── New Flight
├── Open Flight...
├── Save Flight...
├── Export CSV...
└── Quit

View
├── Main Window
├── Console
├── Device Info
├── Device Storage
├── Orientation
└── Flight History

Device
├── Connect
├── Disconnect
├── WiFi Config...
└── Device Info

Help
├── Help
└── About
```
