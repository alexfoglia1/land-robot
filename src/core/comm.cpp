#include "comm.h"

#include <wiringSerial.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
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


void* uartTask(void* arg)
{
    Comm* comm = reinterpret_cast<Comm*>(arg);

    uint8_t byteIn;
    while (1)
    {
         printf("Wait uart...\n");
         byteIn = serialGetchar(comm->_serialFd);
         comm->handleUartByteRx(byteIn);
    }

}


Comm::Comm()
{
    _serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memset(&_addr, 0x00, sizeof(struct sockaddr_in));
    _init = false;
    _started = false;
    _uartInit = false;
    _uartRxLen = 0;

    _throttleData = 0;
    _turnData = 0;
    _servoData = 1500;
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


bool Comm::serialInit()
{
    _serialFd = serialOpen("/dev/ttyAMA3", 115200);
    _uartInit = _serialFd > 0;
    return _uartInit;
}


void Comm::start()
{
    if (_init && !_started)
    {
        pthread_t thread, uartThread;
        pthread_create(&thread, nullptr, *task, reinterpret_cast<void*>(this));

        if (_uartInit)
        {
            pthread_create(&uartThread, nullptr, *uartTask, reinterpret_cast<void*>(this));
        }
        _started = true;
    }


}


void Comm::handleMessageRx(CtrlMessage *message)
{
    //printf("message->xAxis(%d)\n", message->xAxis);
    //printf("message->yAxis(%d)\n", message->yAxis);
    //printf("message->throttle(%d)\n", message->throttle);
    //printf("message->servo(%d)\n", message->servo);
    //printf("message->checksum(%d)\n", message->checksum);

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
    _servoData = message->servo;
}


void Comm::handleUartByteRx(uint8_t byteRx)
{
    switch (_uartStatus)
    {
        case UartStatus::WAIT_SYNC:
        {
            if (byteRx == 0xFF)
            {
                _uartStatus = UartStatus::WAIT_MESSAGE;

                _uartRxLen = 0;
                memset(_uartBuf, 0x00, sizeof(CtrlMessage));
                //printf("sync\n");
            }
        }
        break;
        case UartStatus::WAIT_MESSAGE:
        {
            _uartBuf[_uartRxLen] = byteRx;
            _uartRxLen += 1;

            //printf("rxByte(%hhd)\n", byteRx);

            if (_uartRxLen == sizeof(CtrlMessage))
            {
                handleMessageRx(reinterpret_cast<CtrlMessage*>(_uartBuf));
                _uartStatus = UartStatus::WAIT_SYNC;
            }
        }
        break;
    }
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


uint16_t Comm::servoData()
{
    return _servoData;
}

