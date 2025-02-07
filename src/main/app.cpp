#include "app.h"
#include "control.h"
#include "mpu9265.h"
#include "raspi-servo.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>

int main(int argc, char** argv)
{
    printf("%s %d.%d.%d-%c\r\n", APP_NAME, APP_MAJOR_V, APP_MINOR_V, APP_STAGE_V, APP_VER_TYPE);

    wiringPiSetup();

    Motors motors;
    Comm comm;
    MPU9265 mpu;
    RaspiServo servo;
    servo.init();
    servo.writeMicroseconds(1500);

    if (!mpu.init())
    {
        printf("Warning : IMU not available\n");
    }
    else
    {
        printf("Found MPU9265 IMU\n");
        mpu.gyroByas();
    }

#ifdef __UART_INIT__
    if (comm.serialInit())
    {
        printf("Opened UART\n");
    }
    else
    {
        printf("Cannot open UART\n");
    }
#endif
    if (comm.networkInit())
    {
        int pid = fork();
        if (pid == 0)
        {
            system("gst-launch-1.0 -v v4l2src device=/dev/video0 ! videoconvert    ! video/x-raw,format=I420,width=640,height=480,framerate=30/1 ! videoflip method=rotate-180 ! x264enc tune=zerolatency bitrate=800 speed-preset=superfast ! rtph264pay ! udpsink host=192.168.1.4 port=5000");
        }
        else
        {
            comm.start();

            Control ctrl(&comm, &motors, &mpu, &servo, 5);
            ctrl.loop();
        }
    }
}
