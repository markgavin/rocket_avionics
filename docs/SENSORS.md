# Sensor Fusion: Altitude and Velocity Calculation

## Overview

The flight computer combines data from two sensor types to produce accurate altitude and velocity estimates:

- **Barometric pressure sensor** (BMP390 or BMP581) -- provides absolute altitude reference, low noise, moderate update rate
- **Inertial Measurement Unit** (LSM6DSOX or ICM-20649) -- provides high-frequency acceleration data for fast velocity response

These are fused using a **complementary filter with bias estimation**, producing altitude and velocity outputs at 100 Hz.

When no IMU is available, the system falls back to barometric-only velocity via numerical differentiation with EMA smoothing.

---

## Sensors

### BMP390 Barometric Pressure Sensor

| Parameter | Value |
|-----------|-------|
| I2C Address | 0x77 (default) or 0x76 |
| Chip ID | 0x60 |
| Pressure Accuracy | +/-3 Pa (+/-25 cm) |
| Pressure Noise (RMS) | ~10 cm |
| Max Output Data Rate | 200 Hz |
| Compensation | On-host (11 pressure + 3 temperature coefficients) |
| Configuration | 8x pressure OSR, 2x temp OSR, 50 Hz ODR, IIR coef 3 |

The BMP390 requires calibration data to be read from NVM at init time. Compensation is performed in software using the Bosch polynomial formulas.

### BMP581 Barometric Pressure Sensor

| Parameter | Value |
|-----------|-------|
| I2C Address | 0x47 (default) or 0x46 |
| Chip ID | 0x51 (BMP581) or 0x50 (BMP580) |
| Pressure Accuracy | +/-0.4 Pa (+/-3.3 cm) |
| Pressure Noise (RMS) | ~1 cm |
| Max Output Data Rate | 240 Hz |
| Compensation | On-chip (pre-compensated output) |
| Configuration | 16x pressure OSR, 2x temp OSR, 50 Hz ODR, IIR coef 1 |

The BMP581 outputs pre-compensated data. No calibration coefficients are needed. Conversion from raw register values:

```
Temperature (C) = raw_24bit_signed / 65536.0
Pressure (Pa)   = raw_24bit_unsigned / 64.0
```

### Sensor Comparison (measured 2026-02-05)

Both sensors installed on the same flight computer, sampled simultaneously at 1 Hz over 2+ minutes:

| Metric | BMP390 | BMP581 | Delta |
|--------|--------|--------|-------|
| Pressure | 101065 Pa | 101073 Pa | +8 Pa (fixed offset) |
| Temperature | 21.7 C | 21.4 C | -0.3 C (fixed offset) |
| Pressure drift | none | none | stable within +/-1 Pa |
| Temperature drift | none | none | stable within +/-0.02 C |

The fixed pressure offset of ~8 Pa between sensors is within their combined accuracy specifications. Both sensors track pressure changes identically with no relative drift.

### IMU: LSM6DSOX + LIS3MDL (9-DoF)

| Parameter | Value |
|-----------|-------|
| Accelerometer Range | +/-8g |
| Accelerometer ODR | 416 Hz |
| Gyroscope Range | +/-1000 dps |
| Gyroscope ODR | 416 Hz |
| Magnetometer | LIS3MDL (3-axis) |

### IMU: ICM-20649 (alternate)

| Parameter | Value |
|-----------|-------|
| Accelerometer Range | +/-30g |
| Accelerometer ODR | 1125 Hz (configurable) |
| Gyroscope Range | +/-4000 dps |

---

## Altitude Calculation

### Barometric Formula

Altitude above ground level (AGL) is calculated using the international standard atmosphere hypsometric formula:

```
h = (T0 / L) * (1 - (P / P0) ^ ((R * L) / (g * M)))
```

Where:

| Symbol | Value | Description |
|--------|-------|-------------|
| T0 | 288.15 K | Standard sea-level temperature |
| L | 0.0065 K/m | Temperature lapse rate |
| P | measured | Current pressure (Pa) |
| P0 | ground ref | Ground-level reference pressure (Pa) |
| R | 8.31447 J/(mol*K) | Universal gas constant |
| g | 9.80665 m/s^2 | Gravitational acceleration |
| M | 0.0289644 kg/mol | Molar mass of dry air |

The exponent `(R * L) / (g * M)` evaluates to approximately 0.19026.

### Ground Reference

- P0 is set to the current barometric pressure when the system enters IDLE state
- Updated continuously while on the pad (first valid reading used as temporary reference)
- All altitude readings are relative to ground level (AGL), not sea level

### EMA Smoothing

Raw barometric altitude is smoothed with an Exponential Moving Average before use:

```
smoothed = alpha * raw + (1 - alpha) * previous_smoothed
```

- Alpha = 0.1 (approximately 2 Hz cutoff at 100 Hz sample rate)
- Reduces sensor noise while maintaining adequate response time

---

## Velocity Calculation

### With IMU: Complementary Filter

When an IMU is available, altitude and velocity are computed using a complementary filter that fuses barometric altitude (slow, accurate) with IMU acceleration (fast, drifty).

#### IMU Prediction Step (100 Hz)

Each IMU reading updates the filter state by integrating vertical acceleration:

```
vertical_accel = (accel_z - 1.0) * 9.80665 - bias
velocity += vertical_accel * dt
altitude += velocity * dt
```

- `accel_z` reads approximately 1.0g when stationary (gravity). Subtracting 1.0 removes the gravity component.
- `bias` is the learned accelerometer offset (see Bias Estimation below)
- `dt` is clamped to a maximum of 50 ms to prevent large jumps

#### Barometric Correction Step (100 Hz)

Each barometric reading corrects the filter state using the altitude error:

```
error = smoothed_baro_altitude - cf_altitude

velocity += Kv * error * dt
altitude += Ka * error * dt
bias     -= Kb * error * dt
```

| Gain | Value | Purpose |
|------|-------|---------|
| Ka (altitude) | 6.0 | Pulls filter altitude toward barometric truth |
| Kv (velocity) | 10.0 | Corrects velocity based on position error |
| Kb (bias) | 1.0 | Slowly learns accelerometer offset |

These gains produce a critically-damped response with approximately 0.5 Hz crossover frequency. The barometer dominates at low frequencies (drift-free altitude), while the IMU provides high-frequency dynamics (fast velocity response to thrust and aerodynamic events).

#### Bias Estimation

The accelerometer has a small, temperature-dependent offset that causes velocity and altitude drift when integrated. The bias estimator learns this offset automatically through negative feedback:

```
bias -= Kb * altitude_error * dt
```

- If the filter altitude exceeds the barometric altitude, the bias is too negative (reading too much upward acceleration). The bias is decreased, reducing the integrated acceleration.
- If the filter altitude is below the barometric altitude, the bias is too positive. The bias is increased.
- The slow learning rate (Kb = 1.0) gives a time constant of approximately 10-20 seconds, preventing overshoot from transient measurement errors.
- Typical converged bias values: 0.05 to 0.2 m/s^2

### Without IMU: Barometric-Only Velocity

When no IMU is available, velocity is computed by numerically differentiating the smoothed barometric altitude:

```
instant_velocity = (current_altitude - previous_altitude) / dt
velocity = 0.15 * instant_velocity + 0.85 * previous_velocity
```

This provides a reasonable velocity estimate but with significantly more noise and latency compared to the complementary filter.

---

## Complete Data Flow

```
Sensors (100 Hz each)
|
+-- Barometer (BMP390 or BMP581)
|   |
|   +-- Raw pressure (Pa)
|   |
|   +-- Barometric altitude = hypsometric_formula(pressure, ground_pressure)
|   |
|   +-- Smoothed altitude = EMA(raw_altitude, alpha=0.1)
|   |
|   +-- IF IMU available:
|   |   |
|   |   +-- altitude_error = smoothed_altitude - cf_altitude
|   |   +-- cf_velocity  += Kv * error * dt
|   |   +-- cf_altitude  += Ka * error * dt
|   |   +-- accel_bias   -= Kb * error * dt
|   |
|   +-- ELSE:
|       +-- velocity = EMA(differentiate(smoothed_altitude), alpha=0.15)
|
+-- IMU (LSM6DSOX or ICM-20649)
    |
    +-- accel_z (g)
    |
    +-- vertical_accel = (accel_z - 1.0) * 9.80665 - bias
    |
    +-- cf_velocity += vertical_accel * dt
    +-- cf_altitude += cf_velocity * dt
        |
        v
    Fused Output
    |
    +-- altitude (m AGL)  -- used for flight state detection and telemetry
    +-- velocity (m/s)    -- used for apogee detection and telemetry
    +-- accel_bias (m/s^2) -- internal, continuously updated
```

---

## Telemetry Output

The fused altitude and velocity are transmitted in the LoRa telemetry packet at 10 Hz:

| Field | Encoding | Resolution |
|-------|----------|------------|
| Altitude | int32, centimeters | 0.01 m |
| Velocity | int16, cm/s | 0.01 m/s |
| Pressure | uint32, Pa | 1 Pa |
| Temperature | int16, C * 10 | 0.1 C |
| Accel X/Y/Z | int16, milli-g | 0.001 g |
| Gyro X/Y/Z | int16, 0.1 dps | 0.1 dps |

The gateway decodes these to JSON:
```json
{
  "type": "tel",
  "alt": 152.3,
  "vel": 45.2,
  "pres": 101200,
  "temp": 21.5,
  "ax": 0.021, "ay": -0.030, "az": 1.032,
  "gx": -1.4, "gy": 1.8, "gz": -0.8
}
```

---

## Flight State Detection

The fused altitude and velocity drive the flight state machine:

| Transition | Condition |
|------------|-----------|
| IDLE -> PAD | Arm switch closed |
| PAD -> BOOST | altitude > 10 m OR velocity > 10 m/s |
| BOOST -> COAST | Acceleration drops (thrust ends) |
| COAST -> APOGEE | velocity <= 2 m/s for 3 consecutive samples |
| APOGEE -> DROGUE | Drogue pyro fired |
| DROGUE -> MAIN | altitude < main deploy altitude |
| MAIN -> LANDED | abs(velocity) < 1 m/s AND altitude < 10 m for 5 seconds |

---

## Configuration Summary

All constants are defined in `firmware_flight/src/flight_control.c`:

| Parameter | Value | Description |
|-----------|-------|-------------|
| Sample rate | 100 Hz (10 ms) | Both barometer and IMU |
| EMA altitude alpha | 0.1 | Barometric smoothing |
| EMA velocity alpha | 0.15 | Fallback velocity smoothing |
| CF altitude gain (Ka) | 6.0 | Position correction |
| CF velocity gain (Kv) | 10.0 | Velocity correction |
| CF bias gain (Kb) | 1.0 | Bias learning rate |
| Max dt clamp | 50 ms | Prevents integration blowup |
| Gravity | 9.80665 m/s^2 | Standard value |

Sensor configuration is set during initialization in `main.c` and the respective driver `Configure` calls.
