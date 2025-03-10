#include "telemetry.h"

#include <pthread.h>
#include <string.h>

Telemetry::Telemetry(Comm* comm, Control* monitored) :
    _comm(comm),
    _monitored(monitored)
{
}


void* telemetryTask(void* arg)
{
    Telemetry* telemetry = reinterpret_cast<Telemetry*>(arg);

    while (1)
    {
        telemetry->_comm->waitTelemetry();
        telemetry->serveRequest();
    }
}

void Telemetry::serveRequest()
{
    if (_comm->hasPendingTelemetryRequests())
    {
        TelemetryHeader rxHeader = _comm->getTelemetryRequest();
        TelemetryMessage answer;
        answer.header = rxHeader;

        float gx, gy, gz;
        float ax, ay, az;
        _monitored->_imu->readAccel(&ax, &ay, &az);
        _monitored->_imu->readGyro(&gx, &gy, &gz);

        uint32_t payloadIndex = 0;
        if (answer.header.byte0.Bits.gx)
        {
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&gx), sizeof(float));
        }
        if (answer.header.byte0.Bits.gy)
        {
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&gy), sizeof(float));
        }
        if (answer.header.byte0.Bits.gz)
        {
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&gz), sizeof(float));
        }
        if (answer.header.byte0.Bits.ax)
        {
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&ax), sizeof(float));
        }
        if (answer.header.byte0.Bits.ay)
        {
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&ay), sizeof(float));
        }
        if (answer.header.byte0.Bits.az)
        {
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&az), sizeof(float));
        }
        if (answer.header.byte0.Bits.gz_sp)
        {
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&_monitored->_gyroZSetPoint), sizeof(int16_t));
        }

        if (answer.header.byte1.Bits.pid_p)
        {
            float p = _monitored->_pid.getP();
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&p), sizeof(float));
        }
        if (answer.header.byte1.Bits.pid_i)
        {
            float i = _monitored->_pid.getI();
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&i), sizeof(float));
        }
        if (answer.header.byte1.Bits.pid_d)
        {
            float d = _monitored->_pid.getD();
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&d), sizeof(float));
        }
        if (answer.header.byte1.Bits.pid_u)
        {
            float u = _monitored->_pid.getU();
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&u), sizeof(float));
        }
        if (answer.header.byte1.Bits.thr_l)
        {
            uint16_t thr_l = _monitored->_leftCmd;
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&thr_l), sizeof(uint16_t));
        }
        if (answer.header.byte1.Bits.thr_r)
        {
            uint16_t thr_r = _monitored->_rightCmd;
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&thr_r), sizeof(uint16_t));
        }
        if (answer.header.byte1.Bits.servo)
        {
            uint16_t servo = _monitored->_lastDelayMicroseconds;
            pushToPayload(answer.payload, &payloadIndex, reinterpret_cast<const uint8_t*>(&servo), sizeof(uint16_t));
        }

        _comm->sendTelemetry(reinterpret_cast<char*>(&answer), sizeof(TelemetryHeader) + payloadIndex);
    }
}


void Telemetry::pushToPayload(uint8_t* destBase, uint32_t* destOffset, const uint8_t* src, size_t size)
{
    void* pDest = reinterpret_cast<void*>(&destBase[*destOffset]);
    memcpy(pDest, src, size);
    *destOffset += size;
}


void Telemetry::start()
{
    pthread_t thread;
    pthread_create(&thread, nullptr, *telemetryTask, reinterpret_cast<void*>(this));
}
