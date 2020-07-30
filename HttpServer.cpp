#include "HttpServer.h"

#include <ArduinoJson.h>
#include <ESP32WebServer.h>
#include <ESPmDNS.h>

#include "SdFatJson.h"
#include "Settings.h"
#include "Update.h"

ESP32WebServer server(HTTP_PORT);

MemoryManager *HttpServer::memory;

void HomePage() {
    File file = HttpServer::memory->sd.open("WWW/index.html");
    size_t sent = server.streamFile(file, "text/html");
    file.close();
}

// void handleNotFound() {
//     Serial.println(server.uri());
//     String message = "File Not Found\n\n";
//     message += "URI: ";
//     message += server.uri();
//     message += "\nMethod: ";
//     message += (server.method() == HTTP_GET) ? "GET" : "POST";
//     message += "\nArguments: ";
//     message += server.args();
//     message += "\n";
//     for (uint8_t i = 0; i < server.args(); i++) {
//         message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
//     }
//     server.send(404, "text/html", message);
// }

// void handleRoot() {
//     // digitalWrite(led, 1);
//     File file = SD.open(NETWORK_SETTINGS_FILE);
//     size_t sent = server.streamFile(file, "text/plain");
//     file.close();
//     // server.send(200, "text/plain", "hello from ESP32!");
//     // digitalWrite(led, 0);
// }

File fsUploadFile;

void handleFileUpload() {  // upload a new file to the SPIFFS
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        String filename = upload.filename;
        if (!filename.startsWith("/")) filename = "/" + filename;
        Serial.print("handleFileUpload Name: ");

        Serial.println(filename);

        filename = "WWW" + filename;

        if (HttpServer::memory->sd.exists(
                const_cast<char *>(filename.c_str()))) {
            Serial.println("Deleting existing file");
            HttpServer::memory->sd.remove(const_cast<char *>(filename.c_str()));
        }

        fsUploadFile.open(
            filename.c_str(),
            O_RDWR | O_CREAT);  // Open the file for writing in SPIFFS
                                // (create if it doesn't exist)
        filename = String();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (fsUploadFile)
            fsUploadFile.write(
                upload.buf,
                upload.currentSize);  // Write the received bytes to the file
    } else if (upload.status == UPLOAD_FILE_END) {
        if (fsUploadFile) {        // If the file was successfully created
            fsUploadFile.close();  // Close the file again
            Serial.print("handleFileUpload Size: ");
            Serial.println(upload.totalSize);
            server.sendHeader("Location",
                              "/");  // Redirect the client to the success page
            server.send(303);
        } else {
            server.send(500, "text/plain", "500: couldn't create file");
        }
    }
}

void handleFileFlash() {  // upload a new file to the SPIFFS
    String filename = "";
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        filename = upload.filename;
        if (!filename.startsWith("/")) filename = "/" + filename;
        Serial.print("handleFileUpload Name: ");

        filename = "WWW/firmware.bin";
        Serial.println(filename);

        if (HttpServer::memory->sd.exists(
                const_cast<char *>(filename.c_str()))) {
            Serial.println("Deleting existing file");
            HttpServer::memory->sd.remove(const_cast<char *>(filename.c_str()));
        }

        fsUploadFile.open(
            filename.c_str(),
            O_RDWR | O_CREAT);  // Open the file for writing in SPIFFS
                                // (create if it doesn't exist)
        filename = String();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (fsUploadFile)
            fsUploadFile.write(
                upload.buf,
                upload.currentSize);  // Write the received bytes to the file
    } else if (upload.status == UPLOAD_FILE_END) {
        if (fsUploadFile) {        // If the file was successfully created
            fsUploadFile.close();  // Close the file again
            Serial.print("handleFileUpload Size: ");
            Serial.println(upload.totalSize);
            server.sendHeader("Location",
                              "/");  // Redirect the client to the success page
            server.send(303);

            File file = HttpServer::memory->sd.open("WWW/firmware.bin");

            size_t updateSize = file.fileSize();

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
                        Serial.println(
                            "Update successfully completed. Rebooting.");
                        Serial.println("Reset in 4 seconds...");

                        server.sendHeader(
                            "Location",
                            "/");  // Redirect the client to the success page
                        server.send(303);
                        delay(4000);

                        ESP.restart();
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
            server.send(500, "text/plain", "500: couldn't create file");
        }
    }
}

String generateJson() {
    DynamicJsonDocument doc(1024);

    JsonArray files = doc.createNestedArray("Files");

    // File dir = SD.open("/WWW/");

    // while (true) {
    //     File entry = dir.openNextFile();
    //     if (!entry) {
    //         break;
    //     }

    //     if (entry.isDirectory()) {
    //         Serial.println("/");
    //     } else {
    //         Serial.println(entry.name());
    //         JsonObject object = files.createNestedObject();
    //         object["name"] = entry.name();
    //         object["size"] = entry.size();
    //     }
    //     entry.close();
    // }
    // dir.close();

    SdFile dirFile;
    SdFatJson file;

    if (!dirFile.open("/WWW", O_RDONLY)) {
        // sd.errorHalt("open root failed");
    }
    while (file.openNext(&dirFile, O_RDONLY)) {
        // Skip directories and hidden files.
        if (!file.isSubDir() && !file.isHidden()) {
            // Save dirIndex of file in directory.
            //   dirIndex[n] = file.dirIndex();

            // Print the file number and name.
            //   Serial.print(n++);
            //  Serial.write(' ');
            // file.printName(&Serial);
            JsonObject object = files.createNestedObject();
            object["name"] = file.getStringName();
            object["size"] = file.fileSize();
            // Serial.println(file.getStringName());
        }
        file.close();
    }

    // for(int i = 0; i < 6; i++){
    //     JsonObject object = files.createNestedObject();
    //     object["name"] = "File " ;
    //     object["size"] = i;
    //     files.add(object);
    // }

    // serialize the object and send the result to Serial
    String Json = "";
    serializeJson(doc, Json);
    return Json;
}

HttpServer::HttpServer(MemoryManager *memory2) {
    memory = memory2;
    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }

    server.on("/", HomePage);

    server.on("/styles.css", []() {
        File file = memory->sd.open("WWW/styles.css");
        size_t sent = server.streamFile(file, "text/css");
        file.close();
    });

    server.on("/delete.png", []() {
        File file = memory->sd.open("WWW/delete.png");
        size_t sent = server.streamFile(file, "image/png");
        file.close();
    });

    server.on("/download.png", []() {
        File file = memory->sd.open("WWW/download.png");
        size_t sent = server.streamFile(file, "image/png");
        file.close();
    });

    server.on("/flash.png", []() {
        File file = memory->sd.open("WWW/flash.png");
        size_t sent = server.streamFile(file, "image/png");
        file.close();
    });

    server.on("/data.json", []() {
        // File file = SD.open("WWW/FILES~1.JSO");
        //  size_t sent = server.streamFile(file, "text/plain");
        // file.close();
        server.send(200, "text/plain", generateJson());
    });

    server.on("/delete", []() {
        for (uint8_t i = 0; i < server.args(); i++) {
           // Serial.print("Parametr: ");
           // Serial.print(server.argName(i));
           // Serial.print(" wartość: ");
           String filename = "WWW/" + server.arg(i);
            if (HttpServer::memory->sd.exists(
                const_cast<char *>(filename.c_str()))) {
                Serial.print("Deleting existing file \"");
                Serial.print(const_cast<char *>(filename.c_str()));
                Serial.println("\"");
                HttpServer::memory->sd.remove(const_cast<char *>(filename.c_str()));
            }
        //   memory->sd.remove(const_cast<char *>(server.arg(i).c_str()));
        }
        server.sendHeader("Location",
                          "/");  // Redirect the client to the success page
        server.send(303);
    });

    server.on("/download", []() {
        for (uint8_t i = 0; i < server.args(); i++) {
           // Serial.print("Parametr: ");
           // Serial.print(server.argName(i));
           // Serial.print(" wartość: ");
           String filename = "WWW/" + server.arg(i);
            File download;
            download.open(const_cast<char *>(filename.c_str()));
            if (download) {
                server.sendHeader("Content-Type", "text/text");
                server.sendHeader("Content-Disposition", "attachment; filename=" + filename);
                server.sendHeader("Connection", "close");
                server.streamFile(download, "application/octet-stream");

                download.close();
            }
        //   memory->sd.remove(const_cast<char *>(server.arg(i).c_str()));
        }
        server.sendHeader("Location",
                          "/");  // Redirect the client to the success page
        server.send(303);
    });

    // server.on("/network", handleRoot);

    server.on("/inline",
              []() { server.send(200, "text/plain", "this works as well"); });

    server.on("/update", []() {
        File file = memory->sd.open("WWW/firmware.bin");

        size_t updateSize = file.fileSize();

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
                    Serial.println("Reset in 4 seconds...");

                    server.sendHeader(
                        "Location",
                        "/");  // Redirect the client to the success page
                    server.send(303);
                    delay(4000);

                    ESP.restart();
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
    });

    server.on("/flash", HTTP_GET,
              []() {  // if the client requests the upload page
                  File file;
                  file.open("WWW/upload.html");
                  size_t sent = server.streamFile(file, "text/html");
                  file.close();
              });

    server.on(
        "/flash", HTTP_POST,  // if the client posts to the upload page
        []() {
            server.send(200);
        },  // Send status 200 (OK) to tell the client we are ready to receive
        handleFileFlash  // Receive and save the file
    );

    server.on("/upload", HTTP_GET,
              []() {  // if the client requests the upload page
                  File file;
                  file.open("WWW/upload.html");
                  size_t sent = server.streamFile(file, "text/html");
                  file.close();
              });

    server.on(
        "/upload", HTTP_POST,  // if the client posts to the upload page
        []() {
            server.send(200);
        },  // Send status 200 (OK) to tell the client we are ready to receive
        handleFileUpload  // Receive and save the file
    );

    // server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
}

void HttpServer::process() { server.handleClient(); }