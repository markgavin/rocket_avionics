//----------------------------------------------
// Module: main.c
// Description: Rocket Avionics Flight Computer
//   Main application entry point
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit Feather RP2040 with RFM95 LoRa 915MHz
//   - BMP390 barometric sensor (STEMMA QT)
//   - Adalogger FeatherWing (RTC + SD card)
//   - FeatherWing OLED 128x64
//----------------------------------------------

#include "pins.h"
#include "version.h"
#include "flight_control.h"
#include "bmp390.h"
#include "lora_radio.h"
#include "ssd1306.h"
#include "status_display.h"
#include "heartbeat_led.h"
#include "rtc_pcf8523.h"
#include "sd_logger.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/watchdog.h"

#include <stdio.h>
#include <string.h>

//----------------------------------------------
// Module Constants
//----------------------------------------------
#define kMainLoopIntervalUs     1000    // 1ms main loop (1 kHz)
#define kButtonDebounceMs       50
#define kStartupDelayMs         1000
#define kSplashDisplayMs        2000

//----------------------------------------------
// Telemetry Sample Buffer
//----------------------------------------------
static TelemetrySample sSampleBuffer[kMaxSamples] ;

//----------------------------------------------
// Module State
//----------------------------------------------
static FlightController sFlightController ;
static BMP390 sBmp390 ;
static LoRa_Radio sLoRaRadio ;

// Hardware status
static bool sBmp390Ok = false ;
static bool sLoRaOk = false ;
static bool sSdOk = false ;
static bool sRtcOk = false ;
static bool sOledOk = false ;

// Timing
static uint32_t sLastSensorReadMs = 0 ;
static uint32_t sLastDisplayUpdateMs = 0 ;
static uint32_t sLastHeartbeatUpdateMs = 0 ;
static uint32_t sLastSdLogMs = 0 ;

// Button state
static bool sButtonAPressed = false ;
static bool sButtonBPressed = false ;
static bool sButtonCPressed = false ;
static uint32_t sButtonADownMs = 0 ;
static uint32_t sButtonBDownMs = 0 ;
static uint32_t sButtonCDownMs = 0 ;

//----------------------------------------------
// Forward Declarations
//----------------------------------------------
static void InitializeHardware(void) ;
static void InitializeI2C(void) ;
static void InitializeSPI(void) ;
static void InitializeButtons(void) ;
static void ProcessButtons(uint32_t inCurrentMs) ;
static void UpdateDisplay(uint32_t inCurrentMs) ;
static void SendTelemetry(uint32_t inCurrentMs) ;
static void LogToSdCard(uint32_t inCurrentMs) ;
static void ProcessLoRaCommands(void) ;

//----------------------------------------------
// Function: main
//----------------------------------------------
int main(void)
{
  // Initialize stdio (USB serial for debug)
  stdio_init_all() ;
  sleep_ms(kStartupDelayMs) ;

  printf("\n\n") ;
  printf("======================================\n") ;
  printf("  ROCKET AVIONICS FLIGHT COMPUTER\n") ;
  printf("  Version %s\n", FIRMWARE_VERSION_STRING) ;
  printf("  Build: %s %s\n", __DATE__, __TIME__) ;
  printf("======================================\n\n") ;

  // Initialize all hardware
  InitializeHardware() ;

  // Initialize flight controller
  printf("Initializing flight controller...\n") ;
  FlightControl_Init(&sFlightController, sSampleBuffer, kMaxSamples) ;
  printf("Flight controller initialized\n") ;

  // Show splash screen
  if (sOledOk)
  {
    StatusDisplay_ShowSplash() ;
    sleep_ms(kSplashDisplayMs) ;

    // Show device info briefly
    StatusDisplay_ShowDeviceInfo(
      FIRMWARE_VERSION_STRING,
      sBmp390Ok,
      sLoRaOk,
      sSdOk,
      sRtcOk) ;
    sleep_ms(kSplashDisplayMs) ;
  }

  // Print startup summary
  printf("\nStartup complete:\n") ;
  printf("  BMP390:  %s\n", sBmp390Ok ? "OK" : "FAIL") ;
  printf("  LoRa:    %s\n", sLoRaOk ? "OK" : "FAIL") ;
  printf("  SD Card: %s\n", sSdOk ? "OK" : "FAIL") ;
  printf("  RTC:     %s\n", sRtcOk ? "OK" : "FAIL") ;
  printf("  OLED:    %s\n", sOledOk ? "OK" : "FAIL") ;
  printf("\nEntering main loop...\n\n") ;

  // Put LoRa in receive mode to listen for commands
  if (sLoRaOk)
  {
    LoRa_StartReceive(&sLoRaRadio) ;
  }

  // Main loop
  uint32_t theLastLoopUs = time_us_32() ;

  while (1)
  {
    uint32_t theCurrentMs = to_ms_since_boot(get_absolute_time()) ;

    //------------------------------------------
    // 1. Read sensors (100 Hz)
    //------------------------------------------
    if ((theCurrentMs - sLastSensorReadMs) >= kSensorSampleIntervalMs)
    {
      sLastSensorReadMs = theCurrentMs ;

      if (sBmp390Ok)
      {
        float thePressure = 0 ;
        float theTemperature = 0 ;

        if (BMP390_ReadPressureTemperature(&sBmp390, &thePressure, &theTemperature))
        {
          // Update flight controller with new sensor data
          FlightControl_UpdateSensors(
            &sFlightController,
            thePressure,
            theTemperature,
            theCurrentMs) ;
        }
      }
    }

    //------------------------------------------
    // 2. Update flight state machine
    //------------------------------------------
    FlightControl_Update(&sFlightController, theCurrentMs) ;

    //------------------------------------------
    // 3. Send LoRa telemetry (10 Hz)
    //------------------------------------------
    if (sLoRaOk && FlightControl_ShouldSendTelemetry(&sFlightController, theCurrentMs))
    {
      SendTelemetry(theCurrentMs) ;
    }

    //------------------------------------------
    // 4. Log to SD card (100 Hz during flight)
    //------------------------------------------
    if (sSdOk)
    {
      LogToSdCard(theCurrentMs) ;
    }

    //------------------------------------------
    // 5. Process incoming LoRa commands
    //------------------------------------------
    if (sLoRaOk)
    {
      ProcessLoRaCommands() ;
    }

    //------------------------------------------
    // 6. Update display (5 Hz)
    //------------------------------------------
    if ((theCurrentMs - sLastDisplayUpdateMs) >= kDisplayUpdateIntervalMs)
    {
      sLastDisplayUpdateMs = theCurrentMs ;
      UpdateDisplay(theCurrentMs) ;
    }

    //------------------------------------------
    // 7. Update heartbeat LED
    //------------------------------------------
    HeartbeatLED_Update(
      FlightControl_GetState(&sFlightController),
      theCurrentMs) ;

    //------------------------------------------
    // 8. Process button inputs
    //------------------------------------------
    ProcessButtons(theCurrentMs) ;

    //------------------------------------------
    // 9. Maintain loop timing
    //------------------------------------------
    uint32_t theLoopTimeUs = time_us_32() - theLastLoopUs ;
    if (theLoopTimeUs < kMainLoopIntervalUs)
    {
      sleep_us(kMainLoopIntervalUs - theLoopTimeUs) ;
    }
    theLastLoopUs = time_us_32() ;
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

  // Initialize buttons
  InitializeButtons() ;

  // Initialize NeoPixel LED
  if (HeartbeatLED_Init(kPinNeoPixel))
  {
    printf("NeoPixel LED initialized\n") ;
  }
  else
  {
    printf("WARNING: NeoPixel LED initialization failed\n") ;
  }

  // Initialize BMP390 barometric sensor
  printf("Initializing BMP390...\n") ;
  if (BMP390_Init(&sBmp390, kI2cAddrBMP390))
  {
    // Configure for high-rate altitude measurement
    BMP390_Configure(
      &sBmp390,
      BMP390_OSR_8X,      // Pressure oversampling
      BMP390_OSR_2X,      // Temperature oversampling
      BMP390_ODR_50_HZ,   // 50 Hz output rate
      BMP390_IIR_COEF_3)  ; // Light filtering
    sBmp390Ok = true ;
    printf("BMP390 initialized\n") ;
  }
  else
  {
    printf("WARNING: BMP390 initialization failed\n") ;
  }

  // Initialize OLED display
  printf("Initializing OLED display...\n") ;
  if (StatusDisplay_Init())
  {
    sOledOk = true ;
    printf("OLED display initialized\n") ;
  }
  else
  {
    printf("WARNING: OLED display initialization failed\n") ;
  }

  // Initialize RTC
  printf("Initializing RTC...\n") ;
  if (RTC_Init())
  {
    sRtcOk = true ;
    printf("RTC initialized\n") ;
  }
  else
  {
    printf("WARNING: RTC initialization failed\n") ;
  }

  // Initialize SD card
  printf("Initializing SD card...\n") ;
  if (SdLogger_Init())
  {
    sSdOk = true ;
    printf("SD card initialized\n") ;
  }
  else
  {
    printf("WARNING: SD card initialization failed\n") ;
  }

  // Initialize LoRa radio
  printf("Initializing LoRa radio...\n") ;
  if (LoRa_Init(&sLoRaRadio))
  {
    // Configure for flight telemetry
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
    printf("WARNING: LoRa radio initialization failed\n") ;
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

  // Initialize SPI1 (shared by LoRa and SD card)
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

  // SD card chip select (shared SPI bus)
  gpio_init(kPinSdCs) ;
  gpio_set_dir(kPinSdCs, GPIO_OUT) ;
  gpio_put(kPinSdCs, 1) ;

  printf("SPI initialized\n") ;
}

//----------------------------------------------
// Function: InitializeButtons
//----------------------------------------------
static void InitializeButtons(void)
{
  printf("Initializing buttons...\n") ;

  // Button A
  gpio_init(kPinButtonA) ;
  gpio_set_dir(kPinButtonA, GPIO_IN) ;
  gpio_pull_up(kPinButtonA) ;

  // Button B
  gpio_init(kPinButtonB) ;
  gpio_set_dir(kPinButtonB, GPIO_IN) ;
  gpio_pull_up(kPinButtonB) ;

  // Button C
  gpio_init(kPinButtonC) ;
  gpio_set_dir(kPinButtonC, GPIO_IN) ;
  gpio_pull_up(kPinButtonC) ;

  printf("Buttons initialized\n") ;
}

//----------------------------------------------
// Function: ProcessButtons
//----------------------------------------------
static void ProcessButtons(uint32_t inCurrentMs)
{
  // Read button states (active low)
  bool theButtonA = !gpio_get(kPinButtonA) ;
  bool theButtonB = !gpio_get(kPinButtonB) ;
  bool theButtonC = !gpio_get(kPinButtonC) ;

  FlightState theState = FlightControl_GetState(&sFlightController) ;

  //------------------------------------------
  // Button A: ARM / DISARM
  //------------------------------------------
  if (theButtonA && !sButtonAPressed)
  {
    sButtonAPressed = true ;
    sButtonADownMs = inCurrentMs ;
  }
  else if (!theButtonA && sButtonAPressed)
  {
    sButtonAPressed = false ;
    uint32_t theHoldTime = inCurrentMs - sButtonADownMs ;

    if (theHoldTime > kButtonDebounceMs)
    {
      if (theState == kFlightIdle)
      {
        // Short press to arm
        printf("Button A: Arming...\n") ;
        FlightError theError = FlightControl_Arm(&sFlightController) ;
        if (theError != kFlightErrorNone)
        {
          printf("Arm failed: error %d\n", theError) ;
          if (sOledOk)
          {
            StatusDisplay_ShowError("ARM FAILED") ;
          }
        }
      }
      else if (theState == kFlightArmed)
      {
        // Short press to disarm
        printf("Button A: Disarming...\n") ;
        FlightControl_Disarm(&sFlightController) ;
      }
    }
  }

  //------------------------------------------
  // Button B: Cycle display mode
  //------------------------------------------
  if (theButtonB && !sButtonBPressed)
  {
    sButtonBPressed = true ;
    sButtonBDownMs = inCurrentMs ;
  }
  else if (!theButtonB && sButtonBPressed)
  {
    sButtonBPressed = false ;
    uint32_t theHoldTime = inCurrentMs - sButtonBDownMs ;

    if (theHoldTime > kButtonDebounceMs && theHoldTime < kButtonLongPressMs)
    {
      // Short press: cycle display mode
      printf("Button B: Cycling display mode...\n") ;
      StatusDisplay_CycleMode() ;
    }
  }

  //------------------------------------------
  // Button C: Reset (long press in COMPLETE state)
  //------------------------------------------
  if (theButtonC && !sButtonCPressed)
  {
    sButtonCPressed = true ;
    sButtonCDownMs = inCurrentMs ;
  }
  else if (!theButtonC && sButtonCPressed)
  {
    sButtonCPressed = false ;
    uint32_t theHoldTime = inCurrentMs - sButtonCDownMs ;

    if (theHoldTime >= kButtonLongPressMs && theState == kFlightComplete)
    {
      // Long press to reset
      printf("Button C: Resetting flight controller...\n") ;
      FlightControl_Reset(&sFlightController) ;
    }
  }
}

//----------------------------------------------
// Function: UpdateDisplay
//----------------------------------------------
static void UpdateDisplay(uint32_t inCurrentMs)
{
  if (!sOledOk) return ;

  FlightState theState = FlightControl_GetState(&sFlightController) ;
  DisplayMode theMode = StatusDisplay_GetMode() ;

  // During active flight, always show flight data
  if (theState >= kFlightBoost && theState <= kFlightDescent)
  {
    StatusDisplay_ShowInFlight(
      theState,
      sFlightController.pCurrentAltitudeM,
      sFlightController.pCurrentVelocityMps,
      sFlightController.pApogeeAltitudeM,
      inCurrentMs - sFlightController.pLaunchTimeMs) ;
    return ;
  }

  // Handle display modes
  switch (theMode)
  {
    case kDisplayModeLive:
      if (theState == kFlightArmed)
      {
        StatusDisplay_ShowArmed(sFlightController.pGroundPressurePa) ;
      }
      else if (theState == kFlightLanded || theState == kFlightComplete)
      {
        StatusDisplay_ShowFlightComplete(&sFlightController.pResults) ;
      }
      else
      {
        StatusDisplay_Update(
          theState,
          sFlightController.pCurrentAltitudeM,
          sFlightController.pCurrentVelocityMps,
          sLoRaOk) ;
      }
      break ;

    case kDisplayModeDeviceInfo:
      StatusDisplay_ShowDeviceInfo(
        FIRMWARE_VERSION_STRING,
        sBmp390Ok,
        sLoRaOk,
        sSdOk,
        sRtcOk) ;
      break ;

    case kDisplayModeFlightStats:
      StatusDisplay_ShowFlightComplete(&sFlightController.pResults) ;
      break ;

    case kDisplayModeLoRaStatus:
      StatusDisplay_ShowLoRaStatus(
        sLoRaOk,
        sLoRaRadio.pLastRssi,
        sLoRaRadio.pPacketsSent,
        sLoRaRadio.pPacketsReceived) ;
      break ;

    case kDisplayModeSensors:
      StatusDisplay_ShowSensorReadings(
        sFlightController.pCurrentPressurePa,
        sFlightController.pCurrentTemperatureC,
        sFlightController.pCurrentAltitudeM) ;
      break ;

    default:
      break ;
  }
}

//----------------------------------------------
// Function: SendTelemetry
//----------------------------------------------
static void SendTelemetry(uint32_t inCurrentMs)
{
  // Build telemetry packet
  LoRaTelemetryPacket thePacket ;
  uint8_t theLen = FlightControl_BuildTelemetryPacket(&sFlightController, &thePacket) ;

  // Send via LoRa (blocking to ensure packet completes)
  if (LoRa_SendBlocking(&sLoRaRadio, (uint8_t *)&thePacket, theLen, 100))
  {
    // Mark telemetry as sent (updates timestamp and sequence number)
    FlightControl_MarkTelemetrySent(&sFlightController, inCurrentMs) ;
  }

  // Return to receive mode for commands
  LoRa_StartReceive(&sLoRaRadio) ;
}

//----------------------------------------------
// Function: LogToSdCard
//----------------------------------------------
static void LogToSdCard(uint32_t inCurrentMs)
{
  FlightState theState = FlightControl_GetState(&sFlightController) ;

  // Start logging when armed
  if (theState == kFlightArmed && sLastSdLogMs == 0)
  {
    uint32_t theTimestamp = sRtcOk ? RTC_GetUnixTimestamp() : inCurrentMs / 1000 ;
    SdLogger_StartFlight(theTimestamp) ;
    sLastSdLogMs = inCurrentMs ;
  }

  // Log samples during flight
  if (theState >= kFlightArmed && theState <= kFlightDescent)
  {
    if ((inCurrentMs - sLastSdLogMs) >= kSdLogIntervalMs)
    {
      sLastSdLogMs = inCurrentMs ;

      SdFlightSample theSample ;
      theSample.timeMs = inCurrentMs - sFlightController.pLaunchTimeMs ;
      theSample.altitudeM = sFlightController.pCurrentAltitudeM ;
      theSample.velocityMps = sFlightController.pCurrentVelocityMps ;
      theSample.pressurePa = sFlightController.pCurrentPressurePa ;
      theSample.temperatureC = sFlightController.pCurrentTemperatureC ;

      SdLogger_LogSample(&theSample) ;
    }
  }

  // End logging when landed
  if (theState == kFlightLanded && sLastSdLogMs > 0)
  {
    const FlightResults * theResults = FlightControl_GetResults(&sFlightController) ;
    SdLogger_EndFlight(
      theResults->pMaxAltitudeM,
      theResults->pFlightTimeMs,
      theResults->pMaxVelocityMps) ;
    sLastSdLogMs = 0 ;
  }
}

//----------------------------------------------
// Function: ProcessLoRaCommands
//----------------------------------------------
static void ProcessLoRaCommands(void)
{
  uint8_t theBuffer[64] ;
  uint8_t theLen = LoRa_Receive(&sLoRaRadio, theBuffer, sizeof(theBuffer)) ;

  if (theLen == 0) return ;

  // Check for valid command packet
  if (theLen < 3 || theBuffer[0] != kLoRaMagic) return ;

  uint8_t thePacketType = theBuffer[1] ;

  if (thePacketType == kLoRaPacketCommand)
  {
    uint8_t theCommand = theBuffer[2] ;

    switch (theCommand)
    {
      case 0x01:  // Arm
        printf("LoRa: Arm command received\n") ;
        FlightControl_Arm(&sFlightController) ;
        break ;

      case 0x02:  // Disarm
        printf("LoRa: Disarm command received\n") ;
        FlightControl_Disarm(&sFlightController) ;
        break ;

      case 0x03:  // Status request
        printf("LoRa: Status request received\n") ;
        // Send status response
        break ;

      case 0x04:  // Reset
        printf("LoRa: Reset command received\n") ;
        FlightControl_Reset(&sFlightController) ;
        break ;

      default:
        printf("LoRa: Unknown command 0x%02X\n", theCommand) ;
        break ;
    }
  }

  // Return to receive mode
  LoRa_StartReceive(&sLoRaRadio) ;
}
