/**
 * WiFi Configuration Storage
 *
 * Stores multiple WiFi station credentials in flash memory.
 * Gateway tries each configured station in order, falls back to AP mode if all fail.
 */

#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

// Maximum number of stored WiFi networks
#define WIFI_CONFIG_MAX_NETWORKS    4

// Maximum lengths for SSID and password
#define WIFI_CONFIG_SSID_MAX_LEN    32
#define WIFI_CONFIG_PASS_MAX_LEN    64

// WiFi mode
typedef enum {
    kWifiModeNone = 0,      // Not connected
    kWifiModeStation,       // Connected to existing network
    kWifiModeAP             // Running as access point
} WifiMode_t;

// Single WiFi network configuration
typedef struct {
    char ssid[WIFI_CONFIG_SSID_MAX_LEN + 1];
    char password[WIFI_CONFIG_PASS_MAX_LEN + 1];
    uint8_t priority;       // Lower = higher priority (0 = first to try)
    bool enabled;           // Whether to try this network
} WifiNetwork_t;

// Full WiFi configuration
typedef struct {
    uint32_t magic;         // Magic number to validate flash data
    uint8_t version;        // Config version for future compatibility
    uint8_t networkCount;   // Number of configured networks
    uint8_t reserved[2];    // Padding for alignment
    WifiNetwork_t networks[WIFI_CONFIG_MAX_NETWORKS];

    // AP mode settings (fallback)
    char apSsid[WIFI_CONFIG_SSID_MAX_LEN + 1];
    char apPassword[WIFI_CONFIG_PASS_MAX_LEN + 1];
    uint8_t apChannel;
    uint8_t reserved2[3];   // Padding for alignment

    uint32_t checksum;      // Data integrity checksum (must be last!)
} WifiConfig_t;

// Current WiFi status
typedef struct {
    WifiMode_t mode;
    char ssid[WIFI_CONFIG_SSID_MAX_LEN + 1];    // Connected SSID or AP SSID
    uint8_t ip[4];                               // Current IP address
    int8_t rssi;                                 // Signal strength (station mode)
    bool connected;                              // Client connected (AP mode) or connected to network (station)
} WifiStatus_t;

/**
 * Initialize WiFi configuration system
 * Loads config from flash or creates default
 */
void WifiConfig_Init(void);

/**
 * Get current configuration (read-only)
 */
const WifiConfig_t* WifiConfig_Get(void);

/**
 * Get current WiFi status
 */
const WifiStatus_t* WifiConfig_GetStatus(void);

/**
 * Update WiFi status (called by wifi_nina)
 */
void WifiConfig_SetStatus(WifiMode_t inMode, const char* inSsid,
                          const uint8_t* inIp, int8_t inRssi, bool inConnected);

/**
 * Add or update a station network
 * Returns index on success, -1 on failure (full)
 */
int WifiConfig_AddNetwork(const char* inSsid, const char* inPassword, uint8_t inPriority);

/**
 * Remove a station network by SSID
 * Returns true if found and removed
 */
bool WifiConfig_RemoveNetwork(const char* inSsid);

/**
 * Remove a station network by index
 * Returns true if valid index
 */
bool WifiConfig_RemoveNetworkByIndex(uint8_t inIndex);

/**
 * Enable/disable a network
 */
bool WifiConfig_SetNetworkEnabled(uint8_t inIndex, bool inEnabled);

/**
 * Clear all configured networks
 */
void WifiConfig_ClearNetworks(void);

/**
 * Set AP mode configuration
 */
void WifiConfig_SetAP(const char* inSsid, const char* inPassword, uint8_t inChannel);

/**
 * Save configuration to flash
 * Returns true on success
 */
bool WifiConfig_Save(void);

/**
 * Get network by priority order (for connection attempts)
 * Returns NULL if no more networks at or after given priority
 */
const WifiNetwork_t* WifiConfig_GetNetworkByPriority(uint8_t inPriority);

/**
 * Get number of enabled networks
 */
uint8_t WifiConfig_GetEnabledCount(void);

#endif // WIFI_CONFIG_H
