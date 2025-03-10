#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "comm.h"
#include "control.h"

class Telemetry
{
public:
    Telemetry(Comm* comm, Control* monitored);
    void start();
    void serveRequest();
    Comm* _comm;

private:
    Control* _monitored;


    void pushToPayload(uint8_t* destBase, uint32_t* destOffset, const uint8_t* src, size_t size);

};

#endif //TELEMETRY_H
