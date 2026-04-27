#include "ServerManager.h"

WebServer ServerManager::_server(80);

unsigned long bootTime = millis();
bool ServerManager::_isRunning = false;
bool ServerManager::_otaConfirmed = false;

bool ServerManager::isRunning() {
    return _isRunning;
}

// --- Setup
void ServerManager::begin() {
    // Only start if Wifi mode is AP
    GeneralConfigType config = configManager.getConfig();
    WifiMode wifiMode = config.wifiConfig.wifiMode;

    if (!(wifiMode == MODE_AP || wifiMode == MODE_BOTH)) {
        Serial.println("[DEBUG] Not in AP mode, turn off webserver.");
        return;
    }

    // --- Static files
    _serveStaticFiles();

    // --- Render
    _server.on("/", _renderHome);
    _server.on("/config/wifi", _renderConfigWifi);
    _server.on("/upload/ota", _renderUploadOta);

    // --- API
    _server.on("/api/wifi", HTTP_GET, _handleGetWifi);
    _server.on("/api/wifi", HTTP_POST, _handlePostWifi);

    // --- OTA
    // Action communication (instead of Service)
    // Upload large file --> Send chunks till done
    _server.on("/api/ota", HTTP_POST, _handlePostOtaResult, _handlePostOta);

    // Start server
    _server.begin();
    _isRunning = true;

    // Support only AP mode for now
    Serial.print("[AP] SSID: ");
    Serial.println(config.wifiConfig.ssid);
}

void ServerManager::handleClient() {
    if (!_isRunning) return;
    _server.handleClient();
}

void ServerManager::confirmOta(bool isWifiOk) {
    // Check if system healthy?
    // Wait system to be stable
    if (_otaConfirmed || (millis() - bootTime < OTA_CONFIRM_DELAY)) {
        return;
    }

    // Check Wifi for now
    if (!isWifiOk) {
        Serial.println("[OTA] System isn't healthy, firmware confirm failed");
        if (!_isRunning) {
            // Server is running only in AP mode
            // STA mode always return _isRunning == false
        }
        return;
    }

    // Confirm firmware
    if (esp_ota_mark_app_valid_cancel_rollback() == ESP_OK) {
        Serial.println("[OTA] Firmware valid");
        _otaConfirmed = true;
    } else {
        Serial.println("[OTA] Firmware confirm failed");
    }
}



// --- Static files
void ServerManager::_serveStaticFiles() {
    _server.on("/style.css", []() {
        File file = LittleFS.open("/style.css", "r");
        if (!file) {
            _server.send(404, "text/plain", "style.css not found");
            return;
        }
        _server.streamFile(file, "text/css");
        file.close();
    });

    _server.on("/app.js", []() {
        File file = LittleFS.open("/app.js", "r");
        if (!file) {
            _server.send(404, "text/plain", "app.js not found");
            return;
        }
        _server.streamFile(file, "application/javascript");
        file.close();
    });
}



// --- Render pages
void ServerManager::_renderHome() {
    File file = LittleFS.open("/index.html", "r");

    if (!file) {
        _server.send(500, "text/plain", "File not found");
        return;
    }

    _server.streamFile(file, "text/html");
    file.close();
}

void ServerManager::_renderConfigWifi() {
    File file = LittleFS.open("/pages/config_wifi.html", "r");

    if (!file) {
        _server.send(500, "text/plain", "File not found");
        return;
    }

    _server.streamFile(file, "text/html");
    file.close();
}

void ServerManager::_renderUploadOta() {
    File file = LittleFS.open("/pages/upload_ota.html", "r");

    if (!file) {
        _server.send(500, "text/plain", "File not found");
        return;
    }

    _server.streamFile(file, "text/html");
    file.close();
}



// --- Handle APIs
void ServerManager::_handleGetWifi() {
    // Take current config
    GeneralConfigType config = configManager.getConfig();

    // Json body
    JsonDocument doc;
    doc["mode"] = config.wifiConfig.wifiMode;
    doc["ssid"] = config.wifiConfig.ssid;
    doc["pass"] = config.wifiConfig.pass;

    String output;
    serializeJson(doc, output);

    // Return
    _server.send(200, "application/json", output);
}

void ServerManager::_handlePostWifi() {
    // Get raw json body
    String body = _server.arg("plain");
    
    // Json buffer
    JsonDocument doc;

    // Parse Json and check error
    DeserializationError err = deserializeJson(doc, body);
    if (err) {
        Serial.print("[ERROR] JSON parse failed: ");
        Serial.println(err.c_str());

        _server.send(400, "application/json", "{\"status\":\"json_error\"}");
        return;
    }

    // Take params (mode, ssid, pass)
    WifiConfigType config;

    const char* mode = doc["mode"] | "";

    if (strcmp(mode, "STA") == 0) config.wifiMode = MODE_STA;
    else if (strcmp(mode, "AP") == 0) config.wifiMode = MODE_AP;
    else if (strcmp(mode, "DUAL") == 0) config.wifiMode = MODE_BOTH;
    else config.wifiMode = MODE_STA;

    strlcpy(config.ssid, doc["ssid"] | "", sizeof(config.ssid));
    strlcpy(config.pass, doc["pass"] | "", sizeof(config.pass));
    strlcpy(config.macAddress, doc["mac"] | "", sizeof(config.macAddress));
    
    // Debug
    Serial.println("[DEBUG] POST /api/wifi");
    Serial.print("    SSID: ");
    Serial.println(config.ssid);
    Serial.print("    MAC Address: ");
    Serial.println(config.macAddress);

    // Save config
    configManager.updateConfig<WifiConfigType>(config, CONFIG_TYPE_WIFI);

    // Response
    _server.send(200, "application/json", "{\"status\":\"ok\"}");
}



// --- OTA
void ServerManager::_handlePostOta() {
    HTTPUpload& upload = _server.upload();

    switch (upload.status) {
        case UPLOAD_FILE_START:
            Serial.println("[OTA] Start uploading...");

            // Prepare to write (choose app1)
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(Serial);
            }

            break;

        case UPLOAD_FILE_WRITE:
            // Write each chunk (buf)
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
            break;

        case UPLOAD_FILE_END:
            // Verify
            if (Update.end(true)) {
                Serial.printf("[OTA] Write done: %u bytes\n", upload.totalSize);
            } else {
                Update.printError(Serial);
            }
            break;
            
        case UPLOAD_FILE_ABORTED:
            Serial.println("[OTA] Canceled");
            Update.end();
            break;
    }
}

void ServerManager::_handlePostOtaResult() {
    // When all chunks written -> Send result
    _server.send(200, "application/json", "{\"status\":\"ok\"}");

    // Wait server to send response
    delay(1000);

    // Restart ESP32
    ESP.restart();
}
