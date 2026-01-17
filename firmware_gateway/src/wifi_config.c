/**
 * WiFi Configuration Storage Implementation
 */

#include "wifi_config.h"
#include "pins.h"

#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

// Flash storage location
// Feather RP2040 RFM95 has 8MB flash (0x800000)
// Use second-to-last sector for WiFi config (last sector reserved for other uses)
#define kFeatherFlashSize           0x800000    // 8MB
#define WIFI_CONFIG_FLASH_OFFSET    (kFeatherFlashSize - (FLASH_SECTOR_SIZE * 2))
#define WIFI_CONFIG_MAGIC           0x57494649  // "WIFI" in ASCII

// Pointer to flash storage location (read as memory-mapped)
#define kFlashStoragePtr ((const WifiConfig_t *)(XIP_BASE + WIFI_CONFIG_FLASH_OFFSET))

// Static configuration and status
static WifiConfig_t sConfig;
static WifiStatus_t sStatus;
static bool sInitialized = false;

// Default AP settings
#define DEFAULT_AP_SSID     "RocketGateway"
#define DEFAULT_AP_PASSWORD ""  // Empty for open AP (testing)
#define DEFAULT_AP_CHANNEL  6

/**
 * Initialize with defaults
 */
static void InitDefaults(void) {
    memset(&sConfig, 0, sizeof(sConfig));

    sConfig.magic = WIFI_CONFIG_MAGIC;
    sConfig.version = 1;
    sConfig.networkCount = 0;

    // Default AP settings
    strncpy(sConfig.apSsid, DEFAULT_AP_SSID, WIFI_CONFIG_SSID_MAX_LEN);
    strncpy(sConfig.apPassword, DEFAULT_AP_PASSWORD, WIFI_CONFIG_PASS_MAX_LEN);
    sConfig.apChannel = DEFAULT_AP_CHANNEL;

    // Initialize status
    memset(&sStatus, 0, sizeof(sStatus));
    sStatus.mode = kWifiModeNone;
}

/**
 * Calculate checksum for config data
 */
static uint32_t CalculateChecksum(const WifiConfig_t* inConfig) {
    uint32_t theChecksum = 0;
    const uint8_t* theBytes = (const uint8_t*)inConfig;
    // Sum all bytes except the checksum field itself
    for (size_t i = 0; i < offsetof(WifiConfig_t, checksum); i++) {
        theChecksum += theBytes[i];
    }
    return theChecksum;
}

/**
 * Validate stored config
 */
static bool ValidateConfig(void) {
    // Check magic number
    if (kFlashStoragePtr->magic != WIFI_CONFIG_MAGIC) {
        return false;
    }

    // Check version
    if (kFlashStoragePtr->version > 1) {
        return false;
    }

    // Verify checksum
    uint32_t theExpected = CalculateChecksum(kFlashStoragePtr);
    if (theExpected != kFlashStoragePtr->checksum) {
        return false;
    }

    return true;
}

/**
 * Load configuration from flash
 */
static bool LoadFromFlash(void) {
    // Validate the stored config
    if (!ValidateConfig()) {
        printf("[WiFi Config] No valid config in flash, using defaults\n");
        return false;
    }

    // Copy config from flash (memory-mapped read)
    memcpy(&sConfig, kFlashStoragePtr, sizeof(sConfig));

    printf("[WiFi Config] Loaded %d networks from flash\n", sConfig.networkCount);
    return true;
}

void WifiConfig_Init(void) {
    if (sInitialized) {
        return;
    }

    // Initialize with defaults first
    InitDefaults();

    // Try to load from flash
    LoadFromFlash();

    sInitialized = true;
}

const WifiConfig_t* WifiConfig_Get(void) {
    return &sConfig;
}

const WifiStatus_t* WifiConfig_GetStatus(void) {
    return &sStatus;
}

void WifiConfig_SetStatus(WifiMode_t inMode, const char* inSsid,
                          const uint8_t* inIp, int8_t inRssi, bool inConnected) {
    sStatus.mode = inMode;

    if (inSsid != NULL) {
        strncpy(sStatus.ssid, inSsid, WIFI_CONFIG_SSID_MAX_LEN);
        sStatus.ssid[WIFI_CONFIG_SSID_MAX_LEN] = '\0';
    } else {
        sStatus.ssid[0] = '\0';
    }

    if (inIp != NULL) {
        memcpy(sStatus.ip, inIp, 4);
    } else {
        memset(sStatus.ip, 0, 4);
    }

    sStatus.rssi = inRssi;
    sStatus.connected = inConnected;
}

int WifiConfig_AddNetwork(const char* inSsid, const char* inPassword, uint8_t inPriority) {
    if (inSsid == NULL || strlen(inSsid) == 0) {
        return -1;
    }

    // Check if network already exists (update it)
    for (int i = 0; i < sConfig.networkCount; i++) {
        if (strcmp(sConfig.networks[i].ssid, inSsid) == 0) {
            // Update existing
            if (inPassword != NULL) {
                strncpy(sConfig.networks[i].password, inPassword, WIFI_CONFIG_PASS_MAX_LEN);
                sConfig.networks[i].password[WIFI_CONFIG_PASS_MAX_LEN] = '\0';
            }
            sConfig.networks[i].priority = inPriority;
            sConfig.networks[i].enabled = true;
            printf("[WiFi Config] Updated network: %s\n", inSsid);
            return i;
        }
    }

    // Check if we have room
    if (sConfig.networkCount >= WIFI_CONFIG_MAX_NETWORKS) {
        printf("[WiFi Config] Network list full\n");
        return -1;
    }

    // Add new network
    int idx = sConfig.networkCount;
    strncpy(sConfig.networks[idx].ssid, inSsid, WIFI_CONFIG_SSID_MAX_LEN);
    sConfig.networks[idx].ssid[WIFI_CONFIG_SSID_MAX_LEN] = '\0';

    if (inPassword != NULL) {
        strncpy(sConfig.networks[idx].password, inPassword, WIFI_CONFIG_PASS_MAX_LEN);
        sConfig.networks[idx].password[WIFI_CONFIG_PASS_MAX_LEN] = '\0';
    } else {
        sConfig.networks[idx].password[0] = '\0';
    }

    sConfig.networks[idx].priority = inPriority;
    sConfig.networks[idx].enabled = true;
    sConfig.networkCount++;

    printf("[WiFi Config] Added network: %s (priority %d)\n", inSsid, inPriority);
    return idx;
}

bool WifiConfig_RemoveNetwork(const char* inSsid) {
    if (inSsid == NULL) {
        return false;
    }

    for (int i = 0; i < sConfig.networkCount; i++) {
        if (strcmp(sConfig.networks[i].ssid, inSsid) == 0) {
            return WifiConfig_RemoveNetworkByIndex(i);
        }
    }

    return false;
}

bool WifiConfig_RemoveNetworkByIndex(uint8_t inIndex) {
    if (inIndex >= sConfig.networkCount) {
        return false;
    }

    printf("[WiFi Config] Removing network: %s\n", sConfig.networks[inIndex].ssid);

    // Shift remaining networks down
    for (int i = inIndex; i < sConfig.networkCount - 1; i++) {
        memcpy(&sConfig.networks[i], &sConfig.networks[i + 1], sizeof(WifiNetwork_t));
    }

    // Clear last slot
    memset(&sConfig.networks[sConfig.networkCount - 1], 0, sizeof(WifiNetwork_t));
    sConfig.networkCount--;

    return true;
}

bool WifiConfig_SetNetworkEnabled(uint8_t inIndex, bool inEnabled) {
    if (inIndex >= sConfig.networkCount) {
        return false;
    }

    sConfig.networks[inIndex].enabled = inEnabled;
    return true;
}

void WifiConfig_ClearNetworks(void) {
    for (int i = 0; i < WIFI_CONFIG_MAX_NETWORKS; i++) {
        memset(&sConfig.networks[i], 0, sizeof(WifiNetwork_t));
    }
    sConfig.networkCount = 0;
    printf("[WiFi Config] Cleared all networks\n");
}

void WifiConfig_SetAP(const char* inSsid, const char* inPassword, uint8_t inChannel) {
    if (inSsid != NULL && strlen(inSsid) > 0) {
        strncpy(sConfig.apSsid, inSsid, WIFI_CONFIG_SSID_MAX_LEN);
        sConfig.apSsid[WIFI_CONFIG_SSID_MAX_LEN] = '\0';
    }

    if (inPassword != NULL) {
        strncpy(sConfig.apPassword, inPassword, WIFI_CONFIG_PASS_MAX_LEN);
        sConfig.apPassword[WIFI_CONFIG_PASS_MAX_LEN] = '\0';
    }

    if (inChannel >= 1 && inChannel <= 13) {
        sConfig.apChannel = inChannel;
    }

    printf("[WiFi Config] AP config: SSID=%s, Channel=%d\n", sConfig.apSsid, sConfig.apChannel);
}

bool WifiConfig_Save(void) {
    printf("[WiFi Config] Saving to flash...\n");

    // Calculate and set checksum
    sConfig.checksum = CalculateChecksum(&sConfig);

    // Calculate buffer size needed (aligned to FLASH_PAGE_SIZE)
    size_t theConfigSize = sizeof(sConfig);
    size_t theNumPages = (theConfigSize + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
    size_t theBufferSize = theNumPages * FLASH_PAGE_SIZE;

    // Prepare data (must be aligned to 256 bytes for flash write)
    uint8_t buffer[FLASH_SECTOR_SIZE];  // Use sector size as max (4096 bytes)
    memset(buffer, 0xFF, theBufferSize);  // Flash erases to 0xFF
    memcpy(buffer, &sConfig, theConfigSize);

    // Flush stdio before flash operations (USB CDC may stall)
    stdio_flush();

    // Disable interrupts during flash operations
    uint32_t ints = save_and_disable_interrupts();

    // Erase sector
    flash_range_erase(WIFI_CONFIG_FLASH_OFFSET, FLASH_SECTOR_SIZE);

    // Write data (all pages)
    flash_range_program(WIFI_CONFIG_FLASH_OFFSET, buffer, theBufferSize);

    // Re-enable interrupts
    restore_interrupts(ints);

    printf("[WiFi Config] Saved %u bytes to flash\n", (unsigned)theConfigSize);

    // Verify write by validating stored config
    return ValidateConfig();
}

const WifiNetwork_t* WifiConfig_GetNetworkByPriority(uint8_t inPriority) {
    const WifiNetwork_t* best = NULL;
    uint8_t bestPriority = 255;

    for (int i = 0; i < sConfig.networkCount; i++) {
        if (sConfig.networks[i].enabled &&
            sConfig.networks[i].priority >= inPriority &&
            sConfig.networks[i].priority < bestPriority) {
            best = &sConfig.networks[i];
            bestPriority = sConfig.networks[i].priority;
        }
    }

    return best;
}

uint8_t WifiConfig_GetEnabledCount(void) {
    uint8_t count = 0;
    for (int i = 0; i < sConfig.networkCount; i++) {
        if (sConfig.networks[i].enabled) {
            count++;
        }
    }
    return count;
}
