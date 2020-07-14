#ifndef NetworkManager_h
#define NetworkManager_h

#include <ArduinoJson.h>
#include <mySD.h>
#include "Settings.h"
#include <WiFi.h>   

class NetworkManager {
   public:
    static IPAddress local_IP;
    static IPAddress gateway;
    static IPAddress subnet;
    static IPAddress dns;

    struct network_struct {
        const char *ssid;
        const char *pass;
    };

    static std::vector<network_struct> printKnownNetworks(File root);



   private:
    std::vector<network_struct> networks;
};

#endif