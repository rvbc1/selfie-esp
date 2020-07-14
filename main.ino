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

WiFiMulti wifiMulti;
ESP32WebServer server(80);

struct network_struct {
    const char *ssid;
    const char *pass;
};

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

    // String input =
    // "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

    Serial.print("Initializing SD card...");
    /* initialize SD library with Soft SPI pins, if using Hard SPI replace with
     * this SD.begin()*/
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
    /* open "test.txt" for writing */
    // root = SD.open("test.txt", FILE_WRITE);
    /* if open succesfully -> root != NULL
    then write string "Hello world!" to it
  */
    // if (root)
    // {
    //     root.println("Hello world!");
    //     root.flush();
    //     /* close the file */
    //     root.close();
    // }
    // else
    // {
    //     /* if the file open error, print an error */
    //     Serial.println("error opening test.txt");
    // }
    delay(1000);

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("Nettigo Config");

    
        std::vector<NetworkManager::network_struct> networks = NetworkManager::printKnownNetworks(SD.open(NETWORK_SETTINGS_FILE));

        if (!WiFi.config( NetworkManager::local_IP, 
                          NetworkManager::gateway, 
                          NetworkManager::subnet, 
                          NetworkManager::dns)) {  //WiFi.config(ip, gateway, subnet, dns1, dns2);
            Serial.println("WiFi STATION Failed to configure Correctly");
        }


        for (NetworkManager::network_struct n : networks)
        {
            Serial.println(n.ssid);
            wifiMulti.addAP(n.ssid, n.pass);
        }

        Serial.println("Connecting ...");
        while (wifiMulti.run() != WL_CONNECTED) {  // Wait for the Wi-Fi to
       //connect: scan for Wi-Fi networks, and connect to the strongest of thenetworks above 
       delay(250); 
       Serial.print('.');
        }


    Serial.println("\nConnected to " + WiFi.SSID() + " Use IP address: " + WiFi.localIP().toString());  // Report which SSID and IP is in use
    // The logical name http://fileserver.local will also access the device if you have 'Bonjour' running or your system supports multicast dns
    if (!MDNS.begin(servername)) {  // Set your preferred server name, if you use "myserver" the address would be http://myserver.local/
        Serial.println(F("Error setting up MDNS responder!"));
        ESP.restart();
    }
    
    /* after writing then reopen the file and read it */

    //   const char* sensor = doc["networks"][0]["ssid"];
    //   const char* pass = doc["networks"][0]["password"];
    // long time = doc["time"];
    //  double latitude = doc["data"][0];
    // double longitude = doc["data"][1];

    //  Serial.println(sensor);
    //   Serial.println(pass);
}

void loop() {}
