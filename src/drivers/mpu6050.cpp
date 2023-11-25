#include "mpu6050.h"

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>


void* task_mpu6050(void* arg)
{
    MPU6050* mpu = reinterpret_cast<MPU6050*>(arg);

    while (1)
    {
        uint8_t byteRx;
        ssize_t lenread = read(mpu->_fd, &byteRx, 1);
        if (lenread > 0)
        {
            mpu->handleByte(byteRx);
        }
    }
}


void onRxData(uint8_t dataRx, void* __mpu6050)
{
    MPU6050* mpu = reinterpret_cast<MPU6050*>(__mpu6050);

    mpu->handleByte(dataRx);
}


MPU6050::MPU6050()
{
    _rxState = MPU6050RxProtoState::WAIT_B0;
    _rxCounter = 0;
    _fd = -1;
    _rxBuffer = new uint8_t(sizeof(MPU6050RawData));

    memset(_rxBuffer, 0x00, sizeof(MPU6050RawData));
    memset(&rawData, 0x00,  sizeof(MPU6050RawData));
}


bool MPU6050::serialInit(const char *serialPort)
{
    _fd = open(serialPort, O_RDONLY);

    if (_fd <= 0)
    {
        perror("open");
        return false;
    }

    struct termios serialPortSettings;

    tcgetattr(_fd, &serialPortSettings);
    cfsetispeed(&serialPortSettings, B38400);

    serialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
    serialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    serialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
    serialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */
    serialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
    serialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */
    serialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
    serialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */
    serialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

    if (tcsetattr(_fd, TCSANOW, &serialPortSettings) != 0)
    {
        perror("tcsetattr");
        return false;
    }

    tcflush(_fd, TCIFLUSH);

    return true;
}


void MPU6050::start()
{
    if (_fd > 0 && !_started)
    {

        pthread_t thread;
        pthread_create(&thread, nullptr, *task_mpu6050, reinterpret_cast<void*>(this));

        _started = true;
    }
}


void MPU6050::handleByte(uint8_t byteRx)
{
    switch (_rxState)
    {
        case MPU6050RxProtoState::WAIT_B0:
        if (byteRx == 0xB0)
        {
            _rxState = MPU6050RxProtoState::WAIT_BA;
            //printf("RX_B0\r\n");
        }
        else
        {
            //printf("UNEXPECTED RX (%hhu)\r\n", byteRx);
            _rxState = MPU6050RxProtoState::WAIT_B0;
        }
        break;
        case MPU6050RxProtoState::WAIT_BA:
        if (byteRx == 0xBA)
        {
            _rxState = MPU6050RxProtoState::WAIT_FE;
            //printf("RX_BA\r\n");
        }
        else
        {
            //printf("UNEXPECTED RX (%hhu)\r\n", byteRx);
            _rxState = MPU6050RxProtoState::WAIT_B0;
        }
        break;
        case MPU6050RxProtoState::WAIT_FE:
        if (byteRx == 0xFE)
        {
            _rxState = MPU6050RxProtoState::WAIT_77;
            //printf("RX_FE\r\n");
        }
        else
        {
            //printf("UNEXPECTED RX (%hhu)\r\n", byteRx);
            _rxState = MPU6050RxProtoState::WAIT_B0;
        }
        break;
        case MPU6050RxProtoState::WAIT_77:
        if (byteRx == 0x77)
        {
            _rxState = MPU6050RxProtoState::WAIT_DATA;
            _rxCounter = 0;
            memset(_rxBuffer, 0x00, sizeof(MPU6050RawData));
            //printf("RX_77\r\n");
        }
        else
        {
            //printf("UNEXPECTED RX (%hhu)\r\n", byteRx);
            _rxState = MPU6050RxProtoState::WAIT_B0;
        }
        break;
        case MPU6050RxProtoState::WAIT_DATA:
        //printf("RX_%hhu (cnt %d)\r\n", byteRx, _rxCounter);
        if (_rxCounter < sizeof(MPU6050RawData) - 1)
        {
            _rxBuffer[_rxCounter] = byteRx;
            _rxCounter += 1;
        }
        else
        {
            _rxBuffer[_rxCounter] = byteRx;
            memcpy(&rawData, _rxBuffer, sizeof(MPU6050RawData));

            //printf("RX COMPLETED - GYRO Z(%d)\r\n", rawData.gz);
            _rxState = MPU6050RxProtoState::WAIT_B0;
        }

        //printf("RxBuffer: [ ");
        //for (int i = 0; i  <sizeof (MPU6050RawData); i++)
        //{
        //    printf("%hhu, ", _rxBuffer[i]);
        //}
        //printf("]\r\n");

        break;
    }
}



