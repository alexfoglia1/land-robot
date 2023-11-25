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
            /** Set sample rate = 8Khz (div = 0 + 1) **/
            int writeRes = i2cWrite(MPU9265::SMPLRT_DIV, &zero, 1);
            if (writeRes == 1)
            {

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


int MPU9265::i2cWrite(uint8_t reg, uint8_t* data, uint32_t data_len)
{
    uint8_t* buf = new uint8_t[data_len + 1];
    memset(buf, 0x00, data_len + 1);

    buf[0] = reg;

    for (uint32_t i = 0; i < data_len; i++)
    {
        buf[i + 1] = data[i];
    }

    int writeResult = write(_fd, buf, data_len);
    delete[] buf;

    return writeResult;
}


