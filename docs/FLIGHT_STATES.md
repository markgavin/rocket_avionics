# Flight State Machine

## Overview

The flight computer uses a finite state machine to manage the flight lifecycle from pre-launch through landing and data recovery. The state machine runs in the main loop at 1 kHz and makes transition decisions based on altitude, velocity, and timing data from the sensor fusion system.

Source: `firmware_flight/src/flight_control.c`

## State Diagram

```
               arm cmd
  IDLE ──────────────────► ARMED
   ▲                         │
   │ reset cmd               │ alt > 10m OR vel > 10 m/s
   │                         ▼
COMPLETE                   BOOST
   ▲                         │
   │ download                │ vel < 95% of max AND alt > 20m
   │                         ▼
LANDED ◄──── DESCENT ◄── COAST
   5s stationary    ▲        │
   vel < 1 m/s      │        │ vel < 2 m/s for 3 consecutive samples
   alt < 10m         │        ▼
                    APOGEE ──┘
                  (immediate transition)
```

## States

### IDLE (0)
- **Entry:** Power-on or after reset command
- **Behavior:** Sensors read and displayed; first valid pressure used as temporary ground reference for display; telemetry sent at 0.5 Hz
- **Exit:** Arm command received via LoRa

### ARMED (1)
- **Entry:** Arm command accepted (only from IDLE or COMPLETE)
- **Behavior:** Ground pressure and temperature recorded as reference; altitude/velocity counters reset; complementary filter altitude and velocity zeroed (but accel bias preserved); telemetry sent at 2 Hz
- **Exit:** Launch detected (altitude > 10m OR velocity > 10 m/s)

### BOOST (2)
- **Entry:** Launch detection threshold exceeded
- **Behavior:** Launch timestamp recorded; telemetry enabled at 10 Hz; flash recording started; max velocity tracked; flight results initialized
- **Exit:** Velocity drops below 95% of recorded max velocity AND altitude > 20m

### COAST (3)
- **Entry:** Motor burnout detected
- **Behavior:** Max altitude tracked; apogee descent counter monitored; telemetry continues at 10 Hz
- **Exit:** Velocity < 2 m/s for 3 consecutive samples (`kApogeeDescendCount`)

### APOGEE (4)
- **Entry:** Apogee detected
- **Behavior:** Apogee altitude and time recorded in flight results; immediate transition to DESCENT (single-frame state)
- **Exit:** Immediate transition to DESCENT

### DESCENT (5)
- **Entry:** Apogee recorded
- **Behavior:** Stationary counter tracks consecutive low-velocity samples; telemetry continues at 10 Hz
- **Exit:** |velocity| < 1 m/s AND altitude < 10m for 50 consecutive samples (5 seconds at 10 Hz)

### LANDED (6)
- **Entry:** Landing detected
- **Behavior:** Flight time and sample count finalized; flash recording ended; telemetry reduced to 1 Hz
- **Exit:** Download command transitions to COMPLETE

### COMPLETE (7)
- **Entry:** Flight data downloaded
- **Behavior:** Telemetry at 1 Hz; ready for reset
- **Exit:** Reset command returns to IDLE

## Detection Thresholds

All thresholds are defined in `firmware_flight/include/pins.h`:

| Parameter | Value | Purpose |
|-----------|-------|---------|
| `kLaunchAltitudeThresholdM` | 10.0 m | Altitude to detect launch |
| `kLaunchVelocityThresholdMps` | 10.0 m/s | Velocity to detect launch |
| `kApogeeVelocityThresholdMps` | 2.0 m/s | Velocity threshold for apogee |
| `kApogeeDescendCount` | 3 samples | Consecutive descending samples for apogee confirmation |
| `kLandingVelocityThresholdMps` | 1.0 m/s | Velocity threshold for landing |
| `kLandingStationaryCount` | 50 samples | 5 seconds stationary at 10 Hz |
| Burnout altitude minimum | 20.0 m | Prevents false coast detection near pad |
| Burnout velocity ratio | 95% of max | Velocity drop ratio for coast transition |
| Landing altitude ceiling | 10.0 m | Must be near ground to detect landing |

## Telemetry Rates by State

| State | Rate | Interval | Purpose |
|-------|------|----------|---------|
| IDLE | 0.5 Hz | 2000 ms | Battery conservation |
| IDLE + Orientation Mode | 10 Hz | 100 ms | Real-time testing |
| ARMED | 2 Hz | 500 ms | Ready monitoring |
| BOOST through DESCENT | 10 Hz | 100 ms | Full flight data |
| LANDED / COMPLETE | 1 Hz | 1000 ms | Recovery beacon |

## Commands

Commands are received via LoRa as binary packets (`kLoRaPacketCommand`):

| Command | ID | Effect |
|---------|----|--------|
| Arm | 0x01 | IDLE/COMPLETE -> ARMED; sets ground reference |
| Disarm | 0x02 | ARMED -> IDLE; only valid before launch |
| Reset | 0x04 | Any state -> IDLE; clears all counters |
| Orientation Mode | 0x08 | Enables 10 Hz telemetry in IDLE; auto-disables after 30 seconds |

## Safety Features

- **Disarm only in ARMED state:** Cannot disarm during active flight
- **Arm only from IDLE/COMPLETE:** Prevents accidental re-arming during flight
- **Orientation mode timeout:** Auto-disables after 30 seconds to prevent battery drain
- **Burnout altitude guard:** Coast transition requires altitude > 20m to avoid false triggers
- **Landing hysteresis:** 5 seconds of stationary behavior required before declaring landing
- **Ground reference at arming:** Pressure reference set at arm time, not at boot, for accurate AGL altitude
- **Accel bias preservation:** When arming, the complementary filter's learned accelerometer bias is kept (it converges on the pad before arming)

## Flash Storage Triggers

The main loop monitors state transitions for flash recording:

- **ARMED -> BOOST:** Calls `FlightStorage_StartFlight()` with ground pressure and GPS launch coordinates
- **DESCENT -> LANDED:** Calls `FlightStorage_EndFlight()` with max altitude, max velocity, apogee time, and flight duration
- **During flight (BOOST through LANDED):** Samples logged at 10 Hz via `FlightStorage_LogSample()`
