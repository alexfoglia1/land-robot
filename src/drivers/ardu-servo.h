#ifndef ARDU_SERVO_H
#define ARDU_SERVO_H

#include <stdint.h>
#include <list>
#include <string>

#include "servo.h"

#define SERVO_BAUD 115200

#define RASPI_SERVO_SYNC_BYTE 0xFF

#define ARDU_SERVO_WHOAMI "ARDUSERVO"

typedef struct
{
    uint8_t sync;
    uint8_t id;
    uint16_t delay_microseconds;
} __attribute__((packed)) raspi_servo_msg_t;


typedef struct
{
    uint8_t sync;
    char whoami[9];
} __attribute__((packed)) servo_raspi_msg_t;



class ArduServo : public Servo
{
public:
    ArduServo();

    bool init() override;
    void writeMicroseconds(uint16_t delayUs) override;
    void writeAngle(float angleDeg) override;

private:
   void readDir(const char* path);
   int initSerial(const char* path);
   bool testDevice(int fd);
   void servoTx(raspi_servo_msg_t* msg);

   bool _servoFound;
   int _servoFd;
   std::list<std::string> _usbDevices;

};



#endif //ARDU_SERVO_H
