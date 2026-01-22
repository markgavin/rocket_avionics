# Conversation Archive: Software Versioning Implementation

**Date:** 2026-01-22
**Topics:** Carrier board design finalization, software versioning system

---

## Summary

This session focused on finalizing the carrier board design document and implementing a per-component software versioning system with automatic build timestamp updates.

---

## 1. Carrier Board Design Finalization

### Context
Continued from previous session where we designed a custom carrier board for the Feather RP2040 RFM95.

### Final Design (v2.1)
- **Dual SPI Architecture:**
  - SPI1 (1 MHz): LoRa radio + BMP390 barometer
  - SPI0 (5 MHz): ICM-42688 IMU + ADXL375 high-G accelerometer
- Socket-mounted Feather for ease of development
- Document: `docs/flight_computer_carrier_board_design.md`

### Commits
- `248cadf` - Add flight computer carrier board design document v2.1

---

## 2. Software Versioning Implementation

### Problem Statement
User noticed that the build date displayed on gateway/flight computer was sometimes stale because the file containing `__DATE__` wasn't recompiled when other files changed.

### Solution Discussion

Four approaches were discussed:

| Approach | Description |
|----------|-------------|
| A. Shared Version File | Single version.h at project root |
| B. Per-Component Versions | Each firmware has own version (chosen) |
| C. Git Tag + Auto-Generate | Version from git describe |
| D. Hybrid | Shared VERSION file + build injection |

**User selected: Approach B (Per-Component Versions)** with force-rebuilt build timestamps.

### Implementation Details

#### File Structure
```
firmware_flight/
├── include/version.h      # Declarations + version string
└── src/version.c          # Build timestamp (force-rebuilt)

firmware_gateway/
├── include/version.h      # Declarations + version string
└── src/version.c          # Build timestamp (force-rebuilt)

firmware_gateway_heltec/
└── version.h              # Version + timestamp (Arduino rebuilds all)
```

#### version.h Pattern
```c
#pragma once

#define FIRMWARE_VERSION_MAJOR      1
#define FIRMWARE_VERSION_MINOR      0
#define FIRMWARE_VERSION_PATCH      0
#define FIRMWARE_VERSION_STRING     "1.0.0"

// Build timestamp (defined in version.c)
extern const char * const kBuildDate ;
extern const char * const kBuildTime ;
```

#### version.c Pattern
```c
#include "version.h"

const char * const kBuildDate = __DATE__ ;
const char * const kBuildTime = __TIME__ ;
```

#### CMake Force-Rebuild
```cmake
add_custom_command(
    TARGET rocket_gateway PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/src/version.c
    COMMENT "Touching version.c to update build timestamp"
)
```

### Files Modified

| Project | Files |
|---------|-------|
| firmware_gateway | `include/version.h`, `src/version.c` (new), `CMakeLists.txt`, `src/main.c` |
| firmware_flight | `include/version.h`, `src/version.c` (new), `CMakeLists.txt`, `src/main.c` |
| firmware_gateway_heltec | `version.h` (new), `firmware_gateway_heltec.ino` |

### Code Changes in main.c

Before:
```c
printf("  Build: %s %s\n", __DATE__, __TIME__) ;
```

After:
```c
printf("  Build: %s %s\n", kBuildDate, kBuildTime) ;
```

### Build Verification
Both firmware projects build successfully with force-rebuild working:
```
Touching version.c to update build timestamp
```

### Commits
- `6be81a5` - Add per-component versioning with force-rebuilt build timestamps
- `15c19e3` - Add software versioning documentation

---

## 3. Documentation Created

- `docs/software_versioning.md` - Comprehensive guide covering:
  - Semantic versioning format
  - File structure and contents
  - Force-rebuild mechanism
  - Usage examples
  - Version update workflow
  - Troubleshooting guide

---

## 4. Other Commits This Session

Prior to versioning work, pushed accumulated changes:
- `3bc2f68` - Update gateway firmware: enable GPS, add NeoPixel driver, WiFi support
- `ab30e67` - Add iOS rocket recovery and flash download features
- `e22d0b5` - Add desktop WiFi configuration and connection updates
- `0bb1830` - Add USB serial console connection script

---

## 5. Version Update Workflow (Going Forward)

1. Before release commit, update `FIRMWARE_VERSION_STRING` in `version.h`
2. Build to verify compilation
3. Commit with version in message: `"Gateway v1.1.0: Add feature X"`
4. Push to remote
5. Build timestamps auto-update on every build

---

## Key Decisions Made

| Decision | Rationale |
|----------|-----------|
| Per-component versions | Each firmware evolves independently |
| Separate version.c | Allows force-rebuild without touching version.h |
| CMake PRE_BUILD touch | Simple, reliable timestamp update |
| Keep FIRMWARE_VERSION_STRING as #define | Backward compatible, easy to grep/update |
| Use extern for kBuildDate/kBuildTime | Clean separation of declaration/definition |

---

## Files Reference

| File | Purpose |
|------|---------|
| `docs/flight_computer_carrier_board_design.md` | Carrier board design v2.1 |
| `docs/software_versioning.md` | Versioning system documentation |
| `firmware_*/include/version.h` | Version declarations |
| `firmware_*/src/version.c` | Build timestamps (CMake projects) |
| `firmware_gateway_heltec/version.h` | Heltec version + timestamps |

---

## Current Component Versions

| Component | Version |
|-----------|---------|
| firmware_flight | 1.0.0 |
| firmware_gateway | 1.0.0 |
| firmware_gateway_heltec | 1.0.0 |
