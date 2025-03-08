#include "raspi-servo.h"

#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>


void* writeMicrosecondsTask(void* params)
{
    int* iParams = reinterpret_cast<int*>(params);

    int delayUs = iParams[0];
    int _range = iParams[1];

    const float PERIOD_S = 0.010f;

    int delayMinusMin = delayUs - MIN_DELAY_MICROSECONDS;
    if (delayMinusMin < 0) delayMinusMin = 0;
    float riseTime_s = delayMinusMin * 1e-6f;
    float totalRiseTime_s = (MIN_DELAY_MICROSECONDS) * 1e-6f + riseTime_s;
    float dc = totalRiseTime_s / PERIOD_S;
    int toWrite = dc * _range;

    //printf("delayMicroseconds(%d), totalRiseTime(%f s), dc(%f), toWrite(%d)\n", delayUs, totalRiseTime_s, dc, toWrite);
    softPwmWrite(SERVO_PIN, toWrite);

    usleep(0.25 * 1e6);
    softPwmWrite(SERVO_PIN, 0);

    free(params);

    return NULL;
}

RaspiServo::RaspiServo() : Servo()
{
    _range = 100;
}


bool RaspiServo::init()
{
    pinMode(SERVO_PIN, OUTPUT);
    digitalWrite(SERVO_PIN, LOW);

    softPwmCreate(SERVO_PIN, 0, _range);

    return false;
}


void RaspiServo::writeMicroseconds(uint16_t delayUs)
{
    pthread_t servoThread;
    int* params = reinterpret_cast<int*>(malloc(2 * sizeof(int)));
    params[0] = delayUs;
    params[1] = _range;
    pthread_create(&servoThread, nullptr, *writeMicrosecondsTask, reinterpret_cast<void*>(params));
}


void RaspiServo::writeAngle(float angleDeg)
{

}
