//----------------------------------------------
// Module: diskio.c
// Description: FatFs disk I/O layer for SD card
// Author: Mark Gavin
// Created: 2025-12-20
// Copyright: (c) 2025 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "ff.h"
#include "diskio.h"
#include "pins.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include <stdio.h>

#define printf(...) ((void)0)

//----------------------------------------------
// SD Card Commands
//----------------------------------------------
#define CMD0    (0)           // GO_IDLE_STATE
#define CMD1    (1)           // SEND_OP_COND
#define CMD8    (8)           // SEND_IF_COND
#define CMD9    (9)           // SEND_CSD
#define CMD10   (10)          // SEND_CID
#define CMD12   (12)          // STOP_TRANSMISSION
#define CMD16   (16)          // SET_BLOCKLEN
#define CMD17   (17)          // READ_SINGLE_BLOCK
#define CMD18   (18)          // READ_MULTIPLE_BLOCK
#define CMD23   (23)          // SET_BLOCK_COUNT
#define CMD24   (24)          // WRITE_SINGLE_BLOCK
#define CMD25   (25)          // WRITE_MULTIPLE_BLOCK
#define CMD55   (55)          // APP_CMD
#define CMD58   (58)          // READ_OCR
#define ACMD23  (0x80 + 23)   // SET_WR_BLK_ERASE_COUNT
#define ACMD41  (0x80 + 41)   // SD_SEND_OP_COND

//----------------------------------------------
// Card Types
//----------------------------------------------
#define CT_NONE     0
#define CT_MMC      1
#define CT_SD1      2
#define CT_SD2      4
#define CT_SDHC     8

//----------------------------------------------
// Module State
//----------------------------------------------
static DSTATUS sStatus = STA_NOINIT ;
static BYTE sCardType = CT_NONE ;

//----------------------------------------------
// Internal: SPI Transfer
//----------------------------------------------
static BYTE SpiXfer(BYTE inData)
{
  BYTE theResult ;
  spi_write_read_blocking(kSpiPort, &inData, &theResult, 1) ;
  return theResult ;
}

//----------------------------------------------
// Internal: Select Card
//----------------------------------------------
static void SelectCard(void)
{
  gpio_put(kPinSdCs, 0) ;
  SpiXfer(0xFF) ; // Dummy clock
}

//----------------------------------------------
// Internal: Deselect Card
//----------------------------------------------
static void DeselectCard(void)
{
  gpio_put(kPinSdCs, 1) ;
  SpiXfer(0xFF) ; // Extra clock for card release
}

//----------------------------------------------
// Internal: Wait for Card Ready
//----------------------------------------------
static int WaitReady(UINT inTimeout)
{
  absolute_time_t theEnd = make_timeout_time_ms(inTimeout) ;

  do {
    if (SpiXfer(0xFF) == 0xFF) return 1 ;
  } while (!time_reached(theEnd)) ;

  return 0 ;
}

//----------------------------------------------
// Internal: Send Command
//----------------------------------------------
static BYTE SendCmd(BYTE inCmd, DWORD inArg)
{
  BYTE theResponse ;
  BYTE theActualCmd = inCmd ;

  // ACMD?
  if (inCmd & 0x80)
  {
    theActualCmd = inCmd & 0x7F ;
    theResponse = SendCmd(CMD55, 0) ;
    if (theResponse > 1) return theResponse ;
  }

  // Select and wait
  DeselectCard() ;
  SelectCard() ;
  if (!WaitReady(500)) return 0xFF ;

  // Send command packet
  SpiXfer(0x40 | theActualCmd) ;
  SpiXfer((BYTE)(inArg >> 24)) ;
  SpiXfer((BYTE)(inArg >> 16)) ;
  SpiXfer((BYTE)(inArg >> 8)) ;
  SpiXfer((BYTE)inArg) ;

  // CRC
  BYTE theCrc = 0x01 ;  // Dummy CRC + stop bit
  if (theActualCmd == CMD0) theCrc = 0x95 ;
  if (theActualCmd == CMD8) theCrc = 0x87 ;
  SpiXfer(theCrc) ;

  // Wait for response
  if (theActualCmd == CMD12) SpiXfer(0xFF) ; // Skip stuff byte

  int theRetry = 10 ;
  do {
    theResponse = SpiXfer(0xFF) ;
  } while ((theResponse & 0x80) && --theRetry) ;

  return theResponse ;
}

//----------------------------------------------
// Internal: Receive Data Block
//----------------------------------------------
static int ReceiveDataBlock(BYTE * outBuff, UINT inLen)
{
  // Wait for data token
  absolute_time_t theEnd = make_timeout_time_ms(200) ;
  BYTE theToken ;
  do {
    theToken = SpiXfer(0xFF) ;
  } while (theToken == 0xFF && !time_reached(theEnd)) ;

  if (theToken != 0xFE) return 0 ; // Invalid token

  // Receive data
  for (UINT i = 0 ; i < inLen ; i++)
  {
    outBuff[i] = SpiXfer(0xFF) ;
  }

  // Discard CRC
  SpiXfer(0xFF) ;
  SpiXfer(0xFF) ;

  return 1 ;
}

//----------------------------------------------
// Internal: Send Data Block
//----------------------------------------------
static int SendDataBlock(const BYTE * inBuff, BYTE inToken)
{
  if (!WaitReady(500)) return 0 ;

  // Send token
  SpiXfer(inToken) ;

  if (inToken != 0xFD) // Not stop token
  {
    // Send data
    for (UINT i = 0 ; i < 512 ; i++)
    {
      SpiXfer(inBuff[i]) ;
    }

    // Send dummy CRC
    SpiXfer(0xFF) ;
    SpiXfer(0xFF) ;

    // Check response
    BYTE theResp = SpiXfer(0xFF) ;
    if ((theResp & 0x1F) != 0x05) return 0 ;
  }

  return 1 ;
}

//----------------------------------------------
// Function: disk_initialize
//----------------------------------------------
DSTATUS disk_initialize(BYTE inDrive)
{
  if (inDrive != 0) return STA_NOINIT ;

  printf("diskio: Initializing SD card...\n") ;

  // Initialize SPI at slow speed
  spi_init(kSpiPort, 400000) ;
  gpio_set_function(kPinSpiSck, GPIO_FUNC_SPI) ;
  gpio_set_function(kPinSpiMosi, GPIO_FUNC_SPI) ;
  gpio_set_function(kPinSpiMiso, GPIO_FUNC_SPI) ;

  // Initialize CS pin
  gpio_init(kPinSdCs) ;
  gpio_set_dir(kPinSdCs, GPIO_OUT) ;
  gpio_put(kPinSdCs, 1) ;

  // Pull-up on MISO
  gpio_pull_up(kPinSpiMiso) ;

  sleep_ms(10) ;

  // Send 80+ clocks with CS high
  for (int i = 0 ; i < 10 ; i++)
  {
    SpiXfer(0xFF) ;
  }

  sCardType = CT_NONE ;

  // CMD0 - Enter SPI mode
  if (SendCmd(CMD0, 0) == 0x01)
  {
    absolute_time_t theTimeout = make_timeout_time_ms(1000) ;

    // CMD8 - Check for SDv2
    if (SendCmd(CMD8, 0x1AA) == 0x01)
    {
      // Read R7 response
      BYTE theR7[4] ;
      for (int i = 0 ; i < 4 ; i++) theR7[i] = SpiXfer(0xFF) ;

      if (theR7[2] == 0x01 && theR7[3] == 0xAA) // Check pattern
      {
        // ACMD41 with HCS bit
        while (!time_reached(theTimeout) && SendCmd(ACMD41, 0x40000000) != 0) ;

        if (!time_reached(theTimeout))
        {
          // CMD58 - Read OCR
          if (SendCmd(CMD58, 0) == 0)
          {
            BYTE theOcr[4] ;
            for (int i = 0 ; i < 4 ; i++) theOcr[i] = SpiXfer(0xFF) ;

            sCardType = (theOcr[0] & 0x40) ? CT_SD2 | CT_SDHC : CT_SD2 ;
          }
        }
      }
    }
    else
    {
      // SDv1 or MMC
      BYTE theCmd ;
      if (SendCmd(ACMD41, 0) <= 1)
      {
        sCardType = CT_SD1 ;
        theCmd = ACMD41 ;
      }
      else
      {
        sCardType = CT_MMC ;
        theCmd = CMD1 ;
      }

      // Wait for initialization
      while (!time_reached(theTimeout) && SendCmd(theCmd, 0) != 0) ;

      // Set block size to 512
      if (time_reached(theTimeout) || SendCmd(CMD16, 512) != 0)
      {
        sCardType = CT_NONE ;
      }
    }
  }

  DeselectCard() ;

  if (sCardType != CT_NONE)
  {
    // Increase SPI speed
    spi_set_baudrate(kSpiPort, 25000000) ; // 25 MHz

    sStatus &= ~STA_NOINIT ;
    printf("diskio: SD card initialized (type=%d)\n", sCardType) ;
  }
  else
  {
    printf("diskio: SD card init failed\n") ;
  }

  return sStatus ;
}

//----------------------------------------------
// Function: disk_status
//----------------------------------------------
DSTATUS disk_status(BYTE inDrive)
{
  if (inDrive != 0) return STA_NOINIT ;
  return sStatus ;
}

//----------------------------------------------
// Function: disk_read
//----------------------------------------------
DRESULT disk_read(BYTE inDrive, BYTE * outBuff, LBA_t inSector, UINT inCount)
{
  if (inDrive != 0 || !inCount) return RES_PARERR ;
  if (sStatus & STA_NOINIT) return RES_NOTRDY ;

  // Convert to byte address if not SDHC
  if (!(sCardType & CT_SDHC)) inSector *= 512 ;

  if (inCount == 1)
  {
    // Single block read
    if (SendCmd(CMD17, inSector) == 0 && ReceiveDataBlock(outBuff, 512))
    {
      inCount = 0 ;
    }
  }
  else
  {
    // Multiple block read
    if (SendCmd(CMD18, inSector) == 0)
    {
      do {
        if (!ReceiveDataBlock(outBuff, 512)) break ;
        outBuff += 512 ;
      } while (--inCount) ;
      SendCmd(CMD12, 0) ; // Stop transmission
    }
  }

  DeselectCard() ;

  return inCount ? RES_ERROR : RES_OK ;
}

//----------------------------------------------
// Function: disk_write
//----------------------------------------------
DRESULT disk_write(BYTE inDrive, const BYTE * inBuff, LBA_t inSector, UINT inCount)
{
  if (inDrive != 0 || !inCount) return RES_PARERR ;
  if (sStatus & STA_NOINIT) return RES_NOTRDY ;
  if (sStatus & STA_PROTECT) return RES_WRPRT ;

  // Convert to byte address if not SDHC
  if (!(sCardType & CT_SDHC)) inSector *= 512 ;

  if (inCount == 1)
  {
    // Single block write
    if (SendCmd(CMD24, inSector) == 0 && SendDataBlock(inBuff, 0xFE))
    {
      inCount = 0 ;
    }
  }
  else
  {
    // Multiple block write
    if (sCardType & (CT_SD1 | CT_SD2))
    {
      SendCmd(ACMD23, inCount) ; // Predefine block count
    }

    if (SendCmd(CMD25, inSector) == 0)
    {
      do {
        if (!SendDataBlock(inBuff, 0xFC)) break ;
        inBuff += 512 ;
      } while (--inCount) ;

      if (!SendDataBlock(0, 0xFD)) inCount = 1 ; // Stop token
    }
  }

  DeselectCard() ;

  return inCount ? RES_ERROR : RES_OK ;
}

//----------------------------------------------
// Function: disk_ioctl
//----------------------------------------------
DRESULT disk_ioctl(BYTE inDrive, BYTE inCmd, void * ioBuff)
{
  if (inDrive != 0) return RES_PARERR ;
  if (sStatus & STA_NOINIT) return RES_NOTRDY ;

  DRESULT theResult = RES_ERROR ;

  switch (inCmd)
  {
    case CTRL_SYNC:
      // Wait for card to be ready
      SelectCard() ;
      if (WaitReady(500)) theResult = RES_OK ;
      DeselectCard() ;
      break ;

    case GET_SECTOR_SIZE:
      *(WORD *)ioBuff = 512 ;
      theResult = RES_OK ;
      break ;

    case GET_BLOCK_SIZE:
      *(DWORD *)ioBuff = 1 ; // Unknown, return 1
      theResult = RES_OK ;
      break ;

    default:
      theResult = RES_PARERR ;
      break ;
  }

  return theResult ;
}
