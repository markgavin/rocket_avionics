//----------------------------------------------
// Module: lora_radio.h
// Description: RFM95 LoRa Radio Driver (SX1276)
// Author: Mark Gavin
// Created: 2026-01-10
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//
// Hardware:
//   - Adafruit Feather RP2040 with RFM95 LoRa (5714)
//   - 915 MHz ISM band (North America)
//   - SPI interface
//----------------------------------------------

#pragma once

#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------
// RFM95/SX1276 Register Addresses
//----------------------------------------------
#define RFM95_REG_FIFO                  0x00
#define RFM95_REG_OP_MODE               0x01
#define RFM95_REG_FRF_MSB               0x06
#define RFM95_REG_FRF_MID               0x07
#define RFM95_REG_FRF_LSB               0x08
#define RFM95_REG_PA_CONFIG             0x09
#define RFM95_REG_PA_RAMP               0x0A
#define RFM95_REG_OCP                   0x0B
#define RFM95_REG_LNA                   0x0C
#define RFM95_REG_FIFO_ADDR_PTR         0x0D
#define RFM95_REG_FIFO_TX_BASE_ADDR     0x0E
#define RFM95_REG_FIFO_RX_BASE_ADDR     0x0F
#define RFM95_REG_FIFO_RX_CURRENT_ADDR  0x10
#define RFM95_REG_IRQ_FLAGS_MASK        0x11
#define RFM95_REG_IRQ_FLAGS             0x12
#define RFM95_REG_RX_NB_BYTES           0x13
#define RFM95_REG_RX_HEADER_CNT_MSB     0x14
#define RFM95_REG_RX_HEADER_CNT_LSB     0x15
#define RFM95_REG_RX_PACKET_CNT_MSB     0x16
#define RFM95_REG_RX_PACKET_CNT_LSB     0x17
#define RFM95_REG_MODEM_STAT            0x18
#define RFM95_REG_PKT_SNR_VALUE         0x19
#define RFM95_REG_PKT_RSSI_VALUE        0x1A
#define RFM95_REG_RSSI_VALUE            0x1B
#define RFM95_REG_HOP_CHANNEL           0x1C
#define RFM95_REG_MODEM_CONFIG_1        0x1D
#define RFM95_REG_MODEM_CONFIG_2        0x1E
#define RFM95_REG_SYMB_TIMEOUT_LSB      0x1F
#define RFM95_REG_PREAMBLE_MSB          0x20
#define RFM95_REG_PREAMBLE_LSB          0x21
#define RFM95_REG_PAYLOAD_LENGTH        0x22
#define RFM95_REG_MAX_PAYLOAD_LENGTH    0x23
#define RFM95_REG_HOP_PERIOD            0x24
#define RFM95_REG_FIFO_RX_BYTE_ADDR     0x25
#define RFM95_REG_MODEM_CONFIG_3        0x26
#define RFM95_REG_PPM_CORRECTION        0x27
#define RFM95_REG_FEI_MSB               0x28
#define RFM95_REG_FEI_MID               0x29
#define RFM95_REG_FEI_LSB               0x2A
#define RFM95_REG_RSSI_WIDEBAND         0x2C
#define RFM95_REG_DETECT_OPTIMIZE       0x31
#define RFM95_REG_INVERT_IQ             0x33
#define RFM95_REG_DETECTION_THRESHOLD   0x37
#define RFM95_REG_SYNC_WORD             0x39
#define RFM95_REG_DIO_MAPPING_1         0x40
#define RFM95_REG_DIO_MAPPING_2         0x41
#define RFM95_REG_VERSION               0x42
#define RFM95_REG_TCXO                  0x4B
#define RFM95_REG_PA_DAC                0x4D
#define RFM95_REG_FORMER_TEMP           0x5B
#define RFM95_REG_AGC_REF               0x61
#define RFM95_REG_AGC_THRESH_1          0x62
#define RFM95_REG_AGC_THRESH_2          0x63
#define RFM95_REG_AGC_THRESH_3          0x64

//----------------------------------------------
// Operating Modes
//----------------------------------------------
#define RFM95_MODE_LONG_RANGE           0x80
#define RFM95_MODE_SLEEP                0x00
#define RFM95_MODE_STDBY                0x01
#define RFM95_MODE_FSTX                 0x02
#define RFM95_MODE_TX                   0x03
#define RFM95_MODE_FSRX                 0x04
#define RFM95_MODE_RX_CONTINUOUS        0x05
#define RFM95_MODE_RX_SINGLE            0x06
#define RFM95_MODE_CAD                  0x07

//----------------------------------------------
// IRQ Flags
//----------------------------------------------
#define RFM95_IRQ_RX_TIMEOUT            0x80
#define RFM95_IRQ_RX_DONE               0x40
#define RFM95_IRQ_PAYLOAD_CRC_ERROR     0x20
#define RFM95_IRQ_VALID_HEADER          0x10
#define RFM95_IRQ_TX_DONE               0x08
#define RFM95_IRQ_CAD_DONE              0x04
#define RFM95_IRQ_FHSS_CHANGE_CHANNEL   0x02
#define RFM95_IRQ_CAD_DETECTED          0x01

//----------------------------------------------
// PA Configuration
//----------------------------------------------
#define RFM95_PA_BOOST                  0x80
#define RFM95_PA_OUTPUT_RFO_PIN         0x00

//----------------------------------------------
// Spreading Factors
//----------------------------------------------
typedef enum
{
  LORA_SF6  = 6 ,
  LORA_SF7  = 7 ,
  LORA_SF8  = 8 ,
  LORA_SF9  = 9 ,
  LORA_SF10 = 10 ,
  LORA_SF11 = 11 ,
  LORA_SF12 = 12
} LoRa_SpreadingFactor ;

//----------------------------------------------
// Bandwidth
//----------------------------------------------
typedef enum
{
  LORA_BW_7_8   = 0 ,   // 7.8 kHz
  LORA_BW_10_4  = 1 ,   // 10.4 kHz
  LORA_BW_15_6  = 2 ,   // 15.6 kHz
  LORA_BW_20_8  = 3 ,   // 20.8 kHz
  LORA_BW_31_25 = 4 ,   // 31.25 kHz
  LORA_BW_41_7  = 5 ,   // 41.7 kHz
  LORA_BW_62_5  = 6 ,   // 62.5 kHz
  LORA_BW_125   = 7 ,   // 125 kHz
  LORA_BW_250   = 8 ,   // 250 kHz
  LORA_BW_500   = 9     // 500 kHz
} LoRa_Bandwidth ;

//----------------------------------------------
// Coding Rates
//----------------------------------------------
typedef enum
{
  LORA_CR_4_5 = 1 ,     // 4/5
  LORA_CR_4_6 = 2 ,     // 4/6
  LORA_CR_4_7 = 3 ,     // 4/7
  LORA_CR_4_8 = 4       // 4/8
} LoRa_CodingRate ;

//----------------------------------------------
// Radio State Structure
//----------------------------------------------
typedef struct
{
  bool pInitialized ;
  uint32_t pFrequencyHz ;
  LoRa_SpreadingFactor pSpreadFactor ;
  LoRa_Bandwidth pBandwidth ;
  LoRa_CodingRate pCodingRate ;
  int8_t pTxPowerDbm ;
  uint8_t pSyncWord ;

  // Statistics
  uint32_t pPacketsSent ;
  uint32_t pPacketsReceived ;
  int16_t pLastRssi ;
  int8_t pLastSnr ;
} LoRa_Radio ;

//----------------------------------------------
// Function: LoRa_Init
// Purpose: Initialize the LoRa radio
// Parameters:
//   outRadio - Radio structure to initialize
// Returns: true if successful
//----------------------------------------------
bool LoRa_Init(LoRa_Radio * outRadio) ;

//----------------------------------------------
// Function: LoRa_SetFrequency
// Purpose: Set the radio frequency
// Parameters:
//   ioRadio - Radio to configure
//   inFrequencyHz - Frequency in Hz (e.g., 915000000)
// Returns: true if successful
//----------------------------------------------
bool LoRa_SetFrequency(LoRa_Radio * ioRadio, uint32_t inFrequencyHz) ;

//----------------------------------------------
// Function: LoRa_SetSpreadingFactor
// Purpose: Set the spreading factor
// Parameters:
//   ioRadio - Radio to configure
//   inSF - Spreading factor (6-12)
// Returns: true if successful
//----------------------------------------------
bool LoRa_SetSpreadingFactor(LoRa_Radio * ioRadio, LoRa_SpreadingFactor inSF) ;

//----------------------------------------------
// Function: LoRa_SetBandwidth
// Purpose: Set the bandwidth
// Parameters:
//   ioRadio - Radio to configure
//   inBW - Bandwidth setting
// Returns: true if successful
//----------------------------------------------
bool LoRa_SetBandwidth(LoRa_Radio * ioRadio, LoRa_Bandwidth inBW) ;

//----------------------------------------------
// Function: LoRa_SetCodingRate
// Purpose: Set the coding rate
// Parameters:
//   ioRadio - Radio to configure
//   inCR - Coding rate
// Returns: true if successful
//----------------------------------------------
bool LoRa_SetCodingRate(LoRa_Radio * ioRadio, LoRa_CodingRate inCR) ;

//----------------------------------------------
// Function: LoRa_SetTxPower
// Purpose: Set transmit power
// Parameters:
//   ioRadio - Radio to configure
//   inPowerDbm - Power in dBm (2-20)
// Returns: true if successful
//----------------------------------------------
bool LoRa_SetTxPower(LoRa_Radio * ioRadio, int8_t inPowerDbm) ;

//----------------------------------------------
// Function: LoRa_SetSyncWord
// Purpose: Set sync word for network isolation
// Parameters:
//   ioRadio - Radio to configure
//   inSyncWord - Sync word (0x12 = LoRaWAN, 0x14 = private)
// Returns: true if successful
//----------------------------------------------
bool LoRa_SetSyncWord(LoRa_Radio * ioRadio, uint8_t inSyncWord) ;

//----------------------------------------------
// Function: LoRa_Send
// Purpose: Send a packet
// Parameters:
//   ioRadio - Radio to use
//   inData - Data to send
//   inLen - Data length (max 255)
// Returns: true if packet queued successfully
//----------------------------------------------
bool LoRa_Send(LoRa_Radio * ioRadio, const uint8_t * inData, uint8_t inLen) ;

//----------------------------------------------
// Function: LoRa_SendBlocking
// Purpose: Send a packet and wait for completion
// Parameters:
//   ioRadio - Radio to use
//   inData - Data to send
//   inLen - Data length
//   inTimeoutMs - Timeout in milliseconds
// Returns: true if sent successfully
//----------------------------------------------
bool LoRa_SendBlocking(
  LoRa_Radio * ioRadio,
  const uint8_t * inData,
  uint8_t inLen,
  uint32_t inTimeoutMs) ;

//----------------------------------------------
// Function: LoRa_StartReceive
// Purpose: Start continuous receive mode
// Parameters:
//   ioRadio - Radio to use
// Returns: true if successful
//----------------------------------------------
bool LoRa_StartReceive(LoRa_Radio * ioRadio) ;

//----------------------------------------------
// Function: LoRa_Available
// Purpose: Check if a packet is available
// Parameters:
//   inRadio - Radio to check
// Returns: Number of bytes available (0 if none)
//----------------------------------------------
uint8_t LoRa_Available(LoRa_Radio * inRadio) ;

//----------------------------------------------
// Function: LoRa_Receive
// Purpose: Receive a packet
// Parameters:
//   ioRadio - Radio to use
//   outData - Buffer for received data
//   inMaxLen - Maximum bytes to receive
// Returns: Number of bytes received (0 if none)
//----------------------------------------------
uint8_t LoRa_Receive(LoRa_Radio * ioRadio, uint8_t * outData, uint8_t inMaxLen) ;

//----------------------------------------------
// Function: LoRa_GetRssi
// Purpose: Get RSSI of last received packet
// Parameters:
//   inRadio - Radio
// Returns: RSSI in dBm
//----------------------------------------------
int16_t LoRa_GetRssi(LoRa_Radio * inRadio) ;

//----------------------------------------------
// Function: LoRa_GetSnr
// Purpose: Get SNR of last received packet
// Parameters:
//   inRadio - Radio
// Returns: SNR in dB (can be negative)
//----------------------------------------------
int8_t LoRa_GetSnr(LoRa_Radio * inRadio) ;

//----------------------------------------------
// Function: LoRa_Sleep
// Purpose: Put radio in sleep mode
// Parameters:
//   ioRadio - Radio to sleep
// Returns: true if successful
//----------------------------------------------
bool LoRa_Sleep(LoRa_Radio * ioRadio) ;

//----------------------------------------------
// Function: LoRa_Idle
// Purpose: Put radio in standby mode
// Parameters:
//   ioRadio - Radio
// Returns: true if successful
//----------------------------------------------
bool LoRa_Idle(LoRa_Radio * ioRadio) ;

//----------------------------------------------
// Function: LoRa_IsTransmitting
// Purpose: Check if currently transmitting
// Parameters:
//   inRadio - Radio to check
// Returns: true if transmitting
//----------------------------------------------
bool LoRa_IsTransmitting(LoRa_Radio * inRadio) ;

//----------------------------------------------
// Function: LoRa_HandleInterrupt
// Purpose: Handle DIO0 interrupt (call from ISR)
// Parameters:
//   ioRadio - Radio
//----------------------------------------------
void LoRa_HandleInterrupt(LoRa_Radio * ioRadio) ;

//----------------------------------------------
// Function: LoRa_Poll
// Purpose: Poll for events (alternative to interrupt)
// Parameters:
//   ioRadio - Radio to poll
// Returns: IRQ flags that were set
//----------------------------------------------
uint8_t LoRa_Poll(LoRa_Radio * ioRadio) ;
