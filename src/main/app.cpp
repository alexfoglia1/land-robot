#include "app.h"
#include "control.h"
#include "mpu9265.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    printf("%s %d.%d.%d-%c\r\n", APP_NAME, APP_MAJOR_V, APP_MINOR_V, APP_STAGE_V, APP_VER_TYPE);

    Motors motors;
    Comm comm;
    MPU9265 mpu;

    if (!mpu.init())
    {
        printf("Warning : IMU not available\n");
    }
    else
    {
        printf("Found MPU9265 IMU\n");
        mpu.gyroByas();
    }

    if (comm.networkInit())
    {
        int pid = 1;//fork();
        if (pid == 0)
        {
            system("raspivid -o - -t 0 -vf -w 800 -h 600 -fps 12 | cvlc -vvv stream:///dev/stdin --sout '#rtp{sdp=rtsp://:8080/}' :demux=h264");
        }
        else
        {
            comm.start();

            Control ctrl(&comm, &motors, &mpu, 5);
            ctrl.loop();
        }
    }
}
