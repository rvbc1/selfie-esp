#include "HttpServer.h"

#include <ArduinoJson.h>
#include <ESP32WebServer.h>
#include <ESPmDNS.h>

#include "SdFatJson.h"
#include "Settings.h"
#include "Update.h"

ESP32WebServer server(HTTP_PORT);

MemoryManager *HttpServer::memory;
Flasher *HttpServer::flasher;

String HttpServer::bt_console = "";

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

uint8_t removeFileIfExists(String filename) {
    if (HttpServer::memory->sd.exists(const_cast<char *>(filename.c_str()))) {
        Serial.print("Deleting existing file ");
        Serial.println(filename);
        return HttpServer::memory->sd.remove(
            const_cast<char *>(filename.c_str()));
    }
    return false;
}

uint8_t removeDirIfExists(String filename) {
    if (HttpServer::memory->sd.exists(const_cast<char *>(filename.c_str()))) {
        Serial.print("Deleting existing dir ");
        Serial.println(filename);
        return HttpServer::memory->sd.rmdir(
            const_cast<char *>(filename.c_str()));
    }
    return false;
}

File fsUploadFile;

void handleFileUpload() {  // upload a new file to the SPIFFS
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        String filename = upload.filename;
        if (!filename.startsWith("/")) filename = "/" + filename;
        Serial.print("handleFileUpload Name: ");

        for (uint8_t i = 0; i < server.args(); i++) {
            Serial.print(server.argName(i));
            Serial.print(" = ");
            Serial.println(server.arg(i));
        }

        String dir = "";

        if (server.hasArg("dir") && server.arg("dir") != NULL) {
            dir = server.arg("dir");
        }

        filename = dir + filename;

        Serial.println(filename);

        removeFileIfExists(filename);

        fsUploadFile.open(filename.c_str(), O_RDWR | O_CREAT);

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
            server.sendHeader("Location", "/");
            server.send(303);
        } else {
            server.send(500, "text/plain", "500: couldn't create file");
        }
    }
}

void handleFileUploadData() {
    Serial.print("Data1: ");
    for (uint8_t i = 0; i < server.args(); i++) {
        Serial.print(server.argName(i));
        Serial.print(" = ");
        Serial.println(server.arg(i));
    }
    server.send(200);
}

void handleFileFlash() {  // upload a new file to the SPIFFS
    String filename = "";
    HTTPUpload &upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        filename = upload.filename;
        if (!filename.startsWith("/")) filename = "/" + filename;
        Serial.print("handleFileUpload Name: ");

        filename = FIRMWARE_FILE;
        String old_filename = OLD_FIRMWARE_FILE;
        Serial.println(filename);

        if (HttpServer::memory->sd.exists(
                const_cast<char *>(filename.c_str()))) {
            removeFileIfExists(OLD_FIRMWARE_FILE);
            HttpServer::memory->sd.rename(
                const_cast<char *>(filename.c_str()),
                const_cast<char *>(old_filename.c_str()));
        }

        fsUploadFile.open(filename.c_str(), O_RDWR | O_CREAT);

        filename = String();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (fsUploadFile) fsUploadFile.write(upload.buf, upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
        if (fsUploadFile) {        // If the file was successfully created
            fsUploadFile.close();  // Close the file again
            Serial.print("handleFileUpload Size: ");
            Serial.println(upload.totalSize);
            server.sendHeader("Location", "/");
            server.send(303);
            HttpServer::flasher->FlashESP32();

        } else {
            server.send(500, "text/plain", "500: couldn't create file");
        }
    }
}

void addJsonArray(String dir, JsonVariant *variant) {
    SdFatJson dirFile;
    SdFatJson file;

    JsonObject json_dir = variant->createNestedObject();

    if (!dirFile.open(const_cast<char *>(dir.c_str()), O_RDONLY)) {
        json_dir["error"] = "cannot open file";
    }

    json_dir["name"] = dirFile.getStringName();
    json_dir["type"] = "Dir";
    JsonArray files = json_dir.createNestedArray("Files");

    while (file.openNext(&dirFile, O_RDONLY)) {
        if (!file.isSubDir()) {
            JsonObject json_file = files.createNestedObject();
            json_file["name"] = file.getStringName();
            json_file["type"] = "File";
            json_file["size"] = file.fileSize();
            if (file.isHidden()) {
                json_file["hidden"] = "true";
            } else {
                json_file["hidden"] = "false";
            }

        } else if (file.isSubDir()) {
            JsonVariant files_variant = files;
            if (dir.endsWith("/")) {
                addJsonArray(dir + file.getStringName(), &files_variant);
            } else {
                addJsonArray(dir + "/" + file.getStringName(), &files_variant);
            }
        }
        file.close();
    }
}

String generateJson() {
    DynamicJsonDocument doc(4096);
    JsonVariant variant = doc.to<JsonVariant>();
    addJsonArray("/", &variant);

    String Json = "";
    serializeJson(doc, Json);
    return Json;
}

void HttpServer::setBTmsg(String msg) {
    bt_console += "<<< " + msg;
    bt_console += ">>> OK\n";
}

uint8_t HttpServer::loadHttpSettingsFromFile(File http_file) {
    DynamicJsonDocument doc(1024);
    String json_raw = "";
    if (http_file) {
        while (http_file.available()) {
            json_raw += (char)http_file.read();
        }
        http_file.close();

        DeserializationError error = deserializeJson(doc, json_raw);

        if (error) {
            Serial.print(http_file.name());
            Serial.print(" Json deserialize failed: ");
            Serial.println(error.c_str());
            return false;
        }

        JsonObject json = doc.as<JsonObject>();

        if (json.containsKey("MDNS_name")) {
            if (json["MDNS_name"].is<String>()) {
                mdns_name = json["MDNS_name"].as<String>();
                Serial.println(mdns_name);
            }
        }
    } else {
        Serial.println("error opening network settings file");
        return false;
    }
    return true;
}

HttpServer::HttpServer(Flasher *flasher2, MemoryManager *memory2,
                       File http_file) {
    memory = memory2;
    flasher = flasher2;

    loadHttpSettingsFromFile(http_file);
    if (MDNS.begin(const_cast<char *>(mdns_name.c_str()))) {
        Serial.println("MDNS responder started");
    }

    //  if (MDNS.begin("esp32")) {
    //      Serial.println("MDNS responder started");
    //  }

    server.on("/", HomePage);

    server.on("/styles.css", []() {
        File file = memory->sd.open("WWW/styles.css");
        size_t sent = server.streamFile(file, "text/css");
        file.close();
    });

    server.on("/mkdir.png", []() {
        File file = memory->sd.open("WWW/icons/mkdir.png");
        size_t sent = server.streamFile(file, "image/png");
        file.close();
    });

    server.on("/delete.png", []() {
        File file = memory->sd.open("WWW/icons/delete.png");
        size_t sent = server.streamFile(file, "image/png");
        file.close();
    });

    server.on("/download.png", []() {
        File file = memory->sd.open("WWW/icons/download.png");
        size_t sent = server.streamFile(file, "image/png");
        file.close();
    });

    server.on("/flash.png", []() {
        File file = memory->sd.open("WWW/icons/flash.png");
        size_t sent = server.streamFile(file, "image/png");
        file.close();
    });

    server.on("/upload.png", []() {
        File file = memory->sd.open("WWW/icons/upload.png");
        size_t sent = server.streamFile(file, "image/png");
        file.close();
    });

    server.on("/reload.png", []() {
        File file = memory->sd.open("WWW/icons/reload.png");
        size_t sent = server.streamFile(file, "image/png");
        file.close();
    });

    server.on("/data.json",
              []() { server.send(200, "text/plain", generateJson()); });

    server.on("/delete", []() {
        uint8_t success_flag = false;
        if (server.hasArg("name") && server.arg("name") != NULL &&
            server.hasArg("type") && server.arg("type") != NULL) {
            String filename = server.arg("name");
            if (server.arg("type") == "file") {
                if (removeFileIfExists(filename)) {
                    success_flag = true;
                }

            } else if (server.arg("type") == "dir") {
                if (removeDirIfExists(filename)) {
                    success_flag = true;
                }
            }
        }
        if (success_flag) {
            server.send(204);
        } else {
            server.send(500);
        }
    });

    server.on("/mkdir", []() {
        if (server.hasArg("name") && server.arg("name") != NULL &&
            server.hasArg("dir") && server.arg("dir") != NULL) {
            String filename = server.arg("dir");
            if (!filename.endsWith("/")) filename += "/";
            filename += server.arg("name");
            if (HttpServer::memory->sd.mkdir(
                    const_cast<char *>(filename.c_str()), false)) {
                server.send(204);
            } else {
                server.send(500);
            }
        } else {
            server.send(500);
        }

        // server.sendHeader("Location", "/");
        // server.send(303);
    });

    server.on("/download", []() {
        if (server.hasArg("file") && server.arg("file") != NULL) {
            String filename = server.arg("file");
            String download_name = filename;
            if (server.hasArg("name") && server.arg("name") != NULL) {
                download_name = server.arg("name");
            }

            File download;
            download.open(const_cast<char *>(filename.c_str()));
            if (download) {
                server.sendHeader("Content-Type", "text/text");
                server.sendHeader("Content-Disposition",
                                  "attachment; filename=" + download_name);
                server.sendHeader("Connection", "close");
                server.streamFile(download, "application/octet-stream");

                download.close();
            }
        }
        server.sendHeader("Location", "/");
        server.send(303);
    });

    // server.on("/network", handleRoot);
    //  server.on("/sent_bt_msg_from_page", handleRoot);

    server.on("/sent_bt_msg_from_page", []() {
        if (server.hasArg("value") && server.arg("value") != NULL) {
            HttpServer::setBTmsg(server.arg("value"));
            server.send(204);
        } else {
            server.send(500);
        }
    });

    server.on("/inline",
              []() { server.send(200, "text/plain", "this works as well"); });

    server.on("/update", []() {
       // if (HttpServer::flasher->FlashESP32()) {
           HttpServer::flasher->FlashESP32();
            server.send(204);
    //    } else {
    //        server.send(500);
     //   }
    });

    server.on("/flash", HTTP_GET,
              []() {  // if the client requests the upload page
                  File file;
                  file.open("WWW/upload.html");
                  size_t sent = server.streamFile(file, "text/html");
                  file.close();
              });

    server.on(
        "/flash", HTTP_POST, []() { server.send(200); }, handleFileFlash);

    server.on("/upload", HTTP_GET,
              []() {  // if the client requests the upload page
                  File file;
                  file.open("WWW/upload.html");
                  size_t sent = server.streamFile(file, "text/html");
                  file.close();
              });

    server.on(
        "/upload", HTTP_POST, []() { server.send(200); }, handleFileUpload);
    // server.on("/upload", HTTP_POST,  // Send status 200 (OK) to tell the
    // client
    //                                  // we are ready to receive
    //           handleFileUploadData,
    //           handleFileUpload  // Receive and save the file
    // );

    // server.onNotFound(handleNotFound);

    server.on("/reboot", []() {
        // server.sendHeader("Location", "/");
        // server.send(303);
        server.send(204);
        ESP.restart();
    });

    server.on("/readBT", []() { server.send(200, "text/plain", bt_console); });

    server.begin();
    Serial.println("HTTP server started");
}

void HttpServer::process() { server.handleClient(); }