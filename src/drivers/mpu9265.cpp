#include "mpu9265.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>


MPU9265::MPU9265()
{
    _fd = -1;
    memset(&_rawData, 0x00, sizeof(MPU9265RawData));
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
            if (writeRes != 1)
            {
                perror("Set sample rate");
                return false;
            }

            /** Reset all sensors **/
            writeRes = i2cWrite(MPU9265::Register::PWR_MGMT_1, &zero, 1);
            if (writeRes != 1)
            {
                perror("Reset all sensors");
                return false;
            }

            /** Power management and crystal settings **/
            writeRes = i2cWrite(MPU9265::Register::PWR_MGMT_1, &one, 1);
            if (writeRes != 1)
            {
                perror("Power management");
                return false;
            }
            
            /** Write to Configuration register **/
            writeRes = i2cWrite(MPU9265::Register::CONFIG, &zero, 1);
            if (writeRes != 1)
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
            if (writeRes != 1)
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
}


bool MPU9265::setGyroResolution(MPU9265::GyroResolution gyroResolution)
{
    uint8_t byte = static_cast<uint8_t>(gyroResolution);

    return i2cWrite(MPU9265::Register::GYRO_CONFIG, &byte, 1);
}


bool MPU9265::setAccelResolution(MPU9265::AccelResolution accelResolution)
{
    uint8_t byte = static_cast<uint8_t>(accelResolution);

    return i2cWrite(MPU9265::Register::ACCEL_CONFIG, &byte, 1);
}


int MPU9265::i2cWrite(MPU9265::Register reg, uint8_t* data, uint32_t data_len)
{
    uint8_t* buf = new uint8_t[data_len + 1];
    memset(buf, 0x00, data_len + 1);

    buf[0] = static_cast<uint8_t>(reg);

    for (uint32_t i = 0; i < data_len; i++)
    {
        buf[i + 1] = data[i];
    }

    int writeResult = write(_fd, buf, data_len);
    delete[] buf;

    usleep(1000);
    return writeResult;
}


