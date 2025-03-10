#include "control.h"

#include <unistd.h>
#include <stdio.h>

Control::Control(Comm* comm, Motors* motors, MPU9265* imu, Servo* servo, float dt_millis) :
    _pid(2.5f, 0.00f, 0.0f, dt_millis, 255.0f)
{
    _comm = comm;
    _motors = motors;
    _imu = imu;
    _servo = servo;

    _gyroZSetPoint = 0;
    _gz = 0;
    _throttleSetPoint = 0x00;
    _directionSetPoint = Direction::FORWARD;
    _dt_millis = dt_millis;

    _lastDelayMicroseconds = 1500;
    _leftCmd = 0;
    _rightCmd = 0;
}


void __attribute__((noreturn)) Control::loop()
{
    while (1)
    {
        if (_comm->isEmergencyStop())
        {
            _pid.reset();
            _gyroZSetPoint = 0;
            _gz= 0;
            _throttleSetPoint = 0x00;
            _directionSetPoint = Direction::FORWARD;
            _leftCmd = 0;
            _rightCmd = 0;
            _lastDelayMicroseconds = 1500;
        }
        else
        {
            _directionSetPoint = _comm->backwardData() ? Direction::BACKWARD : Direction::FORWARD;
            _throttleSetPoint = _comm->throttleData() & 0xFF;
            _gyroZSetPoint = _comm->turnData();

            //printf("_gyroZSetPoint(%d)\n", _gyroZSetPoint);

            float gx, gy, gz;
            _imu->readGyro(&gx, &gy, &gz);

            _gz = _gz * 0.75f + gz * 0.25f;

            float pidOut = _pid.controller(_gyroZSetPoint, gz);

            if (_throttleSetPoint == 0 && _gyroZSetPoint != 0)
            {
                if (_gyroZSetPoint > 0)
                {
                    _motors->backwardRight();

                    _rightCmd = SATURATE(_throttleSetPoint + pidOut, 0x00, 0xFF);
                    _leftCmd = _rightCmd;
                }
                else
                {
                    _motors->backwardLeft();

                    _leftCmd = SATURATE(_throttleSetPoint - pidOut, 0x00, 0xFF);
                    _rightCmd = _leftCmd;
                }
            }
            else
            {
                switch (_directionSetPoint)
                {
                    case Direction::BACKWARD: _motors->backward(); break;
                    case Direction::FORWARD:  _motors->forward(); break;
                }

                _leftCmd = SATURATE(_throttleSetPoint + pidOut, 0x00, 0xFF);
                _rightCmd = SATURATE(_throttleSetPoint - pidOut, 0x00, 0xFF);
            }
        
            _lastDelayMicroseconds = _comm->servoData();
        }

        _motors->setSpeed(_leftCmd & 0xFF, _rightCmd & 0xFF);

        _servo->writeMicroseconds(SATURATE(_lastDelayMicroseconds,
                                            MIN_DELAY_MICROSECONDS,
                                            MAX_DELAY_MICROSECONDS));

        usleep(_dt_millis * 1000);
    }
}
