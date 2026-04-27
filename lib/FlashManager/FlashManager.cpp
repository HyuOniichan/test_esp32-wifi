#include "FlashManager.h"

void FlashManager::mountFileSystem() {
    // If mount failed, erase & reformat before mount again
    if (!LittleFS.begin(true)) {
        Serial.println("Failed to mount file system");
    }
}

void FlashManager::logFileSystemUsage() {
    size_t total = LittleFS.totalBytes();
    size_t used = LittleFS.usedBytes();

    Serial.printf("[DEBUG] LittleFS: %d / %d\n", used, total);
}

// Extern
FlashManager flashManager;
