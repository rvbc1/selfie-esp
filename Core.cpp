#include "Core.h"

#include "Update.h"

#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

Core::Core() {
    Serial.begin(115200);
    SerialBT.begin("ESP32test");
    delay(3000);

    MemoryManager *memory = new MemoryManager();

    NetworkManager* networkManager =
           new NetworkManager(memory->sd.open(NETWORK_SETTINGS_FILE));

      HttpServer* htttpServer = new HttpServer(memory);
    while (true) {
        htttpServer->process();
        if (SerialBT.available()) {
          Serial.write(SerialBT.read());
        }
    }
}