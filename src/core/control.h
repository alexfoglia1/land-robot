#ifndef CONTROL_H
#define CONTROL_H

#define BIT_DATA_RESOLUTION 0.00274658203125f // 180 degrees / 2^16

#include "comm.h"
#include "motors.h"
#include "pid.h"

#include <stdint.h>

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
    float _headingSetPoint;
    Direction _directionSetPoint;
    Pid _pid;

    float toAngle180(uint16_t word);

};

#endif
