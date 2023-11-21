#ifndef CONTROL_H
#define CONTROL_H

#include "comm.h"
#include "motors.h"
#include "pid.h"

#include <stdint.h>

#define SATURATE(_A, _MIN, _MAX)(_A < _MIN ? _MIN : _A > _MAX ? _MAX : _A)

enum class Direction : uint8_t
{
    FORWARD = 0x00,
    BACKWARD
};


class Control
{
public:

    Control(Comm* comm, Motors* motors);

    void loop();
private:

    Comm* _comm;
    Motors* _motors;
    uint8_t _throttleSetPoint;
    int16_t _gyroZSetPoint;
    Direction _directionSetPoint;
    Pid _pid;

};

#endif
