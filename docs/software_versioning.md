# Software Versioning System

**Author:** Mark Gavin
**Created:** 2026-01-22
**Version:** 1.0

## Overview

This document describes the per-component versioning system used across all Rocket Avionics firmware projects. Each firmware maintains its own version number, updated independently when that component changes.

## Version Format

All components use **Semantic Versioning (SemVer)**:

```
MAJOR.MINOR.PATCH
```

| Component | Example |
|-----------|---------|
| MAJOR | Breaking changes, protocol incompatibility |
| MINOR | New features, backward compatible |
| PATCH | Bug fixes, minor improvements |

## Project Structure

```
rocket_avionics/
├── firmware_flight/
│   ├── include/
│   │   └── version.h          # Version declarations
│   └── src/
│       └── version.c          # Build timestamp (force-rebuilt)
│
├── firmware_gateway/
│   ├── include/
│   │   └── version.h          # Version declarations
│   └── src/
│       └── version.c          # Build timestamp (force-rebuilt)
│
└── firmware_gateway_heltec/
    └── version.h              # Version + build timestamp
```

## File Contents

### version.h (CMake Projects)

```c
#pragma once

//----------------------------------------------
// Version Components
//----------------------------------------------
#define FIRMWARE_VERSION_MAJOR      1
#define FIRMWARE_VERSION_MINOR      0
#define FIRMWARE_VERSION_PATCH      0
#define FIRMWARE_VERSION_STRING     "1.0.0"

//----------------------------------------------
// Build Timestamp (defined in version.c)
//----------------------------------------------
extern const char * const kBuildDate ;
extern const char * const kBuildTime ;
```

### version.c (CMake Projects)

```c
#include "version.h"

//----------------------------------------------
// Build Timestamp
// Automatically updated on each build
//----------------------------------------------
const char * const kBuildDate = __DATE__ ;
const char * const kBuildTime = __TIME__ ;
```

### version.h (Arduino/Heltec)

```c
#pragma once

#define FIRMWARE_VERSION_MAJOR      1
#define FIRMWARE_VERSION_MINOR      0
#define FIRMWARE_VERSION_PATCH      0
#define FIRMWARE_VERSION_STRING     "1.0.0"
#define FIRMWARE_BUILD_TYPE         "HeltecGateway"

// Arduino rebuilds all files, so these are always current
#define FIRMWARE_BUILD_DATE         __DATE__
#define FIRMWARE_BUILD_TIME         __TIME__
```

## Force-Rebuild Mechanism (CMake)

The `__DATE__` and `__TIME__` macros are evaluated at compile time. Without intervention, `version.c` would only recompile when its contents change, leaving the build timestamp stale.

**Solution:** CMake touches `version.c` before each build:

```cmake
# In CMakeLists.txt
add_custom_command(
    TARGET rocket_gateway PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/src/version.c
    COMMENT "Touching version.c to update build timestamp"
)
```

This ensures `version.c` is always recompiled, updating the timestamp.

**Build output confirmation:**
```
Touching version.c to update build timestamp
```

## Usage in Firmware

### Startup Banner

```c
printf("======================================\n") ;
printf("  ROCKET AVIONICS FLIGHT COMPUTER\n") ;
printf("  Version %s\n", FIRMWARE_VERSION_STRING) ;
printf("  Build: %s %s\n", kBuildDate, kBuildTime) ;
printf("======================================\n") ;
```

### Display About Screen

```c
StatusDisplay_ShowAbout(FIRMWARE_VERSION_STRING, kBuildDate, kBuildTime) ;
```

### JSON Status Messages

```c
sprintf(json, "{\"type\":\"status\",\"version\":\"%s\",\"build\":\"%s %s\"}",
        FIRMWARE_VERSION_STRING, kBuildDate, kBuildTime) ;
```

## When to Update Versions

### MAJOR Version (1.0.0 → 2.0.0)

- Breaking protocol changes (telemetry format, commands)
- Incompatible hardware changes
- Major architectural rewrites

### MINOR Version (1.0.0 → 1.1.0)

- New features (flash storage, GPS support)
- New commands or telemetry fields
- Significant improvements

### PATCH Version (1.0.0 → 1.0.1)

- Bug fixes
- Performance improvements
- Documentation updates
- Minor tweaks

## Version Update Workflow

### Before a Release Commit

1. **Determine version bump** based on changes (major/minor/patch)

2. **Update version.h** for the affected component:
   ```c
   #define FIRMWARE_VERSION_MAJOR      1
   #define FIRMWARE_VERSION_MINOR      1    // ← Updated
   #define FIRMWARE_VERSION_PATCH      0
   #define FIRMWARE_VERSION_STRING     "1.1.0"  // ← Updated
   ```

3. **Build** to verify compilation

4. **Commit** with version in message:
   ```
   Gateway v1.1.0: Add WiFi AP mode support
   ```

5. **Push** to remote

### Example Commit Messages

```
Flight v1.2.0: Add high-G accelerometer support
Gateway v1.0.3: Fix GPS parsing for southern hemisphere
Heltec v1.1.0: Add station mode WiFi fallback
```

## Current Versions

| Component | Version | Description |
|-----------|---------|-------------|
| firmware_flight | 1.0.0 | Flight computer firmware |
| firmware_gateway | 1.0.0 | RP2040 ground gateway |
| firmware_gateway_heltec | 1.0.0 | Heltec Wireless Tracker gateway |

## Version Compatibility

When updating versions, consider compatibility:

| Flight Computer | Gateway | Compatible? |
|-----------------|---------|-------------|
| 1.x.x | 1.x.x | Yes |
| 2.x.x | 1.x.x | Check protocol |
| 1.x.x | 2.x.x | Check protocol |

**Protocol Version** is defined separately in `version.h`:
```c
#define PROTOCOL_VERSION  "1.0"
```

Update `PROTOCOL_VERSION` when telemetry/command format changes.

## Troubleshooting

### Build timestamp not updating

**Symptom:** Display shows old build date after rebuilding.

**Cause:** `version.c` wasn't recompiled.

**Solutions:**
1. Verify CMakeLists.txt has the `add_custom_command` for touching version.c
2. Clean rebuild: `rm -rf build && mkdir build && cd build && cmake .. && make`
3. Check build output for "Touching version.c to update build timestamp"

### Version mismatch between components

**Symptom:** Gateway and flight computer have different protocol behavior.

**Solution:** Check `PROTOCOL_VERSION` in both `version.h` files. They should match for compatible communication.

## Files Reference

| File | Purpose |
|------|---------|
| `firmware_flight/include/version.h` | Flight computer version declarations |
| `firmware_flight/src/version.c` | Flight computer build timestamp |
| `firmware_gateway/include/version.h` | Gateway version declarations |
| `firmware_gateway/src/version.c` | Gateway build timestamp |
| `firmware_gateway_heltec/version.h` | Heltec version + timestamp |
