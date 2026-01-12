//----------------------------------------------
// Module: main.c
// Description: Rocket Avionics Ground Gateway
//   LoRa to USB serial bridge
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit Feather RP2040 with RFM95 LoRa 915MHz
//----------------------------------------------

#include "pins.h"
#include "version.h"
#include "lora_radio.h"
#include "gateway_protocol.h"
#include "gateway_display.h"
#include "bmp390.h"

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
#define kDisplayUpdateIntervalMs 100
#define kBmp390ReadIntervalMs   100   // 10 Hz sensor reading

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

// USB input buffer
static char sUsbLineBuffer[kUsbLineBufferSize] ;
static int sUsbLinePos = 0 ;

//----------------------------------------------
// Forward Declarations
//----------------------------------------------
static void InitializeHardware(void) ;
static void InitializeI2C(void) ;
static void InitializeSPI(void) ;
static void ProcessLoRaPackets(uint32_t inCurrentMs) ;
static void ProcessUsbInput(uint32_t inCurrentMs) ;
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
  printf("  Frequency: %lu Hz\n", (unsigned long)kLoRaFrequency) ;
  printf("  Sync Word: 0x%02X\n", kLoRaSyncWord) ;
  printf("\nListening for telemetry...\n\n") ;

  // Set initial display state
  if (sDisplayOk)
  {
    GatewayDisplay_SetConnectionState(kConnectionDisconnected) ;
    GatewayDisplay_ShowMessage("Ready - Listening...", false) ;
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

    // Process USB serial input
    ProcessUsbInput(theCurrentMs) ;

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

  // Initialize TFT display
  printf("Initializing TFT display...\n") ;
  if (GatewayDisplay_Init())
  {
    sDisplayOk = true ;
    printf("TFT display initialized\n") ;
  }
  else
  {
    printf("ERROR: TFT display initialization failed!\n") ;
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

  // Initialize SPI1 (shared by LoRa and TFT)
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

  // TFT chip select (active low)
  gpio_init(kPinTftCs) ;
  gpio_set_dir(kPinTftCs, GPIO_OUT) ;
  gpio_put(kPinTftCs, 1) ;

  // TFT data/command select
  gpio_init(kPinTftDc) ;
  gpio_set_dir(kPinTftDc, GPIO_OUT) ;
  gpio_put(kPinTftDc, 1) ;

  printf("SPI1 initialized\n") ;
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
      printf("GND BMP: P=%.0f Pa T=%.1f C\n", sGroundPressurePa, sGroundTemperatureC) ;
    }
  }
  else
  {
    // Debug: show read failure
    static uint32_t sFailCount = 0 ;
    if ((++sFailCount % 10) == 0)
    {
      printf("GND BMP: Read failed (%u)\n", sFailCount) ;
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

    // Update display
    if (sDisplayOk)
    {
      GatewayDisplay_SetConnectionState(kConnectionConnected) ;
      GatewayDisplay_ShowMessage("Link established!", false) ;
    }
  }

  // Debug: show received packet info
  printf("RX: len=%u magic=0x%02X\n", theLen, theBuffer[0]) ;

  // Validate packet
  if (theLen < 3 || theBuffer[0] != kLoRaMagic)
  {
    printf("RX: Invalid packet (len=%u, magic=0x%02X)\n", theLen, theBuffer[0]) ;
    return ;
  }

  uint8_t thePacketType = theBuffer[1] ;

  // Handle telemetry packets
  if (thePacketType == kLoRaPacketTelemetry && theLen >= sizeof(LoRaTelemetryPacket))
  {
    printf("RX: Telemetry packet, expected size=%u\n", (unsigned)sizeof(LoRaTelemetryPacket)) ;
  }
  else if (thePacketType == kLoRaPacketTelemetry)
  {
    printf("RX: Telemetry packet too small (got %u, need %u)\n", theLen, (unsigned)sizeof(LoRaTelemetryPacket)) ;
  }

  if (thePacketType == kLoRaPacketTelemetry && theLen >= sizeof(LoRaTelemetryPacket))
  {
    LoRaTelemetryPacket * thePacket = (LoRaTelemetryPacket *)theBuffer ;

    // Convert to JSON and output to USB
    char theJson[kJsonBufferSize] ;
    int theJsonLen = GatewayProtocol_TelemetryToJson(
      thePacket,
      sGatewayState.pLastRssi,
      sGatewayState.pLastSnr,
      sGroundPressurePa,
      theJson,
      sizeof(theJson)) ;

    if (theJsonLen > 0)
    {
      printf("%s", theJson) ;
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
  }
  // Handle storage list response (SD or Flash)
  else if (thePacketType == kLoRaPacketStorageList && theLen >= 3)
  {
    printf("RX: Storage list packet, len=%u\n", theLen) ;

    // Determine if this is SD or Flash based on a flag in the packet
    // For now, assume SD (we can add a storage type byte later)
    char theJson[1024] ;
    int theJsonLen = GatewayProtocol_StorageListToJson(
      theBuffer, theLen, true, theJson, sizeof(theJson)) ;

    if (theJsonLen > 0)
    {
      printf("%s", theJson) ;
    }
  }
  // Handle storage data chunk (SD or Flash)
  else if (thePacketType == kLoRaPacketStorageData && theLen >= 12)
  {
    printf("RX: Storage data packet, len=%u\n", theLen) ;

    char theJson[1024] ;
    int theJsonLen = GatewayProtocol_StorageDataToJson(
      theBuffer, theLen, true, theJson, sizeof(theJson)) ;

    if (theJsonLen > 0)
    {
      printf("%s", theJson) ;
    }
  }
  // Handle device info response
  else if (thePacketType == kLoRaPacketInfo && theLen >= 5)
  {
    printf("RX: Device info packet, len=%u\n", theLen) ;

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
    printf("{\"type\":\"fc_info\","
           "\"version\":\"%s\","
           "\"build\":\"%s\","
           "\"bmp390\":%s,"
           "\"lora\":%s,"
           "\"sd\":%s,"
           "\"rtc\":%s,"
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
           (theFlags & 0x08) ? "true" : "false",
           (theFlags & 0x10) ? "true" : "false",
           (theFlags & 0x20) ? "true" : "false",
           GatewayProtocol_GetStateName(theState),
           (unsigned long)theSamples,
           (unsigned long)theFreeKb,
           theFlightCount) ;
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
          }
          // Handle status locally
          else if (theCommandType == kUsbCmdStatus)
          {
            char theResponse[kJsonBufferSize] ;
            GatewayProtocol_BuildStatusJson(&sGatewayState, theCommandId, theResponse, sizeof(theResponse)) ;
            printf("%s", theResponse) ;
          }
          // Handle gateway info locally
          else if (theCommandType == kUsbCmdGatewayInfo)
          {
            // Build gateway device info JSON response
            printf("{\"type\":\"gw_info\","
                   "\"version\":\"%s\","
                   "\"build\":\"%s %s\","
                   "\"lora\":%s,"
                   "\"bmp390\":%s,"
                   "\"display\":%s,"
                   "\"connected\":%s,"
                   "\"rx\":%lu,"
                   "\"tx\":%lu,"
                   "\"rssi\":%d,"
                   "\"snr\":%d,"
                   "\"ground_pres\":%.0f,"
                   "\"ground_temp\":%.1f}\n",
                   FIRMWARE_VERSION_STRING,
                   __DATE__, __TIME__,
                   sLoRaOk ? "true" : "false",
                   sBmp390Ok ? "true" : "false",
                   sDisplayOk ? "true" : "false",
                   sGatewayState.pConnected ? "true" : "false",
                   (unsigned long)sGatewayState.pPacketsReceived,
                   (unsigned long)sGatewayState.pPacketsSent,
                   sGatewayState.pLastRssi,
                   sGatewayState.pLastSnr,
                   sGroundPressurePa,
                   sGroundTemperatureC) ;
          }
          // Handle storage read commands (need filename and offset)
          else if ((theCommandType == kUsbCmdSdRead || theCommandType == kUsbCmdFlashRead) && sLoRaOk)
          {
            char theFilename[64] ;
            uint32_t theOffset = 0 ;

            if (GatewayProtocol_ParseStorageParams(sUsbLineBuffer, theFilename, &theOffset))
            {
              uint8_t thePacket[80] ;
              int theLen = GatewayProtocol_BuildStorageReadCommand(
                theCommandType, theFilename, theOffset, thePacket, sizeof(thePacket)) ;

              if (theLen > 0 && LoRa_SendBlocking(&sLoRaRadio, thePacket, theLen, 500))
              {
                sGatewayState.pPacketsSent++ ;
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
              }
              else
              {
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
              }
              LoRa_StartReceive(&sLoRaRadio) ;
            }
            else
            {
              char theResponse[64] ;
              GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
              printf("%s", theResponse) ;
            }
          }
          // Handle storage delete commands (need filename)
          else if ((theCommandType == kUsbCmdSdDelete || theCommandType == kUsbCmdFlashDelete) && sLoRaOk)
          {
            char theFilename[64] ;
            uint32_t theOffset = 0 ;

            if (GatewayProtocol_ParseStorageParams(sUsbLineBuffer, theFilename, &theOffset))
            {
              uint8_t thePacket[72] ;
              int theLen = GatewayProtocol_BuildStorageDeleteCommand(
                theCommandType, theFilename, thePacket, sizeof(thePacket)) ;

              if (theLen > 0 && LoRa_SendBlocking(&sLoRaRadio, thePacket, theLen, 500))
              {
                sGatewayState.pPacketsSent++ ;
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
              }
              else
              {
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
              }
              LoRa_StartReceive(&sLoRaRadio) ;
            }
            else
            {
              char theResponse[64] ;
              GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
              printf("%s", theResponse) ;
            }
          }
          // Forward other commands to flight computer (arm, disarm, reset, sd_list, flash_list, info, etc.)
          else if (sLoRaOk)
          {
            printf("CMD: Forwarding command type %d to flight computer\n", theCommandType) ;
            uint8_t thePacket[8] ;
            int theLen = GatewayProtocol_BuildLoRaCommand(theCommandType, thePacket, sizeof(thePacket)) ;
            printf("CMD: Built LoRa packet, len=%d\n", theLen) ;

            if (theLen > 0)
            {
              printf("CMD: Sending via LoRa...\n") ;
              if (LoRa_SendBlocking(&sLoRaRadio, thePacket, theLen, 500))
              {
                sGatewayState.pPacketsSent++ ;
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, true, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
              }
              else
              {
                char theResponse[64] ;
                GatewayProtocol_BuildAckJson(theCommandId, false, theResponse, sizeof(theResponse)) ;
                printf("%s", theResponse) ;
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
      }
    }
  }

  // Update display at fixed interval
  if ((inCurrentMs - sLastDisplayUpdateMs) < kDisplayUpdateIntervalMs)
  {
    return ;
  }
  sLastDisplayUpdateMs = inCurrentMs ;

  // Update signal strength (always show latest)
  GatewayDisplay_UpdateSignal(sGatewayState.pLastRssi, sGatewayState.pLastSnr) ;

  // Update packet statistics
  GatewayDisplay_UpdatePacketStats(sGatewayState.pPacketsReceived, sGatewayState.pPacketsLost) ;
}
