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
//   - LSM6DSOX + LIS3MDL 9-DoF IMU FeatherWing
//   - FeatherWing OLED 128x64
//----------------------------------------------

#include "pins.h"
#include "version.h"
#include "flight_control.h"
#include "flight_storage.h"
#include "storage.h"
#include "bmp390.h"
#include "lora_radio.h"
#include "ssd1306.h"
#include "status_display.h"
#include "heartbeat_led.h"
#include "imu.h"
#include "gps.h"

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
// Debug Configuration
// Set to 0 to disable debug output for production
// (saves power, reduces code size)
//----------------------------------------------
#define kEnableDebugOutput      0

#if kEnableDebugOutput
  #define DEBUG_PRINT(...)  printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(...)  ((void)0)
#endif

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
static Imu sImu ;

// Hardware status
static bool sBmp390Ok = false ;
static bool sLoRaOk = false ;
static bool sImuOk = false ;
static bool sOledOk = false ;
static bool sGpsOk = false ;
static bool sFlashOk = false ;

// Timing
static uint32_t sLastSensorReadMs = 0 ;
static uint32_t sLastImuReadMs = 0 ;
static uint32_t sLastDisplayUpdateMs = 0 ;
static uint32_t sLastLoRaRxMs = 0 ;
static uint32_t sLastLoRaTxMs = 0 ;  // Last successful telemetry TX
static uint32_t sLastFlashLogMs = 0 ;  // Last flash logging time

// Gateway signal quality (from ACK packets)
static int16_t sGatewayRssi = 0 ;
static int8_t sGatewaySnr = 0 ;
static bool sHasAckData = false ;  // Have we received any ACKs?

// Button state
static bool sButtonAPressed = false ;
static bool sButtonBPressed = false ;
static bool sButtonCPressed = false ;
static uint32_t sButtonADownMs = 0 ;
static uint32_t sButtonBDownMs = 0 ;
static uint32_t sButtonCDownMs = 0 ;

// Flight state tracking (for flash storage transitions)
static FlightState sPreviousFlightState = kFlightIdle ;
static uint32_t sCurrentFlightId = 0 ;

// Rocket ID and name (loaded from flash, can be edited via display)
static uint8_t sRocketId = 0 ;
static bool sRocketIdEditing = false ;  // True when editing rocket ID
static char sRocketName[kRocketNameMaxLen] = "" ;  // Custom rocket name

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
  printf("  Build: %s %s\n", kBuildDate, kBuildTime) ;
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
      sImuOk,
      sGpsOk) ;
    sleep_ms(kSplashDisplayMs) ;
  }

  // Print startup summary
  printf("\nStartup complete:\n") ;
  printf("  BMP390:  %s\n", sBmp390Ok ? "OK" : "FAIL") ;
  printf("  LoRa:    %s\n", sLoRaOk ? "OK" : "FAIL") ;
  printf("  IMU:     %s\n", sImuOk ? "OK" : "FAIL") ;
  printf("  OLED:    %s\n", sOledOk ? "OK" : "FAIL") ;
  printf("  GPS:     %s\n", sGpsOk ? "OK" : "FAIL") ;
  printf("  Flash:   %s\n", sFlashOk ? "OK" : "FAIL") ;
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

        // Check data ready before reading
        bool theDataReady = BMP390_DataReady(&sBmp390) ;

        if (BMP390_ReadPressureTemperature(&sBmp390, &thePressure, &theTemperature))
        {
          // Debug: show pressure reading every 100 samples (1 second)
          static uint32_t sDebugCount = 0 ;
          static float sLastPressure = 0 ;
          if ((++sDebugCount % 100) == 0)
          {
            float theDelta = thePressure - sLastPressure ;
            DEBUG_PRINT("BMP: P=%.0f Pa (d=%.1f) T=%.1f C Alt=%.2f m rdy=%d\n",
              thePressure, theDelta, theTemperature,
              sFlightController.pCurrentAltitudeM, theDataReady) ;
            sLastPressure = thePressure ;
          }

          // Update flight controller with new sensor data
          FlightControl_UpdateSensors(
            &sFlightController,
            thePressure,
            theTemperature,
            theCurrentMs) ;
        }
        else
        {
          // Debug: show read failure
          static uint32_t sFailCount = 0 ;
          if ((++sFailCount % 100) == 0)
          {
            DEBUG_PRINT("BMP: Read failed (%lu) rdy=%d\n", (unsigned long)sFailCount, theDataReady) ;
          }
        }
      }

      // Update GPS (reads UART and parses NMEA sentences)
      if (sGpsOk)
      {
        GPS_Update(theCurrentMs) ;
      }
    }

    //------------------------------------------
    // 2. Read IMU (100 Hz)
    //------------------------------------------
    if (sImuOk && (theCurrentMs - sLastImuReadMs) >= kImuSampleIntervalMs)
    {
      sLastImuReadMs = theCurrentMs ;
      IMU_Read(&sImu) ;

      // Update flight controller with IMU data (for future use)
      // FlightControl_UpdateImu(&sFlightController, &sImu.pData) ;
    }

    //------------------------------------------
    // 3. Update flight state machine
    //------------------------------------------
    FlightControl_Update(&sFlightController, theCurrentMs) ;

    // Check for orientation mode timeout (30 seconds)
    FlightControl_CheckOrientationTimeout(&sFlightController, theCurrentMs, 30000) ;

    //------------------------------------------
    // 3a. Handle flight state transitions for flash storage
    //------------------------------------------
    FlightState theCurrentState = FlightControl_GetState(&sFlightController) ;
    if (sFlashOk && theCurrentState != sPreviousFlightState)
    {
      // State changed - check for recording start/stop
      if (theCurrentState == kFlightBoost && sPreviousFlightState == kFlightArmed)
      {
        // Launch detected - start recording
        const GpsData * theGps = sGpsOk ? GPS_GetData() : NULL ;
        int32_t theLaunchLat = (theGps && theGps->pValid) ?
          (int32_t)(theGps->pLatitude * 1000000.0f) : 0 ;
        int32_t theLaunchLon = (theGps && theGps->pValid) ?
          (int32_t)(theGps->pLongitude * 1000000.0f) : 0 ;

        sCurrentFlightId = FlightStorage_StartFlight(
          sFlightController.pGroundPressurePa,
          theLaunchLat,
          theLaunchLon) ;

        if (sCurrentFlightId > 0)
        {
          DEBUG_PRINT("Flash: Started recording flight %lu\n", (unsigned long)sCurrentFlightId) ;
          sLastFlashLogMs = theCurrentMs ;
        }
        else
        {
          DEBUG_PRINT("Flash: Failed to start recording (storage full?)\n") ;
        }
      }
      else if (theCurrentState == kFlightLanded && sPreviousFlightState == kFlightDescent)
      {
        // Landing detected - end recording
        if (FlightStorage_IsRecording())
        {
          bool theSaved = FlightStorage_EndFlight(
            sFlightController.pResults.pMaxAltitudeM,
            sFlightController.pResults.pMaxVelocityMps,
            sFlightController.pResults.pApogeeTimeMs,
            sFlightController.pResults.pFlightTimeMs) ;

          if (theSaved)
          {
            DEBUG_PRINT("Flash: Flight %lu saved (%lu samples)\n",
              (unsigned long)sCurrentFlightId,
              (unsigned long)sFlightController.pSampleCount) ;
          }
          else
          {
            DEBUG_PRINT("Flash: Failed to save flight %lu\n", (unsigned long)sCurrentFlightId) ;
          }
          sCurrentFlightId = 0 ;
        }
      }

      sPreviousFlightState = theCurrentState ;
    }

    //------------------------------------------
    // 3b. Log samples to flash (10 Hz during flight)
    //------------------------------------------
    if (sFlashOk && FlightStorage_IsRecording() &&
        (theCurrentMs - sLastFlashLogMs) >= kTelemetryIntervalMs)
    {
      sLastFlashLogMs = theCurrentMs ;

      // Build sample from current sensor data
      FlightSample theSample ;
      theSample.pTimeMs = theCurrentMs - sFlightController.pLaunchTimeMs ;
      theSample.pAltitudeCm = (int32_t)(sFlightController.pCurrentAltitudeM * 100.0f) ;
      theSample.pVelocityCmps = (int16_t)(sFlightController.pCurrentVelocityMps * 100.0f) ;
      theSample.pPressurePa = (uint32_t)sFlightController.pCurrentPressurePa ;
      theSample.pTemperatureC10 = (int16_t)(sFlightController.pCurrentTemperatureC * 10.0f) ;

      // GPS data
      const GpsData * theGps = sGpsOk ? GPS_GetData() : NULL ;
      if (theGps != NULL && theGps->pValid)
      {
        theSample.pGpsLatitude = (int32_t)(theGps->pLatitude * 1000000.0f) ;
        theSample.pGpsLongitude = (int32_t)(theGps->pLongitude * 1000000.0f) ;
        theSample.pGpsSpeedCmps = (int16_t)(theGps->pSpeedMps * 100.0f) ;
        theSample.pGpsHeadingDeg10 = (uint16_t)(theGps->pHeadingDeg * 10.0f) ;
        theSample.pGpsSatellites = theGps->pSatellites ;
      }
      else
      {
        theSample.pGpsLatitude = 0 ;
        theSample.pGpsLongitude = 0 ;
        theSample.pGpsSpeedCmps = 0 ;
        theSample.pGpsHeadingDeg10 = 0 ;
        theSample.pGpsSatellites = 0 ;
      }

      // IMU data
      const ImuData * theImuData = sImuOk ? IMU_GetData(&sImu) : NULL ;
      if (theImuData != NULL)
      {
        // Accelerometer: convert from g to milli-g
        theSample.pAccelX = (int16_t)(theImuData->pAccelX * 1000.0f) ;
        theSample.pAccelY = (int16_t)(theImuData->pAccelY * 1000.0f) ;
        theSample.pAccelZ = (int16_t)(theImuData->pAccelZ * 1000.0f) ;

        // Gyroscope: convert from dps to 0.1 dps
        theSample.pGyroX = (int16_t)(theImuData->pGyroX * 10.0f) ;
        theSample.pGyroY = (int16_t)(theImuData->pGyroY * 10.0f) ;
        theSample.pGyroZ = (int16_t)(theImuData->pGyroZ * 10.0f) ;

        // Magnetometer: convert from gauss to milligauss
        theSample.pMagX = (int16_t)(theImuData->pMagX * 1000.0f) ;
        theSample.pMagY = (int16_t)(theImuData->pMagY * 1000.0f) ;
        theSample.pMagZ = (int16_t)(theImuData->pMagZ * 1000.0f) ;
      }
      else
      {
        theSample.pAccelX = 0 ;
        theSample.pAccelY = 0 ;
        theSample.pAccelZ = 0 ;
        theSample.pGyroX = 0 ;
        theSample.pGyroY = 0 ;
        theSample.pGyroZ = 0 ;
        theSample.pMagX = 0 ;
        theSample.pMagY = 0 ;
        theSample.pMagZ = 0 ;
      }

      theSample.pState = (uint8_t)theCurrentState ;

      FlightStorage_LogSample(&theSample) ;
    }

    //------------------------------------------
    // 4. Send LoRa telemetry (10 Hz)
    //------------------------------------------
    if (sLoRaOk && FlightControl_ShouldSendTelemetry(&sFlightController, theCurrentMs))
    {
      SendTelemetry(theCurrentMs) ;
    }

    //------------------------------------------
    // 6. Process incoming LoRa commands
    //------------------------------------------
    if (sLoRaOk)
    {
      ProcessLoRaCommands() ;
    }

    //------------------------------------------
    // 7. Update display (5 Hz)
    //------------------------------------------
    if ((theCurrentMs - sLastDisplayUpdateMs) >= kDisplayUpdateIntervalMs)
    {
      sLastDisplayUpdateMs = theCurrentMs ;
      UpdateDisplay(theCurrentMs) ;
    }

    //------------------------------------------
    // 8. Update heartbeat LED
    //------------------------------------------
    HeartbeatLED_Update(
      FlightControl_GetState(&sFlightController),
      theCurrentMs) ;

    //------------------------------------------
    // 9. Process button inputs
    //------------------------------------------
    ProcessButtons(theCurrentMs) ;

    //------------------------------------------
    // 10. Maintain loop timing
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

  // Initialize IMU (LSM6DSOX + LIS3MDL)
  printf("Initializing IMU...\n") ;
  if (IMU_Init(&sImu))
  {
    // Configure IMU with desired ranges
    IMU_Configure(&sImu, kImuAccelRange, kImuGyroRange, kImuMagRange) ;
    sImuOk = true ;
    printf("IMU initialized\n") ;
  }
  else
  {
    printf("WARNING: IMU initialization failed\n") ;
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

  // Initialize flash storage for flight data
  printf("Initializing flash storage...\n") ;
  if (FlightStorage_Init())
  {
    sFlashOk = true ;
    uint8_t theFlightCount = FlightStorage_GetFlightCount() ;
    uint8_t theFreeSlots = FlightStorage_GetFreeSlots() ;
    printf("Flash storage initialized (%u flights stored, %u slots free)\n",
      theFlightCount, theFreeSlots) ;
  }
  else
  {
    printf("WARNING: Flash storage initialization failed\n") ;
  }

  // Load rocket ID and name from settings storage
  Storage_Init() ;
  sRocketId = Storage_LoadRocketId() ;
  Storage_LoadRocketName(sRocketName) ;
  printf("Rocket ID: %u, Name: '%s'\n", sRocketId, sRocketName[0] ? sRocketName : "(none)") ;

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

  //------------------------------------------
  // Button A: Previous display mode
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

    if (theHoldTime > kButtonDebounceMs && theHoldTime < kButtonLongPressMs)
    {
      DEBUG_PRINT("Button A: Previous display mode...\n") ;
      StatusDisplay_PrevMode() ;
    }
  }

  //------------------------------------------
  // Button B: Home button (except on Rocket ID screen where it edits ID)
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
      if (StatusDisplay_GetMode() == kDisplayModeRocketId)
      {
        // On Rocket ID screen: cycle through rocket IDs 0-15
        sRocketId = (sRocketId + 1) % 16 ;
        DEBUG_PRINT("Button B: Rocket ID changed to %u\n", sRocketId) ;

        // Save to flash
        if (Storage_SaveRocketId(sRocketId))
        {
          DEBUG_PRINT("Rocket ID saved to flash\n") ;
        }
        else
        {
          DEBUG_PRINT("Failed to save rocket ID\n") ;
        }

        // Mark as editing (for display feedback)
        sRocketIdEditing = true ;
      }
      else
      {
        // On all other screens: go Home (Live/Idle screen)
        DEBUG_PRINT("Button B: Home - returning to Live screen\n") ;
        StatusDisplay_SetMode(kDisplayModeLive) ;
      }
    }
  }

  //------------------------------------------
  // Button C: Next display mode
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

    if (theHoldTime > kButtonDebounceMs && theHoldTime < kButtonLongPressMs)
    {
      DEBUG_PRINT("Button C: Next display mode...\n") ;
      StatusDisplay_CycleMode() ;
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

  // Get GPS data
  const GpsData * theGps = sGpsOk ? GPS_GetData() : NULL ;
  bool theGpsFix = (theGps != NULL) && theGps->pValid ;
  uint8_t theGpsSatellites = (theGps != NULL) ? theGps->pSatellites : 0 ;

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
        // Use compact display showing altitude, GPS, and gateway status with signal quality
        // Link is active if we're successfully transmitting telemetry
        // (similar to how gateway tracks connection by receiving packets)
        bool theLinkActive = sLoRaOk && (sLastLoRaTxMs > 0) &&
          ((inCurrentMs - sLastLoRaTxMs) < kLoRaTimeoutMs) ;

        // Pass actual RSSI/SNR from ACK packets (0/0 means no ACK data yet)
        // Display code will show "GW: Active" when RSSI=0/SNR=0 but link is active
        StatusDisplay_UpdateCompact(
          theState,
          sFlightController.pOrientationMode,
          sRocketId,
          sFlightController.pCurrentAltitudeM,
          sFlightController.pCurrentVelocityMps,
          sGpsOk,
          theGpsFix,
          theGpsSatellites,
          theLinkActive,
          sGatewayRssi,
          sGatewaySnr) ;
      }
      break ;

    case kDisplayModeDeviceInfo:
      StatusDisplay_ShowDeviceInfo(
        FIRMWARE_VERSION_STRING,
        sBmp390Ok,
        sLoRaOk,
        sImuOk,
        sGpsOk) ;
      break ;

    case kDisplayModeFlightStats:
      StatusDisplay_ShowFlightComplete(&sFlightController.pResults) ;
      break ;

    case kDisplayModeLoRaStatus:
      {
        // Link is connected if we received a packet within timeout
        uint32_t theCurrentMs = to_ms_since_boot(get_absolute_time()) ;
        bool theLinkActive = sLoRaOk && (sLastLoRaRxMs > 0) &&
          ((theCurrentMs - sLastLoRaRxMs) < kLoRaTimeoutMs) ;
        StatusDisplay_ShowLoRaStatus(
          theLinkActive,
          sLoRaRadio.pLastRssi,
          sLoRaRadio.pPacketsSent,
          sLoRaRadio.pPacketsReceived) ;
      }
      break ;

    case kDisplayModeSensors:
      StatusDisplay_ShowSensorReadings(
        sFlightController.pCurrentPressurePa,
        sFlightController.pCurrentTemperatureC,
        sFlightController.pCurrentAltitudeM) ;
      break ;

    case kDisplayModeGpsStatus:
      if (theGps != NULL)
      {
        StatusDisplay_ShowGpsStatus(
          theGpsFix,
          theGpsSatellites,
          theGps->pLatitude,
          theGps->pLongitude,
          theGps->pSpeedMps,
          theGps->pHeadingDeg) ;
      }
      else
      {
        StatusDisplay_ShowGpsStatus(false, 0, 0.0f, 0.0f, 0.0f, 0.0f) ;
      }
      break ;

    case kDisplayModeImu:
      if (sImuOk)
      {
        const ImuData * theImuData = IMU_GetData(&sImu) ;
        StatusDisplay_ShowImu(
          theImuData->pPitchDeg,
          theImuData->pRollDeg,
          theImuData->pAccelX,
          theImuData->pAccelY,
          theImuData->pAccelZ,
          theImuData->pAccelMagnitude,
          theImuData->pGyroX,
          theImuData->pGyroY,
          theImuData->pGyroZ,
          theImuData->pHeadingDeg) ;
      }
      else
      {
        StatusDisplay_ShowError("IMU NOT READY") ;
      }
      break ;

    case kDisplayModeSpin:
      if (sImuOk)
      {
        const ImuData * theImuData = IMU_GetData(&sImu) ;
        StatusDisplay_ShowSpin(theImuData->pGyroY) ;
      }
      else
      {
        StatusDisplay_ShowError("IMU NOT READY") ;
      }
      break ;

    case kDisplayModeCompass:
      if (sImuOk)
      {
        const ImuData * theImuData = IMU_GetData(&sImu) ;
        StatusDisplay_ShowCompass(
          theImuData->pHeadingDeg,
          theImuData->pMagX,
          theImuData->pMagY,
          theImuData->pMagZ) ;
      }
      else
      {
        StatusDisplay_ShowError("IMU NOT READY") ;
      }
      break ;

    case kDisplayModeRates:
      // Show sensor sampling rates
      // Rates derived from pins.h timing constants
      StatusDisplay_ShowRates(
        1000 / kSensorSampleIntervalMs,     // BMP390: 100 Hz
        kImuAccelOdr,                        // Accel: 416 Hz
        kImuGyroOdr,                         // Gyro: 416 Hz
        1,                                   // GPS: 1 Hz (NMEA default)
        1000 / kTelemetryIntervalMs,         // Telemetry: 10 Hz
        1000 / kDisplayUpdateIntervalMs) ;   // Display: 5 Hz
      break ;

    case kDisplayModeRocketId:
      StatusDisplay_ShowRocketId(sRocketId, sRocketName, sRocketIdEditing) ;
      // Reset editing flag after display update
      sRocketIdEditing = false ;
      break ;

    case kDisplayModeAbout:
      StatusDisplay_ShowAbout(
        FIRMWARE_VERSION_STRING,
        kBuildDate,
        kBuildTime) ;
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
  // Build telemetry packet with IMU data
  LoRaTelemetryPacket thePacket ;
  const ImuData * theImuData = sImuOk ? IMU_GetData(&sImu) : NULL ;
  uint8_t theLen = FlightControl_BuildTelemetryPacket(&sFlightController, theImuData, sRocketId, &thePacket) ;

  // Send via LoRa (blocking to ensure packet completes)
  // Timeout increased to 200ms for 42-byte GPS-enabled packet
  if (LoRa_SendBlocking(&sLoRaRadio, (uint8_t *)&thePacket, theLen, 200))
  {
    // Mark telemetry as sent (updates timestamp and sequence number)
    FlightControl_MarkTelemetrySent(&sFlightController, inCurrentMs) ;
    sLastLoRaTxMs = inCurrentMs ;  // Track successful TX for link status
    DEBUG_PRINT("TX: seq=%u len=%u\n", thePacket.pSequence, theLen) ;
  }
  else
  {
    DEBUG_PRINT("TX FAIL: len=%u\n", theLen) ;
  }

  // Return to receive mode for commands
  LoRa_StartReceive(&sLoRaRadio) ;
}

//----------------------------------------------
// Function: SendDeviceInfo
// Purpose: Send device information over LoRa
//----------------------------------------------
static void SendDeviceInfo(void)
{
  // Build info packet with device details
  uint8_t thePacket[128] ;
  int theOffset = 0 ;

  thePacket[theOffset++] = kLoRaMagic ;
  thePacket[theOffset++] = kLoRaPacketInfo ;

  // Firmware version string
  const char * theVersion = FIRMWARE_VERSION_STRING ;
  uint8_t theVersionLen = strlen(theVersion) ;
  thePacket[theOffset++] = theVersionLen ;
  memcpy(&thePacket[theOffset], theVersion, theVersionLen) ;
  theOffset += theVersionLen ;

  // Build date
  char theBuildDateBuf[32] ;
  snprintf(theBuildDateBuf, sizeof(theBuildDateBuf), "%s %s", kBuildDate, kBuildTime) ;
  const char * theBuildDate = theBuildDateBuf ;
  uint8_t theBuildLen = strlen(theBuildDate) ;
  thePacket[theOffset++] = theBuildLen ;
  memcpy(&thePacket[theOffset], theBuildDate, theBuildLen) ;
  theOffset += theBuildLen ;

  // Hardware status flags
  uint8_t theFlags = 0 ;
  if (sBmp390Ok) theFlags |= 0x01 ;
  if (sLoRaOk) theFlags |= 0x02 ;
  if (sImuOk) theFlags |= 0x04 ;
  if (sOledOk) theFlags |= 0x10 ;
  if (sGpsOk) theFlags |= 0x20 ;
  thePacket[theOffset++] = theFlags ;

  // Current flight state
  thePacket[theOffset++] = (uint8_t)sFlightController.pState ;

  // Sample count (current flight)
  thePacket[theOffset++] = (sFlightController.pSampleCount) & 0xFF ;
  thePacket[theOffset++] = (sFlightController.pSampleCount >> 8) & 0xFF ;
  thePacket[theOffset++] = (sFlightController.pSampleCount >> 16) & 0xFF ;
  thePacket[theOffset++] = (sFlightController.pSampleCount >> 24) & 0xFF ;

  // Rocket ID
  thePacket[theOffset++] = sRocketId ;

  // Rocket name (length-prefixed string)
  uint8_t theNameLen = strlen(sRocketName) ;
  thePacket[theOffset++] = theNameLen ;
  if (theNameLen > 0)
  {
    memcpy(&thePacket[theOffset], sRocketName, theNameLen) ;
    theOffset += theNameLen ;
  }

  DEBUG_PRINT("LoRa: Sending device info (%d bytes)\n", theOffset) ;
  LoRa_SendBlocking(&sLoRaRadio, thePacket, theOffset, 500) ;
}

//----------------------------------------------
// Function: SendFlashList
// Purpose: Send list of stored flights over LoRa
//----------------------------------------------
static void SendFlashList(void)
{
  if (!sFlashOk)
  {
    DEBUG_PRINT("Flash: Not initialized\n") ;
    return ;
  }

  // Build flight list packet
  // Format: magic, type, count, then for each flight: slot, flightId, maxAlt, flightTime, sampleCount
  uint8_t thePacket[128] ;
  int theOffset = 0 ;

  thePacket[theOffset++] = kLoRaMagic ;
  thePacket[theOffset++] = kLoRaPacketStorageList ;

  uint8_t theFlightCount = FlightStorage_GetFlightCount() ;
  thePacket[theOffset++] = theFlightCount ;

  // Add summary for each stored flight
  for (uint8_t i = 0 ; i < kMaxStoredFlights && theOffset < 120 ; i++)
  {
    FlightHeader theHeader ;
    if (FlightStorage_GetHeader(i, &theHeader))
    {
      thePacket[theOffset++] = i ;  // Slot index

      // Flight ID (4 bytes)
      thePacket[theOffset++] = theHeader.pFlightId & 0xFF ;
      thePacket[theOffset++] = (theHeader.pFlightId >> 8) & 0xFF ;
      thePacket[theOffset++] = (theHeader.pFlightId >> 16) & 0xFF ;
      thePacket[theOffset++] = (theHeader.pFlightId >> 24) & 0xFF ;

      // Max altitude in cm (4 bytes)
      int32_t theAltCm = (int32_t)(theHeader.pMaxAltitudeM * 100.0f) ;
      thePacket[theOffset++] = theAltCm & 0xFF ;
      thePacket[theOffset++] = (theAltCm >> 8) & 0xFF ;
      thePacket[theOffset++] = (theAltCm >> 16) & 0xFF ;
      thePacket[theOffset++] = (theAltCm >> 24) & 0xFF ;

      // Flight time in ms (4 bytes)
      thePacket[theOffset++] = theHeader.pFlightTimeMs & 0xFF ;
      thePacket[theOffset++] = (theHeader.pFlightTimeMs >> 8) & 0xFF ;
      thePacket[theOffset++] = (theHeader.pFlightTimeMs >> 16) & 0xFF ;
      thePacket[theOffset++] = (theHeader.pFlightTimeMs >> 24) & 0xFF ;

      // Sample count (4 bytes)
      thePacket[theOffset++] = theHeader.pSampleCount & 0xFF ;
      thePacket[theOffset++] = (theHeader.pSampleCount >> 8) & 0xFF ;
      thePacket[theOffset++] = (theHeader.pSampleCount >> 16) & 0xFF ;
      thePacket[theOffset++] = (theHeader.pSampleCount >> 24) & 0xFF ;
    }
  }

  DEBUG_PRINT("LoRa: Sending flash list (%d flights, %d bytes)\n", theFlightCount, theOffset) ;
  LoRa_SendBlocking(&sLoRaRadio, thePacket, theOffset, 500) ;
}

//----------------------------------------------
// Function: SendFlashData
// Purpose: Send flight data chunk over LoRa
// Parameters:
//   inSlotIndex - Flight slot (0-6)
//   inStartSample - Starting sample index
//----------------------------------------------
static void SendFlashData(uint8_t inSlotIndex, uint32_t inStartSample)
{
  if (!sFlashOk)
  {
    DEBUG_PRINT("Flash: Not initialized\n") ;
    return ;
  }

  // Get flight header to check sample count
  FlightHeader theHeader ;
  if (!FlightStorage_GetHeader(inSlotIndex, &theHeader))
  {
    DEBUG_PRINT("Flash: Invalid slot %u\n", inSlotIndex) ;
    return ;
  }

  // Build data packet
  // Format: magic, type, slot, startSample, sampleCount, then sample data
  // LoRa packet limit is ~255 bytes, leave room for header
  // FlightSample is 52 bytes, so we can fit ~3 samples per packet
  #define kSamplesPerPacket 3
  uint8_t thePacket[200] ;
  int theOffset = 0 ;

  thePacket[theOffset++] = kLoRaMagic ;
  thePacket[theOffset++] = kLoRaPacketStorageData ;
  thePacket[theOffset++] = inSlotIndex ;

  // Start sample (4 bytes)
  thePacket[theOffset++] = inStartSample & 0xFF ;
  thePacket[theOffset++] = (inStartSample >> 8) & 0xFF ;
  thePacket[theOffset++] = (inStartSample >> 16) & 0xFF ;
  thePacket[theOffset++] = (inStartSample >> 24) & 0xFF ;

  // Total samples in flight (4 bytes)
  thePacket[theOffset++] = theHeader.pSampleCount & 0xFF ;
  thePacket[theOffset++] = (theHeader.pSampleCount >> 8) & 0xFF ;
  thePacket[theOffset++] = (theHeader.pSampleCount >> 16) & 0xFF ;
  thePacket[theOffset++] = (theHeader.pSampleCount >> 24) & 0xFF ;

  // Calculate how many samples to send
  uint32_t theSamplesRemaining = 0 ;
  if (inStartSample < theHeader.pSampleCount)
  {
    theSamplesRemaining = theHeader.pSampleCount - inStartSample ;
  }
  uint8_t theSamplesToSend = (theSamplesRemaining > kSamplesPerPacket) ?
    kSamplesPerPacket : (uint8_t)theSamplesRemaining ;

  thePacket[theOffset++] = theSamplesToSend ;

  // Add sample data
  for (uint8_t i = 0 ; i < theSamplesToSend ; i++)
  {
    FlightSample theSample ;
    if (FlightStorage_GetSample(inSlotIndex, inStartSample + i, &theSample))
    {
      // Copy sample data directly (packed structure)
      memcpy(&thePacket[theOffset], &theSample, sizeof(FlightSample)) ;
      theOffset += sizeof(FlightSample) ;
    }
  }

  DEBUG_PRINT("LoRa: Sending flash data slot=%u start=%lu count=%u (%d bytes)\n",
    inSlotIndex, (unsigned long)inStartSample, theSamplesToSend, theOffset) ;
  LoRa_SendBlocking(&sLoRaRadio, thePacket, theOffset, 500) ;
}

//----------------------------------------------
// Function: SendFlashHeader
// Purpose: Send flight header over LoRa
// Parameters:
//   inSlotIndex - Flight slot (0-6)
//----------------------------------------------
static void SendFlashHeader(uint8_t inSlotIndex)
{
  if (!sFlashOk)
  {
    DEBUG_PRINT("Flash: Not initialized\n") ;
    return ;
  }

  FlightHeader theHeader ;
  if (!FlightStorage_GetHeader(inSlotIndex, &theHeader))
  {
    DEBUG_PRINT("Flash: Invalid slot %u\n", inSlotIndex) ;
    return ;
  }

  // Build header packet
  uint8_t thePacket[100] ;
  int theOffset = 0 ;

  thePacket[theOffset++] = kLoRaMagic ;
  thePacket[theOffset++] = kLoRaPacketStorageData ;
  thePacket[theOffset++] = inSlotIndex ;
  thePacket[theOffset++] = 0xFF ;  // Marker for header packet (startSample = 0xFFFFFFFF)
  thePacket[theOffset++] = 0xFF ;
  thePacket[theOffset++] = 0xFF ;
  thePacket[theOffset++] = 0xFF ;

  // Copy header data
  memcpy(&thePacket[theOffset], &theHeader, sizeof(FlightHeader)) ;
  theOffset += sizeof(FlightHeader) ;

  DEBUG_PRINT("LoRa: Sending flash header slot=%u (%d bytes)\n", inSlotIndex, theOffset) ;
  LoRa_SendBlocking(&sLoRaRadio, thePacket, theOffset, 500) ;
}

//----------------------------------------------
// Function: ProcessLoRaCommands
//----------------------------------------------
static void ProcessLoRaCommands(void)
{
  uint8_t theBuffer[128] ;
  uint8_t theLen = LoRa_Receive(&sLoRaRadio, theBuffer, sizeof(theBuffer)) ;

  if (theLen == 0) return ;

  // Debug: show any received packet
  DEBUG_PRINT("RX: len=%u magic=0x%02X type=0x%02X\n", theLen, theBuffer[0], theLen > 1 ? theBuffer[1] : 0) ;

  // Check for valid command packet
  if (theLen < 3 || theBuffer[0] != kLoRaMagic)
  {
    DEBUG_PRINT("RX: Invalid packet\n") ;
    return ;
  }

  uint8_t thePacketType = theBuffer[1] ;

  // Update last receive time for link status
  sLastLoRaRxMs = to_ms_since_boot(get_absolute_time()) ;

  // Handle ACK packets from gateway (contains signal quality info)
  if (thePacketType == kLoRaPacketAck && theLen >= 5)
  {
    // Extract RSSI and SNR from ACK packet
    // These represent how well the gateway received our telemetry
    sGatewayRssi = (int16_t)(theBuffer[2] | (theBuffer[3] << 8)) ;
    sGatewaySnr = (int8_t)theBuffer[4] ;
    sHasAckData = true ;  // We have valid signal quality data from gateway
    DEBUG_PRINT("ACK: RSSI=%d SNR=%d\n", sGatewayRssi, sGatewaySnr) ;
  }
  else if (thePacketType == kLoRaPacketCommand)
  {
    // Command format: magic, type, targetRocketId, commandId, ...params
    if (theLen < 4)
    {
      DEBUG_PRINT("LoRa: Command packet too short\n") ;
      return ;
    }

    uint8_t theTargetId = theBuffer[2] ;
    uint8_t theCommand = theBuffer[3] ;

    // Check if this command is for us (or broadcast 0xFF)
    if (theTargetId != sRocketId && theTargetId != 0xFF)
    {
      DEBUG_PRINT("LoRa: Command for rocket %u (we are %u), ignoring\n", theTargetId, sRocketId) ;
      LoRa_StartReceive(&sLoRaRadio) ;
      return ;
    }

    DEBUG_PRINT("LoRa: Command 0x%02X for rocket %u\n", theCommand, theTargetId) ;

    switch (theCommand)
    {
      case kCmdArm:
        DEBUG_PRINT("LoRa: Arm command received\n") ;
        FlightControl_Arm(&sFlightController) ;
        break ;

      case kCmdDisarm:
        DEBUG_PRINT("LoRa: Disarm command received\n") ;
        FlightControl_Disarm(&sFlightController) ;
        break ;

      case kCmdStatus:
        DEBUG_PRINT("LoRa: Status request received\n") ;
        // Send status response
        break ;

      case kCmdReset:
        DEBUG_PRINT("LoRa: Reset command received\n") ;
        FlightControl_Reset(&sFlightController) ;
        break ;

      case kCmdInfo:
        DEBUG_PRINT("LoRa: Info request received\n") ;
        SendDeviceInfo() ;
        break ;

      case kCmdOrientationMode:
        {
          bool theEnabled = (theLen > 4) ? (theBuffer[4] != 0) : false ;
          DEBUG_PRINT("LoRa: Orientation mode %s\n", theEnabled ? "enabled" : "disabled") ;
          FlightControl_SetOrientationMode(&sFlightController, theEnabled) ;
        }
        break ;

      case kCmdSetRocketName:
        {
          // Format: magic, type, targetId, cmd, name bytes (null-terminated)
          if (theLen > 4)
          {
            // Extract name from packet (starts at byte 4)
            char theName[kRocketNameMaxLen] ;
            size_t theNameLen = theLen - 4 ;
            if (theNameLen > kRocketNameMaxLen - 1)
            {
              theNameLen = kRocketNameMaxLen - 1 ;
            }
            memcpy(theName, &theBuffer[4], theNameLen) ;
            theName[theNameLen] = '\0' ;

            DEBUG_PRINT("LoRa: Set rocket name '%s'\n", theName) ;
            if (Storage_SaveRocketName(theName))
            {
              // Reload name into static variable
              Storage_LoadRocketName(sRocketName) ;
            }
          }
        }
        break ;

      case kCmdFlashList:
        DEBUG_PRINT("LoRa: Flash list request\n") ;
        SendFlashList() ;
        break ;

      case kCmdFlashRead:
        {
          // Format: magic, type, targetId, cmd, slot, startSample (4 bytes)
          // If startSample == 0xFFFFFFFF, send header instead
          if (theLen >= 8)
          {
            uint8_t theSlot = theBuffer[4] ;
            uint32_t theStartSample = theBuffer[5] |
              (theBuffer[6] << 8) |
              (theBuffer[7] << 16) |
              ((theLen >= 9) ? (theBuffer[8] << 24) : 0) ;

            DEBUG_PRINT("LoRa: Flash read slot=%u start=%lu\n",
              theSlot, (unsigned long)theStartSample) ;

            if (theStartSample == 0xFFFFFFFF)
            {
              // Request for header
              SendFlashHeader(theSlot) ;
            }
            else
            {
              // Request for sample data
              SendFlashData(theSlot, theStartSample) ;
            }
          }
          else
          {
            DEBUG_PRINT("LoRa: Invalid flash read request (len=%u)\n", theLen) ;
          }
        }
        break ;

      case kCmdFlashDelete:
        {
          if (theLen >= 5)
          {
            uint8_t theSlot = theBuffer[4] ;
            DEBUG_PRINT("LoRa: Flash delete slot=%u\n", theSlot) ;

            if (theSlot == 0xFF)
            {
              // Delete all flights
              uint8_t theDeleted = FlightStorage_DeleteAllFlights() ;
              DEBUG_PRINT("Flash: Deleted %u flights\n", theDeleted) ;
            }
            else
            {
              // Delete specific slot
              if (FlightStorage_DeleteFlight(theSlot))
              {
                DEBUG_PRINT("Flash: Deleted slot %u\n", theSlot) ;
              }
              else
              {
                DEBUG_PRINT("Flash: Failed to delete slot %u\n", theSlot) ;
              }
            }
          }
        }
        break ;

      default:
        DEBUG_PRINT("LoRa: Unknown command 0x%02X\n", theCommand) ;
        break ;
    }
  }

  // Return to receive mode
  LoRa_StartReceive(&sLoRaRadio) ;
}
