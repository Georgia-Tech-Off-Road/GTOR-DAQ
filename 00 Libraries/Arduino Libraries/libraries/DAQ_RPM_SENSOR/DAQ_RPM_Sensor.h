#ifndef DAQ_RPM_SENSOR_H
#define DAQ_RPM_SENSOR_H

#include <Arduino.h>

// Number of entries we can hold
#define DAQ_RPM_BUFFER_CAPACITY 64


class RPMSensor {
    //everything has to be static so each instance owns all of it's own items
    public:
        RPMSensor(uint8_t pin, uint16_t numTeeth, unsigned long long maxIntervalMicros);        
        //volatile flag to let data collection flag check (reduces num bits loaded per data save loop)
        volatile bool RPMUpdateFlag;
        float calculateRPM(); 
        float checkRPM();
        void handleInterrupt();
    private:
        uint8_t _pin;
        uint16_t _numTeeth;
        volatile unsigned long long int _prevMicros;
        unsigned long long _maxIntervalMicros;
        volatile float _RPM;

        // Holds the last X RPM entries
        volatile unsigned long long _ringBuffer[DAQ_RPM_BUFFER_CAPACITY];
        volatile uint16_t _bufferIdx; // Points to the index beyond last data point
        volatile uint16_t _bufferSize;


};

#endif