#include "control.h"

#include <stdio.h>

Control::Control(Comm* comm, Motors* motors) :
    _pid(1.0, 0.0, 0.0, 0.0, 0.0, 255.0)
{
    _comm = comm;
    _motors = motors;

    _headingSetPoint = 0.0f;
    _throttleSetPoint = 0x00;
    _directionSetPoint = Direction::FORWARD;
}


void __attribute__((noreturn)) Control::loop()
{
    while (1)
    {
        _directionSetPoint = _comm->backwardData() ? Direction::BACKWARD : Direction::FORWARD;
        _throttleSetPoint = _comm->throttleData() & 0xFF;
        _headingSetPoint = _comm->turnRightData() > 0 ? toAngle180(_comm->turnRightData()) :
                           _comm->turnLeftData() > 0 ? -toAngle180(_comm->turnLeftData()) : 0.0f;

        float heading = 0.0f; //todo: read from mpu6050
        float pidOut = _pid.controller(_headingSetPoint, heading);

        uint16_t leftCmd = static_cast<uint16_t>(_throttleSetPoint + pidOut);
        uint16_t rightCmd = static_cast<uint16_t>(_throttleSetPoint - pidOut);

        switch (_directionSetPoint)
        {
            case Direction::BACKWARD: _motors->backward(); break;
            case Direction::FORWARD:  _motors->forward(); break;
        }

        _motors->setSpeed(leftCmd & 0xFF, rightCmd & 0xFF);

        printf("throttleSp(%d), headingSp(%f), pidOut(%f), leftCmd(%d), rightCmd(%d)\r\n", _throttleSetPoint, _headingSetPoint, pidOut, leftCmd, rightCmd);
    }
}


float Control::toAngle180(uint16_t word)
{
    return static_cast<float>(word) * BIT_DATA_RESOLUTION;
}
