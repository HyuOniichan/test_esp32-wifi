#include "WifiManager.h"

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

void WifiManager::begin() {
    // Get config
    GeneralConfigType config = configManager.getConfig();
    WifiConfigType cfg = config.wifiConfig;

    // Set credentials
    setCredentials(cfg.wifiMode, cfg.ssid, cfg.pass);
    _wifiMode = cfg.wifiMode;
    
    // Check for mode to begin
    switch (_wifiMode) {
        case MODE_STA:
            WiFi.mode(WIFI_STA);
            if (retryConnection(MODE_STA)) Serial.println("Starting STA mode...");
                else Serial.println("Failed to start STA mode...");
            break;
            
        case MODE_AP:
            WiFi.mode(WIFI_AP);
            if (retryConnection(MODE_AP)) Serial.println("Starting AP mode...");
                else Serial.println("Failed to start AP mode...");
            break;
            
        case MODE_BOTH:
            WiFi.mode(WIFI_AP_STA);
            if (retryConnection(MODE_AP)) Serial.println("Starting AP mode...");
                else Serial.println("Failed to start AP mode...");
            if (retryConnection(MODE_STA)) Serial.println("Starting STA mode...");
                else Serial.println("Failed to start STA mode...");
            break;
    }
}

bool WifiManager::retryConnection(WifiMode mode) {
    unsigned long start = millis();

    while (millis() - start < CHECK_CONNECTION_INTERVAL) {
        if (checkConnection(mode)) {
            _lastCheckConn = millis();
            return true;
        }

        if (millis() - _lastRetryConn >= RETRY_CONNECTION_INTERVAL) {
            switch (mode) {
                case MODE_STA:
                    WiFi.begin(_sta_ssid, _sta_pass);
                    break;
                case MODE_AP:
                    WiFi.softAP(_ap_ssid, _ap_pass); 
                    break;
                case MODE_BOTH:
                    WiFi.begin(_sta_ssid, _sta_pass);
                    WiFi.softAP(_ap_ssid, _ap_pass); 
                    break;
            }

            _lastRetryConn = millis();
        }
        
        // Prevent CPU spam
        delay(100);
    }

    return false;
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

void WifiManager::logStatus() {
    Serial.println("--- Network Status");
    
    if (_wifiMode == MODE_AP || _wifiMode == MODE_BOTH) {
        Serial.print("AP IP: ");
        Serial.print(WiFi.softAPIP());
        Serial.println();
    }
    
    if (_wifiMode == MODE_STA || _wifiMode == MODE_BOTH) {
        Serial.print("STA IP: ");
        Serial.print(WiFi.localIP());
        Serial.println();
    }
}
