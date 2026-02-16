# eInk Feather Friend Board Modifications

## Overview

The Adafruit eInk Feather Friend (PID 4446) is designed to plug directly into the Feather header and use the Feather's hardware SPI bus. For the Rocket Avionics flight computer (Feather RP2040 RFM95), this creates two problems:

1. **SPI bus conflict**: The Feather Friend's SPI (SCK/MOSI/MISO) routes through the Feather header to SPI1 (GP14/GP15/GP8), which is shared with the LoRa radio. Both devices cannot share a bus across two cores.
2. **RST tied to Feather Reset**: The display RST is connected to the Feather's reset circuit, not a GPIO pin. The firmware needs GPIO-controlled reset on GP12.

The solution: cut the SPI traces on the Feather Friend and wire them to dedicated bit-banged SPI pins (GP24/GP25), and cut the RST trace to wire to GP12.

---

## Feather Friend Default Pin Mapping

The Feather Friend connects these signals through the Feather header:

| Signal | Feather Pin | RP2040 GPIO | Purpose |
|--------|-------------|-------------|---------|
| SCK | SPI SCK | GP14 | SPI clock (shared with LoRa) |
| MOSI | SPI MOSI | GP15 | SPI data out (shared with LoRa) |
| MISO | SPI MISO | GP8 | SPI data in (shared with LoRa) |
| ECS | D9 | GP9 | eInk chip select |
| DC | D10 | GP10 | Data/Command |
| SRCS | D6 | GP6 | SRAM chip select |
| SDCS | D5 | GP5 | SD card chip select |
| RST | RST | (reset circuit) | Display reset (auto-reset) |
| BUSY | (breakout pad) | (not connected) | Busy signal |

## Firmware Pin Mapping (After Modifications)

| Signal | RP2040 GPIO | Source |
|--------|-------------|--------|
| SCK | GP24 | Bodge wire from Feather Friend SCK pad |
| MOSI | GP25 | Bodge wire from Feather Friend MOSI pad |
| ECS | GP9 | Feather header (unchanged) |
| DC | GP10 | Feather header (unchanged) |
| SRCS | GP6 | Feather header (unchanged, held high) |
| SDCS | GP5 | Feather header (unchanged, held high) |
| RST | GP12 | Bodge wire from Feather Friend RST pad |
| BUSY | GP13 | Wire from BUSY breakout pad |

---

 Required trace cuts (3):                                                                                                                                        
  1. SCK — disconnect from Feather SPI header (GP14), rewire to GP24
  2. MOSI — disconnect from Feather SPI header (GP15), rewire to GP25                                                                                             
  3. RST — disconnect from Feather reset circuit, rewire to GP12                

  Optional trace cuts (3):
  4. MISO — unused (display is write-only), prevents interference with LoRa reads
  5. SDCS (D5) — SD card unused, frees GP5 for Button C
  6. SRCS (D6) — SRAM unused, frees GP6 for Button B

  Wires to add (4):
  - SCK pad → GP24, MOSI pad → GP25, RST pad → GP12, BUSY breakout pad → GP13

  The ECS (D9/GP9) and DC (D10/GP10) pins pass through the Feather header unchanged — no cuts needed for those.



## Traces to Cut

Cut these traces on the **back** of the Feather Friend PCB. Use an X-Acto knife or PCB trace cutter. Verify each cut with a multimeter (continuity test between the pad and Feather header pin).

### Cut 1: SCK Trace

**Why:** Disconnect SPI clock from the Feather header (GP14) to prevent bus contention with LoRa radio.

- Locate the trace running from the SCK pad on the 24-pin FPC connector side to the Feather header SCK pin.
- Cut the trace between the pad and the header pin.
- After cutting, wire the SCK pad to **GP24 (D24)** on the Feather.

### Cut 2: MOSI Trace

**Why:** Disconnect SPI data out from the Feather header (GP15) to prevent bus contention with LoRa radio.

- Locate the trace running from the MOSI pad to the Feather header MOSI pin.
- Cut the trace between the pad and the header pin.
- After cutting, wire the MOSI pad to **GP25 (D25)** on the Feather.

### Cut 3: RST Trace

**Why:** Disconnect display reset from the Feather's reset circuit. The firmware needs to control RST via GPIO for the triple-reset init sequence and board auto-detection.

- Locate the trace running from the RST pad to the Feather RST header pin (or auto-reset circuit components).
- Cut the trace.
- After cutting, wire the RST pad to **GP12 (D12)** on the Feather.

---

## Traces to Optionally Cut

These traces are not strictly required to cut because the firmware initializes the corresponding GPIO pins HIGH (deselected), but cutting them is recommended to eliminate any possibility of bus noise.

### Optional Cut 4: MISO Trace

**Why:** The eInk display is write-only (no data read back). MISO is only used by the SRAM chip and SD card, neither of which are used. Cutting prevents any MISO drive from interfering with LoRa SPI reads.

- Locate the MISO trace running to the Feather header MISO pin (GP8).
- Cut the trace.
- No bodge wire needed (MISO is unused).

### Optional Cut 5: SDCS Trace (D5)

**Why:** The SD card slot is not used. Cutting the trace frees GP5 for the OLED FeatherWing Button C.

- Locate the SDCS trace to Feather header D5.
- Cut the trace.

### Optional Cut 6: SRCS Trace (D6)

**Why:** The 32KB SRAM chip is not used (the firmware uses its own framebuffer in RP2040 RAM). Cutting the trace frees GP6 for the OLED FeatherWing Button B.

- Locate the SRCS trace to Feather header D6.
- Cut the trace.

---

## Wires to Add

After cutting traces, add these bodge wires:

| From (Feather Friend) | To (Feather RP2040 RFM95) | Wire |
|------------------------|---------------------------|------|
| SCK pad | GP24 (D24) | 30 AWG wire-wrap or Kynar |
| MOSI pad | GP25 (D25) | 30 AWG wire-wrap or Kynar |
| RST pad | GP12 (D12) | 30 AWG wire-wrap or Kynar |
| BUSY breakout pad | GP13 (D13) | 30 AWG wire-wrap or Kynar |

### BUSY Pin Note

The BUSY pin is **not** connected to any Feather header pin by default on the Feather Friend. There is a breakout pad labeled BUSY on the board. Solder a wire from this pad to GP13 (D13) on the Feather. The firmware uses BUSY to detect when the display has finished refreshing. If BUSY is not wired, the driver falls back to fixed timing delays (3s full refresh, 500ms partial), which works but is slower.

---

## Pin Conflict Notes

The eInk Feather Friend's default pins overlap with the OLED FeatherWing buttons:

| GPIO | eInk Feather Friend | OLED FeatherWing |
|------|---------------------|------------------|
| GP9 | ECS (eInk CS) | Button A |
| GP6 | SRCS (SRAM CS) | Button B |
| GP5 | SDCS (SD CS) | Button C |

**Resolution:** The firmware handles this by initializing these pins as SPI outputs (HIGH/deselected) during `InitializeSPI()`, then re-initializing GP9/GP6/GP5 as inputs with pull-ups during `InitializeButtons()`. Since the eInk CS is controlled via software (driven LOW only during SPI transfers), and the SRAM/SD chip selects are held permanently HIGH (deselected), the buttons continue to function normally.

If you cut the SRCS (D6) and SDCS (D5) traces as recommended above, this conflict is eliminated entirely for those two pins.

---

## Verification Checklist

After making modifications:

- [ ] Continuity: SCK pad to GP24 (D24) on Feather
- [ ] Continuity: MOSI pad to GP25 (D25) on Feather
- [ ] Continuity: RST pad to GP12 (D12) on Feather
- [ ] Continuity: BUSY pad to GP13 (D13) on Feather
- [ ] No continuity: SCK pad to Feather header SCK (GP14)
- [ ] No continuity: MOSI pad to Feather header MOSI (GP15)
- [ ] No continuity: RST pad to Feather header RST
- [ ] No continuity: MISO pad to Feather header MISO (GP8) (if cut)
- [ ] Flash firmware and verify display initializes (check USB console for `EPD: Detected: eInk Feather Friend`)

---

## Reference

- [Adafruit eInk Feather Friend (PID 4446)](https://www.adafruit.com/product/4446)
- [Adafruit 2.9" eInk Display Pinouts](https://learn.adafruit.com/adafruit-2-9-eink-display-breakouts-and-featherwings/pinouts)
- [Feather RP2040 RFM95 Pinouts](https://learn.adafruit.com/feather-rp2040-rfm95/pinouts)
- Schematics: `hardware/eInkBreakoutFriend/Adafruit eInk Feather Friend.sch`
- Firmware pin definitions: `firmware_flight/include/pins.h`
- eInk display architecture: `docs/EINK_DISPLAY.md`
