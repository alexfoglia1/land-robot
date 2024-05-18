#ifndef CONTROL_H
#define CONTROL_H

#include "comm.h"
#include "mpu9265.h"
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

    Control(Comm* comm, Motors* motors, MPU9265* imu, float dt_millis);

    void loop();
private:

    Comm* _comm;
    Motors* _motors;
    MPU9265* _imu;
    uint8_t _throttleSetPoint;
    int16_t _gyroZSetPoint;
    float _gz;
    Direction _directionSetPoint;
    Pid _pid;
    float _dt_millis;

};

#endif
