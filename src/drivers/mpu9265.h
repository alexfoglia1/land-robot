#ifndef MPU9265_H
#define MPU9265_H

#include <stdint.h>


typedef struct
{
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
} MPU9265RawData;



class MPU9265
{
public:
    const uint8_t I2C_ADDR     = 0x68;

    // Registers
    const uint8_t PWR_MGMT_1   = 0x6B;
    const uint8_t SMPLRT_DIV   = 0x19;
    const uint8_t CONFIG       = 0x1A;
    const uint8_t GYRO_CONFIG  = 0x1B;
    const uint8_t ACCEL_CONFIG = 0x1C;
    const uint8_t INT_ENABLE   = 0x38;
    const uint8_t ACCEL_XOUT_H = 0x3B;
    const uint8_t ACCEL_YOUT_H = 0x3D;
    const uint8_t ACCEL_ZOUT_H = 0x3F;
    const uint8_t TEMP_OUT_H   = 0x41;
    const uint8_t GYRO_XOUT_H  = 0x43;
    const uint8_t GYRO_YOUT_H  = 0x45;
    const uint8_t GYRO_ZOUT_H  = 0x47;

    MPU9265();

    bool init();

private:

    int _fd;
    MPU9265RawData _rawData;

    int i2cWrite(uint8_t reg, uint8_t* data, uint32_t data_len);

};

#endif
