#ifndef MOTORS_H
#define MOTORS_H

#include <stdint.h>

#define MOTOR_RIGHT_SPEED 4
#define MOTOR_RIGHT_DIR1  16
#define MOTOR_RIGHT_DIR2  1
#define MOTOR_LEFT_SPEED  0
#define MOTOR_LEFT_DIR1   3
#define MOTOR_LEFT_DIR2   2

class Motors
{
public:
    Motors();

    void stopMotors();
    void forward();
    void backward();
    void setSpeed(uint8_t leftSpeed, uint8_t rightSpeed);

private:
    uint8_t _minSpeed;
    uint8_t _maxSpeed;
};


#endif
