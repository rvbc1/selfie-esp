#ifndef STM32Flasher_h
#define STM32Flasher_h

#include <string>

#include "FileReader.h"
#include "UARTLink.h"

class STM32Flasher {
   public:
    STM32Flasher(std::string port);

    void connect();

    void readChip();

    void flashFile(FileReader::file_struct file);
    void flashFile(uint8_t *data, uint16_t size);

   private:
    static const uint8_t START_CODE =              0x7F;
    static const uint8_t ACK =                     0x79;
    static const uint8_t NACK =                    0x1F;

    static const uint8_t GET_COMMAND =             0x00;
    static const uint8_t GET_VERSION_COMMAND =     0x01;
    static const uint8_t GET_ID_COMMAND =          0x02;
    
    static const uint8_t READ_MEMORY_COMMAND =     0x11;

    static const uint8_t GO_COMMAND =              0x21;

    static const uint8_t WRITE_MEMORY_COMMAND =    0x31;

    static const uint8_t ERASE_COMMAND =           0x43;
    static const uint8_t FULL_CHIP_ERASE_COMMAND = 0xFF;

    UARTLink *uart;

    UARTLink::buffer_struct *buffer;

    void openConnection();
    void getCommand();
    void getVersionCommand();
    void getIdCommand();
    void readMemoryCommand(uint32_t start_address, uint8_t length);
    void goCommand(uint32_t address);
    void flashCommand(uint32_t start_address, uint8_t *buffer, uint16_t length);
    void eraseCommand();

   private:
    uint8_t is_port_open = false;
    uint8_t is_connection_open = false;

    enum ack_pos { ACK_AT_BEGIN,
                   ACK_AT_END,
                   ACK_AT_BEGIN_AND_END,
                   NONE_ACK };

    void writeCommand(uint8_t command);
    void writeAddress(uint32_t address);
    uint8_t checkResponse(ack_pos pos = NONE_ACK);
};

#endif