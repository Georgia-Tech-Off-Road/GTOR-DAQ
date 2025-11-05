#ifndef DAQ_RPM_SENSOR_H
#define DAQ_RPM_SENSOR_H

#include <Arduino.h>

#define MAX_SENSORS 3  // Adjust based on the number of sensors you expect

class RPMSensor {
    //everything has to be static so each instance owns all of it's own items
    public:
        RPMSensor(uint8_t pin, uint16_t numTeeth, uint32_t _minExpectedRPM, uint32_t _maxExpectedRPM);        
        //volatile flag to let data collection flag check (reduces num bits loaded per data save loop)
        volatile bool RPMUpdateFlag;
        float calculateRPM(); 
        float checkRPM();
        bool getRPMValueGood();
        void handleInterrupt();
    private:
        uint8_t _pin;
        uint16_t _numTeeth;
        volatile unsigned long long int _prevMicros;
        uint32_t _minExpectedRPM;
        uint32_t _maxExpectedRPM;
        volatile bool _RPMValueGood;
        volatile uint32_t _lastGoodRPMValueTimeStamp;
        volatile unsigned long long int _mostRecentInterval;
        volatile float _RPM;

};

#endif