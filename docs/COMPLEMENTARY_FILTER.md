# Complementary Filter Tuning Guide

## Overview

The flight computer uses a complementary filter to fuse barometer altitude data (accurate but slow) with IMU accelerometer data (fast but drifty) to produce smooth, responsive altitude and velocity estimates. This document explains how the filter works and how to tune it for different rockets.

Source: `firmware_flight/src/flight_control.c`

## How It Works

The filter maintains three state variables:
- **Altitude** (`pCfAltitudeM`) - estimated altitude in meters
- **Velocity** (`pCfVelocityMps`) - estimated vertical velocity in m/s
- **Accel Bias** (`pCfAccelBiasMps2`) - learned accelerometer offset in m/s^2

### Two-Step Update Cycle

The filter runs in two steps every loop iteration:

#### Step 1: IMU Prediction (100 Hz)

When new IMU data arrives, integrate acceleration to update velocity and altitude:

```
vertical_accel = (accelZ - 1.0) * 9.80665 - bias
velocity += vertical_accel * dt
altitude += velocity * dt
```

The IMU provides fast response to acceleration changes but drifts over time due to sensor noise and bias.

#### Step 2: Barometric Correction (100 Hz)

When new barometric data arrives, correct the filter state toward the barometric truth:

```
error = smoothed_baro_altitude - filter_altitude

velocity += Kv * error * dt
altitude += Ka * error * dt
bias     -= Kb * error * dt
```

The barometer provides accurate absolute altitude but responds slowly to rapid changes.

### Bias Estimation

The bias term continuously learns the accelerometer's DC offset. At rest, if the accelerometer reads slightly more or less than 1.0g, the bias integrator slowly adjusts to cancel this error. This happens through the negative feedback loop:

```
bias -= Kb * altitude_error * dt
```

When the filter altitude drifts above the barometric altitude (positive error), the bias increases, which reduces the next acceleration integration, bringing the altitude back down.

## Current Gain Settings

Defined in `flight_control.c`:

| Gain | Symbol | Value | Unit |
|------|--------|-------|------|
| Altitude correction | `kCfGainAltitude` | 6.0 | 1/s |
| Velocity correction | `kCfGainVelocity` | 10.0 | 1/s^2 |
| Bias learning rate | `kCfGainBias` | 1.0 | 1/s^3 |
| Max integration dt | `kCfMaxDtS` | 0.05 | seconds |

The altitude smoothing applied to the barometric input before it enters the filter:

| Parameter | Value | Effect |
|-----------|-------|--------|
| `kAltitudeSmoothingAlpha` | 0.1 | ~2 Hz cutoff at 100 Hz sample rate |

## Tuning Guide

### Crossover Frequency

The gains determine where the filter transitions from trusting the IMU (high frequency) to trusting the barometer (low frequency). The current settings give a crossover around 0.5 Hz, meaning:
- Events faster than ~2 seconds: dominated by IMU
- Events slower than ~2 seconds: dominated by barometer

### Making the Filter More Responsive (Higher Crossover)

Increase Ka and Kv to trust the barometer corrections more aggressively:

```c
#define kCfGainAltitude  10.0f   // was 6.0
#define kCfGainVelocity  16.0f   // was 10.0
```

**When to use:** Slow-burning motors with gradual acceleration changes; rockets where barometric noise is low (larger rockets with better aerodynamic stability).

**Trade-off:** The filter responds faster to barometric changes but becomes noisier if the barometer has turbulence-induced pressure fluctuations during flight.

### Making the Filter Smoother (Lower Crossover)

Decrease Ka and Kv to rely more on IMU integration:

```c
#define kCfGainAltitude  3.0f    // was 6.0
#define kCfGainVelocity  5.0f    // was 10.0
```

**When to use:** High-thrust motors with sharp acceleration spikes; situations where barometric readings may be noisy (small rockets with turbulent airflow over the sensor port).

**Trade-off:** Smoother output but the velocity estimate may drift further from truth during long coast phases.

### Bias Learning Rate

The bias gain controls how quickly the filter learns the accelerometer's DC offset:

- **Higher Kb (e.g., 2.0):** Faster convergence on pad, but may track non-bias errors during flight (e.g., interpreting wind gusts as bias)
- **Lower Kb (e.g., 0.5):** Slower convergence, more stable during flight but takes longer to settle after power-on
- **Current Kb (1.0):** Good compromise; converges within ~10 seconds on the pad

The bias is preserved across the arm command, so it should be well-converged before arming.

### Altitude Smoothing Alpha

The barometric altitude is smoothed with an exponential moving average before entering the filter:

```c
smoothed = alpha * raw + (1 - alpha) * smoothed
```

- **Higher alpha (e.g., 0.3):** Less smoothing, barometer changes reach filter faster, but more noise
- **Lower alpha (e.g., 0.05):** More smoothing, cleaner barometric input, but slower response
- **Current (0.1):** ~2 Hz cutoff at 100 Hz, good balance for most flights

## Baro-Only Fallback

If the IMU is not available (initialization failed or not present), the filter falls back to pure barometric velocity estimation:

```c
velocity = alpha * instantaneous + (1 - alpha) * previous
```

Where:
- `kVelocitySmoothingAlpha` = 0.15
- `instantaneous = (alt_current - alt_previous) / dt`

This provides usable velocity but with more noise and latency than the fused estimate.

## Practical Tips

### Before First Flight

1. Power on the flight computer on the pad
2. Wait 10+ seconds for the accel bias to converge (visible as stable near-zero velocity on the idle display)
3. Arm the rocket
4. Do not move the rocket after arming (ground reference is captured at arm time)

### Verifying Filter Health

In telemetry data, check:
- **Velocity near zero at rest** (< 0.1 m/s) - indicates good bias convergence
- **Altitude near zero at rest** (< 0.5 m) - indicates filter is tracking barometer
- **Smooth velocity profile** during flight - no sudden jumps or oscillations

### Interpreting dt Clamping

The filter clamps dt to 50ms (`kCfMaxDtS`). If the actual dt exceeds this (e.g., after a pause in sensor reads), the excess time is discarded to prevent large integration jumps. This should not happen during normal flight operation.

## Mathematical Background

The complementary filter is equivalent to a second-order observer for the system:

```
State: [altitude, velocity, bias]
Prediction: double integration of (measured_accel - bias - gravity)
Correction: proportional feedback from barometric altitude error

d/dt [alt]  = vel
d/dt [vel]  = accel_measured - g - bias
d/dt [bias] = 0  (slowly varying)

Correction terms:
alt  += Ka * (baro_alt - filter_alt) * dt
vel  += Kv * (baro_alt - filter_alt) * dt
bias -= Kb * (baro_alt - filter_alt) * dt
```

The gains Ka, Kv, Kb determine the observer poles. For a critically-damped response at crossover frequency w_c:
- Ka ~ 2 * w_c
- Kv ~ w_c^2
- Kb ~ w_c^3 (very slow, for bias only)

With the current gains (Ka=6, Kv=10), the crossover is approximately w_c ~ 3 rad/s (~0.5 Hz).
