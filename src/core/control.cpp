#include "control.h"

#include <unistd.h>
#include <stdio.h>

Control::Control(Comm* comm, Motors* motors, MPU6050* imu) :
    _pid(3.0f, 0.0, 0.0, 0.0, 0.0, 255.0)
{
    _comm = comm;
    _motors = motors;
    _imu = imu;

    _gyroZSetPoint = 0;
    _gz = 0;
    _throttleSetPoint = 0x00;
    _directionSetPoint = Direction::FORWARD;
}


void __attribute__((noreturn)) Control::loop()
{
    while (1)
    {
        _directionSetPoint = _comm->backwardData() ? Direction::BACKWARD : Direction::FORWARD;
        _throttleSetPoint = _comm->throttleData() & 0xFF;
        _gyroZSetPoint = _comm->turnData();

        float gz = -_imu->rawData.gz / 131.f;
        _gz = _gz * 0.75f + gz*0.25f;
        float pidOut = _pid.controller(_gyroZSetPoint, gz);

        uint16_t leftCmd = SATURATE(_throttleSetPoint + pidOut, 0x00, 0xFF);
        uint16_t rightCmd = SATURATE(_throttleSetPoint - pidOut, 0x00, 0xFF);

        switch (_directionSetPoint)
        {
            case Direction::BACKWARD: _motors->backward(); break;
            case Direction::FORWARD:  _motors->forward(); break;
        }

        _motors->setSpeed(leftCmd & 0xFF, rightCmd & 0xFF);

        printf("gyroZ(%f)\tpidOut(%f)\r\n", gz, pidOut);


        usleep(5 * 1000);
    }
}
