//----------------------------------------------
// FatFs Configuration for Rocket Test Stand
// Feather RP2350 with Adalogger SD Card
//----------------------------------------------

#define FFCONF_DEF 80286

//----------------------------------------------
// Function Configuration
//----------------------------------------------
#define FF_FS_READONLY   0     // Read/Write
#define FF_FS_MINIMIZE   0     // Full API
#define FF_USE_FIND      0     // Disable filtered directory read
#define FF_USE_MKFS      0     // Disable mkfs (we don't format cards)
#define FF_USE_FASTSEEK  0     // Disable fast seek
#define FF_USE_EXPAND    0     // Disable f_expand
#define FF_USE_CHMOD     0     // Disable chmod
#define FF_USE_LABEL     0     // Disable volume label
#define FF_USE_FORWARD   0     // Disable f_forward
#define FF_USE_STRFUNC   1     // Enable string functions (f_printf, etc.)
#define FF_PRINT_LLI     0     // Disable long long in f_printf
#define FF_PRINT_FLOAT   1     // Enable float in f_printf
#define FF_STRF_ENCODE   0     // ANSI/OEM encoding

//----------------------------------------------
// Locale and Namespace
//----------------------------------------------
#define FF_CODE_PAGE     437   // U.S. code page
#define FF_USE_LFN       1     // Enable LFN with static buffer
#define FF_MAX_LFN       64    // Max LFN length
#define FF_LFN_UNICODE   0     // ANSI/OEM character set
#define FF_LFN_BUF       64    // LFN buffer size
#define FF_SFN_BUF       12    // SFN buffer size
#define FF_FS_RPATH      0     // Disable relative path

//----------------------------------------------
// Drive/Volume Configuration
//----------------------------------------------
#define FF_VOLUMES       1     // Single volume (SD card)
#define FF_STR_VOLUME_ID 0     // Disable string volume ID
#define FF_MULTI_PARTITION 0   // Single partition

//----------------------------------------------
// System Configuration
//----------------------------------------------
#define FF_FS_TINY       1     // Use tiny buffer (reduces RAM usage)
#define FF_FS_EXFAT      0     // Disable exFAT (FAT32 only)
#define FF_FS_NORTC      1     // No RTC support in FatFs (we handle timestamps)
#define FF_NORTC_MON     1     // Default month
#define FF_NORTC_MDAY    1     // Default day
#define FF_NORTC_YEAR    2025  // Default year
#define FF_FS_NOFSINFO   0     // Use FSINFO sector
#define FF_FS_LOCK       0     // Disable file lock

//----------------------------------------------
// Sync Functions (single-threaded)
//----------------------------------------------
#define FF_FS_REENTRANT  0     // Disable re-entrancy
#define FF_FS_TIMEOUT    1000  // Timeout period (unused if not reentrant)

//----------------------------------------------
// Sector Size
//----------------------------------------------
#define FF_MIN_SS        512   // Minimum sector size
#define FF_MAX_SS        512   // Maximum sector size (same for SD cards)
