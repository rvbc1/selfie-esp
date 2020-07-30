#ifndef HttpServer_h
#define HttpServer_h

#define HTTP_PORT 80

#include <SdFat.h>
#include "MemoryManager.h"


class HttpServer {
   public:
   HttpServer(MemoryManager *memory2);
   void process();
   //private:
   static MemoryManager *memory;
};

#endif