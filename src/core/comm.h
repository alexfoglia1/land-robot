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
    TURN_LEFT,
    TURN_RIGHT
};


typedef struct
{
    uint8_t msgId;
    MsgType type;
    uint16_t data;
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
    uint16_t turnLeftData();
    uint16_t turnRightData();

private:
    bool _init;
    bool _started;

    bool _backwardData;
    uint16_t _throttleData;
    uint16_t _turnLeftData;
    uint16_t _turnRightData;


};

#endif
