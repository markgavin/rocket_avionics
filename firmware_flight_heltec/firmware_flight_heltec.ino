//----------------------------------------------
// Rocket Avionics Flight Computer - Heltec Wireless Tracker
//
// Hardware: Heltec Wireless Tracker (ESP32-S3 + SX1262 + GPS)
//           External: BMP390 (I2C), LSM6DSOX+LIS3MDL IMU (I2C)
// Purpose:  Rocket flight computer with telemetry, GPS, dual deploy
//
// Author: Mark Gavin
// Created: 2026-02-01
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include <RadioLib.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <Preferences.h>
#include <TinyGPSPlus.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Wire.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <math.h>
#include "version.h"

//----------------------------------------------
// Pin Definitions
//----------------------------------------------
// LoRa SX1262 (built-in)
#define LORA_NSS    8
#define LORA_DIO1   14
#define LORA_RST    12
#define LORA_BUSY   13

// GPS UART (built-in UC6580)
#define GPS_RX      33
#define GPS_TX      34
#define GPS_BAUD    115200

// TFT Display (built-in ST7735)
#define TFT_MOSI    42
#define TFT_SCLK    41
#define TFT_CS      38
#define TFT_DC      40
#define TFT_RST     39
#define TFT_BL      21
#define TFT_POWER   3       // VEXT power control

// I2C for external sensors (BMP390, IMU)
#define I2C_SDA     5
#define I2C_SCL     6

// Pyro channels
#define PYRO1_FIRE  7       // Drogue
#define PYRO2_FIRE  45      // Main
#define PYRO1_CONT  2       // ADC - Drogue continuity
#define PYRO2_CONT  4       // ADC - Main continuity
#define ARM_SWITCH  46      // Arm switch input (active low)

// Display dimensions
#define TFT_WIDTH   160
#define TFT_HEIGHT  80

//----------------------------------------------
// Color Definitions (RGB565)
//----------------------------------------------
#define COLOR_BLACK       ST77XX_BLACK
#define COLOR_WHITE       ST77XX_WHITE
#define COLOR_GREEN       ST77XX_GREEN
#define COLOR_RED         ST77XX_RED
#define COLOR_YELLOW      ST77XX_YELLOW
#define COLOR_CYAN        ST77XX_CYAN
#define COLOR_BLUE        ST77XX_BLUE
#define COLOR_ORANGE      0xFD20

//----------------------------------------------
// LoRa Configuration (MUST match gateway!)
//----------------------------------------------
#define LORA_FREQUENCY      915.0
#define LORA_BANDWIDTH      125.0
#define LORA_SPREAD_FACTOR  7
#define LORA_CODING_RATE    5
#define LORA_SYNC_WORD      0x14
#define LORA_PREAMBLE_LEN   8
#define LORA_TX_POWER       20      // Full power for flight

//----------------------------------------------
// Flight Configuration
//----------------------------------------------
#define MAIN_DEPLOY_ALTITUDE_M  150     // Main chute deployment altitude
#define LAUNCH_ACCEL_THRESHOLD  2.0     // g's to detect launch
#define LANDING_VEL_THRESHOLD   2.0     // m/s to detect landing
#define APOGEE_LOCKOUT_MS       2000    // Minimum time before apogee detection
#define PYRO_FIRE_DURATION_MS   500     // Pyro channel fire time

//----------------------------------------------
// Telemetry Configuration
//----------------------------------------------
#define TELEMETRY_RATE_HZ       10
#define SENSOR_RATE_HZ          100
#define DISPLAY_RATE_HZ         5

//----------------------------------------------
// I2C Addresses
//----------------------------------------------
#define BMP390_ADDR     0x77
#define LSM6DSOX_ADDR   0x6A
#define LIS3MDL_ADDR    0x1E

//----------------------------------------------
// Flight States
//----------------------------------------------
typedef enum {
    FLIGHT_STATE_IDLE = 0,
    FLIGHT_STATE_ARMED,
    FLIGHT_STATE_BOOST,
    FLIGHT_STATE_COAST,
    FLIGHT_STATE_APOGEE,
    FLIGHT_STATE_DROGUE,
    FLIGHT_STATE_MAIN,
    FLIGHT_STATE_LANDED,
    FLIGHT_STATE_COMPLETE
} FlightState;

const char* flightStateNames[] = {
    "IDLE", "ARMED", "BOOST", "COAST", "APOGEE",
    "DROGUE", "MAIN", "LANDED", "COMPLETE"
};

//----------------------------------------------
// Telemetry Packet Structure (55 bytes)
//----------------------------------------------
typedef struct __attribute__((packed)) {
    uint8_t  magic;         // 0xAA
    uint8_t  type;          // 0x01 = telemetry
    uint8_t  rocketId;      // 0-15
    uint16_t sequence;      // Packet counter
    uint8_t  state;         // Flight state
    int32_t  altitudeCm;    // Altitude in cm
    int16_t  velocityCmS;   // Velocity in cm/s
    int16_t  accelMg;       // Acceleration in milli-g
    int32_t  maxAltCm;      // Max altitude in cm
    int32_t  latitude;      // Lat * 1e7
    int32_t  longitude;     // Lon * 1e7
    uint8_t  gpsSats;       // Satellites in view
    uint8_t  gpsValid;      // GPS fix valid
    int16_t  gyroX;         // Gyro X (deg/s * 10)
    int16_t  gyroY;         // Gyro Y
    int16_t  gyroZ;         // Gyro Z
    int16_t  magX;          // Mag X
    int16_t  magY;          // Mag Y
    int16_t  magZ;          // Mag Z
    uint8_t  pyro1Cont;     // Pyro 1 continuity
    uint8_t  pyro2Cont;     // Pyro 2 continuity
    uint8_t  armed;         // Arm switch state
    int16_t  tempC10;       // Temperature * 10
    uint16_t battMv;        // Battery voltage mV
    uint8_t  checksum;      // XOR checksum
} TelemetryPacket;

//----------------------------------------------
// BMP390 Calibration Data
//----------------------------------------------
typedef struct {
    float T1, T2, T3;
    float P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11;
} BMP390Calibration;

//----------------------------------------------
// Global Objects
//----------------------------------------------
SX1262 radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
Preferences prefs;

//----------------------------------------------
// Flight State
//----------------------------------------------
FlightState flightState = FLIGHT_STATE_IDLE;
uint32_t stateEntryTime = 0;

// Altitude and velocity
float altitudeM = 0;
float velocityMps = 0;
float maxAltitudeM = 0;
float groundPressurePa = 101325;
float temperatureC = 25;

// Acceleration
float accelX = 0, accelY = 0, accelZ = 0;
float accelMagnitude = 0;

// Gyro and magnetometer
float gyroX = 0, gyroY = 0, gyroZ = 0;
float magX = 0, magY = 0, magZ = 0;

// GPS data
double gpsLatitude = 0;
double gpsLongitude = 0;
int gpsSatellites = 0;
bool gpsValid = false;

// Pyro state
bool pyro1Continuity = false;
bool pyro2Continuity = false;
bool isArmed = false;
bool pyro1Fired = false;
bool pyro2Fired = false;
uint32_t pyro1FireTime = 0;
uint32_t pyro2FireTime = 0;

// Settings
uint8_t rocketId = 0;
char rocketName[17] = "Rocket";

// Telemetry
uint16_t telemetrySeq = 0;
uint32_t lastTelemetryMs = 0;

// Sensors
uint32_t lastSensorMs = 0;
uint32_t lastDisplayMs = 0;
BMP390Calibration bmpCal;
bool bmpInitialized = false;
bool imuInitialized = false;

// LoRa
volatile bool loraPacketReceived = false;
uint32_t loraTxCount = 0;

//----------------------------------------------
// LoRa Interrupt Handler
//----------------------------------------------
void IRAM_ATTR onLoraReceive() {
    loraPacketReceived = true;
}

//----------------------------------------------
// I2C Helper Functions
//----------------------------------------------
bool i2cWriteReg(uint8_t addr, uint8_t reg, uint8_t value) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(value);
    return (Wire.endTransmission() == 0);
}

bool i2cReadRegs(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t len) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) return false;

    Wire.requestFrom(addr, len);
    for (uint8_t i = 0; i < len && Wire.available(); i++) {
        buffer[i] = Wire.read();
    }
    return true;
}

//----------------------------------------------
// BMP390 Functions
//----------------------------------------------
bool initBMP390() {
    Serial.println("Initializing BMP390...");

    // Check chip ID
    uint8_t chipId;
    if (!i2cReadRegs(BMP390_ADDR, 0x00, &chipId, 1)) {
        Serial.println("  BMP390 not found!");
        return false;
    }

    if (chipId != 0x60) {
        Serial.printf("  Wrong chip ID: 0x%02X (expected 0x60)\n", chipId);
        return false;
    }

    // Soft reset
    i2cWriteReg(BMP390_ADDR, 0x7E, 0xB6);
    delay(10);

    // Read calibration data
    uint8_t calData[21];
    if (!i2cReadRegs(BMP390_ADDR, 0x31, calData, 21)) {
        Serial.println("  Failed to read calibration!");
        return false;
    }

    // Parse calibration (see BMP390 datasheet)
    uint16_t T1_raw = calData[0] | (calData[1] << 8);
    uint16_t T2_raw = calData[2] | (calData[3] << 8);
    int8_t T3_raw = calData[4];

    bmpCal.T1 = (float)T1_raw / powf(2, -8);
    bmpCal.T2 = (float)T2_raw / powf(2, 30);
    bmpCal.T3 = (float)T3_raw / powf(2, 48);

    int16_t P1_raw = calData[5] | (calData[6] << 8);
    int16_t P2_raw = calData[7] | (calData[8] << 8);
    int8_t P3_raw = calData[9];
    int8_t P4_raw = calData[10];
    uint16_t P5_raw = calData[11] | (calData[12] << 8);
    uint16_t P6_raw = calData[13] | (calData[14] << 8);
    int8_t P7_raw = calData[15];
    int8_t P8_raw = calData[16];
    int16_t P9_raw = calData[17] | (calData[18] << 8);
    int8_t P10_raw = calData[19];
    int8_t P11_raw = calData[20];

    bmpCal.P1 = ((float)P1_raw - powf(2, 14)) / powf(2, 20);
    bmpCal.P2 = ((float)P2_raw - powf(2, 14)) / powf(2, 29);
    bmpCal.P3 = (float)P3_raw / powf(2, 32);
    bmpCal.P4 = (float)P4_raw / powf(2, 37);
    bmpCal.P5 = (float)P5_raw / powf(2, -3);
    bmpCal.P6 = (float)P6_raw / powf(2, 6);
    bmpCal.P7 = (float)P7_raw / powf(2, 8);
    bmpCal.P8 = (float)P8_raw / powf(2, 15);
    bmpCal.P9 = (float)P9_raw / powf(2, 48);
    bmpCal.P10 = (float)P10_raw / powf(2, 48);
    bmpCal.P11 = (float)P11_raw / powf(2, 65);

    // Configure: OSR x8 for both, ODR 50Hz, enable both
    i2cWriteReg(BMP390_ADDR, 0x1C, 0x03);  // OSR: temp x1, press x8
    i2cWriteReg(BMP390_ADDR, 0x1D, 0x00);  // ODR: 200Hz
    i2cWriteReg(BMP390_ADDR, 0x1F, 0x02);  // IIR filter coefficient 1
    i2cWriteReg(BMP390_ADDR, 0x1B, 0x33);  // Enable temp + press, normal mode

    Serial.println("  BMP390 initialized");
    return true;
}

bool readBMP390(float* pressure, float* temperature) {
    uint8_t data[6];
    if (!i2cReadRegs(BMP390_ADDR, 0x04, data, 6)) {
        return false;
    }

    uint32_t rawPress = data[0] | (data[1] << 8) | (data[2] << 16);
    uint32_t rawTemp = data[3] | (data[4] << 8) | (data[5] << 16);

    // Temperature compensation
    float pd1 = (float)rawTemp - bmpCal.T1;
    float pd2 = pd1 * bmpCal.T2;
    float tempComp = pd2 + (pd1 * pd1) * bmpCal.T3;

    // Pressure compensation
    float pd3 = bmpCal.P6 * tempComp;
    float pd4 = bmpCal.P7 * (tempComp * tempComp);
    float pd5 = bmpCal.P8 * (tempComp * tempComp * tempComp);
    float po1 = bmpCal.P5 + pd3 + pd4 + pd5;

    float pd6 = bmpCal.P2 * tempComp;
    float pd7 = bmpCal.P3 * (tempComp * tempComp);
    float pd8 = bmpCal.P4 * (tempComp * tempComp * tempComp);
    float po2 = (float)rawPress * (bmpCal.P1 + pd6 + pd7 + pd8);

    float pd9 = (float)rawPress * (float)rawPress;
    float pd10 = bmpCal.P9 + bmpCal.P10 * tempComp;
    float pd11 = pd9 * pd10;
    float pd12 = pd9 * (float)rawPress * bmpCal.P11;

    float pressComp = po1 + po2 + pd11 + pd12;

    *temperature = tempComp;
    *pressure = pressComp;
    return true;
}

float pressureToAltitude(float pressure, float groundPressure) {
    // Hypsometric formula
    return 44330.0f * (1.0f - powf(pressure / groundPressure, 0.1903f));
}

//----------------------------------------------
// IMU Functions (LSM6DSOX + LIS3MDL)
//----------------------------------------------
bool initIMU() {
    Serial.println("Initializing IMU...");

    // Check LSM6DSOX
    uint8_t whoAmI;
    if (!i2cReadRegs(LSM6DSOX_ADDR, 0x0F, &whoAmI, 1) || whoAmI != 0x6C) {
        Serial.printf("  LSM6DSOX not found (0x%02X)\n", whoAmI);
        return false;
    }

    // Configure LSM6DSOX
    i2cWriteReg(LSM6DSOX_ADDR, 0x10, 0x60);  // Accel: 416Hz, +-2g
    i2cWriteReg(LSM6DSOX_ADDR, 0x11, 0x60);  // Gyro: 416Hz, 250dps

    // Check LIS3MDL
    if (!i2cReadRegs(LIS3MDL_ADDR, 0x0F, &whoAmI, 1) || whoAmI != 0x3D) {
        Serial.printf("  LIS3MDL not found (0x%02X)\n", whoAmI);
        return false;
    }

    // Configure LIS3MDL
    i2cWriteReg(LIS3MDL_ADDR, 0x20, 0x70);  // Temp enable, high perf XY, 10Hz
    i2cWriteReg(LIS3MDL_ADDR, 0x21, 0x00);  // +-4 gauss
    i2cWriteReg(LIS3MDL_ADDR, 0x22, 0x00);  // Continuous mode
    i2cWriteReg(LIS3MDL_ADDR, 0x23, 0x0C);  // High perf Z, little endian

    Serial.println("  IMU initialized (LSM6DSOX + LIS3MDL)");
    return true;
}

bool readIMU() {
    // Read accelerometer (6 bytes)
    uint8_t data[6];
    if (i2cReadRegs(LSM6DSOX_ADDR, 0x28, data, 6)) {
        int16_t ax = (int16_t)(data[0] | (data[1] << 8));
        int16_t ay = (int16_t)(data[2] | (data[3] << 8));
        int16_t az = (int16_t)(data[4] | (data[5] << 8));

        // Convert to g's (+-2g range = 0.061 mg/LSB)
        accelX = ax * 0.061f / 1000.0f;
        accelY = ay * 0.061f / 1000.0f;
        accelZ = az * 0.061f / 1000.0f;
        accelMagnitude = sqrtf(accelX*accelX + accelY*accelY + accelZ*accelZ);
    }

    // Read gyroscope (6 bytes)
    if (i2cReadRegs(LSM6DSOX_ADDR, 0x22, data, 6)) {
        int16_t gx = (int16_t)(data[0] | (data[1] << 8));
        int16_t gy = (int16_t)(data[2] | (data[3] << 8));
        int16_t gz = (int16_t)(data[4] | (data[5] << 8));

        // Convert to deg/s (250dps range = 8.75 mdps/LSB)
        gyroX = gx * 8.75f / 1000.0f;
        gyroY = gy * 8.75f / 1000.0f;
        gyroZ = gz * 8.75f / 1000.0f;
    }

    // Read magnetometer (6 bytes)
    if (i2cReadRegs(LIS3MDL_ADDR, 0x28, data, 6)) {
        int16_t mx = (int16_t)(data[0] | (data[1] << 8));
        int16_t my = (int16_t)(data[2] | (data[3] << 8));
        int16_t mz = (int16_t)(data[4] | (data[5] << 8));

        // Convert to gauss (+-4 gauss range = 6842 LSB/gauss)
        magX = mx / 6842.0f;
        magY = my / 6842.0f;
        magZ = mz / 6842.0f;
    }

    return true;
}

//----------------------------------------------
// Pyro Channel Functions
//----------------------------------------------
void initPyro() {
    Serial.println("Initializing pyro channels...");

    // Fire pins as output, LOW
    pinMode(PYRO1_FIRE, OUTPUT);
    pinMode(PYRO2_FIRE, OUTPUT);
    digitalWrite(PYRO1_FIRE, LOW);
    digitalWrite(PYRO2_FIRE, LOW);

    // Arm switch with pullup
    pinMode(ARM_SWITCH, INPUT_PULLUP);

    // Continuity pins as analog input
    pinMode(PYRO1_CONT, INPUT);
    pinMode(PYRO2_CONT, INPUT);

    Serial.println("  Pyro channels initialized");
}

void updatePyro() {
    // Read arm switch (active low)
    isArmed = (digitalRead(ARM_SWITCH) == LOW);

    // Read continuity (voltage divider, ~1.5V when connected)
    int cont1 = analogRead(PYRO1_CONT);
    int cont2 = analogRead(PYRO2_CONT);

    // ESP32 ADC is 12-bit (0-4095), 3.3V reference
    // Continuity threshold: ~1.0V = ~1240
    pyro1Continuity = (cont1 > 1000);
    pyro2Continuity = (cont2 > 1000);

    // Check if pyro fire duration has elapsed
    if (pyro1Fired && (millis() - pyro1FireTime > PYRO_FIRE_DURATION_MS)) {
        digitalWrite(PYRO1_FIRE, LOW);
    }
    if (pyro2Fired && (millis() - pyro2FireTime > PYRO_FIRE_DURATION_MS)) {
        digitalWrite(PYRO2_FIRE, LOW);
    }
}

void firePyro1() {
    if (isArmed && !pyro1Fired) {
        Serial.println("FIRING PYRO 1 (DROGUE)!");
        digitalWrite(PYRO1_FIRE, HIGH);
        pyro1Fired = true;
        pyro1FireTime = millis();
    }
}

void firePyro2() {
    if (isArmed && !pyro2Fired) {
        Serial.println("FIRING PYRO 2 (MAIN)!");
        digitalWrite(PYRO2_FIRE, HIGH);
        pyro2Fired = true;
        pyro2FireTime = millis();
    }
}

//----------------------------------------------
// GPS Functions
//----------------------------------------------
void initGPS() {
    Serial.println("Initializing GPS...");

    gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);
    delay(1000);

    // Flush buffer
    while (gpsSerial.available()) {
        gpsSerial.read();
    }

    // Configure UC6580
    gpsSerial.println("$CFGSYS,h11*61");
    delay(100);
    gpsSerial.println("$CFGMSG,0,1,1*23");  // GGA
    delay(50);
    gpsSerial.println("$CFGMSG,4,1,1*27");  // RMC
    delay(50);

    Serial.println("  GPS initialized");
}

void readGPS() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    if (gps.location.isValid()) {
        gpsLatitude = gps.location.lat();
        gpsLongitude = gps.location.lng();
        gpsValid = true;
    }
    gpsSatellites = gps.satellites.value();
}

//----------------------------------------------
// Display Functions
//----------------------------------------------
void initDisplay() {
    Serial.println("Initializing display...");

    // Power on VEXT
    pinMode(TFT_POWER, OUTPUT);
    digitalWrite(TFT_POWER, HIGH);
    delay(200);

    // Backlight PWM
    ledcAttach(TFT_BL, 5000, 8);
    ledcWrite(TFT_BL, 128);  // 50% brightness

    // Initialize display
    tft.initR(INITR_MINI160x80_PLUGIN);
    tft.setRotation(1);
    tft.fillScreen(COLOR_BLACK);

    // Splash screen
    tft.setTextColor(COLOR_WHITE);
    tft.setTextSize(1);
    tft.setCursor(20, 20);
    tft.print("ROCKET AVIONICS");
    tft.setCursor(30, 35);
    tft.print("Flight Computer");
    tft.setCursor(40, 55);
    tft.setTextColor(COLOR_CYAN);
    tft.print(FIRMWARE_VERSION_STRING);

    delay(1500);
    tft.fillScreen(COLOR_BLACK);

    Serial.println("  Display initialized");
}

void updateDisplay() {
    // Clear screen
    tft.fillScreen(COLOR_BLACK);

    // Line 1: State
    tft.setCursor(0, 2);
    tft.setTextSize(1);

    // Color based on state
    switch (flightState) {
        case FLIGHT_STATE_IDLE:
            tft.setTextColor(COLOR_WHITE);
            break;
        case FLIGHT_STATE_ARMED:
            tft.setTextColor(COLOR_YELLOW);
            break;
        case FLIGHT_STATE_BOOST:
        case FLIGHT_STATE_COAST:
            tft.setTextColor(COLOR_RED);
            break;
        case FLIGHT_STATE_APOGEE:
        case FLIGHT_STATE_DROGUE:
        case FLIGHT_STATE_MAIN:
            tft.setTextColor(COLOR_ORANGE);
            break;
        case FLIGHT_STATE_LANDED:
        case FLIGHT_STATE_COMPLETE:
            tft.setTextColor(COLOR_GREEN);
            break;
    }
    tft.printf("STATE: %s", flightStateNames[flightState]);

    // Rocket ID on right
    tft.setCursor(120, 2);
    tft.setTextColor(COLOR_CYAN);
    tft.printf("R%d", rocketId);

    // Line 2: Altitude and velocity
    tft.setCursor(0, 14);
    tft.setTextColor(COLOR_WHITE);
    tft.printf("ALT: %.1fm  VEL: %.1fm/s", altitudeM, velocityMps);

    // Line 3: Max altitude
    tft.setCursor(0, 26);
    tft.printf("MAX: %.1fm  ACC: %.1fg", maxAltitudeM, accelMagnitude);

    // Line 4: GPS
    tft.setCursor(0, 38);
    if (gpsValid) {
        tft.setTextColor(COLOR_GREEN);
        tft.printf("GPS: %d sats  FIX", gpsSatellites);
    } else {
        tft.setTextColor(COLOR_RED);
        tft.printf("GPS: %d sats  NO FIX", gpsSatellites);
    }

    // Line 5: Pyro status
    tft.setCursor(0, 50);
    tft.print("P1:");
    tft.setTextColor(pyro1Continuity ? COLOR_GREEN : COLOR_RED);
    tft.print(pyro1Continuity ? "OK " : "NO ");
    tft.setTextColor(COLOR_WHITE);
    tft.print("P2:");
    tft.setTextColor(pyro2Continuity ? COLOR_GREEN : COLOR_RED);
    tft.print(pyro2Continuity ? "OK " : "NO ");
    tft.setTextColor(COLOR_WHITE);
    tft.print("ARM:");
    tft.setTextColor(isArmed ? COLOR_GREEN : COLOR_RED);
    tft.print(isArmed ? "YES" : "NO");

    // Line 6: LoRa TX count and temperature
    tft.setCursor(0, 62);
    tft.setTextColor(COLOR_CYAN);
    tft.printf("TX:%d  T:%.1fC", loraTxCount, temperatureC);
}

//----------------------------------------------
// LoRa Functions
//----------------------------------------------
bool initLoRa() {
    Serial.println("Initializing LoRa...");

    int state = radio.begin(LORA_FREQUENCY, LORA_BANDWIDTH,
                           LORA_SPREAD_FACTOR, LORA_CODING_RATE,
                           LORA_SYNC_WORD, LORA_TX_POWER,
                           LORA_PREAMBLE_LEN);

    if (state != RADIOLIB_ERR_NONE) {
        Serial.printf("  LoRa init failed: %d\n", state);
        return false;
    }

    // Set up receive interrupt
    radio.setDio1Action(onLoraReceive);
    radio.startReceive();

    Serial.println("  LoRa initialized");
    return true;
}

void buildTelemetryPacket(TelemetryPacket* pkt) {
    memset(pkt, 0, sizeof(TelemetryPacket));

    pkt->magic = 0xAA;
    pkt->type = 0x01;
    pkt->rocketId = rocketId;
    pkt->sequence = telemetrySeq++;
    pkt->state = (uint8_t)flightState;
    pkt->altitudeCm = (int32_t)(altitudeM * 100);
    pkt->velocityCmS = (int16_t)(velocityMps * 100);
    pkt->accelMg = (int16_t)(accelMagnitude * 1000);
    pkt->maxAltCm = (int32_t)(maxAltitudeM * 100);
    pkt->latitude = (int32_t)(gpsLatitude * 1e7);
    pkt->longitude = (int32_t)(gpsLongitude * 1e7);
    pkt->gpsSats = gpsSatellites;
    pkt->gpsValid = gpsValid ? 1 : 0;
    pkt->gyroX = (int16_t)(gyroX * 10);
    pkt->gyroY = (int16_t)(gyroY * 10);
    pkt->gyroZ = (int16_t)(gyroZ * 10);
    pkt->magX = (int16_t)(magX * 1000);
    pkt->magY = (int16_t)(magY * 1000);
    pkt->magZ = (int16_t)(magZ * 1000);
    pkt->pyro1Cont = pyro1Continuity ? 1 : 0;
    pkt->pyro2Cont = pyro2Continuity ? 1 : 0;
    pkt->armed = isArmed ? 1 : 0;
    pkt->tempC10 = (int16_t)(temperatureC * 10);
    pkt->battMv = 3700;  // TODO: Read battery

    // Calculate checksum
    uint8_t* data = (uint8_t*)pkt;
    uint8_t checksum = 0;
    for (int i = 0; i < sizeof(TelemetryPacket) - 1; i++) {
        checksum ^= data[i];
    }
    pkt->checksum = checksum;
}

void transmitTelemetry() {
    TelemetryPacket pkt;
    buildTelemetryPacket(&pkt);

    int state = radio.transmit((uint8_t*)&pkt, sizeof(pkt));
    if (state == RADIOLIB_ERR_NONE) {
        loraTxCount++;
    }

    // Restart receive mode
    radio.startReceive();
}

void handleLoRaReceive() {
    if (!loraPacketReceived) return;
    loraPacketReceived = false;

    uint8_t buffer[64];
    int len = radio.getPacketLength();
    if (len > 0 && len <= sizeof(buffer)) {
        int state = radio.readData(buffer, len);
        if (state == RADIOLIB_ERR_NONE) {
            // Parse command packet
            if (len >= 4 && buffer[0] == 0xAA && buffer[1] == 0x02) {
                uint8_t targetId = buffer[2];
                uint8_t cmdId = buffer[3];

                if (targetId == rocketId || targetId == 0xFF) {
                    handleCommand(cmdId, buffer + 4, len - 4);
                }
            }
        }
    }

    radio.startReceive();
}

void handleCommand(uint8_t cmdId, uint8_t* data, int len) {
    Serial.printf("Command received: 0x%02X\n", cmdId);

    switch (cmdId) {
        case 0x01:  // ARM
            if (flightState == FLIGHT_STATE_IDLE) {
                flightState = FLIGHT_STATE_ARMED;
                stateEntryTime = millis();
                Serial.println("  -> ARMED");
            }
            break;

        case 0x02:  // DISARM
            if (flightState == FLIGHT_STATE_ARMED) {
                flightState = FLIGHT_STATE_IDLE;
                stateEntryTime = millis();
                Serial.println("  -> IDLE");
            }
            break;

        case 0x10:  // Set rocket ID
            if (len >= 1) {
                rocketId = data[0] & 0x0F;
                saveSettings();
                Serial.printf("  -> Rocket ID: %d\n", rocketId);
            }
            break;
    }
}

//----------------------------------------------
// Settings Functions
//----------------------------------------------
void loadSettings() {
    prefs.begin("flight", true);
    rocketId = prefs.getUChar("rocketId", 0);
    prefs.getString("rocketName", rocketName, sizeof(rocketName));
    prefs.end();

    Serial.printf("Loaded settings: Rocket ID=%d, Name=%s\n", rocketId, rocketName);
}

void saveSettings() {
    prefs.begin("flight", false);
    prefs.putUChar("rocketId", rocketId);
    prefs.putString("rocketName", rocketName);
    prefs.end();
}

//----------------------------------------------
// Flight State Machine
//----------------------------------------------
void updateFlightState() {
    uint32_t now = millis();
    uint32_t timeInState = now - stateEntryTime;

    switch (flightState) {
        case FLIGHT_STATE_IDLE:
            // Wait for ARM command
            break;

        case FLIGHT_STATE_ARMED:
            // Detect launch
            if (accelMagnitude > LAUNCH_ACCEL_THRESHOLD || altitudeM > 10) {
                flightState = FLIGHT_STATE_BOOST;
                stateEntryTime = now;
                Serial.println("LAUNCH DETECTED -> BOOST");
            }
            break;

        case FLIGHT_STATE_BOOST:
            // Motor burnout (acceleration drops)
            if (accelMagnitude < 1.5 && timeInState > 500) {
                flightState = FLIGHT_STATE_COAST;
                stateEntryTime = now;
                Serial.println("Burnout -> COAST");
            }
            break;

        case FLIGHT_STATE_COAST:
            // Apogee detection (velocity crosses zero)
            if (velocityMps < 0 && timeInState > APOGEE_LOCKOUT_MS) {
                flightState = FLIGHT_STATE_APOGEE;
                stateEntryTime = now;
                Serial.println("APOGEE DETECTED");
                firePyro1();  // Deploy drogue
            }
            break;

        case FLIGHT_STATE_APOGEE:
            // Transition to drogue descent
            flightState = FLIGHT_STATE_DROGUE;
            stateEntryTime = now;
            break;

        case FLIGHT_STATE_DROGUE:
            // Main deployment altitude
            if (altitudeM < MAIN_DEPLOY_ALTITUDE_M) {
                flightState = FLIGHT_STATE_MAIN;
                stateEntryTime = now;
                Serial.printf("Main altitude reached (%.1fm)\n", altitudeM);
                firePyro2();  // Deploy main
            }
            break;

        case FLIGHT_STATE_MAIN:
            // Landing detection
            if (fabsf(velocityMps) < LANDING_VEL_THRESHOLD && timeInState > 5000) {
                flightState = FLIGHT_STATE_LANDED;
                stateEntryTime = now;
                Serial.println("LANDED");
            }
            break;

        case FLIGHT_STATE_LANDED:
            // Wait for download or reset
            break;

        case FLIGHT_STATE_COMPLETE:
            break;
    }

    // Track max altitude
    if (altitudeM > maxAltitudeM) {
        maxAltitudeM = altitudeM;
    }
}

//----------------------------------------------
// Velocity Estimation
//----------------------------------------------
float lastAltitudeM = 0;
uint32_t lastVelocityUpdateMs = 0;
float velocityEMA = 0;
const float velocityAlpha = 0.3;

void updateVelocity() {
    uint32_t now = millis();
    uint32_t dt = now - lastVelocityUpdateMs;

    if (dt >= 10) {  // 100Hz
        float rawVelocity = (altitudeM - lastAltitudeM) * 1000.0f / dt;
        velocityEMA = velocityAlpha * rawVelocity + (1.0f - velocityAlpha) * velocityEMA;
        velocityMps = velocityEMA;

        lastAltitudeM = altitudeM;
        lastVelocityUpdateMs = now;
    }
}

//----------------------------------------------
// Ground Pressure Calibration
//----------------------------------------------
void calibrateGroundPressure() {
    Serial.println("Calibrating ground pressure...");

    float totalPressure = 0;
    int samples = 0;

    for (int i = 0; i < 50; i++) {
        float pressure, temp;
        if (readBMP390(&pressure, &temp)) {
            totalPressure += pressure;
            samples++;
        }
        delay(20);
    }

    if (samples > 0) {
        groundPressurePa = totalPressure / samples;
        Serial.printf("  Ground pressure: %.1f Pa\n", groundPressurePa);
    }
}

//----------------------------------------------
// Setup
//----------------------------------------------
void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("========================================");
    Serial.println("  ROCKET AVIONICS FLIGHT COMPUTER");
    Serial.println("  Heltec Wireless Tracker");
    Serial.printf("  %s\n", FIRMWARE_VERSION_STRING);
    Serial.printf("  %s %s\n", FIRMWARE_BUILD_DATE, FIRMWARE_BUILD_TIME);
    Serial.println("========================================");
    Serial.println();

    // Initialize I2C
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(400000);  // 400kHz

    // Initialize subsystems
    initDisplay();
    loadSettings();
    initPyro();
    initGPS();

    bmpInitialized = initBMP390();
    imuInitialized = initIMU();

    if (!initLoRa()) {
        Serial.println("LoRa init failed! Halting.");
        while (1) delay(1000);
    }

    // Calibrate ground pressure
    if (bmpInitialized) {
        calibrateGroundPressure();
    }

    Serial.println();
    Serial.println("========================================");
    Serial.println("Flight Computer Ready!");
    Serial.printf("  Rocket ID: %d\n", rocketId);
    Serial.printf("  Rocket Name: %s\n", rocketName);
    Serial.printf("  BMP390: %s\n", bmpInitialized ? "OK" : "FAIL");
    Serial.printf("  IMU: %s\n", imuInitialized ? "OK" : "FAIL");
    Serial.printf("  Main Deploy Alt: %dm\n", MAIN_DEPLOY_ALTITUDE_M);
    Serial.println("========================================");
    Serial.println();
}

//----------------------------------------------
// Main Loop
//----------------------------------------------
void loop() {
    uint32_t now = millis();

    // 100Hz: Read sensors
    if (now - lastSensorMs >= (1000 / SENSOR_RATE_HZ)) {
        lastSensorMs = now;

        // Read barometer
        if (bmpInitialized) {
            float pressure;
            if (readBMP390(&pressure, &temperatureC)) {
                altitudeM = pressureToAltitude(pressure, groundPressurePa);
                updateVelocity();
            }
        }

        // Read IMU
        if (imuInitialized) {
            readIMU();
        }

        // Update pyro channels
        updatePyro();

        // Update flight state
        updateFlightState();
    }

    // 10Hz: GPS and telemetry
    if (now - lastTelemetryMs >= (1000 / TELEMETRY_RATE_HZ)) {
        lastTelemetryMs = now;

        readGPS();
        transmitTelemetry();
    }

    // 5Hz: Display
    if (now - lastDisplayMs >= (1000 / DISPLAY_RATE_HZ)) {
        lastDisplayMs = now;
        updateDisplay();
    }

    // Handle LoRa receive
    handleLoRaReceive();

    // Small delay
    delay(1);
}
