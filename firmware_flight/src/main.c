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
#include "bmp581.h"
#include "lora_radio.h"
#ifdef DISPLAY_EINK
#include "uc8151d.h"
#include "framebuffer.h"
#else
#include "ssd1306.h"
#endif
#include "status_display.h"
#include "heartbeat_led.h"
#include "imu.h"
#include "gps.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/watchdog.h"
#include "hardware/adc.h"

#include <stdio.h>
#include <string.h>

// Eliminate printf at compile time. Both USB and UART stdio are
// disabled in CMakeLists.txt, but printf still acquires internal
// mutexes and runs formatting code. This macro removes all overhead.
// snprintf/sprintf are NOT affected (used for display formatting).
#define printf(...) ((void)0)

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
static BMP581 sBmp581 ;
static LoRa_Radio sLoRaRadio ;
static Imu sImu ;

// Hardware status
static bool sI2cBusOk = false ;
static bool sBmp390Ok = false ;
static bool sBmp581Ok = false ;
static bool sLoRaOk = false ;
static bool sImuOk = false ;
static bool sDisplayOk = false ;
static bool sGpsOk = false ;
static bool sFlashOk = false ;

// Timing
static uint32_t sLastSensorReadMs = 0 ;
static uint32_t sLastImuReadMs = 0 ;
#ifndef DISPLAY_EINK
static uint32_t sLastDisplayUpdateMs = 0 ;
#endif
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

// Baro comparison streaming
static bool sBaroCompareEnabled = false ;
static uint32_t sLastBaroCompareMs = 0 ;
static char sRocketName[kRocketNameMaxLen] = "" ;  // Custom rocket name

//----------------------------------------------
// Forward Declarations
//----------------------------------------------
static void InitializeHardware(void) ;
static void InitializeI2C(void) ;
static void InitializeSPI(void) ;
#ifndef DISPLAY_EINK
static void InitializeButtons(void) ;
#endif
static void ProcessButtons(uint32_t inCurrentMs) ;
#ifndef DISPLAY_EINK
static void UpdateDisplay(uint32_t inCurrentMs) ;
#endif
static void SendTelemetry(uint32_t inCurrentMs) ;
static void SendBaroCompare(void) ;
static void ProcessLoRaCommands(void) ;

//----------------------------------------------
// Core1 Display Data (shared between cores)
// Lock-free design using volatile flags.
// RP2040 Cortex-M0+ has no cache and no out-of-order
// execution, so volatile alone guarantees coherence.
//----------------------------------------------
#ifdef DISPLAY_EINK

typedef struct {
  // Flight state
  FlightState pState ;
  bool pOrientationMode ;
  uint8_t pRocketId ;
  char pRocketName[16] ;
  bool pRocketIdEditing ;

  // Telemetry values
  float pAltitudeM ;
  float pVelocityMps ;
  float pAccelMagnitude ;
  float pPressurePa ;
  float pTemperatureC ;

  // Pyro continuity (ADC voltage 0-3.3V)
  float pPyro1Voltage ;
  float pPyro2Voltage ;

  // GPS
  bool pGpsOk ;
  bool pGpsFix ;
  uint8_t pGpsSatellites ;
  float pGpsLatitude ;
  float pGpsLongitude ;
  float pGpsSpeedMps ;
  float pGpsHeadingDeg ;

  // LoRa
  bool pLoRaOk ;
  bool pLoRaConnected ;
  int16_t pRssi ;
  int8_t pSnr ;
  int16_t pLastRssi ;
  uint32_t pPacketsSent ;
  uint32_t pPacketsReceived ;

  // Device status
  bool pBaroOk ;
  bool pImuOk ;
  bool pGpsStatus ;
  const char * pBaroType ;

  // Flight results
  FlightResults pResults ;
  float pGroundPressurePa ;

  // Timing
  uint32_t pCurrentMs ;
  uint32_t pLastLoRaTxMs ;
  uint32_t pLastLoRaRxMs ;
} DisplaySharedData ;

// Core0 → Core1: bulk telemetry data (core0 writes, core1 reads)
// volatile ensures compiler doesn't cache across cores
static volatile DisplaySharedData sDisplayShared ;

// Core1 → Core0: debug monitoring
static volatile uint32_t sCore1Iterations = 0 ;
static volatile float sCore1Altitude = -999.0f ;  // Altitude core1 received

// Core0 → Core1: button events (individual atomic flags)
static volatile bool sBtnCycleMode = false ;
static volatile bool sBtnHome = false ;
static volatile bool sBtnEditRocket = false ;

// Core1 → Core0: current display mode (atomic word read)
static volatile DisplayMode sCore1CurrentMode = kDisplayModeLive ;

// Core1 display loop — runs independently, can block freely
static void Core1_DisplayLoop(void) ;

#endif // DISPLAY_EINK

//----------------------------------------------
// Function: main
//----------------------------------------------
int main(void)
{
  // Initialize stdio (USB serial for debug)
  stdio_init_all() ;
  sleep_ms(kStartupDelayMs) ;

  // Check if last reboot was caused by the watchdog
  bool theWatchdogReboot = watchdog_caused_reboot() ;

  printf("\n\n") ;
  printf("======================================\n") ;
  printf("  ROCKET AVIONICS FLIGHT COMPUTER\n") ;
  printf("  Version %s\n", FIRMWARE_VERSION_STRING) ;
  printf("  Build: %s %s\n", kBuildDate, kBuildTime) ;
  if (theWatchdogReboot)
    printf("  *** WATCHDOG REBOOT ***\n") ;
  printf("======================================\n\n") ;

  // Initialize all hardware
  InitializeHardware() ;

  // If watchdog caused this reboot, flash red 3 times as warning
  if (theWatchdogReboot)
  {
    for (int i = 0 ; i < 3 ; i++)
    {
      HeartbeatLED_SetColor(kLedColorRed) ;
      sleep_ms(200) ;
      HeartbeatLED_Off() ;
      sleep_ms(200) ;
    }
  }

  // Initialize flight controller
  printf("Initializing flight controller...\n") ;
  FlightControl_Init(&sFlightController, sSampleBuffer, kMaxSamples) ;
  printf("Flight controller initialized\n") ;

  // Show splash screen
  if (sDisplayOk)
  {
    StatusDisplay_ShowSplash() ;
    sleep_ms(kSplashDisplayMs) ;

    // Show device info briefly
    const char * theBaroType = sBmp390Ok ? "BMP390" :
      (sBmp581Ok ? "BMP581" : (sI2cBusOk ? "None" : "I2C FAIL")) ;
    StatusDisplay_ShowDeviceInfo(
      FIRMWARE_VERSION_STRING,
      theBaroType,
      sBmp581Ok || sBmp390Ok,
      sLoRaOk,
      sImuOk,
      sGpsOk) ;
    sleep_ms(kSplashDisplayMs) ;

    sleep_ms(kSplashDisplayMs) ;
  }

  // Print startup summary
  printf("\nStartup complete:\n") ;
  if (sBmp390Ok)
    printf("  Baro:    BMP390 (primary)\n") ;
  if (sBmp581Ok)
    printf("  Baro:    BMP581 (%s)\n", sBmp390Ok ? "secondary" : "primary") ;
  if (!sBmp390Ok && !sBmp581Ok)
    printf("  Baro:    NONE (FAIL)\n") ;
  printf("  LoRa:    %s\n", sLoRaOk ? "OK" : "FAIL") ;
  printf("  IMU:     %s\n", sImuOk ? "OK" : "FAIL") ;
  printf("  Display: %s\n", sDisplayOk ? "OK" : "FAIL") ;
  printf("  GPS:     %s\n", sGpsOk ? "OK" : "FAIL") ;
  printf("  Flash:   %s\n", sFlashOk ? "OK" : "FAIL") ;
  printf("\nEntering main loop...\n\n") ;

#ifdef DISPLAY_EINK
  // Launch core1 for eInk display updates.
  // All display I/O moves to core1 so it can block
  // on eInk refresh without affecting LoRa on core0.
  if (sDisplayOk)
  {
    memset((void *)&sDisplayShared, 0, sizeof(sDisplayShared)) ;
    sDisplayShared.pBaroType = sBmp390Ok ? "BMP390" :
      (sBmp581Ok ? "BMP581" : (sI2cBusOk ? "None" : "I2C FAIL")) ;
    sDisplayShared.pBaroOk = sBmp390Ok || sBmp581Ok ;
    sDisplayShared.pLoRaOk = sLoRaOk ;
    sDisplayShared.pImuOk = sImuOk ;
    sDisplayShared.pGpsStatus = sGpsOk ;
    printf("Launching Core1 for eInk display...\n") ;
    multicore_launch_core1(Core1_DisplayLoop) ;
    printf("Core1 launched OK\n") ;
  }
#endif

  printf("Starting main loop...\n") ;

  // Put LoRa in receive mode to listen for commands
  if (sLoRaOk)
  {
    LoRa_StartReceive(&sLoRaRadio) ;
  }

  // Hardware watchdog — temporarily disabled while debugging GPS
  // watchdog_enable(8000, true) ;

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

      // Read barometers
      float thePressure = 0 ;
      float theTemperature = 0 ;
      bool theReadOk = false ;

      // Read BMP390 (primary when available)
      float the390Pressure = 0 ;
      float the390Temperature = 0 ;
      bool the390Ok = false ;
      if (sBmp390Ok)
      {
        the390Ok = BMP390_ReadPressureTemperature(&sBmp390, &the390Pressure, &the390Temperature) ;
      }

      // Read BMP581
      float the581Pressure = 0 ;
      float the581Temperature = 0 ;
      bool the581Ok = false ;
      if (sBmp581Ok)
      {
        the581Ok = BMP581_ReadPressureTemperature(&sBmp581, &the581Pressure, &the581Temperature) ;
      }

      // Use BMP390 as primary if available, else BMP581
      if (the390Ok)
      {
        thePressure = the390Pressure ;
        theTemperature = the390Temperature ;
        theReadOk = true ;
      }
      else if (the581Ok)
      {
        thePressure = the581Pressure ;
        theTemperature = the581Temperature ;
        theReadOk = true ;
      }

      if (theReadOk)
      {
        FlightControl_UpdateSensors(
          &sFlightController,
          thePressure,
          theTemperature,
          theCurrentMs) ;
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

      // Only feed complementary filter when IMU read succeeds.
      // On I2C failure, skip update to avoid stale data corruption.
      if (IMU_Read(&sImu))
      {
        FlightControl_UpdateImu(&sFlightController, &sImu.pData, theCurrentMs) ;
      }
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
    if (theCurrentState != sPreviousFlightState)
    {
      // Log all state transitions
      char theBuf[48] ;
      snprintf(theBuf, sizeof(theBuf), "MAIN STATE: %s -> %s",
        FlightControl_GetStateName(sPreviousFlightState),
        FlightControl_GetStateName(theCurrentState)) ;
      puts(theBuf) ;
    }
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

    // Auto-recover from spurious flight detection.
    // If state reached LANDED/COMPLETE but max altitude < 20m,
    // no real flight occurred — reset to IDLE.
    // Threshold above launch detection (10m) so phantom flights are caught.
    if ((theCurrentState == kFlightLanded || theCurrentState == kFlightComplete) &&
        sFlightController.pResults.pMaxAltitudeM < 20.0f)
    {
      puts("*** AUTO-RESET: spurious flight (maxAlt < 20m) ***") ;
      FlightControl_Reset(&sFlightController) ;
      sPreviousFlightState = kFlightIdle ;
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
    // 4. Send LoRa telemetry (10 Hz) — PRIORITY
    //------------------------------------------
    if (sLoRaOk && FlightControl_ShouldSendTelemetry(&sFlightController, theCurrentMs))
    {
      SendTelemetry(theCurrentMs) ;
    }

    //------------------------------------------
    // 5. Send baro comparison (1 Hz when enabled)
    //------------------------------------------
    if (sLoRaOk && sBaroCompareEnabled && sBmp390Ok && sBmp581Ok &&
        (theCurrentMs - sLastBaroCompareMs) >= 1000)
    {
      sLastBaroCompareMs = theCurrentMs ;
      SendBaroCompare() ;
    }

    //------------------------------------------
    // 6. Process incoming LoRa commands
    //------------------------------------------
    if (sLoRaOk)
    {
      ProcessLoRaCommands() ;
    }

    //------------------------------------------
    // 7. Update display
    //------------------------------------------
#ifdef DISPLAY_EINK
    // eInk: populate shared data for core1 (lock-free)
    // Core0 writes unconditionally; core1 reads when ready.
    // Torn reads are harmless for display purposes.
    if (sDisplayOk)
    {
      const GpsData * theGps = sGpsOk ? GPS_GetData() : NULL ;

      sDisplayShared.pState = FlightControl_GetState(&sFlightController) ;
      sDisplayShared.pOrientationMode = sFlightController.pOrientationMode ;
      sDisplayShared.pRocketId = sRocketId ;
      memcpy((void *)sDisplayShared.pRocketName, sRocketName, sizeof(sDisplayShared.pRocketName) - 1) ;
      sDisplayShared.pRocketIdEditing = sRocketIdEditing ;

      sDisplayShared.pAltitudeM = sFlightController.pCurrentAltitudeM ;
      sDisplayShared.pVelocityMps = sFlightController.pCurrentVelocityMps ;
      sDisplayShared.pPressurePa = sFlightController.pCurrentPressurePa ;
      sDisplayShared.pTemperatureC = sFlightController.pCurrentTemperatureC ;
      sDisplayShared.pGroundPressurePa = sFlightController.pGroundPressurePa ;
      sDisplayShared.pResults = sFlightController.pResults ;

      // IMU acceleration magnitude
      const ImuData * theImuForDisplay = sImuOk ? IMU_GetData(&sImu) : NULL ;
      sDisplayShared.pAccelMagnitude = theImuForDisplay ? theImuForDisplay->pAccelMagnitude : 0.0f ;

      // Pyro continuity ADC (GP26=ADC0, GP27=ADC1)
      adc_select_input(0) ;
      sDisplayShared.pPyro1Voltage = adc_read() * 3.3f / 4095.0f ;
      adc_select_input(1) ;
      sDisplayShared.pPyro2Voltage = adc_read() * 3.3f / 4095.0f ;

      sDisplayShared.pGpsOk = sGpsOk ;
      sDisplayShared.pGpsFix = (theGps != NULL) && theGps->pValid ;
      sDisplayShared.pGpsSatellites = (theGps != NULL) ? theGps->pSatellites : 0 ;
      sDisplayShared.pGpsLatitude = (theGps != NULL) ? theGps->pLatitude : 0.0f ;
      sDisplayShared.pGpsLongitude = (theGps != NULL) ? theGps->pLongitude : 0.0f ;
      sDisplayShared.pGpsSpeedMps = (theGps != NULL) ? theGps->pSpeedMps : 0.0f ;
      sDisplayShared.pGpsHeadingDeg = (theGps != NULL) ? theGps->pHeadingDeg : 0.0f ;

      sDisplayShared.pLoRaOk = sLoRaOk ;
      sDisplayShared.pLoRaConnected = sLoRaOk && (sLastLoRaRxMs > 0) &&
        ((theCurrentMs - sLastLoRaRxMs) < kLoRaTimeoutMs) ;

      sDisplayShared.pRssi = sGatewayRssi ;
      sDisplayShared.pSnr = sGatewaySnr ;
      sDisplayShared.pLastRssi = sLoRaRadio.pLastRssi ;
      sDisplayShared.pPacketsSent = sLoRaRadio.pPacketsSent ;
      sDisplayShared.pPacketsReceived = sLoRaRadio.pPacketsReceived ;
      sDisplayShared.pLastLoRaTxMs = sLastLoRaTxMs ;
      sDisplayShared.pLastLoRaRxMs = sLastLoRaRxMs ;
      sDisplayShared.pCurrentMs = theCurrentMs ;

      sRocketIdEditing = false ;

      // Heartbeat — once per minute, minimal output for long-term monitoring
      static uint32_t sLastHeartbeatMs = 0 ;
      if ((theCurrentMs - sLastHeartbeatMs) >= 60000)
      {
        sLastHeartbeatMs = theCurrentMs ;
        char theBuf[64] ;
        snprintf(theBuf, sizeof(theBuf), "st=%d min=%lu disp=%d alt=%.1f",
                 (int)FlightControl_GetState(&sFlightController),
                 (unsigned long)(theCurrentMs / 60000),
                 (int)sCore1CurrentMode,
                 (double)sFlightController.pCurrentAltitudeM) ;
        puts(theBuf) ;
      }
    }
#else
    // OLED: direct update on core0
    if ((theCurrentMs - sLastDisplayUpdateMs) >= kDisplayUpdateIntervalMs)
    {
      sLastDisplayUpdateMs = theCurrentMs ;
      UpdateDisplay(theCurrentMs) ;
    }
#endif


    //------------------------------------------
    // 8. Update heartbeat LED
    //------------------------------------------
    HeartbeatLED_Update(
      FlightControl_GetState(&sFlightController),
      theCurrentMs) ;


    //------------------------------------------
    // 9. Process button inputs
    // Skipped in eInk builds: GP9/GP6/GP5 are eInk SPI
    // chip-select lines, not buttons. Core1 toggling CS
    // during SPI transfers causes phantom button presses
    // that cycle the display mode to FlightStats.
    //------------------------------------------
#ifndef DISPLAY_EINK
    ProcessButtons(theCurrentMs) ;
#endif

    //------------------------------------------
    // 10. Feed watchdog and maintain loop timing
    //------------------------------------------
    watchdog_update() ;

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

#ifndef DISPLAY_EINK
  // OLED FeatherWing buttons (GP9/GP6/GP5).
  // Skipped in eInk builds — those pins are used by Feather Friend
  // for ECS/SRCS/SDCS, and no physical buttons exist on eInk boards.
  InitializeButtons() ;
#endif

  // Initialize ADC for pyro continuity readings (GP26=ADC0, GP27=ADC1)
  adc_init() ;
  adc_gpio_init(kPinPyro1Continuity) ;  // GP26 → ADC0
  adc_gpio_init(kPinPyro2Continuity) ;  // GP27 → ADC1

  // Initialize NeoPixel LED
  HeartbeatLED_Init(kPinNeoPixel) ;

  // Initialize I2C sensors (only if bus test passed)
  if (sI2cBusOk)
  {
    // Initialize barometric sensors (try both for comparison)
    if (BMP390_Init(&sBmp390, kI2cAddrBMP390))
    {
      BMP390_Configure(
        &sBmp390,
        BMP390_OSR_8X,      // Pressure oversampling
        BMP390_OSR_2X,      // Temperature oversampling
        BMP390_ODR_50_HZ,   // 50 Hz output rate
        BMP390_IIR_COEF_3)  ; // Light filtering
      sBmp390Ok = true ;
    }

    // Try BMP581 (always, for comparison when both present)
    if (BMP581_Init(&sBmp581, BMP581_I2C_ADDR_DEFAULT))
    {
      sBmp581Ok = true ;
    }
    else if (BMP581_Init(&sBmp581, BMP581_I2C_ADDR_ALT))
    {
      sBmp581Ok = true ;
    }
  }

  // Initialize display
#ifdef DISPLAY_EINK
  printf("Initializing eInk display...\n") ;
  if (StatusDisplay_Init())
  {
    sDisplayOk = true ;
    printf("eInk display initialized\n") ;
  }
  else
  {
    printf("WARNING: eInk display initialization failed\n") ;
  }
#else
  printf("Initializing OLED display...\n") ;
  if (StatusDisplay_Init())
  {
    sDisplayOk = true ;
    printf("OLED display initialized\n") ;
  }
  else
  {
    printf("WARNING: OLED display initialization failed\n") ;
  }
#endif

  // Initialize IMU (only if I2C bus is healthy)
  if (sI2cBusOk && IMU_Init(&sImu))
  {
    IMU_Configure(&sImu, kImuAccelRange, kImuGyroRange, kImuMagRange) ;
    sImuOk = true ;
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
  if (GPS_Init())
  {
    sGpsOk = true ;
  }

  // Initialize flash storage for flight data
  if (FlightStorage_Init())
  {
    sFlashOk = true ;
  }

  // Load rocket ID and name from settings storage
  Storage_Init() ;
  sRocketId = Storage_LoadRocketId() ;
  Storage_LoadRocketName(sRocketName) ;

  printf("Rocket ID: %u, Name: '%s'\n", sRocketId, sRocketName[0] ? sRocketName : "(none)") ;

  printf("Hardware initialization complete\n\n") ;
}

//----------------------------------------------
// Function: TestI2CBus
// Purpose: Test I2C cable/bus health before init.
//   Detects: SDA/SCL stuck low, SDA-SCL short,
//   missing pull-ups (broken/disconnected cable).
//   Uses raw GPIO — must be called BEFORE i2c_init().
// Returns: true if bus is healthy.
//----------------------------------------------
static bool TestI2CBus(void)
{
  // Configure both pins as GPIO inputs with internal pull-ups
  gpio_init(kPinI2cSda) ;
  gpio_set_dir(kPinI2cSda, GPIO_IN) ;
  gpio_pull_up(kPinI2cSda) ;

  gpio_init(kPinI2cScl) ;
  gpio_set_dir(kPinI2cScl, GPIO_IN) ;
  gpio_pull_up(kPinI2cScl) ;

  busy_wait_us_32(50) ;

  // Test 1: Both lines should idle high (pull-ups working)
  bool theSdaHigh = gpio_get(kPinI2cSda) ;
  bool theSclHigh = gpio_get(kPinI2cScl) ;

  if (!theSdaHigh || !theSclHigh)
  {
    // A line is stuck low — bad cable or slave lockup.
    // Attempt bus recovery: clock SCL up to 9 times.
    gpio_set_dir(kPinI2cScl, GPIO_OUT) ;
    gpio_put(kPinI2cScl, 1) ;

    for (int i = 0 ; i < 9 ; i++)
    {
      gpio_put(kPinI2cScl, 0) ;
      busy_wait_us_32(5) ;
      gpio_put(kPinI2cScl, 1) ;
      busy_wait_us_32(5) ;

      if (gpio_get(kPinI2cSda))
        break ;
    }

    // Generate STOP condition (SDA low→high while SCL high)
    gpio_set_dir(kPinI2cSda, GPIO_OUT) ;
    gpio_put(kPinI2cSda, 0) ;
    busy_wait_us_32(5) ;
    gpio_put(kPinI2cScl, 1) ;
    busy_wait_us_32(5) ;
    gpio_put(kPinI2cSda, 1) ;
    busy_wait_us_32(5) ;

    // Re-check after recovery
    gpio_set_dir(kPinI2cSda, GPIO_IN) ;
    gpio_set_dir(kPinI2cScl, GPIO_IN) ;
    busy_wait_us_32(50) ;

    theSdaHigh = gpio_get(kPinI2cSda) ;
    theSclHigh = gpio_get(kPinI2cScl) ;

    if (!theSdaHigh || !theSclHigh)
      return false ;  // Still stuck after recovery — bad cable
  }

  // Test 2: Drive SCL low, verify SDA stays high (not shorted together)
  gpio_set_dir(kPinI2cScl, GPIO_OUT) ;
  gpio_put(kPinI2cScl, 0) ;
  busy_wait_us_32(10) ;
  bool theSdaIndependent = gpio_get(kPinI2cSda) ;
  gpio_put(kPinI2cScl, 1) ;
  gpio_set_dir(kPinI2cScl, GPIO_IN) ;
  busy_wait_us_32(10) ;

  if (!theSdaIndependent)
    return false ;  // SDA followed SCL — lines are shorted

  // Test 3: Drive SDA low, verify SCL stays high
  gpio_set_dir(kPinI2cSda, GPIO_OUT) ;
  gpio_put(kPinI2cSda, 0) ;
  busy_wait_us_32(10) ;
  bool theSclIndependent = gpio_get(kPinI2cScl) ;
  gpio_put(kPinI2cSda, 1) ;
  gpio_set_dir(kPinI2cSda, GPIO_IN) ;
  busy_wait_us_32(10) ;

  if (!theSclIndependent)
    return false ;  // SCL followed SDA — lines are shorted

  return true ;
}

//----------------------------------------------
// Function: InitializeI2C
//----------------------------------------------
static void InitializeI2C(void)
{
  // Test cable/bus health before initializing the I2C peripheral.
  // A bad cable (shorted, broken) can hang the entire system.
  sI2cBusOk = TestI2CBus() ;

  if (!sI2cBusOk)
  {
    // Bus failed — do NOT initialize I2C peripheral.
    // Leave pins as GPIO inputs so a bad cable can't hang
    // the I2C hardware. Sensor init will be skipped.
    return ;
  }

  i2c_init(kI2cPort, kI2cBaudrate) ;
  gpio_set_function(kPinI2cSda, GPIO_FUNC_I2C) ;
  gpio_set_function(kPinI2cScl, GPIO_FUNC_I2C) ;
  gpio_pull_up(kPinI2cSda) ;
  gpio_pull_up(kPinI2cScl) ;
}

//----------------------------------------------
// Function: InitializeSPI
//----------------------------------------------
static void InitializeSPI(void)
{
  printf("Initializing SPI1 bus...\n") ;

  // Safe-state ALL candidate eInk pins for both board variants.
  // Prevents floating pins from interfering with SPI1 (LoRa).
  // Breakout Friend: CS=GP10, DC=GP11, SRAM=GP28
  // Feather Friend:  CS=GP9,  DC=GP10, SRAM=GP6, SD=GP5

  // CS/SRAM/SD pins: HIGH = deselected
  uint8_t theCsPins[] = {
    kPinEpdCs, kPinEpdSramCs,
    kPinEpdCsAlt, kPinEpdSramCsAlt, kPinEpdSdCs
  } ;
  for (int i = 0 ; i < (int)sizeof(theCsPins) ; i++)
  {
    gpio_init(theCsPins[i]) ;
    gpio_set_dir(theCsPins[i], GPIO_OUT) ;
    gpio_put(theCsPins[i], 1) ;   // HIGH = deselected
  }

  // DC pins: LOW = command mode (safe default, prevents stray data writes)
  uint8_t theDcPins[] = { kPinEpdDc, kPinEpdDcAlt } ;
  for (int i = 0 ; i < (int)sizeof(theDcPins) ; i++)
  {
    gpio_init(theDcPins[i]) ;
    gpio_set_dir(theDcPins[i], GPIO_OUT) ;
    gpio_put(theDcPins[i], 0) ;   // LOW = command mode
  }

  // Shared eInk pins (same for both boards)
  gpio_init(kPinEpdReset) ;
  gpio_set_dir(kPinEpdReset, GPIO_OUT) ;
  gpio_put(kPinEpdReset, 0) ;     // Hold in reset

  gpio_init(kPinEpdBusy) ;
  gpio_set_dir(kPinEpdBusy, GPIO_IN) ;
  gpio_pull_up(kPinEpdBusy) ;    // If BUSY not wired, reads HIGH (not busy)

#ifdef DISPLAY_EINK
  // eInk bit-banged SPI pins (dedicated, no SPI1 conflict)
  gpio_init(kPinEpdSck) ;
  gpio_set_dir(kPinEpdSck, GPIO_OUT) ;
  gpio_put(kPinEpdSck, 0) ;

  gpio_init(kPinEpdMosi) ;
  gpio_set_dir(kPinEpdMosi, GPIO_OUT) ;
  gpio_put(kPinEpdMosi, 0) ;
#endif

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
// OLED FeatherWing only — eInk builds skip this
// because GP9/GP6/GP5 are eInk Feather Friend pins.
//----------------------------------------------
#ifndef DISPLAY_EINK
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
#endif

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
#ifdef DISPLAY_EINK
      sBtnCycleMode = true ;
#else
      StatusDisplay_PrevMode() ;
#endif
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
#ifdef DISPLAY_EINK
      {
        // Read current display mode from core1 (atomic word read)
        DisplayMode theMode = sCore1CurrentMode ;

        if (theMode == kDisplayModeRocketId)
        {
          // On Rocket ID screen: cycle the ID (flash save on core0)
          sRocketId = (sRocketId + 1) % 16 ;
          DEBUG_PRINT("Button B: Rocket ID changed to %u\n", sRocketId) ;
          Storage_SaveRocketId(sRocketId) ;
          sRocketIdEditing = true ;
          sBtnEditRocket = true ;
        }
        else
        {
          // On all other screens: go Home (Live)
          DEBUG_PRINT("Button B: Home\n") ;
          sBtnHome = true ;
        }
      }
#else
      // Rocket ID editing on core0 (saves to flash)
      sRocketId = (sRocketId + 1) % 16 ;
      DEBUG_PRINT("Button B: Rocket ID changed to %u\n", sRocketId) ;
      Storage_SaveRocketId(sRocketId) ;
      sRocketIdEditing = true ;

      if (StatusDisplay_GetMode() != kDisplayModeRocketId)
      {
        // On non-Rocket ID screens, undo the ID change and go Home
        sRocketId = (sRocketId + 15) % 16 ;  // Undo increment
        sRocketIdEditing = false ;
        StatusDisplay_SetMode(kDisplayModeLive) ;
      }
#endif
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
#ifdef DISPLAY_EINK
      sBtnCycleMode = true ;
#else
      StatusDisplay_CycleMode() ;
#endif
    }
  }
}

//----------------------------------------------
// Function: UpdateDisplay (OLED only — eInk uses Core1_DisplayLoop)
//----------------------------------------------
#ifndef DISPLAY_EINK

static void UpdateDisplay(uint32_t inCurrentMs)
{
  if (!sDisplayOk) return ;

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
        StatusDisplay_ShowFlightComplete(&sFlightController.pResults,
          theGpsFix, theGpsSatellites,
          theGps ? theGps->pLatitude : 0.0f,
          theGps ? theGps->pLongitude : 0.0f) ;
      }
      else
      {
        // Use compact display showing altitude, GPS, and gateway status with signal quality
        // Link is active if we've received an ACK from the gateway recently
        bool theLinkActive = sLoRaOk && (sLastLoRaRxMs > 0) &&
          ((inCurrentMs - sLastLoRaRxMs) < kLoRaTimeoutMs) ;

        // Pass actual RSSI/SNR from ACK packets (0/0 means no ACK data yet)
        // Display code will show "GW: Active" when RSSI=0/SNR=0 but link is active
        {
          const ImuData * theOledImu = sImuOk ? IMU_GetData(&sImu) : NULL ;
          StatusDisplay_UpdateCompact(
            theState,
            sFlightController.pOrientationMode,
            sRocketId,
            sFlightController.pCurrentAltitudeM,
            sFlightController.pCurrentVelocityMps,
            theOledImu ? theOledImu->pAccelMagnitude : 0.0f,
            &sFlightController.pResults,
            sGpsOk,
            theGpsFix,
            theGpsSatellites,
            theGps ? theGps->pLatitude : 0.0f,
            theGps ? theGps->pLongitude : 0.0f,
            theLinkActive,
            sGatewayRssi,
            sGatewaySnr) ;
        }
      }
      break ;

    case kDisplayModeDeviceInfo:
      {
        const char * theBaroType = sBmp390Ok ? "BMP390" : (sBmp581Ok ? "BMP581" : "None") ;
        StatusDisplay_ShowDeviceInfo(
          FIRMWARE_VERSION_STRING,
          theBaroType,
          sBmp581Ok || sBmp390Ok,
          sLoRaOk,
          sImuOk,
          sGpsOk) ;
      }
      break ;

    case kDisplayModeFlightStats:
      {
        const GpsData * theStatsGps = sGpsOk ? GPS_GetData() : NULL ;
        StatusDisplay_ShowFlightComplete(&sFlightController.pResults,
          (theStatsGps != NULL) && theStatsGps->pValid,
          (theStatsGps != NULL) ? theStatsGps->pSatellites : 0,
          (theStatsGps != NULL) ? theStatsGps->pLatitude : 0.0f,
          (theStatsGps != NULL) ? theStatsGps->pLongitude : 0.0f) ;
      }
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
      {
        const char * theBaroType = sBmp390Ok ? "BMP390" : (sBmp581Ok ? "BMP581" : "None") ;
        StatusDisplay_ShowRates(
          theBaroType,
          1000 / kSensorSampleIntervalMs,     // Baro: 100 Hz
          kImuAccelOdr,                        // Accel: 416 Hz
          kImuGyroOdr,                         // Gyro: 416 Hz
          1,                                   // GPS: 1 Hz (NMEA default)
          1000 / kTelemetryIntervalMs,         // Telemetry: 10 Hz
          1000 / kDisplayUpdateIntervalMs) ;   // Display: 5 Hz
      }
      break ;

    case kDisplayModeRocketId:
      StatusDisplay_ShowRocketId(sRocketId, sRocketName, sRocketIdEditing) ;
      // Reset editing flag after display update
      sRocketIdEditing = false ;
      break ;

    case kDisplayModePyro:
      {
        adc_select_input(0) ;
        float thePyro1V = adc_read() * 3.3f / 4095.0f ;
        adc_select_input(1) ;
        float thePyro2V = adc_read() * 3.3f / 4095.0f ;
        StatusDisplay_ShowPyro(thePyro1V, thePyro2V) ;
      }
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

#endif // !DISPLAY_EINK

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
  // SF7/125kHz 42-byte packet takes ~80ms airtime
  if (LoRa_SendBlocking(&sLoRaRadio, (uint8_t *)&thePacket, theLen, 200))
  {
    // Mark telemetry as sent (updates timestamp and sequence number)
    FlightControl_MarkTelemetrySent(&sFlightController, inCurrentMs) ;
  }
  // Always update TX timestamp — packets transmit even when TX_DONE
  // doesn't fire (200ms timeout expires but gateway receives packet).
  sLastLoRaTxMs = inCurrentMs ;

  // Return to receive mode for commands
  LoRa_StartReceive(&sLoRaRadio) ;
}

//----------------------------------------------
// Function: SendBaroCompare
// Purpose: Send BMP390 vs BMP581 comparison via LoRa
//----------------------------------------------
static void SendBaroCompare(void)
{
  float the390P = 0, the390T = 0 ;
  float the581P = 0, the581T = 0 ;
  bool the390Ok = BMP390_ReadPressureTemperature(&sBmp390, &the390P, &the390T) ;
  bool the581Ok = BMP581_ReadPressureTemperature(&sBmp581, &the581P, &the581T) ;

  if (!the390Ok || !the581Ok) return ;

  // Packet: magic(1), type(1), p390(4), t390(2), p581(4), t581(2) = 14 bytes
  // Pressures in Pa*10 as uint32, temperatures in C*100 as int16
  uint8_t thePacket[14] ;
  thePacket[0] = kLoRaMagic ;
  thePacket[1] = kLoRaPacketBaroCompare ;

  uint32_t theP390 = (uint32_t)(the390P * 10.0f) ;
  int16_t theT390 = (int16_t)(the390T * 100.0f) ;
  uint32_t theP581 = (uint32_t)(the581P * 10.0f) ;
  int16_t theT581 = (int16_t)(the581T * 100.0f) ;

  thePacket[2] = theP390 & 0xFF ;
  thePacket[3] = (theP390 >> 8) & 0xFF ;
  thePacket[4] = (theP390 >> 16) & 0xFF ;
  thePacket[5] = (theP390 >> 24) & 0xFF ;
  thePacket[6] = theT390 & 0xFF ;
  thePacket[7] = (theT390 >> 8) & 0xFF ;
  thePacket[8] = theP581 & 0xFF ;
  thePacket[9] = (theP581 >> 8) & 0xFF ;
  thePacket[10] = (theP581 >> 16) & 0xFF ;
  thePacket[11] = (theP581 >> 24) & 0xFF ;
  thePacket[12] = theT581 & 0xFF ;
  thePacket[13] = (theT581 >> 8) & 0xFF ;

  LoRa_SendBlocking(&sLoRaRadio, thePacket, 14, 200) ;
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
  if (sBmp581Ok || sBmp390Ok) theFlags |= 0x01 ;
  if (sLoRaOk) theFlags |= 0x02 ;
  if (sImuOk) theFlags |= 0x04 ;
  if (sDisplayOk) theFlags |= 0x10 ;
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

  // Barometer type string (for Heltec compatibility)
  const char * theBaroType = sBmp390Ok ? "BMP390" : (sBmp581Ok ? "BMP581" : "None") ;
  uint8_t theBaroTypeLen = strlen(theBaroType) ;
  thePacket[theOffset++] = theBaroTypeLen ;
  memcpy(&thePacket[theOffset], theBaroType, theBaroTypeLen) ;
  theOffset += theBaroTypeLen ;

  // IMU type string (for Heltec compatibility)
  const char * theImuType = sImuOk ? IMU_GetTypeName(&sImu) : "None" ;
  uint8_t theImuTypeLen = strlen(theImuType) ;
  thePacket[theOffset++] = theImuTypeLen ;
  memcpy(&thePacket[theOffset], theImuType, theImuTypeLen) ;
  theOffset += theImuTypeLen ;

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
        puts("*** ARM COMMAND RECEIVED ***") ;
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

      case kCmdBaroCompare:
        sBaroCompareEnabled = !sBaroCompareEnabled ;
        printf("Baro compare streaming %s\n", sBaroCompareEnabled ? "ON" : "OFF") ;
        if (sBaroCompareEnabled)
        {
          if (sBmp390Ok && sBmp581Ok)
          {
            SendBaroCompare() ;
          }
          else
          {
            // Send diagnostic packet: why comparison unavailable
            // Reuse baro compare packet type with error flag
            uint8_t theDiag[14] ;
            theDiag[0] = kLoRaMagic ;
            theDiag[1] = kLoRaPacketBaroCompare ;
            // Error marker: p390 = 0xFFFFFFFF means diagnostic mode
            theDiag[2] = 0xFF ; theDiag[3] = 0xFF ;
            theDiag[4] = 0xFF ; theDiag[5] = 0xFF ;
            theDiag[6] = sBmp390Ok ? 1 : 0 ;
            theDiag[7] = sBmp581Ok ? 1 : 0 ;
            theDiag[8] = sBmp581.pLastError ;
            theDiag[9] = sBmp581.pI2cAddr ;
            // Try reading chip ID right now for live diagnostic
            uint8_t theChipId = 0 ;
            uint8_t theReg = BMP581_REG_CHIP_ID ;
            absolute_time_t theTimeout = make_timeout_time_ms(50) ;
            int theResult = i2c_write_blocking_until(kI2cPort, BMP581_I2C_ADDR_DEFAULT, &theReg, 1, true, theTimeout) ;
            if (theResult == 1)
            {
              theTimeout = make_timeout_time_ms(50) ;
              i2c_read_blocking_until(kI2cPort, BMP581_I2C_ADDR_DEFAULT, &theChipId, 1, false, theTimeout) ;
            }
            theDiag[10] = theChipId ;
            theDiag[11] = 0 ;
            theDiag[12] = 0 ;
            theDiag[13] = 0 ;
            printf("Baro diag: 390ok=%d 581ok=%d err=%d addr=0x%02X chipId=0x%02X\n",
              sBmp390Ok, sBmp581Ok, sBmp581.pLastError, sBmp581.pI2cAddr, theChipId) ;
            LoRa_SendBlocking(&sLoRaRadio, theDiag, 14, 200) ;
            LoRa_StartReceive(&sLoRaRadio) ;
          }
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
              FlightStorage_DeleteAllFlights() ;
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

//----------------------------------------------
// Function: Core1_DisplayLoop
// Purpose: eInk display loop running on core1.
//   Reads shared data from core0, handles button
//   events, and calls StatusDisplay_* functions.
//   Can block freely on eInk refresh without
//   affecting LoRa/sensors on core0.
//----------------------------------------------
#ifdef DISPLAY_EINK

static void Core1_DisplayLoop(void)
{
  // IMPORTANT: No printf on core1. The USB CDC mutex is serviced
  // by core0's IRQ handler. If core1 holds the printf mutex while
  // core0 waits for it, core0 can't service the USB IRQ, causing
  // deadlock. Use sCore1Iterations for debug monitoring from core0.

  DisplaySharedData theData ;
  memset(&theData, 0, sizeof(theData)) ;
  theData.pBaroType = "None" ;

  while (1)
  {
    sCore1Iterations++ ;

    // Copy latest data from core0 (lock-free)
    // Torn reads are harmless — worst case one frame shows
    // a mix of old/new values, imperceptible on eInk.
    // memcpy from volatile to ensure fresh read from shared memory.
    memcpy(&theData, (const void *)&sDisplayShared, sizeof(theData)) ;
    sCore1Altitude = theData.pAltitudeM ;   // Debug: expose for core0 monitoring

    // Read button events (atomic bool reads, clear after reading)
    bool theBtnCycle = sBtnCycleMode ;
    if (theBtnCycle) sBtnCycleMode = false ;
    bool theBtnHome = sBtnHome ;
    if (theBtnHome) sBtnHome = false ;
    bool theBtnEditRocket = sBtnEditRocket ;
    if (theBtnEditRocket) sBtnEditRocket = false ;

    // Process button events
    if (theBtnHome)
    {
      StatusDisplay_SetMode(kDisplayModeLive) ;
    }
    if (theBtnCycle)
    {
      StatusDisplay_CycleMode() ;
    }

    // Write current mode back for core0 Button B decisions
    DisplayMode theMode = StatusDisplay_GetMode() ;
    sCore1CurrentMode = theMode ;

    // Auto-switch to Live when armed (ARMED screen must show immediately)
    if (theData.pState == kFlightArmed && theMode != kDisplayModeLive)
    {
      StatusDisplay_SetMode(kDisplayModeLive) ;
      theMode = kDisplayModeLive ;
    }

    // Skip display updates during active flight (LoRa priority)
    if (theData.pState >= kFlightBoost && theData.pState <= kFlightDescent)
    {
      sleep_ms(kDisplayUpdateIntervalMs) ;
      continue ;
    }

    // Update display based on current mode
    switch (theMode)
    {
      case kDisplayModeLive:
        if (theData.pState == kFlightArmed)
        {
          StatusDisplay_ShowArmed(theData.pGroundPressurePa) ;
        }
        else if (theData.pState == kFlightLanded || theData.pState == kFlightComplete)
        {
          StatusDisplay_ShowFlightComplete(&theData.pResults,
            theData.pGpsFix, theData.pGpsSatellites,
            theData.pGpsLatitude, theData.pGpsLongitude) ;
        }
        else
        {
          StatusDisplay_UpdateCompact(
            theData.pState,
            theData.pOrientationMode,
            theData.pRocketId,
            theData.pAltitudeM,
            theData.pVelocityMps,
            theData.pAccelMagnitude,
            &theData.pResults,
            theData.pGpsOk,
            theData.pGpsFix,
            theData.pGpsSatellites,
            theData.pGpsLatitude,
            theData.pGpsLongitude,
            theData.pLoRaConnected,
            theData.pRssi,
            theData.pSnr) ;
        }
        break ;

      case kDisplayModeDeviceInfo:
        StatusDisplay_ShowDeviceInfo(
          FIRMWARE_VERSION_STRING,
          theData.pBaroType,
          theData.pBaroOk,
          theData.pLoRaOk,
          theData.pImuOk,
          theData.pGpsStatus) ;
        break ;

      case kDisplayModeFlightStats:
        StatusDisplay_ShowFlightComplete(&theData.pResults,
          theData.pGpsFix, theData.pGpsSatellites,
          theData.pGpsLatitude, theData.pGpsLongitude) ;
        break ;

      case kDisplayModeLoRaStatus:
        {
          bool theLinkActive = theData.pLoRaOk && (theData.pLastLoRaRxMs > 0) &&
            ((theData.pCurrentMs - theData.pLastLoRaRxMs) < kLoRaTimeoutMs) ;
          StatusDisplay_ShowLoRaStatus(
            theLinkActive,
            theData.pLastRssi,
            theData.pPacketsSent,
            theData.pPacketsReceived) ;
        }
        break ;

      case kDisplayModeSensors:
        StatusDisplay_ShowSensorReadings(
          theData.pPressurePa,
          theData.pTemperatureC,
          theData.pAltitudeM) ;
        break ;

      case kDisplayModeGpsStatus:
        StatusDisplay_ShowGpsStatus(
          theData.pGpsFix,
          theData.pGpsSatellites,
          theData.pGpsLatitude,
          theData.pGpsLongitude,
          theData.pGpsSpeedMps,
          theData.pGpsHeadingDeg) ;
        break ;

      case kDisplayModeRocketId:
        StatusDisplay_ShowRocketId(
          theData.pRocketId,
          theData.pRocketName,
          theData.pRocketIdEditing) ;
        break ;

      case kDisplayModePyro:
        StatusDisplay_ShowPyro(
          theData.pPyro1Voltage,
          theData.pPyro2Voltage) ;
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

    // Sleep between display updates (sleep_ms uses __wfe() low-power wait)
    uint32_t theInterval = kDisplayUpdateIntervalMs ;
    if (theData.pState == kFlightLanded || theData.pState == kFlightComplete)
    {
      theInterval = kDisplayUpdateSlowMs ;
    }
    if (theBtnCycle || theBtnHome || theBtnEditRocket)
    {
      sleep_ms(200) ;
    }
    else
    {
      sleep_ms(theInterval) ;
    }
  }
}

#endif // DISPLAY_EINK
