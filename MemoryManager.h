#ifndef MemoryManager_h
#define MemoryManager_h

#define FS_NO_GLOBALS  // allow spiffs to coexist with SD card, define BEFORE
#include <FS.h>        // including FS.h
#include <SPIFFS.h>    //spiff file system

#include <BlockDriver.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>
#include <SysCall.h>

const uint8_t SOFT_MISO_PIN = 13;
const uint8_t SOFT_MOSI_PIN = 14;
const uint8_t SOFT_SCK_PIN = 27;
//
// Chip select may be constant or RAM variable.
const uint8_t SD_CHIP_SELECT_PIN = 26;

class MemoryManager {
   public:
    MemoryManager();

    SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;

   private:



};

#endif