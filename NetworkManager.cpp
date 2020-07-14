#include "NetworkManager.h"

NetworkManager::NetworkManager(File networks_file) {
    loadDataFromSettingsFile(networks_file);

    if (!WiFi.config(local_IP, gateway, subnet, dns)) {  
        Serial.println("WiFi STATION Failed to configure Correctly");
    }

    for (NetworkManager::network_struct n : networks) {
        //Serial.println(n.ssid);
        wifiMulti.addAP(n.ssid, n.pass);
    }

    Serial.println("Connecting ...");
    while (wifiMulti.run() != WL_CONNECTED) { 
        delay(250);
        Serial.print('.');
    }

    Serial.println( "\nConnected to " + 
                    WiFi.SSID() +
                   " Use IP address: " + 
                   WiFi.localIP().toString());
}

uint8_t NetworkManager::loadDataFromSettingsFile(File networks_file) {
    DynamicJsonDocument doc(1024);
    String json_networks = "";
    if (networks_file) {
        while (networks_file.available()) {
            json_networks += (char)networks_file.read();
        }
        networks_file.close();

        DeserializationError error = deserializeJson(doc, json_networks);

        if (error) {
            Serial.print(F("Json deserialize failed: "));
            Serial.println(error.c_str());
            return false;
        }

        loadNetworkSettings(doc);
        loadSavedNetworks(doc);

    } else {
        Serial.println("error opening test.txt");
        return false;
    }
    return true;
}

void NetworkManager::loadNetworkSettings(DynamicJsonDocument doc) {
    const char* use_local_IP_string = doc["use_local_IP"];
    if (strcmp(use_local_IP_string, "yes") == 0) {
        Serial.print("Using local IP: ");
        Serial.println(use_local_IP_string);
    } else {
        Serial.print("Using local IP: ");
        Serial.println(use_local_IP_string);
    }

    const char* local_IP_string = doc["local_IP"];
    if (local_IP.fromString(local_IP_string)) {
        Serial.print("local_IP is valid: ");
        Serial.println(local_IP_string);
    }

    const char* gateway_string = doc["gateway"];
    if (gateway.fromString(gateway_string)) {
        Serial.print("gateway is valid: ");
        Serial.println(gateway_string);
    }

    const char* subnet_string = doc["subnet"];
    if (subnet.fromString(subnet_string)) {
        Serial.print("subnet is valid: ");
        Serial.println(subnet_string);
    }

    const char* dns_string = doc["dns"];
    if (dns.fromString(dns_string)) {
        Serial.print("dns is valid: ");
        Serial.println(dns_string);
    }
}

void NetworkManager::loadSavedNetworks(DynamicJsonDocument doc) {
    JsonArray array = doc["networks"];

    for (JsonVariant v : array) {
        network_struct network;
        network.ssid = v["ssid"];
        network.pass = v["password"];
        networks.push_back(network);
    }

    printKnownNetworks(true);
}

void NetworkManager::printKnownNetworks(uint8_t show_password){
    Serial.println("Saved Networks: ");
    for (network_struct n : networks) {
        Serial.print("  ssid: ");
        Serial.print(n.ssid);
        if(show_password){
            Serial.print("  password: ");
            Serial.print(n.pass);
        }
        Serial.println();
    }
}