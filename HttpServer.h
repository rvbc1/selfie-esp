#ifndef HttpServer_h
#define HttpServer_h

#define HTTP_PORT 80

#include <ArduinoJson.h>
#include <SdFat.h>

#include "MemoryManager.h"
#include "Flasher.h"

class HttpServer {
   public:
    HttpServer(Flasher* flasher2, MemoryManager *memory2, File http_file);
    static void setBTmsg(String msg);
    void process();

    // private:
    static MemoryManager *memory;
    static Flasher *flasher;
    static String bt_console;

   private:
    String mdns_name = "esp32";
    uint8_t loadHttpSettingsFromFile(File http_file);
};

#endif