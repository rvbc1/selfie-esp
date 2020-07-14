#include "NetworkManager.h"

IPAddress NetworkManager::local_IP;
IPAddress NetworkManager::gateway;
IPAddress NetworkManager::subnet;
IPAddress NetworkManager::dns;

std::vector<NetworkManager::network_struct> NetworkManager::printKnownNetworks(File networks_file) {
    std::vector<network_struct> networks;
    DynamicJsonDocument doc(1024);
    String json_networks = "";
   // File networks_file = SD.open(NETWORK_SETTINGS_FILE);
    if (networks_file) {
        while (networks_file.available()) {
            json_networks += (char)networks_file.read();
        }
        networks_file.close();




        DeserializationError error = deserializeJson(doc, json_networks);

        if (error) {
            Serial.print(F("Json deserialize failed: "));
            Serial.println(error.c_str());
        }

        const char* use_local_IP_string = doc["use_local_IP"];
        if(strcmp(use_local_IP_string, "yes") == 0){
            Serial.println("Using local IP: ");
            Serial.println(use_local_IP_string);
        } else {
            Serial.println("No Using local IP: ");
            Serial.println(use_local_IP_string);
        }

        const char* local_IP_string = doc["local_IP"];
        Serial.println(local_IP_string);


         if (local_IP.fromString(local_IP_string)) {
              Serial.println("is valid");
         }

        const char* gateway_string = doc["gateway"];
        Serial.println(gateway_string);


        if (gateway.fromString(gateway_string)) {
             Serial.println("is valid");
        }


        const char* subnet_string = doc["subnet"];
        Serial.println(subnet_string);


        if (subnet.fromString(subnet_string)) {
             Serial.println("is valid");
        }

        const char* dns_string = doc["dns"];
        Serial.println(dns_string);


        if (dns.fromString(dns_string)) {
             Serial.println("is valid");
        }



    
        JsonArray array = doc["networks"];

        for (JsonVariant v : array) {
            network_struct network;
            network.ssid = v["ssid"];
            network.pass = v["password"];
            networks.push_back(network);

            // const char *ssid = v["ssid"];
            // const char *pass = v["password"];

            // Serial.println(ssid);
            // Serial.println(pass);
        }

        for (network_struct n : networks) {
            Serial.println(n.ssid);
            Serial.println(n.pass);
        }
    } else {
        Serial.println("error opening test.txt");
    }
    return networks;
}