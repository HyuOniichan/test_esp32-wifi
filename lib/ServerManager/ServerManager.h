#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <LittleFS.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "ConfigManager.h"

class ServerManager {
private:
    static WebServer _server;
    static bool _isRunning;

    // Serve static files
    static void _serveStaticFiles();
    
    // Render frontend pages
    static void _renderHome();
    static void _renderConfigWifi();
    
    // Handle APIs
    static void _handleGetWifi();
    static void _handlePostWifi();

public:
    // Start server
    void begin();

    // Server built-in function, used in loop
    void handleClient();
    
    // Check if server running
    bool isRunning();
};

#endif
