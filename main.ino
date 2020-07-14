#include <ArduinoJson.h>
#include <ESP32WebServer.h>  // https://github.com/Pedroalbuquerque/ESP32WebServer download and place in your Libraries folder
#include <ESPmDNS.h>
#include <Regexp.h>
#include <WiFi.h>       // Built-in
#include <WiFiMulti.h>  // Built-in
#include <mySD.h>
#include "NetworkManager.h"

#define servername "fileserver"

File root;

ESP32WebServer server(80);

// void printDirectory(File dir, int numTabs) {
//     while (true) {
//         File entry = dir.openNextFile();
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
//         entry.close();
//     }
// }

void setup() {
    Serial.begin(115200);


    Serial.print("Initializing SD card...");

    if (!SD.begin(26, 14, 13, 27)) {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");
    /* Begin at the root "/" */
    root = SD.open("/");
    if (root) {
        //printDirectory(root, 0);
        root.close();
    } else {
        Serial.println("error opening test.txt");
    }

    delay(1000);


    NetworkManager *networkManager = new NetworkManager(SD.open(NETWORK_SETTINGS_FILE));

 // Report which SSID and IP is in use
    // The logical name http://fileserver.local will also access the device if you have 'Bonjour' running or your system supports multicast dns
    if (!MDNS.begin(servername)) {  // Set your preferred server name, if you use "myserver" the address would be http://myserver.local/
        Serial.println(F("Error setting up MDNS responder!"));
        ESP.restart();
    }
}

void loop() {}
