//----------------------------------------------
// UC8151D eInk Display Driver
// SPI driver for 2.9" Flexible 296x128 Monochrome eInk
//----------------------------------------------

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "uc8151d.h"
#include "pins.h"

#define printf(...) ((void)0)

//----------------------------------------------
// Constants
//----------------------------------------------

static const uint32_t kFullRefreshMs = 3000 ;     // Fixed delay: full refresh
static const uint32_t kPartialRefreshMs = 800 ;   // Fixed delay: partial refresh
static const uint32_t kPowerOnMs = 200 ;          // Fixed delay: power on
static const uint32_t kDetectTimeoutMs = 100 ;    // Board detection probe timeout

//----------------------------------------------
// Runtime Pin Configuration
// CS, DC, and SRAM CS differ between Breakout Friend
// and Feather Friend. Set by DetectBoard() during Init.
//----------------------------------------------
static uint8_t sPinCs = kPinEpdCs ;
static uint8_t sPinDc = kPinEpdDc ;
static EpdBoardType sBoardType = kEpdBoardNone ;
static bool sBusyWired = false ;

//----------------------------------------------
// Bit-Banged SPI Write
//
// Dedicated GPIO pins (GP24 SCK, GP25 MOSI) completely
// separate from LoRa SPI1 (GP14/GP15). No pin sharing.
// SPI Mode 0 (CPOL=0 CPHA=0): clock idle low,
// data sampled on rising edge.
//----------------------------------------------

static void BitBangSpiWrite(const uint8_t * inData, size_t inLen)
{
  for (size_t i = 0 ; i < inLen ; i++)
  {
    uint8_t theByte = inData[i] ;
    for (int theBit = 7 ; theBit >= 0 ; theBit--)
    {
      gpio_put(kPinEpdMosi, (theByte >> theBit) & 1) ;
      gpio_put(kPinEpdSck, 1) ;   // Rising edge — device samples
      __asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop") ;
      gpio_put(kPinEpdSck, 0) ;   // Falling edge
      __asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop") ;
    }
  }
}

//----------------------------------------------
// SPI Helpers
//----------------------------------------------

// Wait for display to finish.
// If BUSY is wired, polls until BUSY goes HIGH (ready).
// If BUSY is not wired, uses fixed delay.
// Uses sleep_ms for idle waits — puts core into low-power
// state via __wfe() instead of spinning at 100% CPU.
static void WaitReady(uint32_t inFixedDelayMs)
{
  if (sBusyWired)
  {
    // Poll BUSY pin with timeout
    uint32_t theStart = to_ms_since_boot(get_absolute_time()) ;
    uint32_t theTimeoutMs = inFixedDelayMs + 1000 ;

    while (!gpio_get(kPinEpdBusy))
    {
      uint32_t theElapsed = to_ms_since_boot(get_absolute_time()) - theStart ;
      if (theElapsed > theTimeoutMs)
      {
        break ;  // Timeout — proceed anyway
      }
      sleep_ms(10) ;
    }
  }
  else
  {
    // BUSY not wired — use fixed delay
    sleep_ms(inFixedDelayMs) ;
  }
}

static void SendCommand(uint8_t inCmd)
{
  gpio_put(sPinDc, 0) ;    // DC low = command
  gpio_put(sPinCs, 0) ;    // CS low = select
  BitBangSpiWrite(&inCmd, 1) ;
  gpio_put(sPinCs, 1) ;    // CS high = deselect
}

static void SendData(const uint8_t * inData, size_t inLen)
{
  gpio_put(sPinDc, 1) ;    // DC high = data
  gpio_put(sPinCs, 0) ;    // CS low = select
  BitBangSpiWrite(inData, inLen) ;
  gpio_put(sPinCs, 1) ;    // CS high = deselect
}

static void SendDataByte(uint8_t inByte)
{
  SendData(&inByte, 1) ;
}

//----------------------------------------------
// Hardware Reset (triple reset per Adafruit driver)
//----------------------------------------------

static void HardwareReset(void)
{
  for (int i = 0 ; i < 3 ; i++)
  {
    gpio_put(kPinEpdReset, 1) ;
    sleep_ms(10) ;
    gpio_put(kPinEpdReset, 0) ;
    sleep_ms(10) ;
    gpio_put(kPinEpdReset, 1) ;
    sleep_ms(10) ;
  }
  WaitReady(kPowerOnMs) ;
}

//----------------------------------------------
// Board Detection
//
// Probes the display with both pin sets. First tries
// BUSY-based detection (fast). If BUSY is not wired,
// falls back to assuming Feather Friend with fixed timing.
//----------------------------------------------

static void ConfigureProbePins(uint8_t inCs, uint8_t inDc)
{
  // Re-configure candidate pins as outputs.
  // In DISPLAY_EINK builds, InitializeButtons() is skipped.
  // In OLED builds, it may have set GP9/GP6/GP5 to INPUT.
  gpio_init(inCs) ;
  gpio_set_dir(inCs, GPIO_OUT) ;
  gpio_put(inCs, 1) ;    // CS HIGH = deselected

  gpio_init(inDc) ;
  gpio_set_dir(inDc, GPIO_OUT) ;
  gpio_put(inDc, 0) ;    // DC LOW = command mode

  sPinCs = inCs ;
  sPinDc = inDc ;
}

static bool ProbeWithBusy(uint8_t inCs, uint8_t inDc)
{
  ConfigureProbePins(inCs, inDc) ;

  // Send power-on command and watch for BUSY to toggle
  SendCommand(kCmdPon) ;

  uint32_t theStart = to_ms_since_boot(get_absolute_time()) ;

  // Wait for BUSY to go low (display processing)
  while (gpio_get(kPinEpdBusy))
  {
    uint32_t theElapsed = to_ms_since_boot(get_absolute_time()) - theStart ;
    if (theElapsed > kDetectTimeoutMs)
      return false ;  // BUSY never went low — not wired or wrong pins
    busy_wait_us_32(1000) ;
  }

  // BUSY went low — display received the command. Wait for ready.
  while (!gpio_get(kPinEpdBusy))
  {
    uint32_t theElapsed = to_ms_since_boot(get_absolute_time()) - theStart ;
    if (theElapsed > kPowerOnMs + kDetectTimeoutMs)
      break ;
    busy_wait_us_32(1000) ;
  }

  return true ;
}

static void DetectBoard(void)
{
  printf("EPD: Auto-detecting eInk board...\n") ;

  HardwareReset() ;

  // Try Breakout Friend (CS=GP10, DC=GP11) with BUSY detection
  if (ProbeWithBusy(kPinEpdCs, kPinEpdDc))
  {
    sBoardType = kEpdBoardBreakout ;
    sBusyWired = true ;
    printf("  Detected: eInk Breakout Friend (BUSY wired)\n") ;
    return ;
  }

  HardwareReset() ;

  // Try Feather Friend (CS=GP9, DC=GP10) with BUSY detection
  if (ProbeWithBusy(kPinEpdCsAlt, kPinEpdDcAlt))
  {
    sBoardType = kEpdBoardFeather ;
    sBusyWired = true ;
    printf("  Detected: eInk Feather Friend (BUSY wired)\n") ;
    return ;
  }

  // BUSY-based detection failed — BUSY is probably not wired.
  // Assume Feather Friend and use fixed timing delays.
  printf("  BUSY not wired, assuming Feather Friend (fixed timing)\n") ;

  ConfigureProbePins(kPinEpdCsAlt, kPinEpdDcAlt) ;
  sBoardType = kEpdBoardFeather ;
  sBusyWired = false ;
}

//----------------------------------------------
// Public Functions
//----------------------------------------------

EpdBoardType UC8151D_GetBoardType(void)
{
  return sBoardType ;
}

bool UC8151D_Init(void)
{
  printf("EPD: Initializing UC8151D 296x128 eInk display\n") ;

  // Note: All eInk GPIO pins are initialized in main.c InitializeSPI()

  // Auto-detect which eInk board is connected
  DetectBoard() ;
  if (sBoardType == kEpdBoardNone)
    return false ;

  // Reset for clean init after detection probes
  HardwareReset() ;

  // Power setting
  SendCommand(kCmdPwr) ;
  uint8_t thePwrData[] = { 0x03, 0x00, 0x2B, 0x2B, 0x09 } ;
  SendData(thePwrData, sizeof(thePwrData)) ;

  // Booster soft start
  SendCommand(kCmdBtst) ;
  uint8_t theBtstData[] = { 0x17, 0x17, 0x17 } ;
  SendData(theBtstData, sizeof(theBtstData)) ;

  // Power on and wait
  SendCommand(kCmdPon) ;
  WaitReady(kPowerOnMs) ;

  // Panel setting: 0x1F for monochrome (not 0xCF which is tri-color)
  SendCommand(kCmdPsr) ;
  SendDataByte(0x1F) ;

  // VCOM and data interval
  SendCommand(kCmdCdi) ;
  SendDataByte(0x97) ;

  // PLL clock frequency
  SendCommand(kCmdPll) ;
  SendDataByte(0x29) ;

  // Resolution: height(128), width_hi(0x01), width_lo(0x28)
  SendCommand(kCmdTres) ;
  uint8_t theTresData[] = { 0x80, 0x01, 0x28 } ;
  SendData(theTresData, sizeof(theTresData)) ;

  // VCM DC setting
  SendCommand(kCmdVdcs) ;
  SendDataByte(0x0A) ;

  printf("  EPD initialized (%dx%d, BUSY %s)\n",
         kEpdWidth, kEpdHeight, sBusyWired ? "wired" : "not wired") ;

  return true ;
}

void UC8151D_Clear(uint8_t inColor)
{
  // Send new data (DTM2) — all same color
  SendCommand(kCmdDtm2) ;
  gpio_put(sPinDc, 1) ;
  gpio_put(sPinCs, 0) ;
  for (int i = 0 ; i < kEpdBufferSize ; i++)
  {
    BitBangSpiWrite(&inColor, 1) ;
  }
  gpio_put(sPinCs, 1) ;

  UC8151D_Refresh() ;
}

void UC8151D_WriteImage(const uint8_t * inData)
{
  // Send new data (DTM2)
  SendCommand(kCmdDtm2) ;
  SendData(inData, kEpdBufferSize) ;

  UC8151D_Refresh() ;
}

void UC8151D_Refresh(void)
{
  SendCommand(kCmdDrf) ;
  sleep_ms(100) ;              // Brief delay before checking busy
  WaitReady(kFullRefreshMs) ;
}

void UC8151D_WritePartial(const uint8_t * inOldData, const uint8_t * inNewData,
                          int inPhysX, int inPhysY, int inPhysW, int inPhysH)
{
  // Map physical landscape coords to controller portrait coords
  // 180° rotation: controllerRow = (295 - physX), controllerCol = physY
  int theVstart = kEpdWidth - inPhysX - inPhysW ;
  int theVend = kEpdWidth - inPhysX - 1 ;
  int theHstart = inPhysY & 0xF8 ;                   // byte-aligned start
  int theHend = (inPhysY + inPhysH - 1) | 0x07 ;     // byte-aligned end

  // Clamp to valid range
  if (theVstart < 0) theVstart = 0 ;
  if (theVend >= kEpdBufHeight) theVend = kEpdBufHeight - 1 ;
  if (theHend >= kEpdBufWidth) theHend = kEpdBufWidth - 1 ;

  int theHbyteStart = theHstart / 8 ;
  int theHbyteEnd = theHend / 8 ;
  int theBytesPerRow = theHbyteEnd - theHbyteStart + 1 ;

  // Enter partial mode
  SendCommand(kCmdPtin) ;

  // Set partial window
  SendCommand(kCmdPtl) ;
  SendDataByte((uint8_t)theHstart) ;
  SendDataByte((uint8_t)theHend) ;
  SendDataByte((uint8_t)(theVstart >> 8)) ;
  SendDataByte((uint8_t)(theVstart & 0xFF)) ;
  SendDataByte((uint8_t)(theVend >> 8)) ;
  SendDataByte((uint8_t)(theVend & 0xFF)) ;
  SendDataByte(0x01) ;     // PT_SCAN

  // Send old data (DTM1) — needed for clean partial refresh
  SendCommand(kCmdDtm1) ;
  gpio_put(sPinDc, 1) ;
  gpio_put(sPinCs, 0) ;
  for (int v = theVstart ; v <= theVend ; v++)
  {
    BitBangSpiWrite(&inOldData[v * kEpdBytesPerRow + theHbyteStart],
                    theBytesPerRow) ;
  }
  gpio_put(sPinCs, 1) ;

  // Send new data (DTM2)
  SendCommand(kCmdDtm2) ;
  gpio_put(sPinDc, 1) ;
  gpio_put(sPinCs, 0) ;
  for (int v = theVstart ; v <= theVend ; v++)
  {
    BitBangSpiWrite(&inNewData[v * kEpdBytesPerRow + theHbyteStart],
                    theBytesPerRow) ;
  }
  gpio_put(sPinCs, 1) ;

  // Refresh partial region
  SendCommand(kCmdDrf) ;
  sleep_ms(100) ;
  WaitReady(kPartialRefreshMs) ;

  // Exit partial mode
  SendCommand(kCmdPtout) ;
}

void UC8151D_Sleep(void)
{
  // CDI before sleep
  SendCommand(kCmdCdi) ;
  SendDataByte(0xF7) ;

  // Deep sleep
  SendCommand(kCmdDslp) ;
  SendDataByte(kDslpCheckByte) ;
}
