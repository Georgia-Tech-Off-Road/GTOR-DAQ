#ifndef DAQ_RPM_SENSOR_H
#define DAQ_RPM_SENSOR_H

#include <Arduino.h>

// Number of entries we can hold
#define BUFFER_LEN 8


class RPMSensor {
    //everything has to be static so each instance owns all of it's own items
    public:
        RPMSensor(uint8_t pin, uint16_t numTeeth, uint32_t _minExpectedRPM, uint32_t _maxExpectedRPM, float maxIntervalMicros);        
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
        float _maxIntervalMicros;
        volatile bool _RPMValueGood;
        volatile uint32_t _lastGoodRPMValueTimeStamp;
        volatile unsigned long long int _mostRecentInterval;
        volatile float _RPM;

        // Holds the last X RPM entries
        float ringBuffer[BUFFER_LEN]
        uint8_t buffer_idx;


};

#endif