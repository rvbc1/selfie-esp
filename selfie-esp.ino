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

}

void loop() {
    // 
}
