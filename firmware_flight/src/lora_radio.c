//----------------------------------------------
// Module: lora_radio.c
// Description: RFM95 LoRa Radio Driver (SX1276)
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "lora_radio.h"
#include "pins.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include <stdio.h>
#include <string.h>

#define printf(...) ((void)0)

//----------------------------------------------
// Constants
//----------------------------------------------
#define RFM95_FXOSC             32000000    // Crystal frequency
#define RFM95_FSTEP             (RFM95_FXOSC / 524288.0)  // Frequency step

//----------------------------------------------
// Module State
//----------------------------------------------
static volatile bool sPacketReceived = false ;
static volatile bool sTransmitDone = false ;

//----------------------------------------------
// Internal: SPI1 Reinitialize
// Resets SPI1 to recover from stuck state.
//----------------------------------------------
static void SpiReinit(void)
{
  spi_deinit(kSpiPort) ;
  spi_init(kSpiPort, kSpiLoRaBaudrate) ;
  gpio_set_function(kPinSpiSck, GPIO_FUNC_SPI) ;
  gpio_set_function(kPinSpiMosi, GPIO_FUNC_SPI) ;
  gpio_set_function(kPinSpiMiso, GPIO_FUNC_SPI) ;
}

//----------------------------------------------
// Internal: SPI Transfer (timeout-protected)
// 5ms timeout prevents hang if SPI1 is stuck.
// A single byte at 1 MHz takes ~8us, so 5ms is
// very generous.
//----------------------------------------------
static uint8_t SpiTransfer(uint8_t inData)
{
  uint8_t theResult = 0 ;
  absolute_time_t theDeadline = make_timeout_time_ms(5) ;

  // TX: wait for space in FIFO
  while (!spi_is_writable(kSpiPort))
  {
    if (absolute_time_diff_us(get_absolute_time(), theDeadline) <= 0)
    {
      SpiReinit() ;
      return 0 ;
    }
  }
  spi_get_hw(kSpiPort)->dr = inData ;

  // RX: wait for data
  while (!spi_is_readable(kSpiPort))
  {
    if (absolute_time_diff_us(get_absolute_time(), theDeadline) <= 0)
    {
      SpiReinit() ;
      return 0 ;
    }
  }
  theResult = (uint8_t)spi_get_hw(kSpiPort)->dr ;
  return theResult ;
}

//----------------------------------------------
// Internal: Read Register
//----------------------------------------------
static uint8_t ReadRegister(uint8_t inAddr)
{
  gpio_put(kPinLoRaCs, 0) ;
  SpiTransfer(inAddr & 0x7F) ;  // Read: MSB = 0
  uint8_t theValue = SpiTransfer(0x00) ;
  gpio_put(kPinLoRaCs, 1) ;
  return theValue ;
}

//----------------------------------------------
// Internal: Write Register
//----------------------------------------------
static void WriteRegister(uint8_t inAddr, uint8_t inValue)
{
  gpio_put(kPinLoRaCs, 0) ;
  SpiTransfer(inAddr | 0x80) ;  // Write: MSB = 1
  SpiTransfer(inValue) ;
  gpio_put(kPinLoRaCs, 1) ;
}

//----------------------------------------------
// Internal: Read FIFO
//----------------------------------------------
static void ReadFifo(uint8_t * outData, uint8_t inLen)
{
  gpio_put(kPinLoRaCs, 0) ;
  SpiTransfer(RFM95_REG_FIFO & 0x7F) ;
  for (uint8_t i = 0 ; i < inLen ; i++)
  {
    outData[i] = SpiTransfer(0x00) ;
  }
  gpio_put(kPinLoRaCs, 1) ;
}

//----------------------------------------------
// Internal: Write FIFO
//----------------------------------------------
static void WriteFifo(const uint8_t * inData, uint8_t inLen)
{
  gpio_put(kPinLoRaCs, 0) ;
  SpiTransfer(RFM95_REG_FIFO | 0x80) ;
  for (uint8_t i = 0 ; i < inLen ; i++)
  {
    SpiTransfer(inData[i]) ;
  }
  gpio_put(kPinLoRaCs, 1) ;
}

//----------------------------------------------
// Internal: Set Operating Mode
//----------------------------------------------
static void SetMode(uint8_t inMode)
{
  WriteRegister(RFM95_REG_OP_MODE, RFM95_MODE_LONG_RANGE | inMode) ;
}

//----------------------------------------------
// Function: LoRa_Init
//----------------------------------------------
bool LoRa_Init(LoRa_Radio * outRadio)
{
  memset(outRadio, 0, sizeof(LoRa_Radio)) ;

  printf("LoRa: SPI pins SCK=%d MOSI=%d MISO=%d CS=%d RST=%d\n",
    kPinSpiSck, kPinSpiMosi, kPinSpiMiso, kPinLoRaCs, kPinLoRaReset) ;

  // Note: SPI and GPIO pins already initialized in main.c InitializeSPI()
  // Just ensure CS is deselected
  gpio_put(kPinLoRaCs, 1) ;

  // Reset the radio
  printf("LoRa: Resetting radio...\n") ;
  gpio_put(kPinLoRaReset, 0) ;
  sleep_ms(10) ;
  gpio_put(kPinLoRaReset, 1) ;
  sleep_ms(10) ;

  // Check version register
  uint8_t theVersion = ReadRegister(RFM95_REG_VERSION) ;
  printf("LoRa: Version register = 0x%02X (expected 0x12)\n", theVersion) ;
  if (theVersion != 0x12)
  {
    printf("LoRa: FAIL - wrong version, radio not responding\n") ;
    return false ;  // Not an RFM95/SX1276
  }

  // Put in sleep mode to configure
  SetMode(RFM95_MODE_SLEEP) ;
  sleep_ms(10) ;

  // Set FIFO base addresses
  WriteRegister(RFM95_REG_FIFO_TX_BASE_ADDR, 0x00) ;
  WriteRegister(RFM95_REG_FIFO_RX_BASE_ADDR, 0x00) ;

  // Set LNA boost
  WriteRegister(RFM95_REG_LNA, ReadRegister(RFM95_REG_LNA) | 0x03) ;

  // Set auto AGC
  WriteRegister(RFM95_REG_MODEM_CONFIG_3, 0x04) ;

  // Configure with defaults
  outRadio->pFrequencyHz = kLoRaFrequency ;
  outRadio->pSpreadFactor = LORA_SF7 ;
  outRadio->pBandwidth = LORA_BW_125 ;
  outRadio->pCodingRate = LORA_CR_4_5 ;
  outRadio->pTxPowerDbm = kLoRaTxPower ;
  outRadio->pSyncWord = kLoRaSyncWord ;

  // Apply configuration
  LoRa_SetFrequency(outRadio, outRadio->pFrequencyHz) ;
  LoRa_SetSpreadingFactor(outRadio, outRadio->pSpreadFactor) ;
  LoRa_SetBandwidth(outRadio, outRadio->pBandwidth) ;
  LoRa_SetCodingRate(outRadio, outRadio->pCodingRate) ;
  LoRa_SetTxPower(outRadio, outRadio->pTxPowerDbm) ;
  LoRa_SetSyncWord(outRadio, outRadio->pSyncWord) ;

  // Go to standby mode
  SetMode(RFM95_MODE_STDBY) ;

  // Initialize DIO0 interrupt pin (optional)
  gpio_init(kPinLoRaDio0) ;
  gpio_set_dir(kPinLoRaDio0, GPIO_IN) ;

  outRadio->pInitialized = true ;
  return true ;
}

//----------------------------------------------
// Function: LoRa_SetFrequency
//----------------------------------------------
bool LoRa_SetFrequency(LoRa_Radio * ioRadio, uint32_t inFrequencyHz)
{
  ioRadio->pFrequencyHz = inFrequencyHz ;

  uint64_t theFrf = ((uint64_t)inFrequencyHz << 19) / RFM95_FXOSC ;

  WriteRegister(RFM95_REG_FRF_MSB, (uint8_t)(theFrf >> 16)) ;
  WriteRegister(RFM95_REG_FRF_MID, (uint8_t)(theFrf >> 8)) ;
  WriteRegister(RFM95_REG_FRF_LSB, (uint8_t)(theFrf >> 0)) ;

  return true ;
}

//----------------------------------------------
// Function: LoRa_SetSpreadingFactor
//----------------------------------------------
bool LoRa_SetSpreadingFactor(LoRa_Radio * ioRadio, LoRa_SpreadingFactor inSF)
{
  ioRadio->pSpreadFactor = inSF ;

  if (inSF == LORA_SF6)
  {
    WriteRegister(RFM95_REG_DETECT_OPTIMIZE, 0xC5) ;
    WriteRegister(RFM95_REG_DETECTION_THRESHOLD, 0x0C) ;
  }
  else
  {
    WriteRegister(RFM95_REG_DETECT_OPTIMIZE, 0xC3) ;
    WriteRegister(RFM95_REG_DETECTION_THRESHOLD, 0x0A) ;
  }

  uint8_t theConfig2 = ReadRegister(RFM95_REG_MODEM_CONFIG_2) ;
  theConfig2 = (theConfig2 & 0x0F) | ((inSF << 4) & 0xF0) ;
  WriteRegister(RFM95_REG_MODEM_CONFIG_2, theConfig2) ;

  return true ;
}

//----------------------------------------------
// Function: LoRa_SetBandwidth
//----------------------------------------------
bool LoRa_SetBandwidth(LoRa_Radio * ioRadio, LoRa_Bandwidth inBW)
{
  ioRadio->pBandwidth = inBW ;

  uint8_t theConfig1 = ReadRegister(RFM95_REG_MODEM_CONFIG_1) ;
  theConfig1 = (theConfig1 & 0x0F) | ((inBW << 4) & 0xF0) ;
  WriteRegister(RFM95_REG_MODEM_CONFIG_1, theConfig1) ;

  return true ;
}

//----------------------------------------------
// Function: LoRa_SetCodingRate
//----------------------------------------------
bool LoRa_SetCodingRate(LoRa_Radio * ioRadio, LoRa_CodingRate inCR)
{
  ioRadio->pCodingRate = inCR ;

  uint8_t theConfig1 = ReadRegister(RFM95_REG_MODEM_CONFIG_1) ;
  theConfig1 = (theConfig1 & 0xF1) | ((inCR << 1) & 0x0E) ;
  WriteRegister(RFM95_REG_MODEM_CONFIG_1, theConfig1) ;

  return true ;
}

//----------------------------------------------
// Function: LoRa_SetTxPower
//----------------------------------------------
bool LoRa_SetTxPower(LoRa_Radio * ioRadio, int8_t inPowerDbm)
{
  ioRadio->pTxPowerDbm = inPowerDbm ;

  // Clamp power to valid range
  if (inPowerDbm < 2) inPowerDbm = 2 ;
  if (inPowerDbm > 20) inPowerDbm = 20 ;

  if (inPowerDbm > 17)
  {
    // Use PA_BOOST with high power settings
    WriteRegister(RFM95_REG_PA_DAC, 0x87) ;  // Enable +20dBm
    WriteRegister(RFM95_REG_PA_CONFIG, RFM95_PA_BOOST | (inPowerDbm - 5)) ;
  }
  else
  {
    WriteRegister(RFM95_REG_PA_DAC, 0x84) ;  // Default power
    WriteRegister(RFM95_REG_PA_CONFIG, RFM95_PA_BOOST | (inPowerDbm - 2)) ;
  }

  return true ;
}

//----------------------------------------------
// Function: LoRa_SetSyncWord
//----------------------------------------------
bool LoRa_SetSyncWord(LoRa_Radio * ioRadio, uint8_t inSyncWord)
{
  ioRadio->pSyncWord = inSyncWord ;
  WriteRegister(RFM95_REG_SYNC_WORD, inSyncWord) ;
  return true ;
}

//----------------------------------------------
// Function: LoRa_ReadVersion
// Reads the version register (0x42) for diagnostics.
// Returns 0x12 for RFM95/SX1276, 0x00 if SPI broken.
//----------------------------------------------
uint8_t LoRa_ReadVersion(LoRa_Radio * inRadio)
{
  (void)inRadio ;
  return ReadRegister(RFM95_REG_VERSION) ;
}

//----------------------------------------------
// Function: LoRa_EnsureReady
// Checks SPI1 communication with the radio by
// reading the version register. If the radio is
// not responding, performs a full hardware reset
// (GP17 toggle) and reconfigures all settings.
//----------------------------------------------
bool LoRa_EnsureReady(LoRa_Radio * ioRadio)
{
  if (!ioRadio->pInitialized)
    return false ;

  // Full hardware reset — matches LoRa_Init timing exactly.
  // NO SpiReinit (SPI peripheral is fine, only radio needs reset).
  gpio_put(kPinLoRaCs, 1) ;
  gpio_put(kPinLoRaReset, 0) ;
  sleep_ms(10) ;
  gpio_put(kPinLoRaReset, 1) ;
  sleep_ms(10) ;

  // Verify radio came back
  uint8_t theVersion = ReadRegister(RFM95_REG_VERSION) ;
  if (theVersion != 0x12)
    return false ;

  // Reconfigure radio (sleep → configure → standby)
  SetMode(RFM95_MODE_SLEEP) ;
  sleep_ms(10) ;

  WriteRegister(RFM95_REG_FIFO_TX_BASE_ADDR, 0x00) ;
  WriteRegister(RFM95_REG_FIFO_RX_BASE_ADDR, 0x00) ;
  WriteRegister(RFM95_REG_LNA, ReadRegister(RFM95_REG_LNA) | 0x03) ;
  WriteRegister(RFM95_REG_MODEM_CONFIG_3, 0x04) ;

  LoRa_SetFrequency(ioRadio, ioRadio->pFrequencyHz) ;
  LoRa_SetSpreadingFactor(ioRadio, ioRadio->pSpreadFactor) ;
  LoRa_SetBandwidth(ioRadio, ioRadio->pBandwidth) ;
  LoRa_SetCodingRate(ioRadio, ioRadio->pCodingRate) ;
  LoRa_SetTxPower(ioRadio, ioRadio->pTxPowerDbm) ;
  LoRa_SetSyncWord(ioRadio, ioRadio->pSyncWord) ;

  SetMode(RFM95_MODE_STDBY) ;
  return true ;
}

//----------------------------------------------
// Function: LoRa_Send
//----------------------------------------------
bool LoRa_Send(LoRa_Radio * ioRadio, const uint8_t * inData, uint8_t inLen)
{
  if (!ioRadio->pInitialized || inLen == 0 || inLen > 255)
  {
    return false ;
  }

  // Drain stale RX FIFO data (noise from eInk bit-bang
  // can inject phantom bytes into SPI1 RX FIFO)
  while (spi_is_readable(kSpiPort))
    (void)spi_get_hw(kSpiPort)->dr ;

  // Go to standby mode
  SetMode(RFM95_MODE_STDBY) ;

  // Reset FIFO address
  WriteRegister(RFM95_REG_FIFO_ADDR_PTR, 0x00) ;

  // Write data to FIFO
  WriteFifo(inData, inLen) ;

  // Set payload length
  WriteRegister(RFM95_REG_PAYLOAD_LENGTH, inLen) ;

  // Configure DIO0 for TxDone
  WriteRegister(RFM95_REG_DIO_MAPPING_1, 0x40) ;

  // Clear IRQ flags
  WriteRegister(RFM95_REG_IRQ_FLAGS, 0xFF) ;

  // Start transmission
  sTransmitDone = false ;
  SetMode(RFM95_MODE_TX) ;

  ioRadio->pPacketsSent++ ;
  return true ;
}

//----------------------------------------------
// Function: LoRa_SendBlocking
//----------------------------------------------
bool LoRa_SendBlocking(
  LoRa_Radio * ioRadio,
  const uint8_t * inData,
  uint8_t inLen,
  uint32_t inTimeoutMs)
{
  if (!LoRa_Send(ioRadio, inData, inLen))
  {
    return false ;
  }

  // Wait for transmit complete
  uint32_t theStart = to_ms_since_boot(get_absolute_time()) ;
  while (!sTransmitDone)
  {
    uint8_t theFlags = ReadRegister(RFM95_REG_IRQ_FLAGS) ;
    if (theFlags & RFM95_IRQ_TX_DONE)
    {
      WriteRegister(RFM95_REG_IRQ_FLAGS, RFM95_IRQ_TX_DONE) ;
      sTransmitDone = true ;
      break ;
    }

    if ((to_ms_since_boot(get_absolute_time()) - theStart) > inTimeoutMs)
    {
      SetMode(RFM95_MODE_STDBY) ;
      return false ;
    }

    busy_wait_us_32(100) ;
  }

  // Return to standby
  SetMode(RFM95_MODE_STDBY) ;
  return true ;
}

//----------------------------------------------
// Function: LoRa_StartReceive
//----------------------------------------------
bool LoRa_StartReceive(LoRa_Radio * ioRadio)
{
  if (!ioRadio->pInitialized)
  {
    return false ;
  }

  // Drain stale RX FIFO data
  while (spi_is_readable(kSpiPort))
    (void)spi_get_hw(kSpiPort)->dr ;

  // Configure DIO0 for RxDone
  WriteRegister(RFM95_REG_DIO_MAPPING_1, 0x00) ;

  // Reset FIFO address
  WriteRegister(RFM95_REG_FIFO_ADDR_PTR, 0x00) ;

  // Clear IRQ flags
  WriteRegister(RFM95_REG_IRQ_FLAGS, 0xFF) ;

  // Start continuous receive
  sPacketReceived = false ;
  SetMode(RFM95_MODE_RX_CONTINUOUS) ;

  return true ;
}

//----------------------------------------------
// Function: LoRa_Available
//----------------------------------------------
uint8_t LoRa_Available(LoRa_Radio * inRadio)
{
  uint8_t theFlags = ReadRegister(RFM95_REG_IRQ_FLAGS) ;

  if (theFlags & RFM95_IRQ_RX_DONE)
  {
    // Check for CRC error
    if (theFlags & RFM95_IRQ_PAYLOAD_CRC_ERROR)
    {
      WriteRegister(RFM95_REG_IRQ_FLAGS, RFM95_IRQ_PAYLOAD_CRC_ERROR | RFM95_IRQ_RX_DONE) ;
      return 0 ;
    }

    return ReadRegister(RFM95_REG_RX_NB_BYTES) ;
  }

  return 0 ;
}

//----------------------------------------------
// Function: LoRa_Receive
//----------------------------------------------
uint8_t LoRa_Receive(LoRa_Radio * ioRadio, uint8_t * outData, uint8_t inMaxLen)
{
  uint8_t theLen = LoRa_Available(ioRadio) ;
  if (theLen == 0)
  {
    return 0 ;
  }

  // Limit to buffer size
  if (theLen > inMaxLen)
  {
    theLen = inMaxLen ;
  }

  // Read RSSI and SNR before reading FIFO
  ioRadio->pLastRssi = -157 + ReadRegister(RFM95_REG_PKT_RSSI_VALUE) ;
  ioRadio->pLastSnr = (int8_t)ReadRegister(RFM95_REG_PKT_SNR_VALUE) / 4 ;

  // Set FIFO address to current RX address
  WriteRegister(RFM95_REG_FIFO_ADDR_PTR, ReadRegister(RFM95_REG_FIFO_RX_CURRENT_ADDR)) ;

  // Read FIFO
  ReadFifo(outData, theLen) ;

  // Clear IRQ flags
  WriteRegister(RFM95_REG_IRQ_FLAGS, RFM95_IRQ_RX_DONE) ;

  ioRadio->pPacketsReceived++ ;
  return theLen ;
}

//----------------------------------------------
// Function: LoRa_GetRssi
//----------------------------------------------
int16_t LoRa_GetRssi(LoRa_Radio * inRadio)
{
  return inRadio->pLastRssi ;
}

//----------------------------------------------
// Function: LoRa_GetSnr
//----------------------------------------------
int8_t LoRa_GetSnr(LoRa_Radio * inRadio)
{
  return inRadio->pLastSnr ;
}

//----------------------------------------------
// Function: LoRa_Sleep
//----------------------------------------------
bool LoRa_Sleep(LoRa_Radio * ioRadio)
{
  SetMode(RFM95_MODE_SLEEP) ;
  return true ;
}

//----------------------------------------------
// Function: LoRa_Idle
//----------------------------------------------
bool LoRa_Idle(LoRa_Radio * ioRadio)
{
  SetMode(RFM95_MODE_STDBY) ;
  return true ;
}

//----------------------------------------------
// Function: LoRa_IsTransmitting
//----------------------------------------------
bool LoRa_IsTransmitting(LoRa_Radio * inRadio)
{
  uint8_t theMode = ReadRegister(RFM95_REG_OP_MODE) & 0x07 ;
  return theMode == RFM95_MODE_TX ;
}

//----------------------------------------------
// Function: LoRa_HandleInterrupt
//----------------------------------------------
void LoRa_HandleInterrupt(LoRa_Radio * ioRadio)
{
  uint8_t theFlags = ReadRegister(RFM95_REG_IRQ_FLAGS) ;

  if (theFlags & RFM95_IRQ_TX_DONE)
  {
    sTransmitDone = true ;
    WriteRegister(RFM95_REG_IRQ_FLAGS, RFM95_IRQ_TX_DONE) ;
  }

  if (theFlags & RFM95_IRQ_RX_DONE)
  {
    sPacketReceived = true ;
    // Don't clear flag yet - let Receive() handle it
  }
}

//----------------------------------------------
// Function: LoRa_Poll
//----------------------------------------------
uint8_t LoRa_Poll(LoRa_Radio * ioRadio)
{
  uint8_t theFlags = ReadRegister(RFM95_REG_IRQ_FLAGS) ;

  if (theFlags & RFM95_IRQ_TX_DONE)
  {
    sTransmitDone = true ;
  }

  if (theFlags & RFM95_IRQ_RX_DONE)
  {
    sPacketReceived = true ;
  }

  return theFlags ;
}
