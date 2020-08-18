#ifndef NetworkManager_h
#define NetworkManager_h

#include <ArduinoJson.h>
#include <SdFat.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#include "Settings.h"
#include "bitoperations.h"

#define STATIC_IP_FLAG_BIT 0
#define GATEWAY_FLAG_BIT 1
#define SUBNET_FLAG_BIT 2
#define DNS_FLAG_BIT 3

#define DEFAULT_AP_CHANNEL 1
#define DEFAULT_AP_MAX_CONNECTION 4
#define DEFAULT_TIMEOUT 10

class NetworkManager {
   public:
    NetworkManager(File networks_file);

    struct network_struct {
        String ssid;
        String pass;
    };

    void printLoadedData();
    void printSavedNetworks(uint8_t show_password = false);

   private:
    uint8_t using_access_point = false;
    uint8_t using_existing_wifi = false;

    String access_point_ssid = "default_ssid";
    uint8_t access_point_use_password = false;
    String access_point_password = "";
    uint8_t access_point_channel = DEFAULT_AP_CHANNEL;
    uint8_t access_point_hidden = false;
    uint8_t access_point_max_connection = DEFAULT_AP_MAX_CONNECTION;

    WiFiMulti wifiMulti;

    IPAddress access_point_static_IP;
    IPAddress access_point_gateway;
    IPAddress access_point_subnet;

    IPAddress static_IP;
    IPAddress gateway;
    IPAddress subnet;
    IPAddress dns;

    uint8_t access_point_flagsIP = 0b00000000;
    uint8_t flagsIP = 0b00000000;

    uint8_t using_access_point_static_IP = false;
    uint8_t using_static_IP = false;
    uint8_t using_DNS = false;

    uint16_t timeout = DEFAULT_TIMEOUT;

    uint8_t loadDataFromSettingsFile(File networks_file);
    std::vector<network_struct*> networks;

    uint8_t checkKeyUse(JsonObject json);

    void loadAccessPointSettings(JsonObject json);
    void loadNetworkSettings(JsonObject json);
    void loadSavedNetworks(JsonObject json);

    void loadAccessPointStaticIP(JsonObject json);
    void loadStaticIP(JsonObject json);
    void loadDNS(JsonObject json);

    void startAccessPoint();
    void connectToSavedNetwork();
};

#endif