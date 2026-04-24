#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "ConfigTypes.h"
#include "FlashManager.h"

class ConfigManager {
private:
    GeneralConfigType _config;
    unsigned long _lastGetConfig = 0;

public:
    // Get general config
    GeneralConfigType getConfig();

    // Set general config
    void updateGeneralConfig(GeneralConfigType config);

    // Set a specific config
    template<typename T>
    void updateConfig(T config, ConfigType type) {
        switch (type) {
            case CONFIG_TYPE_WIFI:
                flashManager.saveConfig<WifiConfigType>(config, CONFIG_TYPE_WIFI);
                break;
        }
    }

    // Check any changes of config every interval
    bool checkConfigUpdate();

    // Check if default config
    template<typename T>
    bool isValidConfig(T config, ConfigType type) {
        switch (type) {
            case CONFIG_TYPE_WIFI:
                return (strcmp(config.ssid, "") != 0);
        }
    }
};

extern ConfigManager configManager;

#endif
