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

    char _mac_address[MAC_ADDRESS_BUFFER_SIZE];
    uint8_t _mac_values[6];

    // Listen to event (event-driven) instead of while loop (polling)
    static void WifiEvent(WiFiEvent_t event, WiFiEventInfo_t info);

public:
    WifiManager();

    // Get credentials (ssid, pass, MAC address) of specific mode (STA, AP)
    WifiConfigType getCredentials(WifiMode mode);

    // Set credentials (ssid, pass, MAC address) of specific mode (STA, AP)
    void setCredentials(WifiConfigType wifiConfig);

    // Begin Wifi connection
    void begin();

    // Check if Wifi connected
    bool checkConnection(WifiMode mode);

    // MAC Spoofing
    void setMacAddress(const char* macAddress);
};

#endif
