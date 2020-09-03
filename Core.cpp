#include "Core.h"

TaskHandle_t Task1;
TaskHandle_t Task2;

HttpServer* htttpServer = nullptr;
BluetoothManager* bt_manager = nullptr;

// Task1code: blinks an LED every 1000 ms
void Task1code(void* pvParameters) {
    Serial.print("Task1 running on core ");
    Serial.println(xPortGetCoreID());

    while (true) {
        htttpServer->process();
    }
}

// Task2code: blinks an LED every 700 ms
void Task2code(void* pvParameters) { bt_manager->process(); }

Core::Core() {
    Serial.begin(115200);
    delay(3000);

    MemoryManager* memory = new MemoryManager();

    Serial.print("Opening ");
    Serial.println(NETWORK_SETTINGS_FILE);
    NetworkManager* networkManager =
        new NetworkManager(memory->sd.open(NETWORK_SETTINGS_FILE));

    // networkManager->printLoadedData();

    Flasher* flasher = new Flasher(memory);

    bt_manager = new BluetoothManager(memory->sd.open(BLUETOOTH_SETTINGS_FILE));

    htttpServer = new HttpServer(flasher, bt_manager, networkManager, memory,
                                 memory->sd.open(HTTP_SETTINGS_FILE));



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
