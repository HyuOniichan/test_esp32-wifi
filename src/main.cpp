#include <Arduino.h>
#include "config.h"
#include "ConfigTypes.h"
#include "ConfigManager.h"
#include "WifiManager.h"
#include "ServerManager.h"

// configManager already initialized (extern)
// flashManager already initialized (extern)
WifiManager wifiManager;
ServerManager serverManager;

// Load config
GeneralConfigType config;

void setup() {
    // Debug
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    // // Manually reset to default Wifi config
    // configManager.updateConfig<WifiConfigType>(
    //     generateDefaultConfig<WifiConfigType>(CONFIG_TYPE_WIFI),
    //     CONFIG_TYPE_WIFI
    // );

    // Get config from Flash
    config = configManager.getConfig();

    // Start Wifi connection
    wifiManager.begin();
    
    // Mount file system (before init server)
    flashManager.mountFileSystem();

    // Webserver
    serverManager.begin();
}

void loop() {
    // Server loop
    serverManager.handleClient();
    
    // Check for any new changes in config
    if (configManager.checkConfigUpdate()) {
        config = configManager.getConfig();
    }

    // LED debug
    digitalWrite(LED_BUILTIN, HIGH);
    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(2000);
}
