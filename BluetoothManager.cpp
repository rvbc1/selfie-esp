#include "BluetoothManager.h"

BluetoothSerial SerialBT;

String blueooth_name = "ESP32test";

uint8_t loadBluetoothSettingsFromFile(File bluetooth_file) {
    DynamicJsonDocument doc(1024);
    String json_raw = "";
    if (bluetooth_file) {
        while (bluetooth_file.available()) {
            json_raw += (char)bluetooth_file.read();
        }
        bluetooth_file.close();

        DeserializationError error = deserializeJson(doc, json_raw);

        if (error) {
            Serial.print(bluetooth_file.name());
            Serial.print(" Json deserialize failed: ");
            Serial.println(error.c_str());
            return false;
        }

        JsonObject json = doc.as<JsonObject>();

        if (json.containsKey("name")) {
            if (json["name"].is<String>()) {
                blueooth_name = json["name"].as<String>();
                Serial.println(blueooth_name);
            }
        }
    } else {
        Serial.println("error opening network settings file");
        return false;
    }
    return true;
}

BluetoothManager::BluetoothManager(File bluetooth_file) {
    Serial.print("Task2 running on core ");
    Serial.println(xPortGetCoreID());

    loadBluetoothSettingsFromFile(bluetooth_file);

    SerialBT.begin(blueooth_name);
    delay(3000);
}
void BluetoothManager::process() {
    String bt_data = "";
    while (true) {
        if (SerialBT.available()) {
            bt_data = "";
            while (SerialBT.available()) {
                bt_data += (char)SerialBT.read();
                // Serial.write(SerialBT.read());
            }
            Serial.print(bt_data.c_str());
            recivedMessage(bt_data);
        }
    }
}

String BluetoothManager::getConversation() { return bt_conversation; }

void BluetoothManager::internalResponse(String message) {
    message.trim();
    message.toUpperCase();
    String response = "";
    if (message == "IP") {
        response = "IP: 192.168.1.105";
    }
    bt_conversation += ">>> " + response + "\n";
    SerialBT.println(response);
}

void BluetoothManager::recivedMessage(String message) {
    bt_conversation += "<<< " + message;
    if (message.startsWith("$")) {
        message.remove(0, 1);
        internalResponse(message);
    }
}
