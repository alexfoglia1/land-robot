#ifndef COMM_H
#define COMM_H

#define SERVER_PORT 7777
#define BUFLEN 256
#define PAYLOAD_SIZE 128
#define EMRG_MSG_ID 0xAA
#define CTRL_MSG_ID 0x7E
#define TLMT_MSG_ID 0x5F

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>


typedef struct
{
    int16_t xAxis;
    int16_t yAxis;
    int16_t throttle;
    uint16_t servo;
    uint16_t checksum;
} __attribute__((packed)) CtrlMessage;


typedef union
{
    struct
    {
        uint8_t gx    :1;
        uint8_t gy    :1;
        uint8_t gz    :1;
        uint8_t ax    :1;
        uint8_t ay    :1;
        uint8_t az    :1;
        uint8_t gz_sp :1;
        uint8_t zero  :1;
    } Bits;
    uint8_t byte;
} TelemetryByte0;


typedef union
{
    struct
    {
        uint8_t pid_p :1;
        uint8_t pid_i :1;
        uint8_t pid_d :1;
        uint8_t pid_u :1;
        uint8_t thr_l :1;
        uint8_t thr_r :1;
        uint8_t servo :1;
        uint8_t zero  :1;
    } Bits;
    uint8_t byte;
} TelemetryByte1;

typedef struct
{
    TelemetryByte0 byte0;
    TelemetryByte1 byte1;
} __attribute__((packed)) TelemetryHeader;

typedef struct
{
    TelemetryHeader header;
    uint8_t payload[PAYLOAD_SIZE];
} __attribute__((packed)) TelemetryMessage;

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
    bool isEmergencyStop();
    void toggleEmergencyStop();
    void handleMessageRx(CtrlMessage* message);
    void handleTelemetryRequest(TelemetryHeader* headerRx, in_addr_t cliAddr, in_port_t cliPort, socklen_t cliLen);
    ssize_t sendTelemetry(char* data, size_t len);
    bool hasPendingTelemetryRequests();
    void waitTelemetry();
    void handleUartByteRx(uint8_t byteRx);

    bool backwardData();
    uint16_t throttleData();
    uint16_t servoData();
    int16_t turnData();
    TelemetryHeader getTelemetryRequest();

    int _serverSocket;
    struct sockaddr_in _addr;

    int _serialFd;
    bool _init;
    bool _uartInit;
    bool _started;

private:
    sem_t _telemetrySemaphore;
    bool _isEmergencyStop;
    TelemetryHeader _rxTelemetryHeader;
    in_addr_t _telemetryAddress;
    in_port_t _telemetryPort;
    socklen_t _telemetryLen;
    UartStatus _uartStatus;
    int _uartRxLen;
    uint8_t _uartBuf[10];

    bool _rxTelemetryRequest;
    bool _backwardData;
    uint16_t _throttleData;
    uint16_t _servoData;
    int16_t _turnData;
};

#endif
