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

    _P = _Kp * _error;
    _I = _Ki * _integral;
    _D = _Kd * _derivative;

    _U = _P + _I + _D;

    _satFlag = minMax(&_U, -_Sat, _Sat);

    return _U;
}


void Pid::reset()
{
    _error = 0.0f;
    _integral = 0.0f;
    _derivative = 0.0f;
    _P = 0.0f;
    _I = 0.0f;
    _D = 0.0f;
    _U = 0.0f;
    _satFlag = false;
}


float Pid::getP()
{
    return _P;
}

float Pid::getI()
{
    return _I;
}

float Pid::getD()
{
    return _D;
}

float Pid::getU()
{
    return _U;
}
