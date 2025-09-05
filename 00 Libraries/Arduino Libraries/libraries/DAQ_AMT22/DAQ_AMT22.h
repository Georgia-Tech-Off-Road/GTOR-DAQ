#ifndef DAQ_AMT22_H
#define DAQ_AMT22_H

#include <Arduino.h>
#include <TeensyThreads.h>
#include <SPI.h>
#include <Arduino.h>
/* Library for AMT22 absolute rotary encoder heavily based off of Simone Di Blasi's version available online.*/


class AMT22 {
    public:
        AMT22(uint8_t cs, uint8_t resolution);
        void getPosition() volatile;
        void zeroPosition();
        void resetAMT22();
        //volatile to make thread/interrupt safe
        volatile float steeringPosition;
        //volatile flag to reduce num bits loaded per loop
        volatile bool steeringPositionUpdateFlag;

    private:
        uint8_t _cs, _resolution;
        void transmitOpCode(int opCode) volatile;
};

#endif