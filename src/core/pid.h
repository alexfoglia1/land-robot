#ifndef PID_H
#define PID_H

class Pid
{
public:
    Pid(float kp, float ki, float kt, float ad, float bd, float sat);

    void reset();
    float controller(float ysp, float y);

private:
    float _ysp;
    float _y;
    float _ykm1;
    float _error;
    float _P;
    float _I;
    float _D;
    float _U;

    float _Kp;
    float _Ki;
    float _Kt;
    float _Ad;
    float _Bd;
    float _Sat;
};

#endif //PID_H
