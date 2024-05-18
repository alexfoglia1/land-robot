#include "pid.h"

static float minMax(float val, float min, float max)
{
    return (val < min) ? min :
           (val > max) ? max :
           val;
}


Pid::Pid(float kp, float ki, float kd, float kt, float sat)
{
    _Kp = kp;
    _Ki = ki;
    _Kd = kd;
    _Kt = kt;
    _Sat = sat;

    reset();
}


float Pid::controller(float ysp, float y)
{
    float error = (ysp - y);

    _derivative = (error - _error) / _Kt;
    _integral += (error * _Kt);
    _error = error;

    float P = _Kp * _error;
    float I = _Ki * _integral;
    float D = _Kd * _derivative;

    float U = P + I + D;

    if (U < -_Sat)
    {
        U = -_Sat;
    }
    else if (U > _Sat)
    {
        U = _Sat;
    }

    return U;
}


void Pid::reset()
{
    _error = 0.0f;
    _error = 0.0f;
    _integral = 0.0f;
    _derivative = 0.0f;
}
