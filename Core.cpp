#include "Core.h"

#include "Update.h"

Core::Core() {
    Serial.begin(115200);
    delay(3000);

    MemoryManager *memory = new MemoryManager();




      NetworkManager* networkManager =
           new NetworkManager(memory->sd.open(NETWORK_SETTINGS_FILE));

      HttpServer* htttpServer = new HttpServer(memory);
    while (true) {
        htttpServer->process();
    }
}