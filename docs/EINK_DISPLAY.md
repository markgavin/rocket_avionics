# eInk Display: Dual-Core Architecture and Partial Refresh

## Overview

The flight computer supports an optional UC8151D 2.9" flexible monochrome eInk display (296x128 pixels, Adafruit PID 4262) as an alternative to the SSD1306 OLED FeatherWing. The eInk display presents a unique challenge: full refresh takes 2-3 seconds, which would block LoRa telemetry (10 Hz) and sensor reads on the main loop. The solution uses RP2040 dual-core architecture to run the display independently on core1.

Build with `-DDISPLAY_EINK=1` in CMakeLists.txt to enable eInk mode.

---

## Hardware

| Parameter | Value |
|-----------|-------|
| Display | Adafruit 2.9" Flexible 296x128 Monochrome eInk (PID 4262) |
| Controller | UC8151D |
| Interface | Bit-banged SPI on GP24 (SCK) / GP25 (MOSI) |
| Control Pins | CS (GP9), DC (GP10), RESET (GP11), BUSY (GP13) |
| Orientation | Landscape 296x128, ribbon cable on left (180° rotation) |
| Buffer Size | 4,736 bytes (128 wide x 296 tall, 1 bit per pixel) |

The eInk display uses **dedicated GPIO pins** for bit-banged SPI, completely separate from the LoRa radio's hardware SPI1 (GP14/GP15). This eliminates bus contention between cores.

### BUSY Pin

The BUSY pin (GP13) signals when the display is ready (high) or processing (low). The driver checks BUSY with a 200ms timeout; if the pin doesn't respond (hardware issue on some boards), it falls back to a fixed delay (3s for full refresh, 500ms for partial refresh).

---

## Dual-Core Architecture

### Core Assignment

| Core | Responsibilities |
|------|-----------------|
| Core 0 | Sensors, flight state machine, LoRa telemetry (10 Hz), GPS, heartbeat LED, flash storage |
| Core 1 | eInk display rendering and refresh (can block freely for seconds) |

### Why Dual-Core?

eInk refresh blocks for 500ms-3s depending on refresh type. On a single core, this would starve LoRa telemetry and sensor reads. Moving the display to core1 allows it to block independently while core0 continues real-time operations uninterrupted.

### Inter-Core Communication

Communication between cores uses a **lock-free** design. No mutexes, no shared alarm pool, no printf from core1.

```
Core 0 (writes)                    Core 1 (reads)
─────────────────                  ─────────────────
sDisplayShared ──────────────────> theData = sDisplayShared
  (struct copy)                      (lock-free copy)

sBtnCycleMode  ──────────────────> read + clear flag
sBtnHome       ──────────────────> read + clear flag
sBtnEditRocket ──────────────────> read + clear flag

                                   sCore1CurrentMode ──> Core 0 reads
                                   sCore1Iterations  ──> Core 0 reads
```

**DisplaySharedData** struct contains all telemetry values (altitude, velocity, pressure, temperature, GPS, LoRa status, flight state, etc.). Core0 writes it unconditionally every main loop iteration (~1 kHz). Core1 reads it every display update cycle (~0.5 Hz). Torn reads are harmless — worst case, one frame shows a mix of old/new values, imperceptible on eInk.

**Button events** use individual `volatile bool` flags. Core0 sets them on button press; core1 reads and clears them.

**Display mode** feedback uses a `volatile DisplayMode` that core1 writes and core0 reads (for Button B context-dependent behavior).

### RP2040 Safety Rules

The RP2040's Cortex-M0+ has no cache and no out-of-order execution, so `volatile` alone guarantees memory coherence between cores. However, two Pico SDK mechanisms create hidden inter-core dependencies:

1. **Alarm Pool Deadlock**: `sleep_ms()` and `sleep_us()` use a shared alarm pool backed by hardware spin lock `PICO_SPINLOCK_ID_TIMER`. The alarm pool's IRQ fires on core0. When core1 calls `sleep_ms()`, it registers an alarm dependent on core0's timer IRQ — if core0 is busy, core1 hangs waiting for its alarm. **Fix**: Use `busy_wait_us_32()` for ALL timing on core1 and in any core0 code that could contend.

2. **USB CDC Printf Deadlock**: `printf` uses an internal mutex. The USB IRQ handler runs on core0. If core1 holds the printf mutex, core0 can't service the USB IRQ to drain the buffer, so core1's printf never completes — both cores deadlock. **Fix**: Never call printf from core1. Use `sCore1Iterations` counter for debug monitoring from core0.

**Files with busy_wait fixes**: `uc8151d.c`, `lora_radio.c` (TX polling loop), `heartbeat_led.c` (SendColor).

---

## Display Driver (uc8151d.c)

### Initialization

The UC8151D init sequence runs on core0 before core1 launches:

1. Triple hardware reset (per Adafruit driver reference)
2. Power settings (0x01): VDS=3.3V, VDG=internal, VCOM=2B/2B
3. Booster soft start (0x06): phase A/B/C = 0x17
4. Power on (0x04) + wait for BUSY
5. Panel setting (0x00): **0x1F for monochrome** (not 0xCF which is tri-color)
6. VCOM and data interval (0x50): 0x97
7. PLL clock (0x30): 0x29
8. Resolution (0x61): 128 wide x 296 tall
9. VCM DC setting (0xBD): 0x0A

### Coordinate System

The physical display is landscape (296 wide x 128 tall) but the UC8151D controller uses portrait orientation (128 wide x 296 tall). A 180° rotation is applied because the ribbon cable is on the left:

```
Physical (landscape)         Controller (portrait)
+------------------+         +--------+
|                  |         |        |
| (0,0)    (295,0) |   →     | row 295|  row = (295 - physX)
|                  |         |  ...   |
| (0,127) (295,127)|         | row 0  |
+------------------+         +--------+
  ribbon cable (left)        16 bytes/row
```

**Pixel mapping** (framebuffer.c):
```c
buffer_row = (295 - physical_X)
byte_index = physical_Y / 8
bit_index  = 7 - (physical_Y % 8)     // MSB first
offset     = buffer_row * 16 + byte_index
```

### Full Refresh

`UC8151D_WriteImage()` sends the entire 4,736-byte framebuffer as DTM2 (new data), then triggers refresh. Takes ~3 seconds with BUSY wait.

### Partial Refresh

`UC8151D_WritePartial()` updates a rectangular region without redrawing the entire screen. Takes ~500ms. Requires both old data (DTM1) and new data (DTM2) — the display uses the difference to compute the optimal waveform.

```
Partial Refresh Sequence:
1. PTIN (0x91)           — Enter partial mode
2. PTL  (0x90)           — Set partial window (H start/end, V start/end, scan mode)
3. DTM1 (0x10)           — Send OLD data for the region
4. DTM2 (0x13)           — Send NEW data for the region
5. DRF  (0x12)           — Trigger refresh
6. Wait for BUSY/delay   — Display processes the update
7. PTOUT (0x92)          — Exit partial mode
```

The partial window coordinates must be **byte-aligned** horizontally (H values rounded to 8-pixel boundaries).

---

## Partial Refresh Strategy (eink_display.c)

### Refresh Hierarchy

The display uses progressively larger refresh regions depending on what changed:

| Level | Region Size | When Used |
|-------|------------|-----------|
| Per-digit | 12x16 px | Individual digit changed (same string length) |
| Per-box | ~100x16 px | String length changed (value got longer/shorter) |
| Content area | 296x96 px | Screens that redraw all content (LoRa, GPS) |
| Full refresh | 296x128 px | Every 50 updates, or when switching screens |

### Per-Digit Refresh (RefreshValue)

The core optimization. For each displayed value (altitude, velocity, pressure, etc.):

1. Compare old string with new string character by character
2. **Same length**: For each changed character, clear its 12x16 cell in the framebuffer, draw the new character, and `UC8151D_WritePartial()` just that cell
3. **Different length**: Clear the full column box (~100x16), redraw the value + unit, and `UC8151D_WritePartial()` the box

```
Example: altitude changes from "12.3" to "12.5"

  "12.3"  vs  "12.5"
   ││││       ││││
   ====       ==!= ← only position 3 differs

  → Clear 12x16 cell at digit position 3
  → Draw '5' in framebuffer
  → WritePartial for 12x16 region only
```

This minimizes data transfer (~24 bytes vs 4,736 for full refresh) and reduces ghosting on unchanged pixels.

### Double Buffering

Two 4,736-byte framebuffers maintain display state:

- **sFrameBuffer**: Working buffer — all drawing happens here
- **sPrevBuffer**: Previous display state — used as "old data" for partial refresh

**Critical rule**: `memcpy(sPrevBuffer, sFrameBuffer)` must happen **after** all `UC8151D_WritePartial()` calls in an update cycle, never before. This ensures sPrevBuffer always reflects what was last sent to the display.

```
Update Cycle:
1. RefreshValue() modifies sFrameBuffer + calls WritePartial(sPrevBuffer, sFrameBuffer, ...)
2. RefreshValue() for next value (same pattern)
3. memcpy(sPrevBuffer, sFrameBuffer)  ← sync AFTER all writes
```

### Full Refresh Interval

Every 50 updates (~100 seconds at 2s interval), a full refresh clears accumulated eInk ghosting. The full refresh redraws the entire screen layout (header, labels, values, separators, footer) and uses `UC8151D_WriteImage()`.

---

## Display Screens

### Layout (296x128 landscape)

```
+--[ROCKET AVIONICS  IDLE]--[R:1  GPS:--]--+  ← Header (inverted, 14px)
|  ALTITUDE        VELOCITY                 |  ← Labels (y=20)
|  12.3 m          0.0 m/s                  |  ← Values (y=32, scale 2)
|──────────────────────────────────────────|  ← Separator (y=52)
|  GATEWAY         GPS                      |  ← Labels (y=56)
|  Good            Fix 8                    |  ← Values (y=72, scale 2)
|──────────────────────────────────────────|  ← Separator (y=92)
+--[v2.0.0  26-02-12]─────────────────────+  ← Footer (inverted, 14px)
```

### Available Modes

| Mode | Content | Refresh Strategy |
|------|---------|-----------------|
| Live | Altitude, velocity, gateway, GPS | Per-digit |
| Sensors | Pressure, temperature, altitude | Per-digit |
| LoRa Status | Link, RSSI, TX/RX packet counts | Content area |
| GPS Status | Fix, satellites, lat/lon, speed | Content area |
| Rocket ID | ID number (editable via Button B) | Content area |
| Device Info | Firmware, sensor status | Full (static) |
| About | Version, copyright | Full (static) |
| Armed | "READY FOR LAUNCH" | Full (static) |
| Flight Complete | Max altitude, velocity, times | Full (static) |

### Button Navigation

- **Button A**: Cycle through display modes
- **Button B**: On Rocket ID screen → cycle ID (0-15); on all other screens → return to Live

### Flight Behavior

During active flight (Boost through Descent), core1 skips display updates entirely to avoid any potential interference. The display resumes updating after landing.

---

## Core1 Display Loop

The display loop runs continuously on core1 after launch:

```
Core1_DisplayLoop:
  while (1):
    1. Copy sDisplayShared from core0 (lock-free struct copy)
    2. Read/clear button event flags
    3. Process button events (cycle mode, go home)
    4. Skip display during active flight (Boost..Descent)
    5. Call appropriate StatusDisplay_* function based on mode
    6. Wait 2 seconds (busy_wait_us_32) before next update
       - 5 seconds in LANDED/COMPLETE state
       - 200ms after button press (responsive mode switch)
```

No printf, no sleep_ms, no mutexes — fully independent from core0.

---

## File Reference

| File | Purpose |
|------|---------|
| `src/uc8151d.c` | UC8151D driver: init, clear, full/partial refresh, sleep |
| `include/uc8151d.h` | Display constants, command definitions, function declarations |
| `src/framebuffer.c` | Pixel/line/rect/text drawing with coordinate rotation |
| `include/framebuffer.h` | Framebuffer API |
| `include/font5x7.h` | 5x7 pixel font bitmap (ASCII 32-126) |
| `src/eink_display.c` | All display screens, per-digit refresh logic, mode management |
| `src/main.c` | Core1 launch, DisplaySharedData, Core1_DisplayLoop |
| `include/pins.h` | eInk GPIO pin definitions, update intervals |

---

## Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| Display never updates after boot | Core1 deadlock from sleep_ms or printf | Ensure all core1 code paths use busy_wait_us_32, no printf |
| Display shows stale values | sPrevBuffer synced before WritePartial | Move memcpy(sPrevBuffer, sFrameBuffer) to AFTER all WritePartial calls |
| Both cores hang after 1-2 display updates | Alarm pool contention via sleep_ms/sleep_us | Replace with busy_wait_us_32 in uc8151d.c, lora_radio.c, heartbeat_led.c |
| Partial refresh shows ghosting | Normal eInk behavior | Full refresh every 50 updates clears it; reduce interval if needed |
| Display blank / no init | SPI pin conflict or init failure | Verify GP24/GP25 not used elsewhere; check init printf output |
| BUSY pin timeout messages | Pin not connected or floating | Normal — driver falls back to fixed delays automatically |
