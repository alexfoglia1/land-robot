#ifndef CONTROL_H
#define CONTROL_H

#include "comm.h"
#include "mpu6050.h"
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

    Control(Comm* comm, Motors* motors, MPU6050* imu);

    void loop();
private:

    Comm* _comm;
    Motors* _motors;
    MPU6050* _imu;
    uint8_t _throttleSetPoint;
    int16_t _gyroZSetPoint;
    float _gz;
    Direction _directionSetPoint;
    Pid _pid;

};

#endif
