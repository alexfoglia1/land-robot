#ifndef RASPI_SERVO_H
#define RASPI_SERVO_H

#include "servo.h"

#define SERVO_PIN 11

class RaspiServo : public Servo
{
public:
    RaspiServo();
    bool init() override;
    void writeMicroseconds(uint16_t delayUsAzi, uint16_t delayUsEle) override;
    void writeAngle(float angleDeg) override;

private:
    int _range;
};


#endif // RASPI_SERVO_H
