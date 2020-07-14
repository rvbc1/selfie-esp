#ifndef NetworkManager_h
#define NetworkManager_h

#include <ArduinoJson.h>
#include <WiFi.h>
#include <mySD.h>
#include <WiFiMulti.h> 

#include "Settings.h"

class NetworkManager {
   public:
    NetworkManager(File networks_file);

    struct network_struct {
        const char *ssid;
        const char *pass;
    };

    void printKnownNetworks(uint8_t show_password = false);
   private:
    WiFiMulti wifiMulti;
    
    IPAddress local_IP;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns;

    uint8_t loadDataFromSettingsFile(File networks_file);
    std::vector<network_struct> networks;

    void loadNetworkSettings(DynamicJsonDocument doc);
    void loadSavedNetworks(DynamicJsonDocument doc);
};

#endif