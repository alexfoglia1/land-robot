#ifndef CONTROL_H
#define CONTROL_H

#include "comm.h"
#include "mpu9265.h"
#include "motors.h"
#include "pid.h"
#include "servo.h"

#include <stdint.h>

#define SATURATE(_A, _MIN, _MAX)(_A < _MIN ? _MIN : _A > _MAX ? _MAX : _A)

enum class Direction : uint8_t
{
    FORWARD = 0x00,
    BACKWARD
};

enum class ServoMode : uint8_t
{
   POSITION = 0x00,
   VELOCITY
};

class Telemetry;

class Control
{
public:
    friend class Telemetry;
    Control(Comm* comm, Motors* motors, MPU9265* imu, Servo* servo, float dt_millis);
    
    void loop();
private:
    void servoLoop();

    Comm* _comm;
    Motors* _motors;
    MPU9265* _imu;
    Servo* _servo;
    uint8_t _throttleSetPoint;
    int16_t _gyroZSetPoint;
    float _gz;
    Direction _directionSetPoint;
    ServoMode _servoMode;
    Pid _pid;
    float _dt_millis;
    uint16_t _leftCmd;
    uint16_t _rightCmd;
    uint32_t _lastDelayMicroseconds;

};

#endif
