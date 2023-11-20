#include "pid.h"

static float minMax(float val, float min, float max)
{
    return (val < min) ? min :
           (val > max) ? max :
           val;
}


Pid::Pid(float kp, float ki, float kt, float ad, float bd, float sat)
{
    _Kp = kp;
    _Ki = ki;
    _Kt = kt;
    _Ad = ad;
    _Bd = bd;
    _Sat = sat;

    reset();
}


float Pid::controller(float ysp, float y)
{
    float ftmp;

    //  updates actual values
    _ysp = ysp;
    _y = y;
    _error = _ysp - _y;
    //
    _P = _error * _Kp;
    _D = _D * _Ad - (_y - _ykm1) * _Bd;
    ftmp = _P + _I + _D;
    _U = minMax(ftmp, -_Sat, _Sat);
    //
    _I += _error * _Ki;
    _I += (_U - ftmp) * _Kt;
    //
    _ykm1 = _y;
    //
    return _U;
}


void Pid::reset()
{
    _y = 0.0f;
    _ykm1 = 0.0f;
    _ysp = 0.0f;
    _error = 0.0f;
    _P = 0.0f;
    _I = 0.0f;
    _D = 0.0f;
    _U = 0.0f;
}
