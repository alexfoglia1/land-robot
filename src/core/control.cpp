#include "control.h"

#include <stdio.h>

Control::Control(Comm* comm, Motors* motors) :
    _pid(0.01f, 0.0, 0.0, 0.0, 0.0, 255.0)
{
    _comm = comm;
    _motors = motors;

    _gyroZSetPoint = 0;
    _throttleSetPoint = 0x00;
    _directionSetPoint = Direction::FORWARD;
}


void __attribute__((noreturn)) Control::loop()
{
    int i= 0;
    while (1)
    {
        i++;
        _directionSetPoint = _comm->backwardData() ? Direction::BACKWARD : Direction::FORWARD;
        _throttleSetPoint = _comm->throttleData() & 0xFF;
        _gyroZSetPoint = _comm->turnData();

        float gz = 0.0f; //todo: read from mpu6050
        float pidOut = _pid.controller(_gyroZSetPoint, gz);

        uint16_t leftCmd = SATURATE(_throttleSetPoint + pidOut, 0x00, 0xFF);
        uint16_t rightCmd = SATURATE(_throttleSetPoint - pidOut, 0x00, 0xFF);

        switch (_directionSetPoint)
        {
            case Direction::BACKWARD: _motors->backward(); break;
            case Direction::FORWARD:  _motors->forward(); break;
        }

        _motors->setSpeed(leftCmd & 0xFF, rightCmd & 0xFF);

        if (i == 1000)
        {
            printf("throttleSp(%d), gyroZSetPoint(%d), pidOut(%f), leftCmd(%d), rightCmd(%d)\r\n\r\n", _throttleSetPoint, _gyroZSetPoint, pidOut, leftCmd, rightCmd);
            i = 0;
        }
    }
}
