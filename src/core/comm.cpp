#include "comm.h"

#include <string.h>
#include <stdio.h>
#include <pthread.h>

void* task(void* arg)
{
    Comm* comm = reinterpret_cast<Comm*>(arg);

    char buf[BUFLEN];

    while (1)
    {
        ssize_t recvLen = recv(comm->_serverSocket, buf, BUFLEN, 0);
        if (recvLen > 0)
        {
            CtrlMessage* rxMessage = reinterpret_cast<CtrlMessage*>(buf);
            //if (rxMessage->msgId == COMMAND_MSG_ID)
            //{
                comm->handleMessageRx(rxMessage);
            //}
        }
    }
}


Comm::Comm()
{
    _serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&_addr, 0x00, sizeof(struct sockaddr_in));
    _init = false;
    _started = false;

    _throttleData = 0;
    _turnData = 0;
    _backwardData = false;
}


bool Comm::networkInit()
{
    _addr.sin_port = htons(SERVER_PORT);
    _addr.sin_addr.s_addr = INADDR_ANY;
    _addr.sin_family = AF_INET;

    int res = bind(_serverSocket, reinterpret_cast<struct sockaddr*>(&_addr), sizeof(struct sockaddr_in));

    if (res < 0)
    {
        perror("Server bind");
        _init = false;
    }
    else
    {
        printf("Waiting for data on port %d\n", SERVER_PORT);
        _init = true;
    }

    return _init;
}


void Comm::start()
{
    if (_init && !_started)
    {
        pthread_t thread;
        pthread_create(&thread, nullptr, *task, reinterpret_cast<void*>(this));

        _started = true;
    }
}


void Comm::handleMessageRx(CtrlMessage *message)
{
    if (message->throttle < 0)
    {
        _backwardData = true;
        _throttleData = -message->throttle;
    }
    else
    {
        _backwardData = false;
        _throttleData = message->throttle;
    }

    _turnData = message->xAxis;
}


bool Comm::backwardData()
{
    return _backwardData;
}


uint16_t Comm::throttleData()
{
    return _throttleData;
}


int16_t Comm::turnData()
{
    return _turnData;
}


