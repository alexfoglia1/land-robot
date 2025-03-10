#ifndef PID_H
#define PID_H

class Pid
{
public:
    Pid(float kp, float ki, float kd, float kt, float sat);

    void reset();
    float controller(float ysp, float y);
    float getP();
    float getI();
    float getD();
    float getU();


private:
    float _error;
    float _derivative;
    float _integral;
    float _P;
    float _I;
    float _D;
    float _U;

    float _Kp;
    float _Ki;
    float _Kd;
    float _Kt;
    float _Sat;

    bool _satFlag;
};

#endif //PID_H
