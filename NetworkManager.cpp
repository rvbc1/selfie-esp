#include "NetworkManager.h"

NetworkManager::NetworkManager(File networks_file) {
    loadDataFromSettingsFile(networks_file);

    if (using_access_point && using_existing_wifi) {
        WiFi.mode(WIFI_AP_STA);
        startAccessPoint();
        connectToSavedNetwork();

    } else if (using_access_point) {
        WiFi.mode(WIFI_AP);
        startAccessPoint();
    } else if (using_existing_wifi) {
        WiFi.mode(WIFI_STA);
        connectToSavedNetwork();
    }
}

void NetworkManager::startAccessPoint() {
    if ((using_access_point_static_IP) &&
        (access_point_flagsIP == 0b00000111)) {
        if (!WiFi.softAPConfig(access_point_static_IP, access_point_gateway,
                               access_point_subnet)) {
            Serial.println("WiFi AP Failed to configure Correctly");
        }
    }

    String ap_password = "";
    if (access_point_use_password) {
        ap_password = access_point_password;
    }

    WiFi.softAP(access_point_ssid.c_str(), ap_password.c_str(),
                (int)access_point_channel, (int)access_point_hidden,
                (int)access_point_max_connection);
}

void NetworkManager::connectToSavedNetwork() {
    if ((using_static_IP) && (flagsIP >= 0b00000111)) {
        if ((using_DNS) && (flagsIP == 0b00001111)) {
            if (!WiFi.config(static_IP, gateway, subnet, dns)) {
                Serial.println("WiFi STATION Failed to configure Correctly");
            }
        } else {
            if (!WiFi.config(static_IP, gateway, subnet)) {
                Serial.println("WiFi STATION Failed to configure Correctly");
            }
        }
    }

    for (network_struct* n : networks) {
        wifiMulti.addAP(n->ssid.c_str(), n->pass.c_str());
    }

    Serial.println("Connecting ...");

    uint8_t time_to_timeout = timeout;
    while ((wifiMulti.run() != WL_CONNECTED) && (time_to_timeout > 0)) {
        delay(250);
        Serial.print('.');
        time_to_timeout--;
    }
    if (time_to_timeout > 0) {
        Serial.println("\nConnected to " + WiFi.SSID() +
                       " Use IP address: " + WiFi.localIP().toString());
    } else {
        Serial.println("\nNot connected - timeout");
    }
}

bool inRange(unsigned low, unsigned high, unsigned x) {
    return ((x - low) <= (high - low));
}

uint8_t NetworkManager::loadDataFromSettingsFile(File networks_file) {
    DynamicJsonDocument doc(1024);
    String json_raw = "";
    if (networks_file) {
        while (networks_file.available()) {
            json_raw += (char)networks_file.read();
        }
        networks_file.close();

        DeserializationError error = deserializeJson(doc, json_raw);

        if (error) {
            Serial.print(networks_file.name());
            Serial.print(" Json deserialize failed: ");
            Serial.println(error.c_str());
            return false;
        }

        JsonObject json = doc.as<JsonObject>();

        if (json.containsKey("Access_Point")) {
            if (json["Access_Point"].is<JsonObject>()) {
                loadAccessPointSettings(json["Access_Point"].as<JsonObject>());
            }
        }

        if (json.containsKey("Existing_Networks")) {
            if (json["Existing_Networks"].is<JsonObject>()) {
                loadNetworkSettings(json["Existing_Networks"].as<JsonObject>());
                loadSavedNetworks(json["Existing_Networks"].as<JsonObject>());
            }
        }

    } else {
        Serial.println("error opening network settings file");
        return false;
    }
    return true;
}

void NetworkManager::loadAccessPointStaticIP(JsonObject json) {
    if (json.containsKey("use")) {
        if (json["use"].is<bool>()) {
            using_access_point_static_IP = json["use"].as<bool>();
        }
    }

    if (json.containsKey("IP")) {
        if (json["IP"].is<String>()) {
            if (access_point_static_IP.fromString(json["IP"].as<String>())) {
                setBit(access_point_flagsIP, STATIC_IP_FLAG_BIT);
            }
        }
    }

    if (json.containsKey("Gateway")) {
        if (json["Gateway"].is<String>()) {
            if (access_point_gateway.fromString(json["Gateway"].as<String>())) {
                setBit(access_point_flagsIP, GATEWAY_FLAG_BIT);
            }
        }
    }

    if (json.containsKey("Subnet")) {
        if (json["Subnet"].is<String>()) {
            if (access_point_subnet.fromString(json["Subnet"].as<String>())) {
                setBit(access_point_flagsIP, SUBNET_FLAG_BIT);
            }
        }
    }
}

void NetworkManager::loadAccessPointSettings(JsonObject json) {
    if (json.containsKey("use")) {
        if (json["use"].is<bool>()) {
            using_access_point = json["use"].as<bool>();
        }
    }

    if (json.containsKey("ssid")) {
        if (json["ssid"].is<String>()) {
            access_point_ssid = json["ssid"].as<String>();
        }
    }

    if (json.containsKey("use_password")) {
        if (json["use_password"].is<bool>()) {
            access_point_use_password = json["use_password"].as<bool>();
        }
    }

    if (json.containsKey("password")) {
        if (json["password"].is<String>()) {
            access_point_password = json["password"].as<String>();
        }
    }

    if (json.containsKey("channel")) {
        if (json["channel"].is<uint8_t>()) {
            access_point_channel = json["channel"].as<uint8_t>();
            if (!inRange(1, 13, access_point_channel)) {
                Serial.print("  chanel: ");
                Serial.println(access_point_channel);
                access_point_channel = DEFAULT_AP_CHANNEL;
            }
        }
    }

    if (json.containsKey("hidden")) {
        if (json["hidden"].is<bool>()) {
            access_point_hidden = json["hidden"].as<bool>();
        }
    }

    if (json.containsKey("max_connection")) {
        if (json["max_connection"].is<uint8_t>()) {
            access_point_max_connection = json["max_connection"].as<uint8_t>();
            if (!inRange(1, 4, access_point_max_connection)) {
                Serial.print("  max_connection: ");
                Serial.println(access_point_max_connection);
                access_point_max_connection = DEFAULT_AP_MAX_CONNECTION;
            }
        }
    }

    if (json.containsKey("static_IP")) {
        if (json["static_IP"].is<JsonObject>()) {
            loadAccessPointStaticIP(json["static_IP"].as<JsonObject>());
        }
    }
}

void NetworkManager::loadNetworkSettings(JsonObject json) {
    if (json.containsKey("connect")) {
        if (json["connect"].is<bool>()) {
            using_existing_wifi = json["connect"].as<bool>();
        }
    }

    if (json.containsKey("timeout")) {
        if (json["timeout"].is<uint8_t>()) {
            timeout = json["timeout"].as<uint8_t>();
            if (!inRange(1, 100, timeout)) {
                Serial.print("  timeout: ");
                Serial.println(timeout);
                timeout = DEFAULT_TIMEOUT;
            }
        }
    }

    if (json.containsKey("static_IP")) {
        if (json["static_IP"].is<JsonObject>()) {
            loadStaticIP(json["static_IP"].as<JsonObject>());
        }
    }
}

void NetworkManager::loadStaticIP(JsonObject json) {
    if (json.containsKey("use")) {
        if (json["use"].is<bool>()) {
            using_static_IP = json["use"].as<bool>();
        }
    }

    if (json.containsKey("IP")) {
        if (json["IP"].is<String>()) {
            if (static_IP.fromString(json["IP"].as<String>())) {
                setBit(flagsIP, STATIC_IP_FLAG_BIT);
            }
        }
    }

    if (json.containsKey("Gateway")) {
        if (json["Gateway"].is<String>()) {
            if (gateway.fromString(json["Gateway"].as<String>())) {
                setBit(flagsIP, GATEWAY_FLAG_BIT);
            }
        }
    }

    if (json.containsKey("Subnet")) {
        if (json["Subnet"].is<String>()) {
            if (subnet.fromString(json["Subnet"].as<String>())) {
                setBit(flagsIP, SUBNET_FLAG_BIT);
            }
        }
    }

    if (json.containsKey("DNS")) {
        if (json["DNS"].is<JsonObject>()) {
            loadDNS(json["DNS"].as<JsonObject>());
        }
    }
}

void NetworkManager::loadDNS(JsonObject json) {
    if (json.containsKey("use")) {
        if (json["use"].is<bool>()) {
            using_DNS = json["use"].as<bool>();
        }
    }

    if (json.containsKey("IP")) {
        if (json["IP"].is<String>()) {
            if (dns.fromString(json["IP"].as<String>())) {
                setBit(flagsIP, DNS_FLAG_BIT);
            }
        }
    }
}

void NetworkManager::loadSavedNetworks(JsonObject json) {
    if (json.containsKey("Saved_Networks")) {
        if (json["Saved_Networks"].is<JsonArray>()) {
            JsonArray array = json["Saved_Networks"].as<JsonArray>();

            for (JsonVariant v : array) {
                network_struct* network = new network_struct;
                if (v.containsKey("ssid")) {
                    if (v["ssid"].is<String>()) {
                        network->ssid = v["ssid"].as<String>();
                    }
                }
                if (v.containsKey("password")) {
                    if (v["password"].is<String>()) {
                        network->pass = v["password"].as<String>();
                    }
                } else {
                    network->pass = "";
                }
                networks.push_back(network);
            }
        }
    }
}

void NetworkManager::printSavedNetworks(uint8_t show_password) {
    Serial.println("Saved Networks: ");
    for (network_struct* n : networks) {
        Serial.print("  ssid: ");
        Serial.print(n->ssid);
        if (show_password) {
            Serial.print("  password: ");
            Serial.print(n->pass);
        }
        Serial.println();
    }
}

void NetworkManager::printLoadedData() {
    Serial.println("\nLoaded Network Data:\n");
    Serial.println("Access Point:");
    if (using_access_point)
        Serial.println("  using: yes");
    else
        Serial.println("  using: no");

    Serial.println("  ssid: " + access_point_ssid);
    Serial.println("  password: " + access_point_password);

    if (access_point_use_password)
        Serial.println("  using password: yes");
    else
        Serial.println("  using password: no");

    Serial.print("  channel: ");
    Serial.println(((int)access_point_channel));

    if (access_point_hidden)
        Serial.println("  hidden: yes");
    else
        Serial.println("  hidden: no");

    Serial.print("  max_connection: ");
    Serial.println(((int)access_point_max_connection));

      Serial.println("  static IP:");
    if (using_static_IP)
        Serial.println("    using: yes");
    else
        Serial.println("    using: no");

    Serial.println("    IP: " + access_point_static_IP.toString());
    if (getBit(access_point_flagsIP, STATIC_IP_FLAG_BIT))
        Serial.println("      valid: yes");
    else
        Serial.println("      valid: no");

    Serial.println("    Gateway: " + gateway.toString());
    if (getBit(access_point_flagsIP, GATEWAY_FLAG_BIT))
        Serial.println("      valid: yes");
    else
        Serial.println("      valid: no");

    Serial.println("    Subnet: " + subnet.toString());
    if (getBit(access_point_flagsIP, SUBNET_FLAG_BIT))
        Serial.println("      valid: yes");
    else
        Serial.println("      valid: no");

    Serial.println("\nSaved Networks Settings:");

    if (using_existing_wifi)
        Serial.println("  connect: yes");
    else
        Serial.println("  connect: no");

    Serial.print("  timeout: ");
    Serial.println(((int)timeout));

    Serial.println("  static IP:");
    if (using_static_IP)
        Serial.println("    using: yes");
    else
        Serial.println("    using: no");

    Serial.println("    IP: " + static_IP.toString());
    if (getBit(flagsIP, STATIC_IP_FLAG_BIT))
        Serial.println("      valid: yes");
    else
        Serial.println("      valid: no");

    Serial.println("    Gateway: " + gateway.toString());
    if (getBit(flagsIP, GATEWAY_FLAG_BIT))
        Serial.println("      valid: yes");
    else
        Serial.println("      valid: no");

    Serial.println("    Subnet: " + subnet.toString());
    if (getBit(flagsIP, SUBNET_FLAG_BIT))
        Serial.println("      valid: yes");
    else
        Serial.println("      valid: no");

    Serial.println("    DNS: " + dns.toString());
    if (using_DNS)
        Serial.println("      using: yes");
    else
        Serial.println("      using: no");
    if (getBit(flagsIP, DNS_FLAG_BIT))
        Serial.println("      valid: yes");
    else
        Serial.println("      valid: no");

    Serial.println();
    printSavedNetworks();
    Serial.println();
}