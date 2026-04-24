#include "ConfigManager.h"

GeneralConfigType ConfigManager::getConfig() {
    GeneralConfigType config;

    config.wifiConfig = flashManager.loadConfig<WifiConfigType>(CONFIG_TYPE_WIFI);

    _lastGetConfig = millis();

    return config;
}

void ConfigManager::updateGeneralConfig(GeneralConfigType config) {
    flashManager.saveConfig<WifiConfigType>(config.wifiConfig, CONFIG_TYPE_WIFI);
}

bool ConfigManager::checkConfigUpdate() {
    unsigned long cur = millis();
    if (cur - _lastGetConfig >= GET_CONFIG_INTERVAL) {
        _lastGetConfig = cur;
        return true;
    }
    return false;
}

ConfigManager configManager;
