#include "UARTLink.h"

#include <chrono>
#include <iostream>

//#include "rs232.h"

#include "Arduino.h"

//#define DEBUG

#define ESP32

UARTLink::UARTLink(std::string port) {
    //com_port = RS232_GetPortnr(port.c_str()); /* /dev/ttyS0 (COM1 on windows) */
    baud_rate = 115200;                       /* 9600 baud */

    reading_buffer.size = writing_buffer.size = 0;
    reading_buffer.max_size = writing_buffer.max_size = 256;

    reading_buffer.data = new uint8_t[reading_buffer.max_size];
    writing_buffer.data = new uint8_t[writing_buffer.max_size];
}

uint8_t UARTLink::openPort() {
   // char mode[] = {'8', 'E', '1', 0};
    //if (RS232_OpenComport(com_port, baud_rate, mode, 0)) {
   //     printf("Can not open comport\n");
   //     port_opened = false;

#if defined(__linux__) || defined(__FreeBSD__) || (__Windows__)
        exit(1);
#endif

   // } else {
        port_opened = true;
   // }

    return port_opened;
}

void UARTLink::addDataToBufferTX(uint8_t data) {
    writing_buffer.data[writing_buffer.size] = data;
    writing_buffer.size++;
}

void UARTLink::closePort() {
}

void UARTLink::setFlags() {
}

void UARTLink::setSpeed() {
}

void UARTLink::writeData() {
#ifdef DEBUG
printData("<< ", writing_buffer);
#endif

    if (port_opened) {
        //RS232_SendBuf(com_port, writing_buffer.data, writing_buffer.size);
        Serial2.write(writing_buffer.data, writing_buffer.size);
    } else {
        errorMsg();
    }

    writing_buffer.size = 0;
}

// void UARTLink::writeData(uint8_t byte) {
// #ifdef DEBUG
// printData("<< ", writing_buffer);
// #endif

//     if (port_opened) {
//         RS232_SendByte(com_port, byte);
//     } else {
//         errorMsg();
//     }
// }

int UARTLink::available() {
}

int UARTLink::waitForResponse(uint64_t timeout) {
    // auto start = std::chrono::system_clock::now();
    // auto end = start;
    // std::chrono::milliseconds ms{1000};

    unsigned long start = millis();
    unsigned long end = start;

    unsigned long ms = 500;

    reading_buffer.size = 0;
    while ((reading_buffer.size == 0) && (end - start < ms)) {
        //end = std::chrono::system_clock::now();
        end = millis();

        if (port_opened) {
            //reading_buffer.size = RS232_PollComport(com_port, reading_buffer.data, reading_buffer.max_size);
            reading_buffer.size = Serial2.available();
        } else {
            errorMsg();
        }
        
    }
    reading_buffer.size = Serial2.readBytes(reading_buffer.data, reading_buffer.size);

#ifdef DEBUG
    printData(">> ", reading_buffer);
#endif

    return reading_buffer.size;
}

UARTLink::buffer_struct* UARTLink::getBuff() {
    return &reading_buffer;
}

void UARTLink::errorMsg() {
    std::cout << "Port is not open!" << std::endl;
}

void UARTLink::printData(std::string prefix, buffer_struct buffer){
    // std::cout << prefix;
    // for (int i = 0; i < buffer.size; i++) {
    //     if(buffer.data[i] <= 0x0F){
    //         std::cout << "0x0" << std::hex << (int)buffer.data[i] << " ";
    //     } else {
    //         std::cout << "0x" << std::hex << (int)buffer.data[i] << " ";
    //     }

    // }
    // std::cout << std::endl;

    Serial.print(prefix.c_str());
        for (int i = 0; i < buffer.size; i++) {
            Serial.print(buffer.data[i], HEX);
    }
    Serial.println();
}