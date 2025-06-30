#ifndef DAQ_RPM_SENSOR_H
#define DAQ_RPM_SENSOR_H

#include <Arduino.h>

#define MAX_SENSORS 3  // Adjust based on the number of sensors you expect

class RPMSensor {
    //everything has to be static so each instance owns all of it's own items
    public:
        RPMSensor(uint8_t pin, uint16_t numTeeth);
        //declare as volatile to allow for safe thread/interrupt access
        volatile float RPM;
        //volatile flag to let data collection flag check (reduces num bits loaded per data save loop)
        volatile bool RPMUpdateFlag;
        void calculateRPM(); 
        float checkRPM();
    private:
        uint8_t _pin;
        uint16_t _numTeeth;
        uint32_t _prevMicros;
};

#endif