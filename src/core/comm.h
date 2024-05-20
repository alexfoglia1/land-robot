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
    uint16_t checksum;
} CtrlMessage;


class Comm
{
public:
    Comm();

    bool networkInit();
    void start();
    void handleMessageRx(CtrlMessage* message);

    int _serverSocket;
    struct sockaddr_in _addr;

    bool backwardData();
    uint16_t throttleData();
    int16_t turnData();

private:
    bool _init;
    bool _started;

    bool _backwardData;
    uint16_t _throttleData;
    int16_t _turnData;


};

#endif
