#ifndef Flasher_h
#define Flasher_h

#include "Update.h"
#include "Settings.h"
#include "MemoryManager.h"


class Flasher {
   public:
   Flasher(MemoryManager* memory);
   uint8_t FlashESP32(String filename);
   private:
    MemoryManager* memory;
};

#endif