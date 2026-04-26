#include "WifiManager.h"

static unsigned long _lastReconnectAttempt = 0;

WifiManager::WifiManager() {
    // Default mode
    WifiConfigType cfg = generateDefaultConfig<WifiConfigType>(CONFIG_TYPE_WIFI);
    _wifiMode = cfg.wifiMode;
}

WifiConfigType WifiManager::getCredentials(WifiMode mode) {
    WifiConfigType config = {};
    config.wifiMode = mode;

    switch (mode) {
        case MODE_STA:
            // strlcpy -> used for string copy
            strlcpy(config.ssid, _sta_ssid, sizeof(config.ssid));
            strlcpy(config.pass, _sta_pass, sizeof(config.pass));
            break;
        case MODE_AP:
            strlcpy(config.ssid, _ap_ssid, sizeof(config.ssid));
            strlcpy(config.pass, _ap_pass, sizeof(config.pass));
            break;
    }
    
    return config;
}

void WifiManager::setCredentials(WifiMode mode, const char* ssid, const char* pass) {
    _wifiMode = mode;

    switch (mode) {
        case MODE_STA:
            strlcpy(_sta_ssid, ssid, sizeof(_sta_ssid));
            strlcpy(_sta_pass, pass, sizeof(_sta_pass));
            break;
        case MODE_AP:
            strlcpy(_ap_ssid, ssid, sizeof(_ap_ssid));
            strlcpy(_ap_pass, pass, sizeof(_ap_pass));
            break;
    }
}

void WifiManager::WifiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
        // STA mode
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("[STA] Starting...");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("[STA] Connected");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print("[STA] IP address: ");
            Serial.println(WiFi.localIP());
            Serial.print("[STA] MAC address: ");
            Serial.println(WiFi.macAddress());
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: {
            // Handle reconnect
            Serial.println("[STA] Disconnected");
            
            // Define variable -> Put in a block
            unsigned long cur = millis();
            if (cur - _lastReconnectAttempt >= RECONNECT_INTERVAL) {
                Serial.println("[STA] Reconnecting...");
                WiFi.reconnect();
                _lastReconnectAttempt = cur;
            }

            break;
        }
        
        // AP mode
        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("[AP] Starting...");
            Serial.print("[AP] IP address: ");
            Serial.println(WiFi.softAPIP());
            break;
            
        case ARDUINO_EVENT_WIFI_AP_STOP:
            Serial.println("[AP] Stopped");
            break;

        default:
            break;
    }
}

void WifiManager::begin() {
    // Get config
    GeneralConfigType config = configManager.getConfig();
    WifiConfigType cfg = config.wifiConfig;

    // Set credentials
    setCredentials(cfg.wifiMode, cfg.ssid, cfg.pass);
    _wifiMode = cfg.wifiMode;

    // Listen to event
    WiFi.onEvent(WifiEvent);
    
    // Check for mode to begin
    switch (_wifiMode) {
        case MODE_STA:
            WiFi.mode(WIFI_STA);

            // MAC Spoofing
            setMacAddress(MAC_SPOOFING_ADDRESS);

            WiFi.begin(_sta_ssid, _sta_pass);

            break;
            
        case MODE_AP:
            WiFi.mode(WIFI_AP);
            WiFi.softAP(_ap_ssid, _ap_pass);
            break;
            
        case MODE_BOTH:
            WiFi.mode(WIFI_AP_STA);
            
            // MAC Spoofing for STA mode
            setMacAddress(MAC_SPOOFING_ADDRESS);
            
            WiFi.begin(_sta_ssid, _sta_pass);
            WiFi.softAP(_ap_ssid, _ap_pass);

            break;
    }
}

bool WifiManager::checkConnection(WifiMode mode) {
    switch (mode) {
        case MODE_STA: return (WiFi.status() == WL_CONNECTED);
        case MODE_AP: return (WiFi.softAPIP().toString() != "0.0.0.0");
        case MODE_BOTH:
            return (
                WiFi.status() == WL_CONNECTED 
                && WiFi.softAPIP().toString() != "0.0.0.0"
            );
    }
    return false;
}

void WifiManager::setMacAddress(const char* macAddress) {
    // MAC Spoofing supports only STA mode for now
    if (!(_wifiMode == MODE_STA || _wifiMode == MODE_BOTH)) return;

    // Check MAC address format
    if (sscanf(
        macAddress, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
        &_mac_values[0], &_mac_values[1], &_mac_values[2], 
        &_mac_values[3], &_mac_values[4], &_mac_values[5]
    ) != 6) {
        Serial.println("Wrong MAC address format");
        return;
    }
    
    // Set new MAC address
    esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, _mac_values);
    
    if (err != ESP_OK) {
        Serial.println("Failed to set MAC address");
        return;
    }

    // Support STA mode for now
    Serial.println("[STA] MAC Spoofing done");
}
