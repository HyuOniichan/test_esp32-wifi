#include <Arduino.h>
#include "esp_task_wdt.h"
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

    // Log file system memory usage
    flashManager.logFileSystemUsage();

    // Task Watchdog Timer (reset ESP if hanging)
    esp_task_wdt_init(WATCHDOG_TIMEOUT, true);
    esp_task_wdt_add(NULL);
}

void loop() {
    // Server loop
    serverManager.handleClient();
    
    // Confirm firmware
    serverManager.confirmOta(
        wifiManager.checkConnection(config.wifiConfig.wifiMode)
    );

    // Feed watchdog
    esp_task_wdt_reset();

    // LED debug
    digitalWrite(LED_BUILTIN, HIGH);
    delay(2000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(2000);
}
