#include "pid.h"


static bool minMax(float* val, float min, float max)
{
    bool retValue = false;

    if (*val < min)
    {
        retValue = true;
        *val = min;
    }
    else if (*val > max)
    {
        retValue = true;
        *val = max;
    }

    return retValue;
}


Pid::Pid(float kp, float ki, float kd, float kt, float sat)
{
    _Kp = kp;
    _Ki = ki;
    _Kd = kd;
    _Kt = kt;
    _Sat = sat;
    _satFlag = false;

    reset();
}


float Pid::controller(float ysp, float y)
{
    float error = (ysp - y);

    _derivative = (error - _error) / _Kt;

    if (_satFlag == false)
    {
        _integral += (error * _Kt);
    }

    _error = error;

    float P = _Kp * _error;
    float I = _Ki * _integral;
    float D = _Kd * _derivative;

    float U = P + I + D;

    _satFlag = minMax(&U, -_Sat, _Sat);

    return U;
}


void Pid::reset()
{
    _error = 0.0f;
    _error = 0.0f;
    _integral = 0.0f;
    _derivative = 0.0f;
    _satFlag = false;
}
