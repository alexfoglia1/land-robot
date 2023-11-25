#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>
#include <vector>

enum class MPU6050RxProtoState
{
    WAIT_B0 = 0x00,
    WAIT_BA,
    WAIT_FE,
    WAIT_77,
    WAIT_DATA
};

typedef struct
{
    int16_t gx;
    int16_t gy;
    int16_t gz;
} MPU6050RawData;



class MPU6050
{
public:
    MPU6050();

    bool serialInit(const char* serialPort);
    void handleByte(uint8_t byteRx);

    void start();
    int _fd;
    MPU6050RawData rawData;

private:
    MPU6050RxProtoState _rxState;
    bool _started;
    uint32_t _rxCounter;
    uint8_t* _rxBuffer;

};

#endif
