#ifndef Flasher_h
#define Flasher_h

#include "Update.h"
#include "Settings.h"
#include "MemoryManager.h"


class Flasher {
   public:
   Flasher(MemoryManager* memory);
   void FlashESP32();
   private:
    MemoryManager* memory;
};

#endif