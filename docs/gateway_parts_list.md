# Ground Gateway Parts List

Complete bill of materials for building the ground gateway. Two options are available: the Heltec Wireless Tracker (recommended) or an RP2040-based gateway.

## Option 1: Heltec Gateway (Recommended)

All-in-one solution with built-in WiFi, GPS, display, and LoRa radio.

| Component | Description | Price | Link |
|-----------|-------------|-------|------|
| Heltec Wireless Tracker | ESP32-S3 + SX1262 LoRa + GPS + OLED | $26.90 | [heltec.org](https://heltec.org/project/wireless-tracker/) |
| 915MHz LoRa Antenna (SMA) | Included with board | - | - |
| USB-C Cable | Power and initial programming | $5.00 | - |
| LiPo Battery 3.7V 1000mAh (optional) | Portable operation | $8.00 | - |

**Heltec Gateway Total:** ~$32-40

### Heltec Features
- Built-in WiFi (AP mode or station mode)
- Built-in GPS (UC6580 module)
- Built-in 0.96" OLED display
- SX1262 LoRa radio (915 MHz)
- OTA firmware updates over WiFi
- mDNS discovery (`RocketGateway.local`)
- USB-C connector
- Compact form factor

### Heltec Notes
- The built-in GPS has a weak ceramic antenna - works outdoors but struggles indoors
- For reliable GPS, add an external active GPS antenna with IPEX/U.FL connector
- WiFi range is adequate for field use (~50-100m line of sight)

### Optional: External GPS Antenna

| Component | Description | Price | Link |
|-----------|-------------|-------|------|
| Active GPS Antenna (28dB, IPEX) | External antenna for better reception | $12.00 | Amazon/AliExpress |

---

## Option 2: RP2040 Gateway

Modular Feather-based gateway with optional WiFi.

### Core Components

| Component | Adafruit Product | Price | Link |
|-----------|------------------|-------|------|
| Feather RP2040 with RFM95 LoRa Radio (915MHz) | PID 5714 | $24.95 | [adafruit.com/product/5714](https://www.adafruit.com/product/5714) |
| FeatherWing OLED 128x64 Display | PID 4650 | $14.95 | [adafruit.com/product/4650](https://www.adafruit.com/product/4650) |
| LiPo Battery 3.7V 500mAh | PID 258 | $7.95 | [adafruit.com/product/258](https://www.adafruit.com/product/258) |

**RP2040 Core Total:** ~$48

### Optional: WiFi Support

| Component | Adafruit Product | Price | Link |
|-----------|------------------|-------|------|
| AirLift FeatherWing (ESP32 WiFi) | PID 4264 | $12.95 | [adafruit.com/product/4264](https://www.adafruit.com/product/4264) |
| FeatherWing Tripler Mini Kit | PID 3417 | $8.50 | [adafruit.com/product/3417](https://www.adafruit.com/product/3417) |

**With WiFi Total:** ~$70

### Optional: GPS Support

| Component | Adafruit Product | Price | Link |
|-----------|------------------|-------|------|
| Ultimate GPS FeatherWing | PID 3133 | $39.95 | [adafruit.com/product/3133](https://www.adafruit.com/product/3133) |

**With GPS Total:** ~$88 (or ~$110 with WiFi)

### Optional: Ground Pressure Reference

| Component | Adafruit Product | Price | Link |
|-----------|------------------|-------|------|
| BMP390 Barometric Sensor (STEMMA QT) | PID 4816 | $12.50 | [adafruit.com/product/4816](https://www.adafruit.com/product/4816) |
| STEMMA QT Cable (100mm) | PID 4210 | $0.95 | [adafruit.com/product/4210](https://www.adafruit.com/product/4210) |

---

## Comparison

| Feature | Heltec Gateway | RP2040 Gateway |
|---------|----------------|----------------|
| **Price** | ~$32 | $48-110 |
| **WiFi** | Built-in | Optional ($13) |
| **GPS** | Built-in | Optional ($40) |
| **Display** | Built-in OLED | FeatherWing OLED |
| **LoRa Chip** | SX1262 | RFM95 (SX1276) |
| **OTA Updates** | Yes | No |
| **mDNS** | Yes | No |
| **Build System** | Arduino IDE | CMake/Pico SDK |
| **Form Factor** | Compact | Modular/Stackable |
| **Expandability** | Limited | FeatherWing ecosystem |

---

## Recommendation

**For most users: Heltec Wireless Tracker**

- Lower cost ($32 vs $70+)
- All features built-in (WiFi, GPS, display)
- OTA updates - no USB needed for firmware updates
- mDNS discovery - easy to find on network
- Compact form factor

**Choose RP2040 if you:**

- Already have Feather components
- Need ground pressure reference (differential altitude)
- Want to use the Feather ecosystem for expansion
- Prefer USB serial output over WiFi

---

## Where to Buy

### Heltec
- **Heltec Official:** [heltec.org](https://heltec.org)
- **AliExpress:** Search "Heltec Wireless Tracker"
- **Amazon:** Higher price but faster shipping

### Adafruit Components
- **Adafruit:** [adafruit.com](https://www.adafruit.com)
- **DigiKey:** [digikey.com](https://www.digikey.com)
- **Mouser:** [mouser.com](https://www.mouser.com)
