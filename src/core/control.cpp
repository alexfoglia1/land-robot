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
    _servoMode = ServoMode::VELOCITY;

    _gyroZSetPoint = 0;
    _gz = 0;
    _throttleSetPoint = 0x00;
    _directionSetPoint = Direction::FORWARD;
    _dt_millis = dt_millis;
    _lastDelayMicroseconds = 0;

    _leftCmd = 0;
    _rightCmd = 0;
}


void Control::servoLoop()
{
        _servoMode = static_cast<ServoMode>(_comm->servoMode() & 0x01);
        uint32_t lastDelayMicroseconds = _comm->servoData();
        static uint16_t servoAzi = (MAX_DELAY_MICROSECONDS + MIN_DELAY_MICROSECONDS)/2;
        static uint16_t servoEle = (MAX_DELAY_MICROSECONDS + MIN_DELAY_MICROSECONDS)/2;

        uint16_t velAzi = lastDelayMicroseconds >> 16;
        uint16_t velEle = lastDelayMicroseconds & 0xFFFF;

        if (_servoMode == ServoMode::VELOCITY)
        {
            servoAzi += 4.0 * _dt_millis / 1000.f * (velAzi - ((MAX_DELAY_MICROSECONDS + MIN_DELAY_MICROSECONDS)/2));
            servoEle += 4.0 * _dt_millis / 1000.f * (velEle - ((MAX_DELAY_MICROSECONDS + MIN_DELAY_MICROSECONDS)/2));
        }
        else
        {
            servoAzi = velAzi;
            servoEle = velEle;
        }

        servoAzi = SATURATE(servoAzi, MIN_DELAY_MICROSECONDS, MAX_DELAY_MICROSECONDS);
        servoEle = SATURATE(servoEle, MIN_DELAY_MICROSECONDS, MAX_DELAY_MICROSECONDS);

        _servo->writeMicroseconds(servoAzi, servoEle);

        _lastDelayMicroseconds = servoAzi << 16 | servoEle;
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
        
        }

        _motors->setSpeed(_leftCmd & 0xFF, _rightCmd & 0xFF);

        servoLoop();

        usleep(_dt_millis * 1000);
    }
}
