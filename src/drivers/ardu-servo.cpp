#include "ardu-servo.h"

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <wiringSerial.h>


ArduServo::ArduServo() : Servo()
{
    _servoFound = false;
    _servoFd = -1;

    readDir("/dev/");
}


void ArduServo::readDir(const char* path)
{
    DIR *dir;
    struct dirent *ent;
    
    if ((dir = opendir (path)) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            std::string fileName(ent->d_name);
            if ( (fileName.rfind("ttyUSB", 0) == 0) ||
                 (fileName.rfind("ttyACM", 0) == 0) ||
                 (fileName.rfind("ttyAMA", 0) == 0) )
            {
                _usbDevices.push_back(std::string(path) + fileName);
            }

        }
        closedir (dir);
    }
    else
    {
        /* could not open directory */
        perror ("read devices");
    }
}


int ArduServo::initSerial(const char* path)
{
    return serialOpen(path, SERVO_BAUD);
}


bool ArduServo::testDevice(int fd)
{
    fd_set set;
    struct timeval timeout;
    servo_raspi_msg_t msgIn;
    memset(&msgIn, 0x00, sizeof(servo_raspi_msg_t));

    FD_ZERO(&set); /* clear the set */
    FD_SET(fd, &set); /* add our file descriptor to the set */

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    int rv = select(fd + 1, &set, NULL, NULL, &timeout);
    if (rv == -1)
    {
        perror("select");
        return false;
    }
    else if (rv == 0)
    {
        printf("timeout\n");
        return false;
    }
    else
    {
        int attempts = 0;

        const int MAX_ATTEMPTS = strlen(ARDU_SERVO_WHOAMI) + 1;

        while (msgIn.sync != RASPI_SERVO_SYNC_BYTE &&
               attempts < MAX_ATTEMPTS)
        {
            msgIn.sync = serialGetchar(fd);//read(fd, reinterpret_cast<char*>(&msgIn.sync), 1); /* there was data to read */
            attempts += 1;

            printf("attempts(%d), sync(0x%X)\n", attempts, msgIn.sync);
        }

        if (attempts < MAX_ATTEMPTS)
        {
            int readRes = 0;
            int expectedBytes = static_cast<int>(strlen(ARDU_SERVO_WHOAMI));
            while (readRes < expectedBytes)
            {
                msgIn.whoami[readRes] = serialGetchar(fd);
                                //reinterpret_cast<char*>(&msgIn.whoami[readRes]),
                                //strlen(ARDU_SERVO_WHOAMI) - readRes);

                readRes += 1;
                if (readRes <= 0)
                {
                    perror("read whoami");
                    return false;
                }
                else
                {
                    printf("read chunk(%s)\n", msgIn.whoami);
                }
            }

            printf("whoami read(%s)\n", msgIn.whoami);
            if (strncmp(ARDU_SERVO_WHOAMI, msgIn.whoami, strlen(ARDU_SERVO_WHOAMI)) == 0)
            {
                printf("whoami correct\n");
                return true;
            }
            else
            {
                printf("wrong whoami\n");
                return false;
            }
        }
        else
        {
            printf("No sync received: abort\n");
            return false;
        }
    }
}


void ArduServo::servoTx(raspi_servo_msg_t* msg)
{
    unsigned char* pMsg = reinterpret_cast<unsigned char*>(msg);

    size_t i = 0;
    for (i = 0; i < sizeof(raspi_servo_msg_t); i++)
    {
        serialPutchar(_servoFd, pMsg[i]);
    }
}


bool ArduServo::init()
{
    for (auto& path : _usbDevices)
    {
        printf("Try %s\n", path.c_str());

        int fd = initSerial(path.c_str());
        if (fd >= 0)
        {
            printf("Opened %s\n", path.c_str());

            bool testRes = testDevice(fd);
            if (testRes)
            {
                printf("[OK] Found ARDUSERVO at %s\n", path.c_str());
                _servoFound = true;
                _servoFd = fd;

                break;
            }
            else
            {
                printf("[ERROR] Not found ARDUSERVO at %s\n", path.c_str());
                serialClose(fd);
            }
        }
    }

    return _servoFound;
}


void ArduServo::writeMicroseconds(uint16_t delayUs)
{
    raspi_servo_msg_t msg;
    msg.sync = RASPI_SERVO_SYNC_BYTE;
    //printf("delayUs(%d)\n", delayUs);
    msg.delay_microseconds = delayUs;

    servoTx(&msg);
}


void ArduServo::writeAngle(float angleDeg)
{

}




