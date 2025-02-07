#include "motors.h"

#include <wiringPi.h>
#include <softPwm.h>
#include <limits>

Motors::Motors()
{
    pinMode(MOTOR_RIGHT_SPEED, OUTPUT);
    pinMode(MOTOR_RIGHT_DIR1, OUTPUT);
    pinMode(MOTOR_RIGHT_DIR2, OUTPUT);
    pinMode(MOTOR_LEFT_SPEED, OUTPUT);
    pinMode(MOTOR_LEFT_DIR1, OUTPUT);
    pinMode(MOTOR_LEFT_DIR2, OUTPUT);

    _minSpeed = std::numeric_limits<uint8_t>::min();
    _maxSpeed = std::numeric_limits<uint8_t>::max();

    softPwmCreate(MOTOR_RIGHT_SPEED, _minSpeed, _maxSpeed);
    softPwmCreate(MOTOR_LEFT_SPEED, _minSpeed, _maxSpeed);

    forward();
    setSpeed(_minSpeed, _minSpeed);
}


void Motors::stopMotors()
{
    digitalWrite(MOTOR_RIGHT_DIR1, LOW);
    digitalWrite(MOTOR_RIGHT_DIR2, LOW);
    digitalWrite(MOTOR_LEFT_DIR1, LOW);
    digitalWrite(MOTOR_LEFT_DIR2, LOW);

    softPwmWrite(MOTOR_RIGHT_SPEED, _minSpeed);
    softPwmWrite(MOTOR_LEFT_SPEED, _minSpeed);
}


void Motors::forward()
{
    digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
    digitalWrite(MOTOR_RIGHT_DIR2, LOW);
    digitalWrite(MOTOR_LEFT_DIR1, HIGH);
    digitalWrite(MOTOR_LEFT_DIR2, LOW);
}


void Motors::backward()
{
    digitalWrite(MOTOR_RIGHT_DIR1, LOW);
    digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
    digitalWrite(MOTOR_LEFT_DIR1, LOW);
    digitalWrite(MOTOR_LEFT_DIR2, HIGH);
}

void Motors::forwardRight()
{
    digitalWrite(MOTOR_RIGHT_DIR1, HIGH);
    digitalWrite(MOTOR_RIGHT_DIR2, LOW);
}

void Motors::backwardRight()
{
    digitalWrite(MOTOR_RIGHT_DIR1, LOW);
    digitalWrite(MOTOR_RIGHT_DIR2, HIGH);
}

void Motors::forwardLeft()
{
    digitalWrite(MOTOR_LEFT_DIR1, HIGH);
    digitalWrite(MOTOR_LEFT_DIR2, LOW);
}

void Motors::backwardLeft()
{
    digitalWrite(MOTOR_LEFT_DIR1, LOW);
    digitalWrite(MOTOR_LEFT_DIR2, HIGH);
}


void Motors::setSpeed(uint8_t leftSpeed, uint8_t rightSpeed)
{
    softPwmWrite(MOTOR_LEFT_SPEED, leftSpeed);
    softPwmWrite(MOTOR_RIGHT_SPEED, rightSpeed);
}
