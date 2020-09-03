#ifndef HttpServer_h
#define HttpServer_h

#define HTTP_PORT 80

#include <ArduinoJson.h>
#include <SdFat.h>

#include "MemoryManager.h"
#include "NetworkManager.h"
#include "Flasher.h"
#include "BluetoothManager.h"

class HttpServer {
   public:
    HttpServer(Flasher* flasher2, BluetoothManager* bt_manager2, NetworkManager *network2, MemoryManager *memory2, File http_file);
    void process();

    // private:
    static NetworkManager *network;
    static MemoryManager *memory;
    static Flasher *flasher;
    static BluetoothManager *bt_manager;
    static String bt_console;

   private:
    String mdns_name = "esp32";
    uint8_t loadHttpSettingsFromFile(File http_file);
};

#endif