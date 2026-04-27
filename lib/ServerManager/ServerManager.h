#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <LittleFS.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Update.h>
#include "esp_ota_ops.h"
#include "ConfigManager.h"

class ServerManager {
private:
    static WebServer _server;
    static bool _isRunning;
    static bool _otaConfirmed;

    // Serve static files
    static void _serveStaticFiles();
    
    // Render frontend pages
    static void _renderHome();
    static void _renderConfigWifi();
    static void _renderUploadOta();
    
    // Handle APIs
    static void _handleGetWifi();
    static void _handlePostWifi();

    // OTA
    static void _handlePostOta();
    static void _handlePostOtaResult();

public:
    // Start server
    void begin();

    // Server built-in function, used in loop
    void handleClient();
    
    // Check if server running
    bool isRunning();

    // Confirm firmware (after OTA done)
    void confirmOta(bool isWifiOk);
};

#endif
