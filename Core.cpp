#include "Core.h"

#include "BluetoothSerial.h"
#include "Update.h"

BluetoothSerial SerialBT;

TaskHandle_t Task1;
TaskHandle_t Task2;

HttpServer* htttpServer = nullptr;

// Task1code: blinks an LED every 1000 ms
void Task1code(void* pvParameters) {
    Serial.print("Task1 running on core ");
    Serial.println(xPortGetCoreID());

    while (true) {
        htttpServer->process();
    }
}

// Task2code: blinks an LED every 700 ms
void Task2code(void* pvParameters) {
    Serial.print("Task2 running on core ");
    Serial.println(xPortGetCoreID());

    SerialBT.begin("ESP32test");
    delay(3000);

    String bt_data = "";

    while (true) {
        if (SerialBT.available()) {
            bt_data = "";
            while (SerialBT.available()) {
                bt_data += (char)SerialBT.read();
                // Serial.write(SerialBT.read());
            }
            Serial.print(bt_data.c_str());
            if (htttpServer != nullptr) {
                htttpServer->setBTmsg(bt_data);
            }
        }
    }
}

Core::Core() {
    Serial.begin(115200);
    delay(3000);

    MemoryManager* memory = new MemoryManager();

    Serial.print("Opening ");
    Serial.println(NETWORK_SETTINGS_FILE);
    NetworkManager* networkManager =
        new NetworkManager(memory->sd.open(NETWORK_SETTINGS_FILE));

    networkManager->printLoadedData();

    htttpServer = new HttpServer(memory);

    xTaskCreatePinnedToCore(
        Task2code, /* Task function. */
        "Task2",   /* name of task. */
        10000,     /* Stack size of task */
        NULL,      /* parameter of the task */
        1,         /* priority of the task */
        &Task2,    /* Task handle to keep track of created task */
        1);        /* pin task to core 1 */
    delay(500);

    while (true) {
        htttpServer->process();
    }

    // // create a task that will be executed in the Task1code() function, with
    // // priority 1 and executed on core 0
    // xTaskCreatePinnedToCore(
    //     Task1code, /* Task function. */
    //     "Task1",   /* name of task. */
    //     10000,     /* Stack size of task */
    //     NULL,      /* parameter of the task */
    //     1,         /* priority of the task */
    //     &Task1,    /* Task handle to keep track of created task */
    //     0);        /* pin task to core 0 */
    // delay(500);

    // create a task that will be executed in the Task2code() function, with
    // priority 1 and executed on core 1
}
