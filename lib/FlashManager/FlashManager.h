#ifndef FLASH_MANAGER_H
#define FLASH_MANAGER_H

#include <Preferences.h>
#include <LittleFS.h>
#include "ConfigTypes.h"

class FlashManager {
private:
    Preferences _prefs;

public:
    // Save config of a specific type
    template<typename T>
    void saveConfig(const T& config, ConfigType configType) {
        // Start a namespace --> .begin(namespace, readOnly?)
        _prefs.begin(mapConfigTypeToNamespace(configType), false);

        // Save a variable (struct) --> .put<type>(key, value)
        _prefs.putBytes("config", &config, sizeof(T));
        
        // Close current namespace
        _prefs.end();
    }

    // Load config from Flash memory
    template<typename T>
    T loadConfig(ConfigType configType) {
        T config = {};

        _prefs.begin(mapConfigTypeToNamespace(configType), true);

        // At init, key is not defined -> return nothing -> check length
        size_t len = _prefs.getBytesLength("config");

        if (len == sizeof(T)) {
            _prefs.getBytes("config", &config, sizeof(T));
        } else {
            Serial.print("No config found. Using default config for ");
            Serial.println(mapConfigTypeToNamespace(configType));
            config = generateDefaultConfig<T>(configType);
            saveConfig(config, configType);
        }

        _prefs.end();

        return config;
    }

    // Mount file system (before init server in AP mode)
    void mountFileSystem();

    // Check file system usage
    void logFileSystemUsage();
};

extern FlashManager flashManager;

#endif
