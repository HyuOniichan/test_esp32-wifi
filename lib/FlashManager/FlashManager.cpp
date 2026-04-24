#include "FlashManager.h"

void FlashManager::mountFileSystem() {
    // If mount failed, erase & reformat before mount again
    if (!LittleFS.begin(true)) {
        Serial.println("Failed to mount file system");
    }
}

// Extern
FlashManager flashManager;
