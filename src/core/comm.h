#ifndef COMM_H
#define COMM_H

#define SERVER_PORT 7777
#define COMMAND_MSG_ID 0x7E
#define BUFLEN 256

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

enum class MsgType : uint8_t
{
    THROTTLE = 0x00,
    BREAK,
    TURN
};


typedef struct
{
    int16_t xAxis;
    int16_t yAxis;
    int16_t throttle;
    uint16_t servo;
    uint16_t checksum;
} CtrlMessage;


enum class UartStatus : uint8_t
{
    WAIT_SYNC,
    WAIT_MESSAGE
};


class Comm
{
public:
    Comm();

    bool networkInit();
    bool serialInit();
    void start();
    void handleMessageRx(CtrlMessage* message);
    void handleUartByteRx(uint8_t byteRx);

    int _serverSocket;
    struct sockaddr_in _addr;

    int _serialFd;

    bool backwardData();
    uint16_t throttleData();
    uint16_t servoData();
    int16_t turnData();

private:
    bool _init;
    bool _uartInit;
    bool _started;
    UartStatus _uartStatus;
    int _uartRxLen;
    uint8_t _uartBuf[10];

    bool _backwardData;
    uint16_t _throttleData;
    uint16_t _servoData;
    int16_t _turnData;


};

#endif
