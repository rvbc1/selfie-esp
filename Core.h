#ifndef Core_h
#define Core_h

#include <ArduinoJson.h>
#include <ESP32WebServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include "NetworkManager.h"
#include "HttpServer.h"
#include "MemoryManager.h"
#include "Flasher.h"
#include "BluetoothManager.h"




class Core {
   public:
   Core();
   private:

};

#endif