#ifndef UC8151D_H
#define UC8151D_H

//----------------------------------------------
// UC8151D eInk Display Driver
// 2.9" Flexible 296x128 Monochrome (Adafruit PID 4262)
//----------------------------------------------

#include <stdint.h>
#include <stdbool.h>

// Physical display dimensions (landscape orientation)
#define kEpdWidth         296
#define kEpdHeight        128

// Controller buffer layout (portrait: 128 wide x 296 tall)
#define kEpdBufWidth      128
#define kEpdBufHeight     296
#define kEpdBytesPerRow   (kEpdBufWidth / 8)              // 16 bytes
#define kEpdBufferSize    (kEpdBytesPerRow * kEpdBufHeight) // 4736 bytes

// UC8151D commands
#define kCmdPsr           0x00    // Panel Setting
#define kCmdPwr           0x01    // Power Setting
#define kCmdPof           0x02    // Power Off
#define kCmdPfs           0x03    // Power Off Sequence
#define kCmdPon           0x04    // Power On
#define kCmdBtst          0x06    // Booster Soft Start
#define kCmdDslp          0x07    // Deep Sleep
#define kCmdDtm1          0x10    // Data Start Transmission 1 (old data)
#define kCmdDsp           0x11    // Data Stop
#define kCmdDrf           0x12    // Display Refresh
#define kCmdDtm2          0x13    // Data Start Transmission 2 (new data)
#define kCmdPll           0x30    // PLL Control
#define kCmdTsc           0x40    // Temperature Sensor Command
#define kCmdTse           0x41    // Temperature Sensor Enable
#define kCmdCdi           0x50    // VCOM and Data Interval
#define kCmdLpd           0x51    // Low Power Detection
#define kCmdTcon          0x60    // TCON Setting
#define kCmdTres          0x61    // Resolution Setting
#define kCmdRev           0x70    // Revision
#define kCmdFlg           0x71    // Get Status
#define kCmdPtl           0x90    // Partial Window
#define kCmdPtin          0x91    // Partial In
#define kCmdPtout         0x92    // Partial Out
#define kCmdVdcs          0xBD    // VCOM DC Setting

// Deep sleep check byte
#define kDslpCheckByte    0xA5

//----------------------------------------------
// Board Type (auto-detected during Init)
//----------------------------------------------
typedef enum
{
  kEpdBoardNone = 0,       // No eInk display detected
  kEpdBoardBreakout,       // eInk Breakout Friend (CS=GP10, DC=GP11)
  kEpdBoardFeather         // eInk Feather Friend 4446 (CS=GP9, DC=GP10)
} EpdBoardType ;

// Public functions
bool UC8151D_Init(void) ;
void UC8151D_Reinit(void) ;
EpdBoardType UC8151D_GetBoardType(void) ;
void UC8151D_Clear(uint8_t inColor) ;
void UC8151D_WriteImage(const uint8_t * inData) ;
void UC8151D_WritePartial(const uint8_t * inOldData, const uint8_t * inNewData,
                          int inPhysX, int inPhysY, int inPhysW, int inPhysH) ;
void UC8151D_Refresh(void) ;
void UC8151D_Sleep(void) ;

#endif // UC8151D_H
