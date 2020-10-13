#include "STM32Flasher.h"

#include <chrono>
#include <iostream>

void printInfo(std::string info) {
    std::cout << info << std::endl;
}

STM32Flasher::STM32Flasher(std::string port) {
    uart = new UARTLink(port);
    is_port_open = uart->openPort();
    //  uint8_t *buffer = uart->getBuff();
    buffer = uart->getBuff();
    //writing_buffer = &uart->writing_buffer;

    //readMemoryCommand(0x8000000, 0xFF);
    //eraseCommand();
    //readMemoryCommand(0x8000000, 0xFF);
    //uint8_t test;
    //flashCommand(0x8000000, &test, 0xFF);
    //readMemoryCommand(0x8000000, 0xFF);
}

void STM32Flasher::openConnection() {
    if (is_port_open) {
        uart->addDataToBufferTX(START_CODE);
        uart->writeData();
        is_connection_open = checkResponse(ACK_AT_END);

#if defined(__linux__) || defined(__FreeBSD__) || (__Windows__)
        if (is_connection_open == false){
            printInfo("Cannot start comunication with uC");
            printInfo("Check wiring & set bootloader at chip");
            exit(1);
        }
#endif

    }
}

void STM32Flasher::getCommand() {
    if (is_port_open && is_connection_open) {
        writeCommand(GET_COMMAND);
        checkResponse();

        //std::cout << "Frame size: " << std::dec << +buffer->data[1] << std::endl;
        //std::cout << "bootloader: " << std::hex << +buffer->data[2] << std::endl;
    }
}

void STM32Flasher::getVersionCommand() {
    if (is_port_open && is_connection_open) {
        writeCommand(GET_VERSION_COMMAND);
        checkResponse();
    }
}

void STM32Flasher::getIdCommand() {
    if (is_port_open && is_connection_open) {
        writeCommand(GET_ID_COMMAND);
        checkResponse();
    }
}

void STM32Flasher::readMemoryCommand(uint32_t start_address, uint8_t length) {
    if (is_port_open && is_connection_open) {
        writeCommand(READ_MEMORY_COMMAND);
        checkResponse();

        // writing_buffer->data[0] = *((uint8_t *) &start_address + 3);
        // writing_buffer->data[0] = *((uint8_t *) &start_address + 3);
        // writing_buffer->data[0] = *((uint8_t *) &start_address + 3);

        // uart->writeData(*((uint8_t *) &start_address + 2));
        // uart->writeData(*((uint8_t *) &start_address + 1));
        // uart->writeData(*((uint8_t *) &start_address + 0));

        // uart->writeData(0x08);
        writeAddress(start_address);
        checkResponse();

        writeCommand(length);
        checkResponse();
    }
}

void STM32Flasher::goCommand(uint32_t address) {
    if (is_port_open && is_connection_open) {
        writeCommand(GO_COMMAND);
        checkResponse();
        writeAddress(address);
        checkResponse();
    }
}

void STM32Flasher::flashCommand(uint32_t start_address, uint8_t *buffer, uint16_t length) {
    if (is_port_open && is_connection_open) {
        writeCommand(WRITE_MEMORY_COMMAND);
        checkResponse();
        writeAddress(start_address);
        checkResponse();
        uart->addDataToBufferTX(length);
        uint8_t xor_checksum = 0x00;
        xor_checksum ^= length;
        for (int i = 0; i <= length; i++) {
            uart->addDataToBufferTX(buffer[i]);
            xor_checksum ^= buffer[i];
        }
        uart->addDataToBufferTX(xor_checksum);
        uart->writeData();
        checkResponse();
    }
}

void STM32Flasher::eraseCommand() {
    if (is_port_open && is_connection_open) {
        writeCommand(ERASE_COMMAND);
        checkResponse();

        writeCommand(FULL_CHIP_ERASE_COMMAND);
        checkResponse();
    }
}

void STM32Flasher::writeCommand(uint8_t command) {
    if (is_port_open && is_connection_open) {
        uart->addDataToBufferTX(command);
        uart->addDataToBufferTX(~command);
        uart->writeData();
    }
}

void STM32Flasher::flashFile(FileReader::file_struct file) {
    flashFile(file.data, file.size);
}

void STM32Flasher::flashFile(uint8_t *data, uint16_t size) {
    openConnection();
    getCommand();

    if (is_port_open && is_connection_open) {
        uint16_t pages = size / 256;
        uint16_t unfull_page_size = size - pages * 256;
        uint32_t address = 0;
        eraseCommand();
        for (int page_counter = 0; page_counter < pages; page_counter++, address += 256) {
            flashCommand(0x8000000 + address, &data[address], 0xff);
        }
        flashCommand(0x8000000 + address, &data[address], unfull_page_size - 1);

        goCommand(0x8000000);
    }
}

void STM32Flasher::writeAddress(uint32_t address) {
    uint8_t xor_checksum = 0x00;
    for (int i = 0; i < sizeof(uint32_t); i++) {
        uint8_t msb_byte = *((uint8_t *)&address + sizeof(uint32_t) - i - 1);
        uart->addDataToBufferTX(msb_byte);
        xor_checksum ^= msb_byte;
    }
    uart->addDataToBufferTX(xor_checksum);
    uart->writeData();
}

uint8_t STM32Flasher::checkResponse(ack_pos pos) {
    uint8_t ack_at_right_pos = false;

    if (is_port_open) {
        buffer->size = uart->waitForResponse(500);
        if (buffer->size > 0) {
            switch (pos) {
                case NONE_ACK:
                    ack_at_right_pos = true;
                    break;
                case ACK_AT_BEGIN:
                    if (buffer->data[0] == ACK) {
                        ack_at_right_pos = true;
                    } else {
                        ack_at_right_pos = false;
                    }
                    break;
                case ACK_AT_END:
                    if (buffer->data[buffer->size - 1] == ACK) {
                        ack_at_right_pos = true;
                    } else {
                        ack_at_right_pos = false;
                    }
                    break;
                case ACK_AT_BEGIN_AND_END:
                    if ((buffer->data[0] == ACK) && (buffer->data[buffer->size - 1] == ACK)) {
                        ack_at_right_pos = true;
                    } else {
                        ack_at_right_pos = false;
                    }
                    break;
            };
        }
    }
    return ack_at_right_pos;
}

void STM32Flasher::connect() {
}

void STM32Flasher::readChip() {
}