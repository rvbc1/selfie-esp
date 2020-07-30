#include "Core.h"


// ESP32WebServer server(80);


//  void printDirectory(SdFile dir, int numTabs) {
//     while (true) {
//         SdFile entry = dir.openNext();
//         if (!entry) {
//             break;
//         }
//         for (uint8_t i = 0; i < numTabs; i++) {
//             Serial.print('\t');  // we'll have a nice indentation
//         }
//         // Print the name
//         Serial.print(entry.name());
//         /* Recurse for directories, otherwise print the file size */
//         if (entry.isDirectory()) {
//             Serial.println("/");
//             printDirectory(entry, numTabs + 1);
//         } else {
//             /* files have sizes, directories do not */
//             Serial.print("\t\t");
//             Serial.println(entry.size());
//         }
//     entry.close();
//     }
// }

// HttpServer* htttpServer;

void setup() {
    Core *core = new Core();

    // Serial.println("\n SD:");
    // switch (SD.card.type()) {
    //     case SD_CARD_TYPE_SD1:
    //     Serial.println("SD1");
    //     break;
    //     case SD_CARD_TYPE_SD2:
    //     Serial.println("SD2");
    //     break;
    //     case SD_CARD_TYPE_SDHC:
    //     Serial.println("SDHC");
    //     break;
    //     default:
    //     Serial.println("Unknown");
    // }

    // Serial.print("Volume type is:    FAT");
    // Serial.println(SD.volume.fatType(), DEC);

    // uint32_t volumesize;
    // volumesize = SD.volume.blocksPerCluster();    // clusters are collections
    // of blocks volumesize *= SD.volume.clusterCount();       // we'll have a
    // lot of clusters volumesize /= 2;                           // SD card
    // blocks are always 512 bytes (2 blocks are 1KB) Serial.print("Volume size
    // (Kb):  "); Serial.println(volumesize); Serial.print("Volume size (Mb):
    // "); volumesize /= 1024; Serial.println(volumesize); Serial.print("Volume
    // size (Gb):  "); Serial.println((float)volumesize / 1024.0);




}

void loop() {
    // 
}
