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
#include "bmp581.h"
#include "neopixel.h"
#if kEnableGps
#include "gps.h"
#endif
#if kEnableWifi
#include "wifi_nina.h"
#include "wifi_config.h"
#endif

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
#define kLedBlinkFastMs         250
#define kLedBlinkSlowMs         1000
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
// Output Helper (USB + optional WiFi)
//----------------------------------------------
#if kEnableWifi
  // Forward declaration - implemented at end of file
  static void OutputToAll(const char * inJson) ;
  #define OUTPUT_JSON(json) OutputToAll(json)
#else
  #define OUTPUT_JSON(json) do { printf("%s", json); stdio_flush(); } while(0)
#endif

//----------------------------------------------
// Module State
//----------------------------------------------
static LoRa_Radio sLoRaRadio ;
static GatewayState sGatewayState ;
static BMP390 sBmp390 ;
static BMP581 sBmp581 ;
static bool sLoRaOk = false ;
static bool sDisplayOk = false ;
static bool sUsbConnected = false ;
static bool sBmp390Ok = false ;
static bool sBmp581Ok = false ;
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

// WiFi state (conditional)
#if kEnableWifi
static bool sWifiOk = false ;
static WifiMode_t sWifiMode = kWifiModeNone ;
static int8_t sWifiServerSocket = -1 ;
static int8_t sWifiClientSocket = -1 ;
static bool sWifiClientConnected = false ;
static char sWifiLineBuffer[kUsbLineBufferSize] ;
static int sWifiLinePos = 0 ;
#endif

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
#if kEnableWifi
static void ProcessWifiInput(uint32_t inCurrentMs) ;
static void ProcessCommandLine(const char * inLine, bool inIsWifi) ;
static void OutputToAll(const char * inJson) ;
#endif

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
  printf("  Build: %s %s\n", kBuildDate, kBuildTime) ;
  printf("======================================\n\n") ;

  // Initialize hardware
  InitializeHardware() ;

  // Initialize gateway protocol
  GatewayProtocol_Init(&sGatewayState) ;

  // Print startup status
  printf("Gateway ready:\n") ;
  printf("  LoRa: %s\n", sLoRaOk ? "OK" : "FAIL") ;
  printf("  Display: %s\n", sDisplayOk ? "OK" : "FAIL") ;
  if (sBmp581Ok)
    printf("  Baro: BMP581 (OK)\n") ;
  else if (sBmp390Ok)
    printf("  Baro: BMP390 (OK)\n") ;
  else
    printf("  Baro: NONE (FAIL)\n") ;
  printf("  GPS: %s\n", sGpsOk ? "OK" : "FAIL") ;
#if kEnableWifi
  printf("  WiFi: %s\n", sWifiOk ? "OK" : "FAIL") ;
  if (sWifiOk)
  {
    const WifiStatus_t * theStatus = WifiConfig_GetStatus() ;
    if (sWifiMode == kWifiModeStation)
    {
      printf("  WiFi Mode: Station\n") ;
      printf("  WiFi SSID: %s\n", theStatus->ssid) ;
      printf("  WiFi IP: %d.%d.%d.%d\n", theStatus->ip[0], theStatus->ip[1], theStatus->ip[2], theStatus->ip[3]) ;
    }
    else
    {
      printf("  WiFi Mode: AP\n") ;
      printf("  WiFi SSID: %s\n", theStatus->ssid) ;
    }
    printf("  WiFi Port: %d\n", kWifiServerPort) ;
  }
#endif
  printf("  Frequency: %lu Hz\n", (unsigned long)kLoRaFrequency) ;
  printf("  Sync Word: 0x%02X\n", kLoRaSyncWord) ;
  printf("\nListening for telemetry...\n\n") ;

  // Show splash screen, then device info
  if (sDisplayOk)
  {
    GatewayDisplay_ShowSplash() ;
    sleep_ms(kSplashDurationMs) ;

    // Show device info briefly
    GatewayDisplay_ShowDeviceInfo(
      FIRMWARE_VERSION_STRING,
      sLoRaOk,
      sBmp581Ok || sBmp390Ok,
      sGpsOk,
      sDisplayOk) ;
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
    if (sBmp581Ok || sBmp390Ok)
    {
      ReadGroundBarometer(theCurrentMs) ;
    }

    // Update GPS
#if kEnableGps
    if (sGpsOk)
    {
      GPS_Update(theCurrentMs) ;
    }
#endif

    // Process USB serial input
    ProcessUsbInput(theCurrentMs) ;

    // Process WiFi input
#if kEnableWifi
    if (sWifiOk)
    {
      // Check for new client connections
      if (!sWifiClientConnected)
      {
        int8_t theClient = WiFi_ServerHasClient(sWifiServerSocket) ;
        if (theClient >= 0)
        {
          sWifiClientSocket = theClient ;
          sWifiClientConnected = true ;
          sWifiLinePos = 0 ;
          printf("{\"type\":\"link\",\"status\":\"wifi_connected\"}\n") ;
          stdio_flush() ;
          printf("WiFi: Client connected on socket %d\n", theClient) ;
        }
      }
      else
      {
        // Check if client is still connected
        if (!WiFi_ClientConnected(sWifiClientSocket))
        {
          sWifiClientConnected = false ;
          sWifiClientSocket = -1 ;
          sWifiLinePos = 0 ;
          printf("{\"type\":\"link\",\"status\":\"wifi_disconnected\"}\n") ;
          stdio_flush() ;
          DEBUG_PRINT("WiFi: Client disconnected\n") ;
        }
        else
        {
          // Process WiFi input
          ProcessWifiInput(theCurrentMs) ;
        }
      }

      // Poll WiFi
      WiFi_Poll() ;
    }
#endif

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

  // Initialize NeoPixel for status indication
  printf("Initializing NeoPixel...\n") ;
  if (NeoPixel_Init(kPinNeoPixel))
  {
    printf("NeoPixel initialized on GPIO %d\n", kPinNeoPixel) ;
    NeoPixel_SetColor(NEOPIXEL_DIM_BLUE) ;  // Blue during init
  }
  else
  {
    printf("WARNING: NeoPixel initialization failed\n") ;
  }

  // Initialize barometric sensor (try BMP581 first, then BMP390)
  printf("Initializing barometer...\n") ;

  // Try BMP581 first (newer, more accurate: ±3.3cm vs ±25cm)
  if (BMP581_Init(&sBmp581, BMP581_I2C_ADDR_DEFAULT))
  {
    sBmp581Ok = true ;
    printf("BMP581 initialized at 0x%02X\n", BMP581_I2C_ADDR_DEFAULT) ;
  }
  else if (BMP581_Init(&sBmp581, BMP581_I2C_ADDR_ALT))
  {
    sBmp581Ok = true ;
    printf("BMP581 initialized at 0x%02X\n", BMP581_I2C_ADDR_ALT) ;
  }

  // Fall back to BMP390 if BMP581 not found
  if (!sBmp581Ok)
  {
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
      printf("BMP390 initialized at 0x%02X\n", kI2cAddrBMP390) ;
    }
    else
    {
      printf("WARNING: No barometer found (BMP581 or BMP390)\n") ;
    }
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

  // Initialize GPS (if enabled)
#if kEnableGps
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
#else
  printf("GPS: Disabled (kEnableGps=0)\n") ;
  sGpsOk = false ;
#endif

  // Initialize buttons
  InitializeButtons() ;

  // Initialize WiFi (if enabled)
#if kEnableWifi
  printf("Initializing WiFi (AirLift)...\n") ;

  // Initialize WiFi configuration first (loads from flash)
  WifiConfig_Init() ;
  const WifiConfig_t * theWifiConfig = WifiConfig_Get() ;

  if (WiFi_Init())
  {
    printf("WiFi hardware initialized\n") ;

    // Try to connect to stored station networks first
    bool theConnected = false ;
    uint8_t theEnabledCount = WifiConfig_GetEnabledCount() ;

    if (theEnabledCount > 0)
    {
      printf("Found %d stored WiFi networks, trying station mode...\n", theEnabledCount) ;

      // Try each enabled network in priority order
      for (uint8_t thePriority = 0 ; thePriority < WIFI_CONFIG_MAX_NETWORKS && !theConnected ; thePriority++)
      {
        const WifiNetwork_t * theNetwork = WifiConfig_GetNetworkByPriority(thePriority) ;
        if (theNetwork == NULL)
        {
          break ;  // No more networks
        }

        printf("Trying network: %s (priority %d)...\n", theNetwork->ssid, theNetwork->priority) ;

        if (WiFi_Connect(theNetwork->ssid, theNetwork->password))
        {
          // Wait for connection with timeout
          if (WiFi_WaitForConnection(10000))  // 10 second timeout
          {
            // Get IP address
            uint8_t theIp[4] ;
            if (WiFi_GetLocalIP(theIp))
            {
              printf("Connected to %s\n", theNetwork->ssid) ;
              printf("IP Address: %d.%d.%d.%d\n", theIp[0], theIp[1], theIp[2], theIp[3]) ;

              // Get RSSI
              int8_t theRssi = WiFi_GetRSSI() ;
              printf("Signal strength: %d dBm\n", theRssi) ;

              // Update WiFi config status
              WifiConfig_SetStatus(kWifiModeStation, theNetwork->ssid, theIp, theRssi, true) ;
              sWifiMode = kWifiModeStation ;
              theConnected = true ;
            }
          }
          else
          {
            printf("Connection timeout to %s\n", theNetwork->ssid) ;
          }
        }
        else
        {
          printf("Failed to initiate connection to %s\n", theNetwork->ssid) ;
        }
      }
    }
    else
    {
      printf("No stored WiFi networks configured\n") ;
    }

    // If no station connection, fall back to AP mode
    if (!theConnected)
    {
      printf("Station mode failed, starting AP: %s\n", theWifiConfig->apSsid) ;
      if (WiFi_StartAP(theWifiConfig->apSsid, theWifiConfig->apPassword, theWifiConfig->apChannel))
      {
        printf("WiFi AP started on channel %d\n", theWifiConfig->apChannel) ;

        // Update WiFi config status
        uint8_t theApIp[4] = { 192, 168, 4, 1 } ;  // Standard AP mode IP
        WifiConfig_SetStatus(kWifiModeAP, theWifiConfig->apSsid, theApIp, 0, false) ;
        sWifiMode = kWifiModeAP ;
        theConnected = true ;
      }
      else
      {
        printf("WARNING: WiFi AP failed to start\n") ;
      }
    }

    // Start TCP server (works in both modes)
    if (theConnected)
    {
      printf("Starting TCP server on port %d...\n", kWifiServerPort) ;
      sWifiServerSocket = WiFi_StartServer(kWifiServerPort) ;
      if (sWifiServerSocket >= 0)
      {
        sWifiOk = true ;
        printf("WiFi TCP server started on socket %d\n", sWifiServerSocket) ;
      }
      else
      {
        printf("WARNING: WiFi TCP server failed to start\n") ;
      }
    }
  }
  else
  {
    printf("WARNING: WiFi hardware initialization failed\n") ;
  }
#else
  printf("WiFi: Disabled (kEnableWifi=0)\n") ;
#endif

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

  // Read pressure and temperature from whichever sensor is available
  float thePressure = 0 ;
  float theTemperature = 0 ;
  bool theReadOk = false ;

  if (sBmp581Ok)
  {
    theReadOk = BMP581_ReadPressureTemperature(&sBmp581, &thePressure, &theTemperature) ;
  }
  else if (sBmp390Ok)
  {
    theReadOk = BMP390_ReadPressureTemperature(&sBmp390, &thePressure, &theTemperature) ;
  }

  if (theReadOk)
  {
    sGroundPressurePa = thePressure ;
    sGroundTemperatureC = theTemperature ;

    // Debug: show reading every second
    static uint32_t sDebugCount = 0 ;
    if ((++sDebugCount % 10) == 0)
    {
      DEBUG_PRINT("GND %s: P=%.0f Pa T=%.1f C\n",
        sBmp581Ok ? "BMP581" : "BMP390",
        sGroundPressurePa, sGroundTemperatureC) ;
    }
  }
  else
  {
    // Debug: show read failure
    static uint32_t sFailCount = 0 ;
    if ((++sFailCount % 10) == 0)
    {
      DEBUG_PRINT("GND %s: Read failed (%u)\n",
        sBmp581Ok ? "BMP581" : "BMP390", sFailCount) ;
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
    bool theGwGpsValid = false ;
    float theGwGpsLat = 0.0f ;
    float theGwGpsLon = 0.0f ;
#if kEnableGps
    if (sGpsOk)
    {
      const GpsData * theGwGps = GPS_GetData() ;
      if (theGwGps != NULL && theGwGps->pValid)
      {
        theGwGpsValid = true ;
        theGwGpsLat = theGwGps->pLatitude ;
        theGwGpsLon = theGwGps->pLongitude ;
      }
    }
#endif

    // Convert to JSON and output to USB
    char theJson[kJsonBufferSize] ;
    int theJsonLen = GatewayProtocol_TelemetryToJson(
      thePacket,
      sGatewayState.pLastRssi,
      sGatewayState.pLastSnr,
      sGroundPressurePa,
      theGwGpsValid,
      theGwGpsLat,
      theGwGpsLon,
      theJson,
      sizeof(theJson)) ;

    if (theJsonLen > 0)
    {
      OUTPUT_JSON(theJson) ;
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
      OUTPUT_JSON(theJson) ;
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
      OUTPUT_JSON(theJson) ;
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

    // Rocket ID
    uint8_t theRocketId = 0 ;
    if (theOffset < theLen)
    {
      theRocketId = theBuffer[theOffset++] ;
    }

    // Rocket name (length-prefixed string)
    char theRocketName[32] = "" ;
    if (theOffset < theLen)
    {
      uint8_t theNameLen = theBuffer[theOffset++] ;
      if (theNameLen < 32 && theOffset + theNameLen <= theLen)
      {
        memcpy(theRocketName, &theBuffer[theOffset], theNameLen) ;
        theRocketName[theNameLen] = '\0' ;
        theOffset += theNameLen ;
      }
    }

    // Barometer type string (length-prefixed)
    char theBaroType[32] = "" ;
    if (theOffset < theLen)
    {
      uint8_t theBaroTypeLen = theBuffer[theOffset++] ;
      if (theBaroTypeLen < 32 && theOffset + theBaroTypeLen <= theLen)
      {
        memcpy(theBaroType, &theBuffer[theOffset], theBaroTypeLen) ;
        theBaroType[theBaroTypeLen] = '\0' ;
        theOffset += theBaroTypeLen ;
      }
    }

    // IMU type string (length-prefixed)
    char theImuType[32] = "" ;
    if (theOffset < theLen)
    {
      uint8_t theImuTypeLen = theBuffer[theOffset++] ;
      if (theImuTypeLen < 32 && theOffset + theImuTypeLen <= theLen)
      {
        memcpy(theImuType, &theBuffer[theOffset], theImuTypeLen) ;
        theImuType[theImuTypeLen] = '\0' ;
        theOffset += theImuTypeLen ;
      }
    }

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
           "\"rocket_id\":%u,"
           "\"rocket_name\":\"%s\"",
           theVersion,
           theBuild,
           (theFlags & 0x01) ? "true" : "false",
           (theFlags & 0x02) ? "true" : "false",
           (theFlags & 0x04) ? "true" : "false",
           (theFlags & 0x10) ? "true" : "false",
           (theFlags & 0x20) ? "true" : "false",
           GatewayProtocol_GetStateName(theState),
           (unsigned long)theSamples,
           theRocketId,
           theRocketName) ;

    // Add sensor type strings if present
    if (theBaroType[0] != '\0')
    {
      printf(",\"baro_type\":\"%s\"", theBaroType) ;
    }
    if (theImuType[0] != '\0')
    {
      printf(",\"imu_type\":\"%s\"", theImuType) ;
    }

    printf("}\n") ;
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
            bool theGpsFix = false ;
            float theGpsLat = 0.0f ;
            float theGpsLon = 0.0f ;
            uint8_t theGpsSats = 0 ;
#if kEnableGps
            if (sGpsOk)
            {
              const GpsData * theGpsData = GPS_GetData() ;
              if (theGpsData != NULL && theGpsData->pValid)
              {
                theGpsFix = true ;
                theGpsLat = theGpsData->pLatitude ;
                theGpsLon = theGpsData->pLongitude ;
                theGpsSats = theGpsData->pSatellites ;
              }
            }
#endif

            // Build gateway device info JSON response
            printf("{\"type\":\"gw_info\","
                   "\"version\":\"%s\","
                   "\"build\":\"%s %s\","
                   "\"lora\":%s,"
                   "\"baro\":%s,"
                   "\"baro_type\":\"%s\","
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
                   kBuildDate, kBuildTime,
                   sLoRaOk ? "true" : "false",
                   (sBmp581Ok || sBmp390Ok) ? "true" : "false",
                   sBmp581Ok ? "BMP581" : (sBmp390Ok ? "BMP390" : "None"),
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
                   theGpsLat,
                   theGpsLon,
                   theGpsSats) ;
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
#if kEnableWifi
          // WiFi configuration commands (handled locally)
          else if (theCommandType == kUsbCmdWifiList)
          {
            const WifiConfig_t * theConfig = WifiConfig_Get() ;
            char theResponse[512] ;
            int theResponseLen = snprintf(theResponse, sizeof(theResponse),
              "{\"type\":\"wifi_list\",\"id\":%lu,\"count\":%u,\"networks\":[",
              (unsigned long)theCommandId, theConfig->networkCount) ;

            for (int i = 0 ; i < theConfig->networkCount && theResponseLen < (int)sizeof(theResponse) - 100 ; i++)
            {
              if (i > 0) theResponseLen += snprintf(theResponse + theResponseLen, sizeof(theResponse) - theResponseLen, ",") ;
              theResponseLen += snprintf(theResponse + theResponseLen, sizeof(theResponse) - theResponseLen,
                "{\"ssid\":\"%s\",\"priority\":%u,\"enabled\":%s}",
                theConfig->networks[i].ssid,
                theConfig->networks[i].priority,
                theConfig->networks[i].enabled ? "true" : "false") ;
            }
            theResponseLen += snprintf(theResponse + theResponseLen, sizeof(theResponse) - theResponseLen, "]}\n") ;
            printf("%s", theResponse) ;
            stdio_flush() ;
          }
          else if (theCommandType == kUsbCmdWifiAdd)
          {
            char theSsid[33] ;
            char thePassword[65] ;
            uint8_t thePriority = 0 ;

            if (GatewayProtocol_ParseWifiAddParams(sUsbLineBuffer, theSsid, thePassword, &thePriority))
            {
              int theIdx = WifiConfig_AddNetwork(theSsid, thePassword, thePriority) ;
              char theResponse[64] ;
              GatewayProtocol_BuildAckJson(theCommandId, (theIdx >= 0), theResponse, sizeof(theResponse)) ;
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
          }
          else if (theCommandType == kUsbCmdWifiRemove)
          {
            uint8_t theIndex = 0 ;
            if (GatewayProtocol_ParseWifiRemoveParams(sUsbLineBuffer, &theIndex))
            {
              bool theSuccess = WifiConfig_RemoveNetworkByIndex(theIndex) ;
              char theResponse[64] ;
              GatewayProtocol_BuildAckJson(theCommandId, theSuccess, theResponse, sizeof(theResponse)) ;
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
          }
          else if (theCommandType == kUsbCmdWifiSave)
          {
            bool theSuccess = WifiConfig_Save() ;
            char theResponse[64] ;
            GatewayProtocol_BuildAckJson(theCommandId, theSuccess, theResponse, sizeof(theResponse)) ;
            printf("%s", theResponse) ;
            stdio_flush() ;
          }
          else if (theCommandType == kUsbCmdWifiStatus)
          {
            const WifiStatus_t * theStatus = WifiConfig_GetStatus() ;
            char theResponse[256] ;
            snprintf(theResponse, sizeof(theResponse),
              "{\"type\":\"wifi_status\",\"id\":%lu,"
              "\"mode\":\"%s\","
              "\"ssid\":\"%s\","
              "\"ip\":\"%d.%d.%d.%d\","
              "\"rssi\":%d,"
              "\"connected\":%s}\n",
              (unsigned long)theCommandId,
              theStatus->mode == kWifiModeStation ? "station" : (theStatus->mode == kWifiModeAP ? "ap" : "none"),
              theStatus->ssid,
              theStatus->ip[0], theStatus->ip[1], theStatus->ip[2], theStatus->ip[3],
              theStatus->rssi,
              theStatus->connected ? "true" : "false") ;
            printf("%s", theResponse) ;
            stdio_flush() ;
          }
          else if (theCommandType == kUsbCmdWifiSetAp)
          {
            char theSsid[33] ;
            char thePassword[65] ;
            uint8_t theChannel = 0 ;

            if (GatewayProtocol_ParseWifiApParams(sUsbLineBuffer, theSsid, thePassword, &theChannel))
            {
              WifiConfig_SetAP(
                theSsid[0] ? theSsid : NULL,
                thePassword[0] ? thePassword : NULL,
                theChannel) ;
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
          }
#endif
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
// Uses NeoPixel for status indication:
//   - Green blinking fast: Connected to flight computer
//   - Blue blinking slow: Searching for signal
//   - Red: Error state
//----------------------------------------------
static void UpdateLed(uint32_t inCurrentMs)
{
  uint32_t theBlinkRate = sGatewayState.pConnected ? kLedBlinkFastMs : kLedBlinkSlowMs ;

  if ((inCurrentMs - sLastLedToggleMs) >= theBlinkRate)
  {
    sLastLedToggleMs = inCurrentMs ;
    sLedState = !sLedState ;

    if (sLedState)
    {
      // LED on - show status color
      if (sGatewayState.pConnected)
      {
        NeoPixel_SetColor(NEOPIXEL_DIM_GREEN) ;  // Connected = green
      }
      else
      {
        NeoPixel_SetColor(NEOPIXEL_DIM_BLUE) ;   // Searching = blue
      }
    }
    else
    {
      // LED off (dim)
      NeoPixel_SetColor(NEOPIXEL_OFF) ;
    }
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
#if kEnableGps
  if (sGpsOk)
  {
    const GpsData * theGps = GPS_GetData() ;
    GatewayDisplay_UpdateGps(true, theGps->pValid, theGps->pLatitude, theGps->pLongitude,
      theGps->pSatellites, theGps->pSpeedMps, theGps->pHeadingDeg) ;
  }
  else
#endif
  {
    GatewayDisplay_UpdateGps(false, false, 0.0f, 0.0f, 0, 0.0f, 0.0f) ;
  }

  // Update WiFi display data
#if kEnableWifi
  {
    const WifiStatus_t * theWifiStatus = WifiConfig_GetStatus() ;
    GatewayDisplay_UpdateWifi(
      sWifiOk,
      (uint8_t)theWifiStatus->mode,
      theWifiStatus->ssid,
      theWifiStatus->ip,
      theWifiStatus->rssi,
      sWifiClientConnected) ;
  }
#else
  GatewayDisplay_UpdateWifi(false, 0, NULL, NULL, 0, false) ;
#endif

  // Handle About screen specially with version info
  if (GatewayDisplay_GetMode() == kGwDisplayModeAbout)
  {
    GatewayDisplay_ShowAbout(FIRMWARE_VERSION_STRING, kBuildDate, kBuildTime) ;
  }
  else
  {
    // Update current mode display
    GatewayDisplay_Update() ;
  }
}

//----------------------------------------------
// WiFi Functions (conditional)
//----------------------------------------------
#if kEnableWifi

//----------------------------------------------
// Function: OutputToAll
// Purpose: Output JSON to both USB and WiFi
//----------------------------------------------
static void OutputToAll(const char * inJson)
{
  // Output to USB
  printf("%s", inJson) ;
  stdio_flush() ;

  // Output to WiFi client if connected
  if (sWifiClientConnected && sWifiClientSocket >= 0)
  {
    int theLen = strlen(inJson) ;
    WiFi_Write(sWifiClientSocket, (const uint8_t *)inJson, theLen) ;
  }
}

//----------------------------------------------
// Function: ProcessWifiInput
// Purpose: Process incoming WiFi commands
//----------------------------------------------
static void ProcessWifiInput(uint32_t inCurrentMs)
{
  (void)inCurrentMs ;  // May be used later for timeouts

  // Read available data from WiFi
  uint8_t theBuffer[256] ;
  int theLen = WiFi_Read(sWifiClientSocket, theBuffer, sizeof(theBuffer) - 1) ;

  if (theLen <= 0)
  {
    return ;
  }

  // Process received characters
  for (int i = 0 ; i < theLen ; i++)
  {
    char theChar = (char)theBuffer[i] ;

    if (theChar == '\n' || theChar == '\r')
    {
      // End of line - process command
      if (sWifiLinePos > 0)
      {
        sWifiLineBuffer[sWifiLinePos] = '\0' ;
        ProcessCommandLine(sWifiLineBuffer, true) ;
      }
      sWifiLinePos = 0 ;
    }
    else if (sWifiLinePos < (int)sizeof(sWifiLineBuffer) - 1)
    {
      // Add character to buffer
      sWifiLineBuffer[sWifiLinePos++] = theChar ;
    }
  }
}

//----------------------------------------------
// Function: ProcessCommandLine
// Purpose: Process a command line from USB or WiFi
//----------------------------------------------
static void ProcessCommandLine(const char * inLine, bool inIsWifi)
{
  (void)inIsWifi ;  // Could be used to route responses differently

  // Parse command
  UsbCommandType theCommandType ;
  uint32_t theCommandId ;

  if (!GatewayProtocol_ParseCommand(inLine, &theCommandType, &theCommandId))
  {
    return ;
  }

  // Handle ping locally
  if (theCommandType == kUsbCmdPing)
  {
    char theResponse[64] ;
    GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
    OutputToAll(theResponse) ;
  }
  // Handle status locally
  else if (theCommandType == kUsbCmdStatus)
  {
    char theResponse[kJsonBufferSize] ;
    GatewayProtocol_BuildStatusJson(&sGatewayState, theCommandId, theResponse, sizeof(theResponse)) ;
    OutputToAll(theResponse) ;
  }
  // Handle gateway info locally
  else if (theCommandType == kUsbCmdGatewayInfo)
  {
    // Get GPS data
    bool theGpsFix = false ;
    float theGpsLat = 0.0f ;
    float theGpsLon = 0.0f ;
    uint8_t theGpsSats = 0 ;
#if kEnableGps
    if (sGpsOk)
    {
      const GpsData * theGpsData = GPS_GetData() ;
      if (theGpsData != NULL && theGpsData->pValid)
      {
        theGpsFix = true ;
        theGpsLat = theGpsData->pLatitude ;
        theGpsLon = theGpsData->pLongitude ;
        theGpsSats = theGpsData->pSatellites ;
      }
    }
#endif

    // Build gateway device info JSON response (with WiFi info)
    char theResponse[512] ;
    snprintf(theResponse, sizeof(theResponse),
      "{\"type\":\"gw_info\","
      "\"version\":\"%s\","
      "\"build\":\"%s %s\","
      "\"lora\":%s,"
      "\"baro\":%s,"
      "\"baro_type\":\"%s\","
      "\"gps\":%s,"
      "\"wifi\":%s,"
      "\"display\":%s,"
      "\"connected\":%s,"
      "\"wifi_client\":%s,"
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
      kBuildDate, kBuildTime,
      sLoRaOk ? "true" : "false",
      (sBmp581Ok || sBmp390Ok) ? "true" : "false",
      sBmp581Ok ? "BMP581" : (sBmp390Ok ? "BMP390" : "None"),
      sGpsOk ? "true" : "false",
      sWifiOk ? "true" : "false",
      sDisplayOk ? "true" : "false",
      sGatewayState.pConnected ? "true" : "false",
      sWifiClientConnected ? "true" : "false",
      (unsigned long)sGatewayState.pPacketsReceived,
      (unsigned long)sGatewayState.pPacketsSent,
      sGatewayState.pLastRssi,
      sGatewayState.pLastSnr,
      sGroundPressurePa,
      sGroundTemperatureC,
      theGpsFix ? "true" : "false",
      theGpsLat,
      theGpsLon,
      theGpsSats) ;
    OutputToAll(theResponse) ;
  }
  // Handle flash list/read/delete commands
  else if (theCommandType == kUsbCmdFlashRead && sLoRaOk)
  {
    uint8_t theSlot = 0 ;
    uint32_t theSample = 0 ;

    if (GatewayProtocol_ParseFlashParams(inLine, &theSlot, &theSample))
    {
      uint8_t thePacket[16] ;
      int theLen = GatewayProtocol_BuildFlashReadCommand(
        theSlot, theSample, thePacket, sizeof(thePacket)) ;

      if (theLen > 0 && LoRa_SendBlocking(&sLoRaRadio, thePacket, theLen, 500))
      {
        sGatewayState.pPacketsSent++ ;
        char theResponse[64] ;
        GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
        OutputToAll(theResponse) ;
      }
      else
      {
        char theResponse[64] ;
        GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
        OutputToAll(theResponse) ;
      }
      LoRa_StartReceive(&sLoRaRadio) ;
    }
    else
    {
      char theResponse[64] ;
      GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
      OutputToAll(theResponse) ;
    }
  }
  else if (theCommandType == kUsbCmdFlashDelete && sLoRaOk)
  {
    uint8_t theSlot = 0 ;
    uint32_t theSample = 0 ;

    if (GatewayProtocol_ParseFlashParams(inLine, &theSlot, &theSample))
    {
      uint8_t thePacket[8] ;
      int theLen = GatewayProtocol_BuildFlashDeleteCommand(
        theSlot, thePacket, sizeof(thePacket)) ;

      if (theLen > 0 && LoRa_SendBlocking(&sLoRaRadio, thePacket, theLen, 500))
      {
        sGatewayState.pPacketsSent++ ;
        char theResponse[64] ;
        GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
        OutputToAll(theResponse) ;
      }
      else
      {
        char theResponse[64] ;
        GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
        OutputToAll(theResponse) ;
      }
      LoRa_StartReceive(&sLoRaRadio) ;
    }
    else
    {
      char theResponse[64] ;
      GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
      OutputToAll(theResponse) ;
    }
  }
  else if (theCommandType == kUsbCmdOrientationMode && sLoRaOk)
  {
    bool theEnabled = false ;
    if (GatewayProtocol_ParseOrientationModeEnabled(inLine, &theEnabled))
    {
      uint8_t thePacket[8] ;
      int theLen = GatewayProtocol_BuildOrientationModeCommand(theEnabled, thePacket, sizeof(thePacket)) ;

      if (theLen > 0 && LoRa_SendBlocking(&sLoRaRadio, thePacket, theLen, 500))
      {
        sGatewayState.pPacketsSent++ ;
        char theResponse[64] ;
        GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
        OutputToAll(theResponse) ;
      }
      else
      {
        char theResponse[64] ;
        GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
        OutputToAll(theResponse) ;
      }
      LoRa_StartReceive(&sLoRaRadio) ;
    }
    else
    {
      char theResponse[64] ;
      GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
      OutputToAll(theResponse) ;
    }
  }
  // WiFi configuration commands (handled locally)
  else if (theCommandType == kUsbCmdWifiList)
  {
    // Build JSON response with stored WiFi networks
    const WifiConfig_t * theConfig = WifiConfig_Get() ;
    char theResponse[512] ;
    int theLen = snprintf(theResponse, sizeof(theResponse),
      "{\"type\":\"wifi_list\",\"id\":%lu,\"count\":%u,\"networks\":[",
      (unsigned long)theCommandId, theConfig->networkCount) ;

    for (int i = 0 ; i < theConfig->networkCount && theLen < (int)sizeof(theResponse) - 100 ; i++)
    {
      if (i > 0) theLen += snprintf(theResponse + theLen, sizeof(theResponse) - theLen, ",") ;
      theLen += snprintf(theResponse + theLen, sizeof(theResponse) - theLen,
        "{\"ssid\":\"%s\",\"priority\":%u,\"enabled\":%s}",
        theConfig->networks[i].ssid,
        theConfig->networks[i].priority,
        theConfig->networks[i].enabled ? "true" : "false") ;
    }
    theLen += snprintf(theResponse + theLen, sizeof(theResponse) - theLen, "]}\n") ;
    OutputToAll(theResponse) ;
  }
  else if (theCommandType == kUsbCmdWifiAdd)
  {
    char theSsid[33] ;
    char thePassword[65] ;
    uint8_t thePriority = 0 ;

    if (GatewayProtocol_ParseWifiAddParams(inLine, theSsid, thePassword, &thePriority))
    {
      int theIdx = WifiConfig_AddNetwork(theSsid, thePassword, thePriority) ;
      char theResponse[64] ;
      GatewayProtocol_BuildAckJson(theCommandId, (theIdx >= 0), theResponse, sizeof(theResponse)) ;
      OutputToAll(theResponse) ;
    }
    else
    {
      char theResponse[64] ;
      GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
      OutputToAll(theResponse) ;
    }
  }
  else if (theCommandType == kUsbCmdWifiRemove)
  {
    uint8_t theIndex = 0 ;
    if (GatewayProtocol_ParseWifiRemoveParams(inLine, &theIndex))
    {
      bool theSuccess = WifiConfig_RemoveNetworkByIndex(theIndex) ;
      char theResponse[64] ;
      GatewayProtocol_BuildAckJson(theCommandId, theSuccess, theResponse, sizeof(theResponse)) ;
      OutputToAll(theResponse) ;
    }
    else
    {
      char theResponse[64] ;
      GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
      OutputToAll(theResponse) ;
    }
  }
  else if (theCommandType == kUsbCmdWifiSave)
  {
    bool theSuccess = WifiConfig_Save() ;
    char theResponse[64] ;
    GatewayProtocol_BuildAckJson(theCommandId, theSuccess, theResponse, sizeof(theResponse)) ;
    OutputToAll(theResponse) ;
  }
  else if (theCommandType == kUsbCmdWifiStatus)
  {
    const WifiStatus_t * theStatus = WifiConfig_GetStatus() ;
    char theResponse[256] ;
    snprintf(theResponse, sizeof(theResponse),
      "{\"type\":\"wifi_status\",\"id\":%lu,"
      "\"mode\":\"%s\","
      "\"ssid\":\"%s\","
      "\"ip\":\"%d.%d.%d.%d\","
      "\"rssi\":%d,"
      "\"connected\":%s}\n",
      (unsigned long)theCommandId,
      theStatus->mode == kWifiModeStation ? "station" : (theStatus->mode == kWifiModeAP ? "ap" : "none"),
      theStatus->ssid,
      theStatus->ip[0], theStatus->ip[1], theStatus->ip[2], theStatus->ip[3],
      theStatus->rssi,
      theStatus->connected ? "true" : "false") ;
    OutputToAll(theResponse) ;
  }
  else if (theCommandType == kUsbCmdWifiSetAp)
  {
    char theSsid[33] ;
    char thePassword[65] ;
    uint8_t theChannel = 0 ;

    if (GatewayProtocol_ParseWifiApParams(inLine, theSsid, thePassword, &theChannel))
    {
      WifiConfig_SetAP(
        theSsid[0] ? theSsid : NULL,
        thePassword[0] ? thePassword : NULL,
        theChannel) ;
      char theResponse[64] ;
      GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
      OutputToAll(theResponse) ;
    }
    else
    {
      char theResponse[64] ;
      GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
      OutputToAll(theResponse) ;
    }
  }
  // Forward other commands to flight computer
  else if (sLoRaOk)
  {
    uint8_t thePacket[8] ;
    int theLen = GatewayProtocol_BuildLoRaCommand(theCommandType, thePacket, sizeof(thePacket)) ;

    if (theLen > 0)
    {
      if (LoRa_SendBlocking(&sLoRaRadio, thePacket, theLen, 500))
      {
        sGatewayState.pPacketsSent++ ;
        char theResponse[64] ;
        GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
        OutputToAll(theResponse) ;
      }
      else
      {
        char theResponse[64] ;
        GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
        OutputToAll(theResponse) ;
      }
      LoRa_StartReceive(&sLoRaRadio) ;
    }
  }
}

#endif // kEnableWifi
