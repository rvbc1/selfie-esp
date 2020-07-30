#include "MemoryManager.h"
#include "SdFatJson.h"

void ls(char* path) {
    SdBaseFile dir;
    if (!dir.open(path, O_READ) || !dir.isDir()) {
        Serial.println("bad dir");
        return;
    }
    dir.ls(&Serial, LS_R);
}


MemoryManager::MemoryManager() {
    SdFile dirFile;

    // Number of files found.
    uint16_t n = 0;

    // Max of ten files since files are selected with a single digit.
    const uint16_t nMax = 10;

    // Position of file's directory entry.
    uint16_t dirIndex[nMax];

    // SdFat software SPI template

    // Test file.
    SdFatJson file;

    SdFile root;

    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    fs::File file2 = SPIFFS.open("/test.txt");

    if (!file2) {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.println("File Content:");

    while (file2.available()) {
        Serial.write(file2.read());
    }
    Serial.println();

    file2.close();

    Serial.print("Initializing SD card...");

    if (!sd.begin(SD_CHIP_SELECT_PIN)) {
        sd.initErrorHalt();
    }

    // if (!SD.begin(26, 14, 13, 27)) {
    //     Serial.println("initialization failed!");
    //     return;
    // }
    Serial.println("initialization done.");

    /* Begin at the root "/" */
    // root = SD.open("/WWW/");
    // if (root) {
    //     printDirectory(root, 0);
    //     root.close();
    // } else {
    //     Serial.println("error opening test.txt");
    // }
    // ls("/");
  //  superFatFile rootDir;
   // rootDir.open("/");
    //rootDir.ls(LS_R | LS_SIZE);
    // sd.ls(&Serial, LS_R | LS_SIZE);

    delay(1000);

    // if (!file.open("SoftSPI.txt", O_RDWR | O_CREAT)) {
    //     sd.errorHalt(F("open failed"));
    // }
    // file.println(F("This line was printed using software SPI."));

    // file.rewind();

    // while (file.available()) {
    //     Serial.write(file.read());
    // }

    // file.close();

    // Serial.println(F("Done."));

    // List files in root directory.
    if (!dirFile.open("/", O_RDONLY)) {
        sd.errorHalt("open root failed");
    }
    while (n < nMax && file.openNext(&dirFile, O_RDONLY)) {
        // Skip directories and hidden files.
        if (!file.isSubDir() && !file.isHidden()) {
            // Save dirIndex of file in directory.
            dirIndex[n] = file.dirIndex();

            // Print the file number and name.
            Serial.print(n++);
            Serial.write(' ');
            //file.printName(&Serial);
           Serial.println(file.getStringName());
        }
        file.close();
    }
}