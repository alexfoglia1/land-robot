#ifndef MPU9265_H
#define MPU9265_H

#include <stdint.h>


class MPU9265
{
public:
    const uint8_t I2C_ADDR     = 0x68;

    // Registers
    enum class Register
    {
        PWR_MGMT_1   = 0x6B,
        SMPLRT_DIV   = 0x19,
        CONFIG       = 0x1A,
        GYRO_CONFIG  = 0x1B,
        ACCEL_CONFIG = 0x1C,
        INT_ENABLE   = 0x38,
        ACCEL_XOUT_H = 0x3B,
        ACCEL_YOUT_H = 0x3D,
        ACCEL_ZOUT_H = 0x3F,
        TEMP_OUT_H   = 0x41,
        GYRO_XOUT_H  = 0x43,
        GYRO_YOUT_H  = 0x45,
        GYRO_ZOUT_H  = 0x47
    };

    enum class GyroResolution : uint8_t
    {
        GYRO_250_DPS  = 0x00,
        GYRO_500_DPS  = 0x08,
        GYRO_1000_DPS = 0x10,
        GYRO_2000_DPS = 0x18
    };

    enum class AccelResolution : uint8_t
    {
        ACCEL_2G  = 0x00,
        ACCEL_4G  = 0x08,
        ACCEL_8G  = 0x10,
        ACCEL_16G = 0x18
    };

    MPU9265();

    bool init();
    void gyroByas();
    bool setGyroResolution(MPU9265::GyroResolution gyroResolution);
    bool setAccelResolution(MPU9265::AccelResolution accelResolution);
    void readGyro(float* gx, float* gy, float* gz);
    void readAccel(float* ax, float* ay, float* az);

private:

    int _fd;
    GyroResolution _gyroResolution;
    AccelResolution _accelResolution;
    float _gx0;
    float _gy0;
    float _gz0;

    int i2cWrite(MPU9265::Register reg, uint8_t* data, uint32_t data_len);
    int i2cRead(MPU9265::Register reg, uint8_t* data, uint32_t data_len);

    float toDps(int16_t gyroRaw);
    float toG(int16_t accelRaw);
    float gyroResolutionValue(MPU9265::GyroResolution res);
    float accelResolutionValue(MPU9265::AccelResolution res);
    int16_t big_endian(int16_t little_endian);

};

#endif
