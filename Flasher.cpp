#include "Flasher.h"

Flasher::Flasher(MemoryManager* memory) {
    this->memory = memory;
}

void Flasher::FlashESP32() {
    uint8_t success = false;
    File file = memory->sd.open(FIRMWARE_FILE);

    size_t updateSize = file.fileSize();

    Serial.println("Begin OTA");

    if (Update.begin(updateSize)) {
        size_t written = Update.writeStream(file);
        if (written == updateSize) {
            Serial.println("Written : " + String(written) + " successfully");
        } else {
            Serial.println("Written only : " + String(written) + "/" +
                           String(updateSize) + ". Retry?");
        }
        if (Update.end()) {
            Serial.println("OTA done!");
            if (Update.isFinished()) {
                Serial.println("Update successfully completed. Rebooting.");
                Serial.println("Reset in 4 seconds...");
                success = true;
                delay(4000);

                ESP.restart();
            } else {
                Serial.println("Update not finished? Something went wrong!");
            }
        } else {
            Serial.println("Error Occurred. Error #: " +
                           String(Update.getError()));
        }

    } else {
        Serial.println("Not enough space to begin OTA");
    }

    file.close();
}
