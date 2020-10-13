#include "FileReader.h"

FileReader::FileReader(std::string file_path) {
    // std::streampos size;
    // char* memblock;

    std::ifstream file(file_path, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        size = file.tellg();
        memblock = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(memblock, size);
        file.close();

        //std::cout << "the entire file content is in memory, " << size << " bytes";
        printFile();

        //delete[] memblock;
        this->file.size = size;
        this->file.data = (uint8_t*) memblock;
    } else
        std::cout << "Unable to open file";
}

void FileReader::printFile() {
    for (int i = 0; i < file.size; i++) {
        if (i % 16 == 0) {
            std::cout << std::endl;
        }

        if (((uint8_t)memblock[i]) < 16) {
            std::cout << "0x0" << std::hex << +((uint8_t)file.data[i]) << "\t";
        } else {
            std::cout << "0x" << std::hex << +((uint8_t)file.data[i]) << "\t";
        }
    }
    std::cout << std::dec << std::endl;
}

void FileReader::printPages() {
    std::cout << "the entire file content is in memory, " << size << " bytes" << std::endl;
    int pages = size / 256;
    int unfull_page_size = size - (int)(pages * 256);
    if (unfull_page_size > 0) {
        std::cout << "it is  " << pages << " full pages and " << unfull_page_size << " bytes at last page" << std::endl;
    } else {
        std::cout << "it is  " << pages << " full pages" << std::endl;
    }

    std::cout << "because " << pages << " * 256 + " << unfull_page_size << " = " << pages * 256 << " + " << unfull_page_size << " = " << pages * 256 + unfull_page_size << std::endl;
}

FileReader::file_struct FileReader::getFile(){
    return file;
}