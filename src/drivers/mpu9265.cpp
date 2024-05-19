#include "mpu9265.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <limits>

extern "C"
{
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>
}



MPU9265::MPU9265()
{
    _fd = -1;

    _gx0 = 0;
    _gy0 = 0;
    _gz0 = 0;

    _gyroResolution = GyroResolution::GYRO_250_DPS;
    _accelResolution = AccelResolution::ACCEL_2G;
}

bool MPU9265::init()
{
    _fd = open("/dev/i2c-1", O_RDWR);

    if (_fd >= 0)
    {
        int ioctlRes = ioctl(_fd, I2C_SLAVE, MPU9265::I2C_ADDR);
        if (ioctlRes >= 0)
        {
            uint8_t zero = 0x00;
            uint8_t one  = 0x01;
            /** Set sample rate = 8Khz (div = 0 + 1) **/
            int writeRes = i2cWrite(MPU9265::Register::SMPLRT_DIV, &zero, 1);
            if (writeRes != 0)
            {
                perror("Set sample rate");
                return false;
            }

            /** Reset all sensors **/
            writeRes = i2cWrite(MPU9265::Register::PWR_MGMT_1, &zero, 1);
            if (writeRes != 0)
            {
                perror("Reset all sensors");
                return false;
            }

            /** Power management and crystal settings **/
            writeRes = i2cWrite(MPU9265::Register::PWR_MGMT_1, &one, 1);
            if (writeRes != 0)
            {
                perror("Power management");
                return false;
            }
            
            /** Write to Configuration register **/
            writeRes = i2cWrite(MPU9265::Register::CONFIG, &zero, 1);
            if (writeRes != 0)
            {
                perror("Configuration register");
                return false;
            }

            if (!setGyroResolution(MPU9265::GyroResolution::GYRO_250_DPS))
            {
                perror("Set gyro resolution 250 DPS");
                return false;
            }

            if (!setAccelResolution(MPU9265::AccelResolution::ACCEL_2G))
            {
                perror("Set accelerometer resolution 2G");
                return false;
            }

            writeRes = i2cWrite(MPU9265::Register::INT_ENABLE, &one, 1);
            if (writeRes != 0)
            {
                perror("Interrupt enable");
            }
            else
            {
                return true;
            }
            
        }
        else
        {
            perror("ioctl set i2c slave address");
            return false;
        }

    }
    else
    {
        perror("open");
        return false;
    }

    return true;
}


bool MPU9265::setGyroResolution(MPU9265::GyroResolution gyroResolution)
{
    _gyroResolution = gyroResolution;

    uint8_t byte = static_cast<uint8_t>(gyroResolution);

    return i2cWrite(MPU9265::Register::GYRO_CONFIG, &byte, 1) == 0;
}


bool MPU9265::setAccelResolution(MPU9265::AccelResolution accelResolution)
{
    _accelResolution = accelResolution;

    uint8_t byte = static_cast<uint8_t>(accelResolution);

    return i2cWrite(MPU9265::Register::ACCEL_CONFIG, &byte, 1) == 0;
}


void MPU9265::readGyro(float* gx, float* gy, float* gz)
{
    int16_t rawGyroX = 0;
    i2cRead(MPU9265::Register::GYRO_XOUT_H, (uint8_t*) &rawGyroX, 2);
    int16_t rawGyroY = 0;
    i2cRead(MPU9265::Register::GYRO_YOUT_H, (uint8_t*) &rawGyroY, 2);
    int16_t rawGyroZ = 0;
    i2cRead(MPU9265::Register::GYRO_ZOUT_H, (uint8_t*) &rawGyroZ, 2);

    *gx = toDps(rawGyroX) - _gx0;
    *gy = toDps(rawGyroY) - _gy0;
    *gz = -toDps(rawGyroZ) - _gz0;
}


void MPU9265::readAccel(float* ax, float* ay, float* az)
{
    int16_t rawAccelX = 0;
    i2cRead(MPU9265::Register::ACCEL_XOUT_H, (uint8_t*) &rawAccelX, 2);
    int16_t rawAccelY = 0;
    i2cRead(MPU9265::Register::ACCEL_YOUT_H, (uint8_t*) &rawAccelY, 2);
    int16_t rawAccelZ = 0;
    i2cRead(MPU9265::Register::ACCEL_ZOUT_H, (uint8_t*) &rawAccelZ, 2);

    *ax = toG(rawAccelX);
    *ay = toG(rawAccelY);
    *az = -toG(rawAccelZ);
}


void MPU9265::gyroByas()
{
    const int N = 1000;

    _gx0 = 0.0f;
    _gy0 = 0.0f;
    _gz0 = 0.0f;

    float gx0 = 0.0f;
    float gy0 = 0.0f;
    float gz0 = 0.0f;
    for (int i = 0; i < N; i++)
    {
        float gx, gy, gz;
        gx = 0.0f;
        gy = 0.0f;
        gz = 0.0f;

        readGyro(&gx, &gy, &gz);
        gx0 += gx;
        gy0 += gy;
        gz0 += gz;
    }

    _gx0 = gx0 / (float)N;
    _gy0 = gy0 / (float)N;
    _gz0 = gz0 / (float)N;

    printf("gx0(%f)\tgy0(%f)\tgz0(%f)\n", _gx0, _gy0, _gz0);
}


int MPU9265::i2cWrite(MPU9265::Register reg, uint8_t* data, uint32_t data_len)
{
    return i2c_smbus_write_i2c_block_data(_fd, (uint8_t) reg, data_len, data);
}


int MPU9265::i2cRead(MPU9265::Register reg, uint8_t* data, uint32_t data_len)
{
    return i2c_smbus_read_i2c_block_data(_fd, (uint8_t) reg, data_len, data);
}


float MPU9265::toDps(int16_t gyroRaw)
{
    gyroRaw = big_endian(gyroRaw);
    float gyroResolution = gyroResolutionValue(_gyroResolution);
    return (static_cast<float>(gyroRaw) / static_cast<float>(std::numeric_limits<int16_t>::max())) * gyroResolution;
}


float MPU9265::toG(int16_t accelRaw)
{
    accelRaw = big_endian(accelRaw);
    float accelResolution = accelResolutionValue(_accelResolution);
    return (static_cast<float>(accelRaw) / static_cast<float>(std::numeric_limits<int16_t>::max())) * accelResolution;
}


float MPU9265::gyroResolutionValue(MPU9265::GyroResolution res)
{
    float _ret = 0.0f;
    switch (res)
    {
        case MPU9265::GyroResolution::GYRO_250_DPS:  _ret = 250.f;  break;
        case MPU9265::GyroResolution::GYRO_500_DPS:  _ret = 500.f;  break;
        case MPU9265::GyroResolution::GYRO_1000_DPS: _ret = 1000.f; break;
        case MPU9265::GyroResolution::GYRO_2000_DPS: _ret = 2000.f; break;
    }

    return _ret;
}


float MPU9265::accelResolutionValue(MPU9265::AccelResolution res)
{
    float _ret = 0.0f;
    switch (res)
    {
        case MPU9265::AccelResolution::ACCEL_2G:  _ret = 2.f;  break;
        case MPU9265::AccelResolution::ACCEL_4G:  _ret = 4.f;  break;
        case MPU9265::AccelResolution::ACCEL_8G:  _ret = 8.f;  break;
        case MPU9265::AccelResolution::ACCEL_16G: _ret = 16.f; break;
    }

    return _ret;
}


int16_t MPU9265::big_endian(int16_t little_endian)
{
    uint8_t* bytes = (uint8_t*) &little_endian;
    return ((bytes[0] << 8) | bytes[1]);
}
