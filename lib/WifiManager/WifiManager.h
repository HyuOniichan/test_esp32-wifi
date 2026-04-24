#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <esp_wifi.h>
#include "ConfigManager.h"
#include "config.h"

class WifiManager {
private:
    WifiMode _wifiMode;
    char _sta_ssid[CREDENTIAL_BUFFER_SIZE];
    char _sta_pass[CREDENTIAL_BUFFER_SIZE];
    char _ap_ssid[CREDENTIAL_BUFFER_SIZE];
    char _ap_pass[CREDENTIAL_BUFFER_SIZE];

    uint8_t _mac_values[6];

    unsigned long _lastCheckConn = 0;
    unsigned long _lastRetryConn = 0;

public:
    WifiManager();

    // Get credentials (ssid, pass) of specific mode (STA, AP)
    WifiConfigType getCredentials(WifiMode mode);

    // Set credentials (ssid, pass) of specific mode (STA, AP)
    void setCredentials(WifiMode mode, const char* ssid, const char* pass);

    // Begin Wifi connection
    void begin();
    
    // Retry connection in an defined interval
    bool retryConnection(WifiMode mode);

    // Check if Wifi connected
    bool checkConnection(WifiMode mode);

    // MAC Spoofing
    void setMacAddress(const char* macAddress);

    // Log Wifi status
    void logStatus();
};

#endif
