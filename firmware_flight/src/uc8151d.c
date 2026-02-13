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

//----------------------------------------------
// Constants
//----------------------------------------------

static const uint32_t kBusyTimeoutMs = 200 ;     // Quick check for BUSY pin
static const uint32_t kFullRefreshMs = 3000 ;     // Fixed delay: full refresh
static const uint32_t kPartialRefreshMs = 500 ;   // Fixed delay: partial refresh
static const uint32_t kPowerOnMs = 200 ;          // Fixed delay: power on

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

// Try BUSY pin briefly; returns true if pin responded, false if timed out.
// Uses busy_wait (not sleep_ms) so this is safe to call from core1
// without interacting with core0's alarm pool.
static bool CheckBusy(void)
{
  uint32_t theStart = to_ms_since_boot(get_absolute_time()) ;

  while (!gpio_get(kPinEpdBusy))
  {
    uint32_t theElapsed = to_ms_since_boot(get_absolute_time()) - theStart ;
    if (theElapsed > kBusyTimeoutMs)
    {
      return false ;  // Pin not responding
    }
    busy_wait_us_32(10000) ;
  }
  return true ;  // Pin went high — display is ready
}

// Wait for display to finish.
// Uses BUSY pin if wired, otherwise falls back to fixed delay.
// All waits use busy_wait (safe for core1, no alarm pool).
static void WaitReady(uint32_t inFixedDelayMs)
{
  if (!CheckBusy())
  {
    // BUSY pin not responding — use fixed delay
    busy_wait_us_32(inFixedDelayMs * 1000) ;
  }
}

static void SendCommand(uint8_t inCmd)
{
  gpio_put(kPinEpdDc, 0) ;    // DC low = command
  gpio_put(kPinEpdCs, 0) ;    // CS low = select
  BitBangSpiWrite(&inCmd, 1) ;
  gpio_put(kPinEpdCs, 1) ;    // CS high = deselect
}

static void SendData(const uint8_t * inData, size_t inLen)
{
  gpio_put(kPinEpdDc, 1) ;    // DC high = data
  gpio_put(kPinEpdCs, 0) ;    // CS low = select
  BitBangSpiWrite(inData, inLen) ;
  gpio_put(kPinEpdCs, 1) ;    // CS high = deselect
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
// Public Functions
//----------------------------------------------

bool UC8151D_Init(void)
{
  printf("EPD: Initializing UC8151D 296x128 eInk display\n") ;
  printf("  Bit-banged SPI on GP24/GP25 (separate from LoRa SPI1)\n") ;

  // Note: All eInk GPIO pins are initialized in main.c InitializeSPI()

  // Triple hardware reset (per Adafruit driver)
  printf("  Hardware reset (3x)...\n") ;
  HardwareReset() ;

  // Power setting
  printf("  Sending power settings...\n") ;
  SendCommand(kCmdPwr) ;
  uint8_t thePwrData[] = { 0x03, 0x00, 0x2B, 0x2B, 0x09 } ;
  SendData(thePwrData, sizeof(thePwrData)) ;

  // Booster soft start
  SendCommand(kCmdBtst) ;
  uint8_t theBtstData[] = { 0x17, 0x17, 0x17 } ;
  SendData(theBtstData, sizeof(theBtstData)) ;

  // Power on and wait
  printf("  Power on...\n") ;
  SendCommand(kCmdPon) ;
  WaitReady(kPowerOnMs) ;

  // Panel setting: 0x1F for monochrome (not 0xCF which is tri-color)
  printf("  Panel setting: 0x1F (mono)\n") ;
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

  printf("  EPD initialized successfully (%dx%d, buffer=%d bytes)\n",
         kEpdWidth, kEpdHeight, kEpdBufferSize) ;

  return true ;
}

void UC8151D_Clear(uint8_t inColor)
{
  // Send new data (DTM2) — all same color
  SendCommand(kCmdDtm2) ;
  gpio_put(kPinEpdDc, 1) ;
  gpio_put(kPinEpdCs, 0) ;
  for (int i = 0 ; i < kEpdBufferSize ; i++)
  {
    BitBangSpiWrite(&inColor, 1) ;
  }
  gpio_put(kPinEpdCs, 1) ;

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
  busy_wait_us_32(100000) ;    // Brief delay before checking busy
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
  gpio_put(kPinEpdDc, 1) ;
  gpio_put(kPinEpdCs, 0) ;
  for (int v = theVstart ; v <= theVend ; v++)
  {
    BitBangSpiWrite(&inOldData[v * kEpdBytesPerRow + theHbyteStart],
                    theBytesPerRow) ;
  }
  gpio_put(kPinEpdCs, 1) ;

  // Send new data (DTM2)
  SendCommand(kCmdDtm2) ;
  gpio_put(kPinEpdDc, 1) ;
  gpio_put(kPinEpdCs, 0) ;
  for (int v = theVstart ; v <= theVend ; v++)
  {
    BitBangSpiWrite(&inNewData[v * kEpdBytesPerRow + theHbyteStart],
                    theBytesPerRow) ;
  }
  gpio_put(kPinEpdCs, 1) ;

  // Refresh partial region
  SendCommand(kCmdDrf) ;
  busy_wait_us_32(100000) ;
  WaitReady(kPartialRefreshMs) ;

  // Exit partial mode
  SendCommand(kCmdPtout) ;
}

void UC8151D_Sleep(void)
{
  printf("  EPD: Entering deep sleep\n") ;

  // CDI before sleep
  SendCommand(kCmdCdi) ;
  SendDataByte(0xF7) ;

  // Deep sleep
  SendCommand(kCmdDslp) ;
  SendDataByte(kDslpCheckByte) ;
}
