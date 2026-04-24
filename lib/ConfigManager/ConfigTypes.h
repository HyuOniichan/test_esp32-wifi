#ifndef CONFIG_TYPES_H
#define CONFIG_TYPES_H

#include <stdint.h>
#include <string.h>
#include "config.h"



// --- ConfigManager
#define GET_CONFIG_INTERVAL 60000 // 1m

// Define all config types
enum ConfigType: uint8_t {
    CONFIG_TYPE_WIFI,
};



// --- FlashManager

// Mapping from config type to namespace
inline const char* mapConfigTypeToNamespace(ConfigType type) {
    switch (type) {
        case CONFIG_TYPE_WIFI: return "wifi";
        default: return "";
    }
}



// --- WifiManager
#define CREDENTIAL_BUFFER_SIZE 32
#define CHECK_CONNECTION_INTERVAL 10000 // 10s
#define RETRY_CONNECTION_INTERVAL 1000 // 1s

// uint8 -> avoid changing size later
enum WifiMode: uint8_t {
    MODE_STA,
    MODE_AP,
    MODE_BOTH
};

// Wifi config type (wifiMode, ssid, pass)
struct WifiConfigType {
    // Enum is just an integer -> fix its size to uint8
    WifiMode wifiMode;
    // use char instead of String (a pointer)
    char ssid[CREDENTIAL_BUFFER_SIZE];
    char pass[CREDENTIAL_BUFFER_SIZE];
};



// --- ConfigManager

// General config type (wifi, ...)
struct GeneralConfigType {
    WifiConfigType wifiConfig;
};

// Generate default config
template<typename T>
T generateDefaultConfig(ConfigType type) {
    switch (type) {
        case CONFIG_TYPE_WIFI: 
            WifiConfigType config;
            config.wifiMode = MODE_AP;
            strlcpy(config.ssid, AP_SSID, sizeof(config.ssid));
            strlcpy(config.pass, AP_PASS, sizeof(config.pass));
            return config;
    }
    return T{};
}



#endif
