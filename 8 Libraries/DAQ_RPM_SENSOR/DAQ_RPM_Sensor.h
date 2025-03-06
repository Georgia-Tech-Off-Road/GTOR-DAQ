#ifndef DAQ_RPM_SENSOR_H
#define DAQ_RPM_SENSOR_H

#include <Arduino.h>

class RPMSensor {
    //everything has to be static so each instance owns all of it's own items
    public:
        RPMSensor(uint8_t pin, uint16_t numTeeth);
        //declare as volatile to allow for safe thread/interrupt access
        static volatile float RPM;
        //volatile flag to let data collection flag check (reduces num bits loaded per data save loop)
        static volatile bool RPMUpdateFlag;
    private:
        static uint8_t _pin;
        static uint16_t _numTeeth;
        static uint32_t _prevMicros;
        static void calculateRPM();
};

#endif