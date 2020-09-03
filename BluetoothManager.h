#ifndef BluetoothManager_h
#define BluetoothManager_h

#include "BluetoothSerial.h"
#include <ArduinoJson.h>
#include <SdFat.h>

class BluetoothManager {
   public:
   BluetoothManager(File bluetooth_file);
   void recivedMessage(String message);
   void process();
   String getConversation();
   private:
   String bt_conversation = "";
   void internalResponse(String message);
};

#endif