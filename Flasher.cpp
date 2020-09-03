#include "Flasher.h"

Flasher::Flasher(MemoryManager *memory) { this->memory = memory; }

uint8_t Flasher::FlashESP32(String filename) {
    uint8_t success = false;
    if (memory->sd.exists(const_cast<char *>(filename.c_str()))) {
        Serial.println("Flashing ESP32 with file: " + filename);

        // String new_firmware_ota = FIRMWARE_FILE;
        // String old_firmware_ota = OLD_FIRMWARE_FILE;

        // if (memory->sd.exists(const_cast<char *>(new_firmware_ota.c_str()))) {
        //     memory->removeFileIfExists(old_firmware_ota);
        //     memory->sd.rename(const_cast<char *>(new_firmware_ota.c_str()),
        //                       const_cast<char *>(old_filename.c_str()));
        // }
        // File newFirmwareFile;
        // newFirmwareFile.open(const_cast<char *>(new_firmware_ota.c_str()), O_RDWR | O_CREAT);
        // size_t n;
        // uint8_t buf[64];
        // while ((n = myOrigFile.read(buf, sizeof(buf))) > 0) {
        //     myDestFile.write(buf, n);
        // }

        //    filename = FIRMWARE_FILE;
        File file = memory->sd.open(const_cast<char *>(filename.c_str()));

        size_t updateSize = file.fileSize();

        Serial.println("Begin OTA");

        if (Update.begin(updateSize)) {
            size_t written = Update.writeStream(file);
            if (written == updateSize) {
                Serial.println("Written : " + String(written) +
                               " successfully");
            } else {
                Serial.println("Written only : " + String(written) + "/" +
                               String(updateSize) + ". Retry?");
            }
            if (Update.end()) {
                Serial.println("OTA done!");
                if (Update.isFinished()) {
                    Serial.println("Update successfully completed. Rebooting.");
                    // Serial.println("Reset in 4 seconds...");
                    success = true;
                    // delay(4000);

                    // ESP.restart();
                } else {
                    Serial.println(
                        "Update not finished? Something went wrong!");
                }
            } else {
                Serial.println("Error Occurred. Error #: " +
                               String(Update.getError()));
            }

        } else {
            Serial.println("Not enough space to begin OTA");
        }

        file.close();
    } else {
        Serial.println("File for flashing ESP32 not exists");
    }
    return success;
}
