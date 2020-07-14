
#include <ESP32WebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <mySD.h>

#include "NetworkManager.h"

#include "Sys_Variables.h"
#include "css.h"

#define FS_NO_GLOBALS  // allow spiffs to coexist with SD card, define BEFORE
                       // including FS.h
#include <FS.h>        //spiff file system

#include "SPIFFS.h"

#define servername "esp"

File root;
ESP32WebServer server(80);

void SendHTML_Header() {
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");  // Empty content inhibits Content-length header so we have to close the socket ourselves.
    append_page_header();
    server.sendContent(webpage);
    webpage = "";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Content() {
    server.sendContent(webpage);
    webpage = "";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SendHTML_Stop() {
    server.sendContent("");
    server.client().stop();  // Stop is needed because no content length was sent
}

void HomePage() {
    File file = SD.open("WWW/INDEX~1.HTM");
    size_t sent = server.streamFile(file, "text/html");
    file.close();
}

void handleNotFound() {
    Serial.println(server.uri());
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/html", message);
}

void printDirectory(File dir, int numTabs) {
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            break;
        }
        for (uint8_t i = 0; i < numTabs; i++) {
            Serial.print('\t');  // we'll have a nice indentation
        }
        // Print the name
        Serial.print(entry.name());
        /* Recurse for directories, otherwise print the file size */
        if (entry.isDirectory()) {
            Serial.println("/");
            printDirectory(entry, numTabs + 1);
        } else {
            /* files have sizes, directories do not */
            Serial.print("\t\t");
            Serial.println(entry.size());
        }
        entry.close();
    }
}

void handleRoot() {
    // digitalWrite(led, 1);
    File file = SD.open(NETWORK_SETTINGS_FILE);
    size_t sent = server.streamFile(file, "text/plain");
    file.close();
    // server.send(200, "text/plain", "hello from ESP32!");
    // digitalWrite(led, 0);
}

void setup() {
    Serial.begin(115200);
    delay(3000);
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    fs::File file2 = SPIFFS.open("/test.txt");
 
    if(!file2){
        Serial.println("Failed to open file for reading");
        return;
    }
 
    Serial.println("File Content:");
 
    while(file2.available()){
 
        Serial.write(file2.read());
    }
    Serial.println();
 
    file2.close();

    Serial.print("Initializing SD card...");

    if (!SD.begin(26, 14, 13, 27)) {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");
    /* Begin at the root "/" */
    root = SD.open("/");
    if (root) {
        printDirectory(root, 0);
        root.close();
    } else {
        Serial.println("error opening test.txt");
    }

    delay(1000);

    NetworkManager *networkManager =
        new NetworkManager(SD.open(NETWORK_SETTINGS_FILE));

    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }

    server.on("/", HomePage);

    server.on("/styles.css",
              []() { 
                      File file = SD.open("WWW/STYLES.CSS");
                        size_t sent = server.streamFile(file, "text/css");
                file.close();
                  });

    server.on("/network", handleRoot);

    server.on("/inline",
              []() { server.send(200, "text/plain", "this works as well"); });

    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
}

void loop() { server.handleClient(); }
