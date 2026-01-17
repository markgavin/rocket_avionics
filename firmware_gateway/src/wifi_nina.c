//----------------------------------------------
// Module: wifi_nina.c
// Description: NINA WiFi driver implementation
// Author: Mark Gavin
// Created: 2026-01-14
// Copyright: (c) 2025-2026 by Mark Gavin
// License: Proprietary - All Rights Reserved
//----------------------------------------------

#include "wifi_nina.h"
#include "pins.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include <string.h>
#include <stdio.h>

//----------------------------------------------
// Debug output (conditional)
//----------------------------------------------
#ifdef DEBUG_WIFI
  #define WIFI_DEBUG(fmt, ...) printf("[WiFi] " fmt "\n", ##__VA_ARGS__)
#else
  #define WIFI_DEBUG(fmt, ...) ((void)0)
#endif

//----------------------------------------------
// Static state
//----------------------------------------------
static bool sInitialized = false ;
static int8_t sServerSocket = -1 ;
static int8_t sClientSocket = -1 ;

//----------------------------------------------
// SPI Communication Helpers
//----------------------------------------------

static bool WaitForReady(void)
{
    // Wait for ESP32 BUSY pin to go LOW (ready)
    uint32_t theStart = to_ms_since_boot(get_absolute_time()) ;
    bool theInitialState = gpio_get(kPinWifiBusy) ;

    while (gpio_get(kPinWifiBusy))
    {
        if (to_ms_since_boot(get_absolute_time()) - theStart > WIFI_SPI_TIMEOUT_MS)
        {
            WIFI_DEBUG("WaitForReady TIMEOUT") ;
            // Try to proceed anyway - some commands may work differently
            return true ;  // Changed: return true to try reading
        }
        sleep_us(100) ;
    }

    (void)theInitialState ;  // Unused in release
    return true ;
}

static uint8_t SpiTransfer(uint8_t inByte)
{
    uint8_t theResult = 0 ;
    spi_write_read_blocking(kSpiWifiPort, &inByte, &theResult, 1) ;
    return theResult ;
}

static bool WaitResponseReady(void)
{
    uint32_t theStart = to_ms_since_boot(get_absolute_time()) ;
    while (!gpio_get(kPinWifiBusy))
    {
        if (to_ms_since_boot(get_absolute_time()) - theStart > WIFI_SPI_TIMEOUT_MS)
        {
            WIFI_DEBUG("WaitResponseReady TIMEOUT") ;
            return false ;
        }
        sleep_us(100) ;
    }
    return true ;
}

static bool ReadAndCheckByte(uint8_t inExpected, uint8_t * outActual)
{
    uint8_t theActual = SpiTransfer(NINA_DUMMY_DATA) ;
    if (outActual != NULL)
    {
        *outActual = theActual ;
    }
    return (theActual == inExpected) ;
}

//----------------------------------------------
// Command Send/Receive
//----------------------------------------------

// Drain any leftover data/responses from SPI bus
static void DrainSpi(void)
{
    // Ensure LoRa CS is high (shared bus)
    gpio_put(kPinLoRaCs, 1) ;

    // If BUSY is HIGH, there might be pending data - try to read it
    int drainAttempts = 0 ;
    while (gpio_get(kPinWifiBusy) && drainAttempts < 5)
    {
        drainAttempts++ ;

        gpio_put(kPinWifiCs, 0) ;
        sleep_us(10) ;

        // Read and discard pending data
        for (int i = 0 ; i < 16 ; i++)
        {
            SpiTransfer(NINA_DUMMY_DATA) ;
        }
        gpio_put(kPinWifiCs, 1) ;
        sleep_ms(100) ;
    }
}

static bool SendCmd(uint8_t inCmd, uint8_t inNumParams)
{
    // Ensure LoRa CS is high (shared bus)
    gpio_put(kPinLoRaCs, 1) ;

    // Small delay to let bus settle
    sleep_us(100) ;

    gpio_put(kPinWifiCs, 0) ;
    if (!WaitForReady())
    {
        gpio_put(kPinWifiCs, 1) ;
        return false ;
    }

    SpiTransfer(NINA_START_CMD) ;
    SpiTransfer(inCmd & ~NINA_REPLY_FLAG) ;
    SpiTransfer(inNumParams) ;
    return true ;
}

static void SendParam(const uint8_t * inParam, uint8_t inLen)
{
    SpiTransfer(inLen) ;
    for (uint8_t i = 0 ; i < inLen ; i++)
    {
        SpiTransfer(inParam[i]) ;
    }
}

static void SendParamWord(uint16_t inValue)
{
    SpiTransfer(2) ;
    SpiTransfer((uint8_t)(inValue >> 8)) ;
    SpiTransfer((uint8_t)(inValue & 0xFF)) ;
}

static void SendParamByte(uint8_t inValue)
{
    SpiTransfer(1) ;
    SpiTransfer(inValue) ;
}

static void EndCmd(void)
{
    SpiTransfer(NINA_END_CMD) ;

    // Adafruit-style: add 2 padding bytes before deselecting
    SpiTransfer(NINA_DUMMY_DATA) ;
    SpiTransfer(NINA_DUMMY_DATA) ;

    gpio_put(kPinWifiCs, 1) ;

    // Wait for ESP32 to process command
    sleep_ms(1) ;
}

static bool WaitResponse(uint8_t inCmd, uint8_t * outNumParams)
{
    gpio_put(kPinWifiCs, 0) ;

    if (!WaitResponseReady())
    {
        WIFI_DEBUG("WaitResponse: ResponseReady failed") ;
        gpio_put(kPinWifiCs, 1) ;
        return false ;
    }

    // Small delay to let SPI data settle
    sleep_us(50) ;

    if (!WaitForReady())
    {
        WIFI_DEBUG("WaitResponse: ForReady failed") ;
        gpio_put(kPinWifiCs, 1) ;
        return false ;
    }

    // Additional delay before reading
    sleep_us(10) ;

    // Check start byte
    uint8_t theStartByte = 0 ;
    ReadAndCheckByte(0, &theStartByte) ;
    if (theStartByte != NINA_START_CMD)
    {
        WIFI_DEBUG("Bad start byte: 0x%02X", theStartByte) ;
        gpio_put(kPinWifiCs, 1) ;
        return false ;
    }

    // Check command byte (with reply flag)
    uint8_t theCmdByte = 0 ;
    ReadAndCheckByte(0, &theCmdByte) ;
    if ((theCmdByte & ~NINA_REPLY_FLAG) != (inCmd & ~NINA_REPLY_FLAG))
    {
        WIFI_DEBUG("Bad cmd byte: 0x%02X", theCmdByte) ;
        gpio_put(kPinWifiCs, 1) ;
        return false ;
    }

    // Get number of params
    uint8_t theNumParams = SpiTransfer(NINA_DUMMY_DATA) ;
    if (outNumParams != NULL)
    {
        *outNumParams = theNumParams ;
    }

    return true ;
}

static uint8_t ReadParamByte(void)
{
    uint8_t theLen = SpiTransfer(NINA_DUMMY_DATA) ;
    if (theLen != 1)
    {
        WIFI_DEBUG("ReadParamByte: len=%d", theLen) ;
        return 0 ;
    }
    return SpiTransfer(NINA_DUMMY_DATA) ;
}

static uint16_t ReadParamWord(void)
{
    uint8_t theLen = SpiTransfer(NINA_DUMMY_DATA) ;
    if (theLen != 2)
    {
        WIFI_DEBUG("ReadParamWord: len=%d", theLen) ;
        return 0 ;
    }
    uint16_t theHigh = SpiTransfer(NINA_DUMMY_DATA) ;
    uint16_t theLow = SpiTransfer(NINA_DUMMY_DATA) ;
    return (theHigh << 8) | theLow ;
}

static uint8_t ReadParam(uint8_t * outParam, uint8_t inMaxLen)
{
    uint8_t theLen = SpiTransfer(NINA_DUMMY_DATA) ;
    if (theLen > inMaxLen)
    {
        theLen = inMaxLen ;
    }
    for (uint8_t i = 0 ; i < theLen ; i++)
    {
        outParam[i] = SpiTransfer(NINA_DUMMY_DATA) ;
    }
    return theLen ;
}

static void EndResponse(void)
{
    // Read and discard end byte
    SpiTransfer(NINA_DUMMY_DATA) ;
    gpio_put(kPinWifiCs, 1) ;
}

//----------------------------------------------
// Public API Implementation
//----------------------------------------------

bool WiFi_Init(void)
{
    if (sInitialized)
    {
        return true ;
    }

    // Ensure LoRa CS is high (deselected) to avoid bus contention
    gpio_put(kPinLoRaCs, 1) ;

    // Initialize SPI (shared with LoRa)
    spi_init(kSpiWifiPort, kSpiWifiBaudrate) ;
    spi_set_format(kSpiWifiPort, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST) ;

    gpio_set_function(kPinWifiSck, GPIO_FUNC_SPI) ;
    gpio_set_function(kPinWifiMosi, GPIO_FUNC_SPI) ;
    gpio_set_function(kPinWifiMiso, GPIO_FUNC_SPI) ;

    // Initialize control pins
    gpio_init(kPinWifiCs) ;
    gpio_set_dir(kPinWifiCs, GPIO_OUT) ;
    gpio_put(kPinWifiCs, 1) ;  // CS high (deselected)

    gpio_init(kPinWifiBusy) ;
    gpio_set_dir(kPinWifiBusy, GPIO_IN) ;

    gpio_init(kPinWifiReset) ;
    gpio_set_dir(kPinWifiReset, GPIO_OUT) ;

    // Reset ESP32
    gpio_put(kPinWifiReset, 0) ;
    sleep_ms(10) ;
    gpio_put(kPinWifiReset, 1) ;
    sleep_ms(750) ;  // Wait for ESP32 to boot

    // Check if ESP32 responds by getting firmware version
    char theVersion[12] ;
    if (!WiFi_GetFirmwareVersion(theVersion))
    {
        printf("  WiFi: ERROR - ESP32 not responding\n") ;
        return false ;
    }

    printf("  WiFi: NINA firmware v%s\n", theVersion) ;
    sInitialized = true ;
    return true ;
}

bool WiFi_GetFirmwareVersion(char * outVersion)
{
    if (!SendCmd(NINA_CMD_GET_FW_VERSION, 0))
    {
        return false ;
    }
    EndCmd() ;

    uint8_t theNumParams = 0 ;
    if (!WaitResponse(NINA_CMD_GET_FW_VERSION | NINA_REPLY_FLAG, &theNumParams))
    {
        return false ;
    }

    if (theNumParams >= 1)
    {
        ReadParam((uint8_t *)outVersion, 11) ;
        outVersion[11] = '\0' ;
    }

    EndResponse() ;
    return true ;
}

bool WiFi_GetMacAddress(uint8_t * outMac)
{
    // Request MAC for dummy interface 0xFF to get own MAC
    uint8_t theDummy = 0xFF ;

    SendCmd(NINA_CMD_GET_MACADDR, 1) ;
    SendParam(&theDummy, 1) ;
    EndCmd() ;

    uint8_t theNumParams = 0 ;
    if (!WaitResponse(NINA_CMD_GET_MACADDR | NINA_REPLY_FLAG, &theNumParams))
    {
        return false ;
    }

    if (theNumParams >= 1)
    {
        ReadParam(outMac, 6) ;
    }

    EndResponse() ;
    return true ;
}

bool WiFi_StartAP(const char * inSsid, const char * inPassword, uint8_t inChannel)
{
    WIFI_DEBUG("StartAP: SSID=%s, channel=%d", inSsid, inChannel) ;

    // Wait for any previous command to complete
    sleep_ms(50) ;

    uint8_t theSsidLen = strlen(inSsid) ;
    if (theSsidLen > WIFI_MAX_SSID_LEN)
    {
        theSsidLen = WIFI_MAX_SSID_LEN ;
    }

    uint8_t theCmd ;
    if (inPassword != NULL && strlen(inPassword) >= 8)
    {
        // WPA2 AP with password
        uint8_t thePassLen = strlen(inPassword) ;
        if (thePassLen > WIFI_MAX_PASS_LEN)
        {
            thePassLen = WIFI_MAX_PASS_LEN ;
        }

        theCmd = NINA_CMD_SET_AP_PASSPHRASE ;
        if (!SendCmd(theCmd, 3))
        {
            return false ;
        }
        SendParam((const uint8_t *)inSsid, theSsidLen) ;
        SendParam((const uint8_t *)inPassword, thePassLen) ;
        SendParamByte(inChannel) ;
    }
    else
    {
        // Open AP (no password)
        theCmd = NINA_CMD_SET_AP_NET ;
        if (!SendCmd(theCmd, 2))
        {
            return false ;
        }
        SendParam((const uint8_t *)inSsid, theSsidLen) ;
        SendParamByte(inChannel) ;
    }

    EndCmd() ;

    // AP mode takes time - wait for ESP32 to process
    sleep_ms(500) ;

    uint8_t theNumParams = 0 ;
    if (!WaitResponse(theCmd | NINA_REPLY_FLAG, &theNumParams))
    {
        return false ;
    }

    uint8_t theResult = 0 ;
    if (theNumParams >= 1)
    {
        theResult = ReadParamByte() ;
    }

    EndResponse() ;

    if (theResult != 1)
    {
        WIFI_DEBUG("AP command failed: %d", theResult) ;
        return false ;
    }

    // Give AP time to initialize
    sleep_ms(2000) ;

    // Drain any stale data from SPI bus before next command
    DrainSpi() ;

    return true ;
}

bool WiFi_Connect(const char * inSsid, const char * inPassword)
{
    WIFI_DEBUG("Connecting to: %s", inSsid) ;

    uint8_t theSsidLen = strlen(inSsid) ;
    if (theSsidLen > WIFI_MAX_SSID_LEN)
    {
        theSsidLen = WIFI_MAX_SSID_LEN ;
    }

    if (inPassword != NULL && strlen(inPassword) > 0)
    {
        // WPA/WPA2 connection with password
        uint8_t thePassLen = strlen(inPassword) ;
        if (thePassLen > WIFI_MAX_PASS_LEN)
        {
            thePassLen = WIFI_MAX_PASS_LEN ;
        }

        SendCmd(NINA_CMD_SET_PASSPHRASE, 2) ;
        SendParam((const uint8_t *)inSsid, theSsidLen) ;
        SendParam((const uint8_t *)inPassword, thePassLen) ;
    }
    else
    {
        // Open network (no password)
        SendCmd(NINA_CMD_SET_NET, 1) ;
        SendParam((const uint8_t *)inSsid, theSsidLen) ;
    }

    EndCmd() ;

    uint8_t theNumParams = 0 ;
    uint8_t theCmd = (inPassword != NULL && strlen(inPassword) > 0) ?
        NINA_CMD_SET_PASSPHRASE : NINA_CMD_SET_NET ;

    if (!WaitResponse(theCmd | NINA_REPLY_FLAG, &theNumParams))
    {
        return false ;
    }

    uint8_t theResult = 0 ;
    if (theNumParams >= 1)
    {
        theResult = ReadParamByte() ;
    }

    EndResponse() ;

    if (theResult != 1)
    {
        WIFI_DEBUG("Connect request failed: %d", theResult) ;
        return false ;
    }

    return true ;
}

bool WiFi_WaitForConnection(uint32_t inTimeoutMs)
{
    uint32_t theStart = to_ms_since_boot(get_absolute_time()) ;

    while (to_ms_since_boot(get_absolute_time()) - theStart < inTimeoutMs)
    {
        WifiStatus theStatus = WiFi_GetStatus() ;

        if (theStatus == kWifiStatusConnected)
        {
            WIFI_DEBUG("Connected!") ;
            return true ;
        }

        if (theStatus == kWifiStatusConnectFailed ||
            theStatus == kWifiStatusNoSsidAvail ||
            theStatus == kWifiStatusDisconnected)
        {
            WIFI_DEBUG("Connection failed: status=%d", theStatus) ;
            return false ;
        }

        sleep_ms(100) ;
    }

    WIFI_DEBUG("Connection timeout") ;
    return false ;
}

bool WiFi_GetSSID(char * outSsid)
{
    SendCmd(NINA_CMD_GET_CURR_SSID, 1) ;
    uint8_t theDummy = 0xFF ;
    SendParam(&theDummy, 1) ;
    EndCmd() ;

    uint8_t theNumParams = 0 ;
    if (!WaitResponse(NINA_CMD_GET_CURR_SSID | NINA_REPLY_FLAG, &theNumParams))
    {
        return false ;
    }

    if (theNumParams >= 1)
    {
        uint8_t theLen = ReadParam((uint8_t *)outSsid, WIFI_MAX_SSID_LEN) ;
        outSsid[theLen] = '\0' ;
    }
    else
    {
        outSsid[0] = '\0' ;
    }

    EndResponse() ;
    return (outSsid[0] != '\0') ;
}

int8_t WiFi_GetRSSI(void)
{
    SendCmd(NINA_CMD_GET_CURR_RSSI, 1) ;
    uint8_t theDummy = 0xFF ;
    SendParam(&theDummy, 1) ;
    EndCmd() ;

    uint8_t theNumParams = 0 ;
    if (!WaitResponse(NINA_CMD_GET_CURR_RSSI | NINA_REPLY_FLAG, &theNumParams))
    {
        return 0 ;
    }

    int8_t theRssi = 0 ;
    if (theNumParams >= 1)
    {
        theRssi = (int8_t)ReadParamByte() ;
    }

    EndResponse() ;
    return theRssi ;
}

WifiStatus WiFi_GetStatus(void)
{
    SendCmd(NINA_CMD_GET_CONN_STATUS, 0) ;
    EndCmd() ;

    uint8_t theNumParams = 0 ;
    if (!WaitResponse(NINA_CMD_GET_CONN_STATUS | NINA_REPLY_FLAG, &theNumParams))
    {
        return kWifiStatusDisconnected ;
    }

    uint8_t theStatus = kWifiStatusDisconnected ;
    if (theNumParams >= 1)
    {
        theStatus = ReadParamByte() ;
    }

    EndResponse() ;
    return (WifiStatus)theStatus ;
}

bool WiFi_GetLocalIP(uint8_t * outIp)
{
    // Use dummy interface to get own IP
    uint8_t theDummy = 0 ;

    SendCmd(NINA_CMD_GET_IPADDR, 1) ;
    SendParam(&theDummy, 1) ;
    EndCmd() ;

    uint8_t theNumParams = 0 ;
    if (!WaitResponse(NINA_CMD_GET_IPADDR | NINA_REPLY_FLAG, &theNumParams))
    {
        return false ;
    }

    if (theNumParams >= 1)
    {
        ReadParam(outIp, 4) ;
    }

    EndResponse() ;

    // Check if we have a valid IP (not 0.0.0.0)
    return (outIp[0] != 0 || outIp[1] != 0 || outIp[2] != 0 || outIp[3] != 0) ;
}

int8_t WiFi_StartServer(uint16_t inPort)
{
    WIFI_DEBUG("Starting TCP server on port %d", inPort) ;

    // Drain any stale data from previous commands
    DrainSpi() ;
    sleep_ms(100) ;

    // Step 1: Get a socket from ESP32 (like Adafruit does)
    SendCmd(NINA_CMD_GET_SOCKET, 0) ;
    EndCmd() ;

    uint8_t theNumParams = 0 ;
    if (!WaitResponse(NINA_CMD_GET_SOCKET | NINA_REPLY_FLAG, &theNumParams))
    {
        printf("  WiFi: GET_SOCKET failed\n") ;
        return -1 ;
    }

    uint8_t theSocket = 0xFF ;
    if (theNumParams >= 1)
    {
        theSocket = ReadParamByte() ;
    }
    EndResponse() ;

    printf("  WiFi: Got socket %d for server\n", theSocket) ;

    if (theSocket == 0xFF)
    {
        printf("  WiFi: No socket available\n") ;
        return -1 ;
    }

    // Step 2: Start server (3 params: port, socket, protocol - like Adafruit)
    SendCmd(NINA_CMD_START_SERVER_TCP, 3) ;
    SendParamWord(inPort) ;         // Port (2 bytes)
    SendParamByte(theSocket) ;      // Socket (1 byte)
    SendParamByte(0) ;              // Protocol: 0 = TCP
    EndCmd() ;

    // Wait for response this time
    if (!WaitResponse(NINA_CMD_START_SERVER_TCP | NINA_REPLY_FLAG, &theNumParams))
    {
        printf("  WiFi: START_SERVER failed\n") ;
        // Continue anyway - Adafruit doesn't check response
    }
    else
    {
        uint8_t theResult = 0 ;
        if (theNumParams >= 1) { theResult = ReadParamByte() ; }
        EndResponse() ;
        printf("  WiFi: START_SERVER result=%d\n", theResult) ;
    }

    // Give server time to start
    sleep_ms(500) ;

    // Verify socket state
    SendCmd(NINA_CMD_GET_STATE_TCP, 1) ;
    SendParamByte(theSocket) ;
    EndCmd() ;

    if (WaitResponse(NINA_CMD_GET_STATE_TCP | NINA_REPLY_FLAG, &theNumParams))
    {
        uint8_t theState = 0 ;
        if (theNumParams >= 1) { theState = ReadParamByte() ; }
        EndResponse() ;
        printf("  WiFi: Socket %d state after start: %d (1=LISTEN)\n", theSocket, theState) ;
    }

    printf("  WiFi: Server on socket %d, port %d\n", theSocket, inPort) ;
    sServerSocket = theSocket ;
    return theSocket ;
}

// Get socket state for a specific socket
uint8_t WiFi_GetSocketState(int8_t inSocket)
{
    if (inSocket < 0)
    {
        return 0 ;
    }

    SendCmd(NINA_CMD_GET_STATE_TCP, 1) ;
    SendParamByte((uint8_t)inSocket) ;
    EndCmd() ;

    uint8_t theNumParams = 0 ;
    if (!WaitResponse(NINA_CMD_GET_STATE_TCP | NINA_REPLY_FLAG, &theNumParams))
    {
        return 0 ;
    }

    uint8_t theState = 0 ;
    if (theNumParams >= 1)
    {
        // Read param: length byte then data byte (all in one CS transaction)
        uint8_t theLen = SpiTransfer(NINA_DUMMY_DATA) ;
        uint8_t theData = SpiTransfer(NINA_DUMMY_DATA) ;

        // Debug: show raw bytes once per socket
        static uint8_t sDebugMask = 0x0F ;
        if (sDebugMask & (1 << inSocket))
        {
            printf("  STATE[%d]: len=%d data=%d\n", inSocket, theLen, theData) ;
            sDebugMask &= ~(1 << inSocket) ;
        }

        theState = (theLen == 1) ? theData : theLen ;
    }

    EndResponse() ;
    return theState ;
}

// Debug: dump all socket states
void WiFi_DumpSockets(void)
{
    printf("Socket states: ") ;
    for (int sock = 0 ; sock < 4 ; sock++)
    {
        uint8_t state = WiFi_GetSocketState(sock) ;
        printf("%d=%d ", sock, state) ;
    }
    printf("\n") ;
}

int8_t WiFi_ServerHasClient(int8_t inServerSocket)
{
    if (inServerSocket < 0)
    {
        return -1 ;
    }

    // Rate limit: only check every 3000ms to keep UI responsive
    static uint32_t sLastCheck = 0 ;
    uint32_t now = to_ms_since_boot(get_absolute_time()) ;
    if (now - sLastCheck < 3000)
    {
        return sClientSocket ;  // Return cached result
    }
    sLastCheck = now ;

    // Check ALL sockets (0-3) for ESTABLISHED or CLOSE_WAIT
    for (int sock = 0 ; sock < 4 ; sock++)
    {
        uint8_t state = WiFi_GetSocketState(sock) ;

        if (state == kTcpStateEstablished || state == 7)
        {
            printf("WiFi: Client on sock %d (state=%d)\n", sock, state) ;
            sClientSocket = sock ;
            return sock ;
        }
    }

    // Every 15 seconds: try sending test message to all sockets
    static uint32_t sLastTest = 0 ;
    if (now - sLastTest > 15000)
    {
        printf("WiFi: Testing write to sockets...\n") ;
        const char * testMsg = "GATEWAY_TEST\n" ;
        for (int sock = 0 ; sock < 4 ; sock++)
        {
            int n = WiFi_Write(sock, (const uint8_t *)testMsg, strlen(testMsg)) ;
            printf("  Sock %d write: %d\n", sock, n) ;
        }
        sLastTest = now ;
    }

    sClientSocket = -1 ;
    return -1 ;
}

bool WiFi_ClientConnected(int8_t inSocket)
{
    if (inSocket < 0)
    {
        return false ;
    }

    SendCmd(NINA_CMD_GET_STATE_TCP, 1) ;
    SendParamByte((uint8_t)inSocket) ;
    EndCmd() ;

    uint8_t theNumParams = 0 ;
    if (!WaitResponse(NINA_CMD_GET_STATE_TCP | NINA_REPLY_FLAG, &theNumParams))
    {
        return false ;
    }

    uint8_t theState = 0 ;
    if (theNumParams >= 1)
    {
        theState = ReadParamByte() ;
    }

    EndResponse() ;

    return (theState == kTcpStateEstablished) ;
}

uint16_t WiFi_Available(int8_t inSocket)
{
    if (inSocket < 0)
    {
        return 0 ;
    }

    SendCmd(NINA_CMD_AVAIL_DATA_TCP, 1) ;
    SendParamByte((uint8_t)inSocket) ;
    EndCmd() ;

    uint8_t theNumParams = 0 ;
    if (!WaitResponse(NINA_CMD_AVAIL_DATA_TCP | NINA_REPLY_FLAG, &theNumParams))
    {
        return 0 ;
    }

    uint16_t theAvail = 0 ;
    if (theNumParams >= 1)
    {
        // Read all in one CS transaction
        uint8_t theLen = SpiTransfer(NINA_DUMMY_DATA) ;
        uint8_t b1 = SpiTransfer(NINA_DUMMY_DATA) ;
        uint8_t b2 = SpiTransfer(NINA_DUMMY_DATA) ;

        // Debug: show once
        static bool sDebugOnce = true ;
        if (sDebugOnce && inSocket == 0)
        {
            printf("  AVAIL[0]: len=%d bytes=[%02X %02X]\n", theLen, b1, b2) ;
            sDebugOnce = false ;
        }

        // Interpret based on length
        if (theLen == 1)
        {
            theAvail = b1 ;
        }
        else if (theLen == 2)
        {
            theAvail = (b1 << 8) | b2 ;
        }
    }

    EndResponse() ;
    return theAvail ;
}

int WiFi_Read(int8_t inSocket, uint8_t * outBuffer, uint16_t inMaxLen)
{
    if (inSocket < 0 || outBuffer == NULL || inMaxLen == 0)
    {
        return -1 ;
    }

    // Check how much data is available
    uint16_t theAvail = WiFi_Available(inSocket) ;
    if (theAvail == 0)
    {
        return 0 ;
    }

    uint16_t theReadLen = (theAvail < inMaxLen) ? theAvail : inMaxLen ;

    SendCmd(NINA_CMD_GET_DATABUF_TCP, 2) ;
    SendParamByte((uint8_t)inSocket) ;
    SendParamWord(theReadLen) ;
    EndCmd() ;

    uint8_t theNumParams = 0 ;
    if (!WaitResponse(NINA_CMD_GET_DATABUF_TCP | NINA_REPLY_FLAG, &theNumParams))
    {
        return -1 ;
    }

    uint16_t theActualLen = 0 ;
    if (theNumParams >= 1)
    {
        // Length is sent first as a word
        uint8_t theLenHigh = SpiTransfer(NINA_DUMMY_DATA) ;
        uint8_t theLenLow = SpiTransfer(NINA_DUMMY_DATA) ;
        theActualLen = (theLenHigh << 8) | theLenLow ;

        if (theActualLen > inMaxLen)
        {
            theActualLen = inMaxLen ;
        }

        // Read data bytes
        for (uint16_t i = 0 ; i < theActualLen ; i++)
        {
            outBuffer[i] = SpiTransfer(NINA_DUMMY_DATA) ;
        }
    }

    EndResponse() ;
    return theActualLen ;
}

int WiFi_Write(int8_t inSocket, const uint8_t * inData, uint16_t inLen)
{
    if (inSocket < 0 || inData == NULL || inLen == 0)
    {
        return -1 ;
    }

    // Insert data into buffer first
    SendCmd(NINA_CMD_INSERT_DATABUF, 2) ;
    SendParamByte((uint8_t)inSocket) ;

    // Send length as 2-byte param
    SpiTransfer((uint8_t)(inLen >> 8)) ;
    SpiTransfer((uint8_t)(inLen & 0xFF)) ;

    // Send data bytes
    for (uint16_t i = 0 ; i < inLen ; i++)
    {
        SpiTransfer(inData[i]) ;
    }

    EndCmd() ;

    uint8_t theNumParams = 0 ;
    if (!WaitResponse(NINA_CMD_INSERT_DATABUF | NINA_REPLY_FLAG, &theNumParams))
    {
        return -1 ;
    }

    uint16_t theInserted = 0 ;
    if (theNumParams >= 1)
    {
        theInserted = ReadParamWord() ;
    }

    EndResponse() ;

    if (theInserted != inLen)
    {
        WIFI_DEBUG("Insert failed: %d vs %d", theInserted, inLen) ;
        return -1 ;
    }

    // Now send the buffered data
    SendCmd(NINA_CMD_SEND_DATA_TCP, 1) ;
    SendParamByte((uint8_t)inSocket) ;
    EndCmd() ;

    if (!WaitResponse(NINA_CMD_SEND_DATA_TCP | NINA_REPLY_FLAG, &theNumParams))
    {
        return -1 ;
    }

    uint8_t theSent = 0 ;
    if (theNumParams >= 1)
    {
        theSent = ReadParamByte() ;
    }

    EndResponse() ;

    if (theSent != 1)
    {
        WIFI_DEBUG("Send failed: %d", theSent) ;
        return -1 ;
    }

    return inLen ;
}

void WiFi_CloseClient(int8_t inSocket)
{
    if (inSocket < 0)
    {
        return ;
    }

    SendCmd(NINA_CMD_STOP_CLIENT_TCP, 1) ;
    SendParamByte((uint8_t)inSocket) ;
    EndCmd() ;

    uint8_t theNumParams = 0 ;
    WaitResponse(NINA_CMD_STOP_CLIENT_TCP | NINA_REPLY_FLAG, &theNumParams) ;
    EndResponse() ;

    if (inSocket == sClientSocket)
    {
        sClientSocket = -1 ;
    }
}

void WiFi_Disconnect(void)
{
    SendCmd(NINA_CMD_DISCONNECT, 1) ;
    uint8_t theDummy = 0 ;
    SendParam(&theDummy, 1) ;
    EndCmd() ;

    uint8_t theNumParams = 0 ;
    WaitResponse(NINA_CMD_DISCONNECT | NINA_REPLY_FLAG, &theNumParams) ;
    EndResponse() ;

    sServerSocket = -1 ;
    sClientSocket = -1 ;
}

void WiFi_Poll(void)
{
    // Placeholder for any periodic WiFi maintenance
    // Currently not needed as all operations are synchronous
}

