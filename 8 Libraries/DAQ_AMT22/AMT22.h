#ifndef AMT22_H
#define AMT22_H

#include <Arduino.h>
/* Library for AMT22 absolute rotary encoder heavily based off of Simone Di Blasi's version available online.*/


class AMT22 {
    public:
        AMT22(uint8_t cs, uint8_t resolution);
        float getPosition() volatile;
        float zeroPosition();
        float resetAMT22();
    private:
        uint8_t _cs, _resolution;
        float transmitOpCode(int opCode) volatile;
};

#endif