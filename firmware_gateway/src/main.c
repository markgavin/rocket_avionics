//----------------------------------------------
// Module: main.c
// Description: Rocket Avionics Ground Gateway
//   LoRa to USB serial bridge
// Author: Mark Gavin
// Created: 2026-01-10
// Modified: 2026-01-13 (Switched to OLED display)
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit Feather RP2040 with RFM95 LoRa 915MHz
//   - Adafruit FeatherWing OLED 128x64 (4650)
//   - Adafruit BMP390 Barometric Sensor (4816)
//----------------------------------------------

#include "pins.h"
#include "version.h"
#include "lora_radio.h"
#include "gateway_protocol.h"
#include "gateway_display.h"
#include "bmp390.h"
#include "gps.h"

#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include <stdio.h>
#include <string.h>

//----------------------------------------------
// Module Constants
//----------------------------------------------
#define kMainLoopIntervalMs     1
#define kLedBlinkFastMs         100
#define kLedBlinkSlowMs         500
#define kLinkTimeoutMs          5000
#define kUsbLineBufferSize      256
#define kStartupDelayMs         1000
#define kOledUpdateIntervalMs   200   // 5 Hz display update
#define kBmp390ReadIntervalMs   100   // 10 Hz sensor reading
#define kSplashDurationMs       1500  // Splash screen duration
#define kGpsUpdateIntervalMs    100   // 10 Hz GPS update

//----------------------------------------------
// Debug Configuration
// Set to 0 to disable debug output that can
// interfere with protocol JSON parsing
//----------------------------------------------
#define kEnableDebugOutput      0

#if kEnableDebugOutput
  #define DEBUG_PRINT(...)  printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(...)  ((void)0)
#endif

//----------------------------------------------
// Module State
//----------------------------------------------
static LoRa_Radio sLoRaRadio ;
static GatewayState sGatewayState ;
static BMP390 sBmp390 ;
static bool sLoRaOk = false ;
static bool sDisplayOk = false ;
static bool sUsbConnected = false ;
static bool sBmp390Ok = false ;
static bool sGpsOk = false ;

// Ground barometer data
static float sGroundPressurePa = 0.0f ;
static float sGroundTemperatureC = 0.0f ;

// LED timing
static uint32_t sLastLedToggleMs = 0 ;
static bool sLedState = false ;

// Display timing
static uint32_t sLastDisplayUpdateMs = 0 ;
static uint32_t sLastUsbCheckMs = 0 ;
static uint32_t sLastBmp390ReadMs = 0 ;
static uint32_t sLastGpsUpdateMs = 0 ;

// Button state
static bool sButtonAState = false ;
static bool sButtonBState = false ;
static bool sButtonCState = false ;
static uint32_t sButtonALastMs = 0 ;
static uint32_t sButtonBLastMs = 0 ;
static uint32_t sButtonCLastMs = 0 ;

// USB input buffer
static char sUsbLineBuffer[kUsbLineBufferSize] ;
static int sUsbLinePos = 0 ;

//----------------------------------------------
// Forward Declarations
//----------------------------------------------
static void InitializeHardware(void) ;
static void InitializeI2C(void) ;
static void InitializeSPI(void) ;
static void InitializeButtons(void) ;
static void ProcessLoRaPackets(uint32_t inCurrentMs) ;
static void ProcessUsbInput(uint32_t inCurrentMs) ;
static void ProcessButtons(uint32_t inCurrentMs) ;
static void UpdateLed(uint32_t inCurrentMs) ;
static void UpdateDisplay(uint32_t inCurrentMs) ;
static void ReadGroundBarometer(uint32_t inCurrentMs) ;

//----------------------------------------------
// Function: main
//----------------------------------------------
int main(void)
{
  // Initialize stdio (USB serial)
  stdio_init_all() ;
  sleep_ms(kStartupDelayMs) ;

  printf("\n\n") ;
  printf("======================================\n") ;
  printf("  ROCKET AVIONICS GROUND GATEWAY\n") ;
  printf("  Version %s\n", FIRMWARE_VERSION_STRING) ;
  printf("  Build: %s %s\n", __DATE__, __TIME__) ;
  printf("======================================\n\n") ;

  // Initialize hardware
  InitializeHardware() ;

  // Initialize gateway protocol
  GatewayProtocol_Init(&sGatewayState) ;

  // Print startup status
  printf("Gateway ready:\n") ;
  printf("  LoRa: %s\n", sLoRaOk ? "OK" : "FAIL") ;
  printf("  Display: %s\n", sDisplayOk ? "OK" : "FAIL") ;
  printf("  BMP390: %s\n", sBmp390Ok ? "OK" : "FAIL") ;
  printf("  GPS: %s\n", sGpsOk ? "OK" : "FAIL") ;
  printf("  Frequency: %lu Hz\n", (unsigned long)kLoRaFrequency) ;
  printf("  Sync Word: 0x%02X\n", kLoRaSyncWord) ;
  printf("\nListening for telemetry...\n\n") ;

  // Show splash screen
  if (sDisplayOk)
  {
    GatewayDisplay_ShowSplash() ;
    sleep_ms(kSplashDurationMs) ;
    GatewayDisplay_SetConnectionState(kConnectionDisconnected) ;
  }

  // Put LoRa in receive mode
  if (sLoRaOk)
  {
    LoRa_StartReceive(&sLoRaRadio) ;
  }

  // Main loop
  while (1)
  {
    uint32_t theCurrentMs = to_ms_since_boot(get_absolute_time()) ;

    // Process incoming LoRa packets
    if (sLoRaOk)
    {
      ProcessLoRaPackets(theCurrentMs) ;
    }

    // Read ground barometer
    if (sBmp390Ok)
    {
      ReadGroundBarometer(theCurrentMs) ;
    }

    // Update GPS
    if (sGpsOk)
    {
      GPS_Update(theCurrentMs) ;
    }

    // Process USB serial input
    ProcessUsbInput(theCurrentMs) ;

    // Process button inputs
    ProcessButtons(theCurrentMs) ;

    // Update status LED
    UpdateLed(theCurrentMs) ;

    // Update display
    if (sDisplayOk)
    {
      UpdateDisplay(theCurrentMs) ;
    }

    // Check link timeout
    if (sGatewayState.pConnected)
    {
      if ((theCurrentMs - sGatewayState.pLastPacketTimeMs) > kLinkTimeoutMs)
      {
        sGatewayState.pConnected = false ;
        printf("{\"type\":\"link\",\"status\":\"lost\"}\n") ;
        stdio_flush() ;

        // Update display
        if (sDisplayOk)
        {
          GatewayDisplay_SetConnectionState(kConnectionDisconnected) ;
          GatewayDisplay_ShowMessage("Link lost - searching...", true) ;
        }
      }
    }

    // Small delay
    sleep_ms(kMainLoopIntervalMs) ;
  }

  return 0 ;
}

//----------------------------------------------
// Function: InitializeHardware
//----------------------------------------------
static void InitializeHardware(void)
{
  printf("Initializing hardware...\n") ;

  // Initialize I2C bus
  InitializeI2C() ;

  // Initialize SPI bus
  InitializeSPI() ;

  // Initialize LED
  gpio_init(kPinLed) ;
  gpio_set_dir(kPinLed, GPIO_OUT) ;
  gpio_put(kPinLed, 0) ;

  // Initialize BMP390 barometric sensor
  printf("Initializing BMP390...\n") ;
  if (BMP390_Init(&sBmp390, kI2cAddrBMP390))
  {
    // Configure for barometric reference
    BMP390_Configure(
      &sBmp390,
      BMP390_OSR_8X,      // Pressure oversampling
      BMP390_OSR_2X,      // Temperature oversampling
      BMP390_ODR_50_HZ,   // 50 Hz output rate
      BMP390_IIR_COEF_3) ; // Light filtering
    sBmp390Ok = true ;
    printf("BMP390 initialized\n") ;
  }
  else
  {
    printf("WARNING: BMP390 initialization failed\n") ;
  }

  // Initialize LoRa radio
  printf("Initializing LoRa radio...\n") ;
  if (LoRa_Init(&sLoRaRadio))
  {
    // Configure to match flight computer
    LoRa_SetFrequency(&sLoRaRadio, kLoRaFrequency) ;
    LoRa_SetSpreadingFactor(&sLoRaRadio, kLoRaSpreadFactor) ;
    LoRa_SetBandwidth(&sLoRaRadio, LORA_BW_125) ;
    LoRa_SetCodingRate(&sLoRaRadio, LORA_CR_4_5) ;
    LoRa_SetTxPower(&sLoRaRadio, kLoRaTxPower) ;
    LoRa_SetSyncWord(&sLoRaRadio, kLoRaSyncWord) ;
    sLoRaOk = true ;
    printf("LoRa radio initialized\n") ;
  }
  else
  {
    printf("ERROR: LoRa radio initialization failed!\n") ;
  }

  // Initialize GPS
  printf("Initializing GPS...\n") ;
  if (GPS_Init())
  {
    sGpsOk = true ;
    printf("GPS initialized\n") ;
  }
  else
  {
    printf("WARNING: GPS initialization failed\n") ;
  }

  // Initialize buttons
  InitializeButtons() ;

  // Initialize OLED display
  printf("Initializing OLED display...\n") ;
  if (GatewayDisplay_Init())
  {
    sDisplayOk = true ;
    printf("OLED display initialized\n") ;
  }
  else
  {
    printf("ERROR: OLED display initialization failed!\n") ;
  }

  printf("Hardware initialization complete\n\n") ;
}

//----------------------------------------------
// Function: InitializeI2C
//----------------------------------------------
static void InitializeI2C(void)
{
  printf("Initializing I2C bus...\n") ;

  i2c_init(kI2cPort, kI2cBaudrate) ;
  gpio_set_function(kPinI2cSda, GPIO_FUNC_I2C) ;
  gpio_set_function(kPinI2cScl, GPIO_FUNC_I2C) ;
  gpio_pull_up(kPinI2cSda) ;
  gpio_pull_up(kPinI2cScl) ;

  printf("I2C initialized at %d Hz\n", kI2cBaudrate) ;
}

//----------------------------------------------
// Function: InitializeSPI
//----------------------------------------------
static void InitializeSPI(void)
{
  printf("Initializing SPI1 bus...\n") ;

  // Initialize SPI1 for LoRa radio
  spi_init(kSpiPort, kSpiLoRaBaudrate) ;
  gpio_set_function(kPinSpiSck, GPIO_FUNC_SPI) ;
  gpio_set_function(kPinSpiMosi, GPIO_FUNC_SPI) ;
  gpio_set_function(kPinSpiMiso, GPIO_FUNC_SPI) ;

  // LoRa chip select (active low)
  gpio_init(kPinLoRaCs) ;
  gpio_set_dir(kPinLoRaCs, GPIO_OUT) ;
  gpio_put(kPinLoRaCs, 1) ;

  // LoRa reset
  gpio_init(kPinLoRaReset) ;
  gpio_set_dir(kPinLoRaReset, GPIO_OUT) ;
  gpio_put(kPinLoRaReset, 1) ;

  // LoRa DIO0 interrupt
  gpio_init(kPinLoRaDio0) ;
  gpio_set_dir(kPinLoRaDio0, GPIO_IN) ;

  printf("SPI1 initialized\n") ;
}

//----------------------------------------------
// Function: InitializeButtons
//----------------------------------------------
static void InitializeButtons(void)
{
  printf("Initializing buttons...\n") ;

  // Button A (left) - active low
  gpio_init(kPinButtonA) ;
  gpio_set_dir(kPinButtonA, GPIO_IN) ;
  gpio_pull_up(kPinButtonA) ;

  // Button B (middle) - active low
  gpio_init(kPinButtonB) ;
  gpio_set_dir(kPinButtonB, GPIO_IN) ;
  gpio_pull_up(kPinButtonB) ;

  // Button C (right) - active low
  gpio_init(kPinButtonC) ;
  gpio_set_dir(kPinButtonC, GPIO_IN) ;
  gpio_pull_up(kPinButtonC) ;

  printf("Buttons initialized\n") ;
}

//----------------------------------------------
// Function: ReadGroundBarometer
//----------------------------------------------
static void ReadGroundBarometer(uint32_t inCurrentMs)
{
  // Read at fixed interval
  if ((inCurrentMs - sLastBmp390ReadMs) < kBmp390ReadIntervalMs)
  {
    return ;
  }
  sLastBmp390ReadMs = inCurrentMs ;

  // Read pressure and temperature
  float thePressure = 0 ;
  float theTemperature = 0 ;

  if (BMP390_ReadPressureTemperature(&sBmp390, &thePressure, &theTemperature))
  {
    sGroundPressurePa = thePressure ;
    sGroundTemperatureC = theTemperature ;

    // Debug: show reading every second
    static uint32_t sDebugCount = 0 ;
    if ((++sDebugCount % 10) == 0)
    {
      DEBUG_PRINT("GND BMP: P=%.0f Pa T=%.1f C\n", sGroundPressurePa, sGroundTemperatureC) ;
    }
  }
  else
  {
    // Debug: show read failure
    static uint32_t sFailCount = 0 ;
    if ((++sFailCount % 10) == 0)
    {
      DEBUG_PRINT("GND BMP: Read failed (%u)\n", sFailCount) ;
    }
  }
}

//----------------------------------------------
// Function: ProcessLoRaPackets
//----------------------------------------------
static void ProcessLoRaPackets(uint32_t inCurrentMs)
{
  uint8_t theBuffer[kLoRaPacketMaxSize] ;
  uint8_t theLen = LoRa_Receive(&sLoRaRadio, theBuffer, sizeof(theBuffer)) ;

  if (theLen == 0) return ;

  // Update statistics
  sGatewayState.pPacketsReceived++ ;
  sGatewayState.pLastPacketTimeMs = inCurrentMs ;
  sGatewayState.pLastRssi = LoRa_GetRssi(&sLoRaRadio) ;
  sGatewayState.pLastSnr = LoRa_GetSnr(&sLoRaRadio) ;

  // Check for link establishment
  if (!sGatewayState.pConnected)
  {
    sGatewayState.pConnected = true ;
    printf("{\"type\":\"link\",\"status\":\"connected\"}\n") ;
    stdio_flush() ;

    // Update display
    if (sDisplayOk)
    {
      GatewayDisplay_SetConnectionState(kConnectionConnected) ;
      GatewayDisplay_ShowMessage("Link established!", false) ;
    }
  }

  // Debug: show received packet info
  DEBUG_PRINT("RX: len=%u magic=0x%02X\n", theLen, theBuffer[0]) ;

  // Validate packet
  if (theLen < 3 || theBuffer[0] != kLoRaMagic)
  {
    DEBUG_PRINT("RX: Invalid packet (len=%u, magic=0x%02X)\n", theLen, theBuffer[0]) ;
    return ;
  }

  uint8_t thePacketType = theBuffer[1] ;

  // Handle telemetry packets
  if (thePacketType == kLoRaPacketTelemetry && theLen >= sizeof(LoRaTelemetryPacket))
  {
    DEBUG_PRINT("RX: Telemetry packet, expected size=%u\n", (unsigned)sizeof(LoRaTelemetryPacket)) ;
  }
  else if (thePacketType == kLoRaPacketTelemetry)
  {
    DEBUG_PRINT("RX: Telemetry packet too small (got %u, need %u)\n", theLen, (unsigned)sizeof(LoRaTelemetryPacket)) ;
  }

  if (thePacketType == kLoRaPacketTelemetry && theLen >= sizeof(LoRaTelemetryPacket))
  {
    LoRaTelemetryPacket * thePacket = (LoRaTelemetryPacket *)theBuffer ;

    // Get gateway GPS data
    const GpsData * theGwGps = sGpsOk ? GPS_GetData() : NULL ;
    bool theGwGpsValid = (theGwGps != NULL && theGwGps->pValid) ;

    // Convert to JSON and output to USB
    char theJson[kJsonBufferSize] ;
    int theJsonLen = GatewayProtocol_TelemetryToJson(
      thePacket,
      sGatewayState.pLastRssi,
      sGatewayState.pLastSnr,
      sGroundPressurePa,
      theGwGpsValid,
      theGwGpsValid ? theGwGps->pLatitude : 0.0f,
      theGwGpsValid ? theGwGps->pLongitude : 0.0f,
      theJson,
      sizeof(theJson)) ;

    if (theJsonLen > 0)
    {
      printf("%s", theJson) ;
      stdio_flush() ;
    }

    // Update display with telemetry
    if (sDisplayOk)
    {
      // Convert altitude from cm to m, velocity from cm/s to m/s
      float theAltitudeM = thePacket->pAltitudeCm / 100.0f ;
      float theVelocityMps = thePacket->pVelocityCmps / 100.0f ;

      // Get state name
      const char * theStateName = GatewayProtocol_GetStateName(thePacket->pState) ;

      GatewayDisplay_UpdateTelemetry(theAltitudeM, theVelocityMps, theStateName) ;
    }

    // Send ACK packet back to flight computer with signal quality info
    // This lets the flight computer know the gateway is receiving its telemetry
    uint8_t theAckPacket[5] ;
    theAckPacket[0] = kLoRaMagic ;
    theAckPacket[1] = kLoRaPacketAck ;
    theAckPacket[2] = (uint8_t)(sGatewayState.pLastRssi & 0xFF) ;
    theAckPacket[3] = (uint8_t)((sGatewayState.pLastRssi >> 8) & 0xFF) ;
    theAckPacket[4] = (uint8_t)sGatewayState.pLastSnr ;

    if (LoRa_SendBlocking(&sLoRaRadio, theAckPacket, sizeof(theAckPacket), 100))
    {
      sGatewayState.pPacketsSent++ ;
      DEBUG_PRINT("ACK TX: RSSI=%d SNR=%d\n", sGatewayState.pLastRssi, sGatewayState.pLastSnr) ;
    }
    else
    {
      DEBUG_PRINT("ACK TX FAILED\n") ;
    }

    // Immediately return to receive mode after ACK
    LoRa_StartReceive(&sLoRaRadio) ;
  }
  // Handle storage list response (Flash)
  else if (thePacketType == kLoRaPacketStorageList && theLen >= 3)
  {
    DEBUG_PRINT("RX: Flash list packet, len=%u\n", theLen) ;

    char theJson[1024] ;
    int theJsonLen = GatewayProtocol_FlashListToJson(
      theBuffer, theLen, theJson, sizeof(theJson)) ;

    if (theJsonLen > 0)
    {
      printf("%s", theJson) ;
      stdio_flush() ;
    }
  }
  // Handle storage data chunk (Flash)
  else if (thePacketType == kLoRaPacketStorageData && theLen >= 12)
  {
    DEBUG_PRINT("RX: Flash data packet, len=%u\n", theLen) ;

    char theJson[1024] ;
    int theJsonLen = GatewayProtocol_FlashDataToJson(
      theBuffer, theLen, theJson, sizeof(theJson)) ;

    if (theJsonLen > 0)
    {
      printf("%s", theJson) ;
      stdio_flush() ;
    }
  }
  // Handle device info response
  else if (thePacketType == kLoRaPacketInfo && theLen >= 5)
  {
    DEBUG_PRINT("RX: Device info packet, len=%u\n", theLen) ;

    // Parse info packet
    int theOffset = 2 ;

    // Version string
    uint8_t theVersionLen = theBuffer[theOffset++] ;
    char theVersion[64] = "" ;
    if (theVersionLen < 64 && theOffset + theVersionLen <= theLen)
    {
      memcpy(theVersion, &theBuffer[theOffset], theVersionLen) ;
      theVersion[theVersionLen] = '\0' ;
      theOffset += theVersionLen ;
    }

    // Build date string
    uint8_t theBuildLen = theBuffer[theOffset++] ;
    char theBuild[64] = "" ;
    if (theBuildLen < 64 && theOffset + theBuildLen <= theLen)
    {
      memcpy(theBuild, &theBuffer[theOffset], theBuildLen) ;
      theBuild[theBuildLen] = '\0' ;
      theOffset += theBuildLen ;
    }

    // Hardware flags
    uint8_t theFlags = theBuffer[theOffset++] ;

    // Flight state
    uint8_t theState = theBuffer[theOffset++] ;

    // Sample count
    uint32_t theSamples = theBuffer[theOffset] |
                          (theBuffer[theOffset + 1] << 8) |
                          (theBuffer[theOffset + 2] << 16) |
                          (theBuffer[theOffset + 3] << 24) ;
    theOffset += 4 ;

    // SD free space (KB)
    uint32_t theFreeKb = theBuffer[theOffset] |
                         (theBuffer[theOffset + 1] << 8) |
                         (theBuffer[theOffset + 2] << 16) |
                         (theBuffer[theOffset + 3] << 24) ;
    theOffset += 4 ;

    // Flight count
    uint8_t theFlightCount = theBuffer[theOffset++] ;

    // Build JSON response
    // Note: Hardware flags from flight firmware:
    //   0x01 = BMP390, 0x02 = LoRa, 0x04 = IMU, 0x10 = OLED, 0x20 = GPS
    printf("{\"type\":\"fc_info\","
           "\"version\":\"%s\","
           "\"build\":\"%s\","
           "\"bmp390\":%s,"
           "\"lora\":%s,"
           "\"imu\":%s,"
           "\"oled\":%s,"
           "\"gps\":%s,"
           "\"state\":\"%s\","
           "\"samples\":%lu,"
           "\"sd_free_kb\":%lu,"
           "\"flight_count\":%u}\n",
           theVersion,
           theBuild,
           (theFlags & 0x01) ? "true" : "false",
           (theFlags & 0x02) ? "true" : "false",
           (theFlags & 0x04) ? "true" : "false",
           (theFlags & 0x10) ? "true" : "false",
           (theFlags & 0x20) ? "true" : "false",
           GatewayProtocol_GetStateName(theState),
           (unsigned long)theSamples,
           (unsigned long)theFreeKb,
           theFlightCount) ;
    stdio_flush() ;
  }

  // Return to receive mode
  LoRa_StartReceive(&sLoRaRadio) ;
}

//----------------------------------------------
// Function: ProcessUsbInput
//----------------------------------------------
static void ProcessUsbInput(uint32_t inCurrentMs)
{
  // Read available characters from USB
  int theChar ;
  while ((theChar = getchar_timeout_us(0)) != PICO_ERROR_TIMEOUT)
  {
    if (theChar == '\n' || theChar == '\r')
    {
      // End of line - process command
      if (sUsbLinePos > 0)
      {
        sUsbLineBuffer[sUsbLinePos] = '\0' ;

        // Parse command
        UsbCommandType theCommandType ;
        uint32_t theCommandId ;

        if (GatewayProtocol_ParseCommand(sUsbLineBuffer, &theCommandType, &theCommandId))
        {
          // Handle ping locally
          if (theCommandType == kUsbCmdPing)
          {
            char theResponse[64] ;
            GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
            printf("%s", theResponse) ;
            stdio_flush() ;
          }
          // Handle status locally
          else if (theCommandType == kUsbCmdStatus)
          {
            char theResponse[kJsonBufferSize] ;
            GatewayProtocol_BuildStatusJson(&sGatewayState, theCommandId, theResponse, sizeof(theResponse)) ;
            printf("%s", theResponse) ;
            stdio_flush() ;
          }
          // Handle gateway info locally
          else if (theCommandType == kUsbCmdGatewayInfo)
          {
            // Get GPS data
            const GpsData * theGpsData = sGpsOk ? GPS_GetData() : NULL ;
            bool theGpsFix = (theGpsData != NULL && theGpsData->pValid) ;

            // Build gateway device info JSON response
            printf("{\"type\":\"gw_info\","
                   "\"version\":\"%s\","
                   "\"build\":\"%s %s\","
                   "\"lora\":%s,"
                   "\"bmp390\":%s,"
                   "\"gps\":%s,"
                   "\"display\":%s,"
                   "\"connected\":%s,"
                   "\"rx\":%lu,"
                   "\"tx\":%lu,"
                   "\"rssi\":%d,"
                   "\"snr\":%d,"
                   "\"ground_pres\":%.0f,"
                   "\"ground_temp\":%.1f,"
                   "\"gps_fix\":%s,"
                   "\"gps_lat\":%.6f,"
                   "\"gps_lon\":%.6f,"
                   "\"gps_sats\":%u}\n",
                   FIRMWARE_VERSION_STRING,
                   __DATE__, __TIME__,
                   sLoRaOk ? "true" : "false",
                   sBmp390Ok ? "true" : "false",
                   sGpsOk ? "true" : "false",
                   sDisplayOk ? "true" : "false",
                   sGatewayState.pConnected ? "true" : "false",
                   (unsigned long)sGatewayState.pPacketsReceived,
                   (unsigned long)sGatewayState.pPacketsSent,
                   sGatewayState.pLastRssi,
                   sGatewayState.pLastSnr,
                   sGroundPressurePa,
                   sGroundTemperatureC,
                   theGpsFix ? "true" : "false",
                   theGpsFix ? theGpsData->pLatitude : 0.0f,
                   theGpsFix ? theGpsData->pLongitude : 0.0f,
                   theGpsFix ? theGpsData->pSatellites : 0) ;
            stdio_flush() ;
          }
          // Handle flash read commands (slot and sample index)
          else if (theCommandType == kUsbCmdFlashRead && sLoRaOk)
          {
            uint8_t theSlot = 0 ;
            uint32_t theSample = 0 ;

            if (GatewayProtocol_ParseFlashParams(sUsbLineBuffer, &theSlot, &theSample))
            {
              uint8_t thePacket[16] ;
              int theLen = GatewayProtocol_BuildFlashReadCommand(
                theSlot, theSample, thePacket, sizeof(thePacket)) ;

              DEBUG_PRINT("CMD: Flash read slot=%u sample=%lu\n", theSlot, (unsigned long)theSample) ;

              if (theLen > 0 && LoRa_SendBlocking(&sLoRaRadio, thePacket, theLen, 500))
              {
                sGatewayState.pPacketsSent++ ;
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
                stdio_flush() ;
              }
              else
              {
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
                stdio_flush() ;
              }
              LoRa_StartReceive(&sLoRaRadio) ;
            }
            else
            {
              DEBUG_PRINT("CMD: Flash read - failed to parse params\n") ;
              char theResponse[64] ;
              GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
              printf("%s", theResponse) ;
              stdio_flush() ;
            }
          }
          // Handle flash delete commands (slot number)
          else if (theCommandType == kUsbCmdFlashDelete && sLoRaOk)
          {
            uint8_t theSlot = 0 ;
            uint32_t theSample = 0 ;

            if (GatewayProtocol_ParseFlashParams(sUsbLineBuffer, &theSlot, &theSample))
            {
              uint8_t thePacket[8] ;
              int theLen = GatewayProtocol_BuildFlashDeleteCommand(
                theSlot, thePacket, sizeof(thePacket)) ;

              DEBUG_PRINT("CMD: Flash delete slot=%u\n", theSlot) ;

              if (theLen > 0 && LoRa_SendBlocking(&sLoRaRadio, thePacket, theLen, 500))
              {
                sGatewayState.pPacketsSent++ ;
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
                stdio_flush() ;
              }
              else
              {
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
                stdio_flush() ;
              }
              LoRa_StartReceive(&sLoRaRadio) ;
            }
            else
            {
              DEBUG_PRINT("CMD: Flash delete - failed to parse params\n") ;
              char theResponse[64] ;
              GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
              printf("%s", theResponse) ;
              stdio_flush() ;
            }
          }
          // Handle orientation mode command (has enabled parameter)
          else if (theCommandType == kUsbCmdOrientationMode && sLoRaOk)
          {
            bool theEnabled = false ;
            if (GatewayProtocol_ParseOrientationModeEnabled(sUsbLineBuffer, &theEnabled))
            {
              uint8_t thePacket[8] ;
              int theLen = GatewayProtocol_BuildOrientationModeCommand(theEnabled, thePacket, sizeof(thePacket)) ;

              if (theLen > 0 && LoRa_SendBlocking(&sLoRaRadio, thePacket, theLen, 500))
              {
                sGatewayState.pPacketsSent++ ;
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
                stdio_flush() ;
              }
              else
              {
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
                stdio_flush() ;
              }

              LoRa_StartReceive(&sLoRaRadio) ;
            }
            else
            {
              char theResponse[64] ;
              GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
              printf("%s", theResponse) ;
              stdio_flush() ;
            }
          }
          // Forward other commands to flight computer (arm, disarm, reset, sd_list, flash_list, info, etc.)
          else if (sLoRaOk)
          {
            DEBUG_PRINT("CMD: Forwarding command type %d to flight computer\n", theCommandType) ;
            uint8_t thePacket[8] ;
            int theLen = GatewayProtocol_BuildLoRaCommand(theCommandType, thePacket, sizeof(thePacket)) ;
            DEBUG_PRINT("CMD: Built LoRa packet, len=%d\n", theLen) ;

            if (theLen > 0)
            {
              DEBUG_PRINT("CMD: Sending via LoRa...\n") ;
              if (LoRa_SendBlocking(&sLoRaRadio, thePacket, theLen, 500))
              {
                sGatewayState.pPacketsSent++ ;
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
                stdio_flush() ;
              }
              else
              {
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
                stdio_flush() ;
              }

              // Return to receive mode
              LoRa_StartReceive(&sLoRaRadio) ;
            }
          }
        }
      }

      sUsbLinePos = 0 ;
    }
    else if (sUsbLinePos < kUsbLineBufferSize - 1)
    {
      // Add character to buffer
      sUsbLineBuffer[sUsbLinePos++] = (char)theChar ;
    }
  }
}

//----------------------------------------------
// Function: UpdateLed
//----------------------------------------------
static void UpdateLed(uint32_t inCurrentMs)
{
  uint32_t theBlinkRate = sGatewayState.pConnected ? kLedBlinkFastMs : kLedBlinkSlowMs ;

  if ((inCurrentMs - sLastLedToggleMs) >= theBlinkRate)
  {
    sLastLedToggleMs = inCurrentMs ;
    sLedState = !sLedState ;
    gpio_put(kPinLed, sLedState) ;
  }
}

//----------------------------------------------
// Function: ProcessButtons
// Purpose: Handle button presses for display mode cycling
//----------------------------------------------
static void ProcessButtons(uint32_t inCurrentMs)
{
  // Button A (left) - previous mode
  bool theButtonA = !gpio_get(kPinButtonA) ;  // Active low
  if (theButtonA && !sButtonAState && (inCurrentMs - sButtonALastMs) > kButtonDebounceMs)
  {
    sButtonALastMs = inCurrentMs ;
    GatewayDisplay_PrevMode() ;
    DEBUG_PRINT("BTN: Mode changed to %d\n", GatewayDisplay_GetMode()) ;
  }
  sButtonAState = theButtonA ;

  // Button B (middle) - show About screen
  bool theButtonB = !gpio_get(kPinButtonB) ;  // Active low
  if (theButtonB && !sButtonBState && (inCurrentMs - sButtonBLastMs) > kButtonDebounceMs)
  {
    sButtonBLastMs = inCurrentMs ;
    // Toggle to About mode
    if (GatewayDisplay_GetMode() == kGwDisplayModeAbout)
    {
      GatewayDisplay_SetMode(kGwDisplayModeTelemetry) ;
    }
    else
    {
      GatewayDisplay_SetMode(kGwDisplayModeAbout) ;
    }
    DEBUG_PRINT("BTN: Mode changed to %d\n", GatewayDisplay_GetMode()) ;
  }
  sButtonBState = theButtonB ;

  // Button C (right) - next mode
  bool theButtonC = !gpio_get(kPinButtonC) ;  // Active low
  if (theButtonC && !sButtonCState && (inCurrentMs - sButtonCLastMs) > kButtonDebounceMs)
  {
    sButtonCLastMs = inCurrentMs ;
    GatewayDisplay_CycleMode() ;
    DEBUG_PRINT("BTN: Mode changed to %d\n", GatewayDisplay_GetMode()) ;
  }
  sButtonCState = theButtonC ;
}

//----------------------------------------------
// Function: UpdateDisplay
// Purpose: Periodically update display with signal/packet info
//----------------------------------------------
static void UpdateDisplay(uint32_t inCurrentMs)
{
  // Check USB connection status (every 500ms)
  if ((inCurrentMs - sLastUsbCheckMs) >= 500)
  {
    sLastUsbCheckMs = inCurrentMs ;
    bool theUsbNow = stdio_usb_connected() ;
    if (theUsbNow != sUsbConnected)
    {
      sUsbConnected = theUsbNow ;
      GatewayDisplay_SetUsbConnected(sUsbConnected) ;
      if (sUsbConnected)
      {
        printf("{\"type\":\"link\",\"status\":\"usb_connected\"}\n") ;
        stdio_flush() ;
      }
    }
  }

  // Update display at fixed interval
  if ((inCurrentMs - sLastDisplayUpdateMs) < kOledUpdateIntervalMs)
  {
    return ;
  }
  sLastDisplayUpdateMs = inCurrentMs ;

  // Update cached display data
  GatewayDisplay_UpdateSignal(sGatewayState.pLastRssi, sGatewayState.pLastSnr) ;
  GatewayDisplay_UpdatePacketStats(sGatewayState.pPacketsReceived, sGatewayState.pPacketsLost) ;
  GatewayDisplay_UpdateBarometer(sGroundPressurePa, sGroundTemperatureC) ;

  // Update GPS display data
  if (sGpsOk)
  {
    const GpsData * theGps = GPS_GetData() ;
    GatewayDisplay_UpdateGps(true, theGps->pValid, theGps->pLatitude, theGps->pLongitude,
      theGps->pSatellites, theGps->pSpeedMps, theGps->pHeadingDeg) ;
  }
  else
  {
    GatewayDisplay_UpdateGps(false, false, 0.0f, 0.0f, 0, 0.0f, 0.0f) ;
  }

  // Handle About screen specially with version info
  if (GatewayDisplay_GetMode() == kGwDisplayModeAbout)
  {
    GatewayDisplay_ShowAbout(FIRMWARE_VERSION_STRING, __DATE__, __TIME__) ;
  }
  else
  {
    // Update current mode display
    GatewayDisplay_Update() ;
  }
}
