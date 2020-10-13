#ifndef FileReader_h
#define FileReader_h

#include <fstream>
#include <iostream>
#include <string>

class FileReader {
   public:
   struct file_struct{
       uint8_t *data;
       uint16_t size;
   };
    FileReader(std::string file_path);
    void printFile();
    void printPages();
    file_struct getFile();

   private:
    //std::streampos size;
    uint16_t size;
    char* memblock;
    file_struct file;
};

#endif