#ifndef UARTLink_h
#define UARTLink_h

#include <errno.h>  // Error integer and strerror() function
#include <fcntl.h>  // Contains file controls like O_RDWR
#include <stdio.h>
#include <string.h>
#include <termios.h>  // Contains POSIX terminal control definitions
#include <unistd.h>   // write(), read(), close()

#include <string>

class UARTLink {
   public:
    struct buffer_struct {
        uint8_t* data;
        uint16_t size;
        uint16_t max_size;
    } reading_buffer;

    buffer_struct writing_buffer;

    UARTLink(std::string port);
    uint8_t openPort();
    void closePort();

    void setFlags();

    void setSpeed();

    void writeData();
    //void writeData(uint8_t byte);


    int waitForResponse(uint64_t timeout);

    void addDataToBufferTX(uint8_t data);

    int available();
    buffer_struct* getBuff();

    int com_port = 17;    /* /dev/ttyS0 (COM1 on windows) */
    int baud_rate = 9600; /* 9600 baud */
    int buffer_size = 256;
    // uint8_t* buffer;

    // struct buffer_struct{
    //     uint8_t* data;
    //     uint16_t size;
    //     uint16_t max_size;
    // } buffer;

   private:
    std::string port;
    int serial_port;
    struct termios tty;
    char read_buf[256];
    uint8_t port_opened = false;

    void printData(std::string prefix, buffer_struct buffer);

    void errorMsg();
};

#endif