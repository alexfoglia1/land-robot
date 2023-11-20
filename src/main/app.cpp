#include "app.h"
#include "control.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    printf("%s %d.%d.%d-%c\r\n", APP_NAME, APP_MAJOR_V, APP_MINOR_V, APP_STAGE_V, APP_VER_TYPE);

    Motors motors;
    Comm comm;

    if (comm.networkInit())
    {
        comm.start();

        Control ctrl(&comm, &motors);
        ctrl.loop();
    }
}
