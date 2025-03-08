#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

#define MIN_DELAY_MICROSECONDS 1000
#define MAX_DELAY_MICROSECONDS 2000
#define MIN_ANGLE_DEGREES 0.0f
#define MAX_ANGLE_DEGREES 180.0f

class Servo
{
public:
    Servo() {};
    virtual bool init() { return true; };
    virtual void writeMicroseconds(uint16_t delayUs) {};
    virtual void writeAngle(float angleDeg)  {};
};

#endif // SERVO_H
