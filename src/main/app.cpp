#include "app.h"
#include "control.h"
#include "mpu6050.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    printf("%s %d.%d.%d-%c\r\n", APP_NAME, APP_MAJOR_V, APP_MINOR_V, APP_STAGE_V, APP_VER_TYPE);

    Motors motors;
    Comm comm;
    MPU6050 mpu;

    if (mpu.serialInit("/dev/ttyUSB0"))
    {
        mpu.start();
    }

    if (comm.networkInit())
    {
        comm.start();

        Control ctrl(&comm, &motors, &mpu);
        ctrl.loop();
    }
}
