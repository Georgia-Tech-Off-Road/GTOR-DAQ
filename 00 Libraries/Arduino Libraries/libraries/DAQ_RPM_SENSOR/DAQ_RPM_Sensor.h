#ifndef DAQ_RPM_SENSOR_H
#define DAQ_RPM_SENSOR_H

#include <Arduino.h>
//have to get silly with file paths since I want Globals defined in the DaqPackCode
#include "../../../../05 DAQ Pack/DaqPackCode/Globals.h"

template<uint16_t BUFFER_CAPACITY>
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
        volatile unsigned long long _ringBuffer[BUFFER_CAPACITY];
        volatile uint16_t _bufferIdx; // Points to the index beyond last data point
        volatile uint16_t _bufferSize;
};

template<uint16_t BUFFER_CAPACITY>
RPMSensor<BUFFER_CAPACITY> :: RPMSensor(uint8_t pin, uint16_t numTeeth, unsigned long long maxIntervalMicros) {
    //initialize private variables
    _pin = pin;
    _numTeeth = numTeeth;
    _prevMicros = microsecondsElapsed;
    _maxIntervalMicros = maxIntervalMicros;

    _bufferIdx = 0;
    _bufferSize = 0;    
}

template<uint16_t BUFFER_CAPACITY>
void RPMSensor<BUFFER_CAPACITY> :: handleInterrupt() {
    //always use microsecondsElapsed instead of micros()
    unsigned long long int now = microsecondsElapsed;
    _ringBuffer[_bufferIdx] = now;
    _bufferIdx = (_bufferIdx + 1) % BUFFER_CAPACITY;
    if(_bufferSize < BUFFER_CAPACITY) _bufferSize++;
    _prevMicros = now;
    //set boolean flag
    RPMUpdateFlag = true;
}

template<uint16_t BUFFER_CAPACITY>
float RPMSensor<BUFFER_CAPACITY> :: calculateRPM() {
    if (_bufferSize <= 1) return 0.0f; // Don't have enough timestamps to calculate

    unsigned long long localBuf[BUFFER_CAPACITY];
    unsigned long long accumulatedDiff = 0;

    noInterrupts();
    uint16_t size = _bufferSize;
    uint16_t head = (_bufferIdx + BUFFER_CAPACITY - 1) % BUFFER_CAPACITY;

    memcpy(localBuf, (const void *) _ringBuffer, sizeof(_ringBuffer));

    _bufferIdx = 1;
    _bufferSize = 1;

    // Place the most recent timestamp at the beginning to carry forward to the next calcluation
    _ringBuffer[0] = _ringBuffer[head];
    interrupts();

    uint16_t timestampsProcessed = 0;

    for (uint16_t i = 0; i < size - 1; i++) {
        uint16_t idx = (head + BUFFER_CAPACITY - i) % BUFFER_CAPACITY;
        uint16_t prevIdx = (head + BUFFER_CAPACITY - i - 1) % BUFFER_CAPACITY;

        unsigned long long diff = localBuf[idx] - localBuf[prevIdx];

        if (diff < _maxIntervalMicros) {
            accumulatedDiff += diff;
            timestampsProcessed++;
        }
    }

    if (timestampsProcessed == 0) return 0.0f;
    float averageDiff = (static_cast<float>(accumulatedDiff) / static_cast<float>(timestampsProcessed)) / 1e6f;

    float timePerRev = averageDiff * _numTeeth;
    _RPM = 60.0f / timePerRev;

    return _RPM;
}

template<uint16_t BUFFER_CAPACITY>
float RPMSensor<BUFFER_CAPACITY> :: checkRPM() {
    //always use microsecondsElapsed instead of micros()
    noInterrupts();
    unsigned long long prev = _prevMicros;
    unsigned long long now = microsecondsElapsed;
    interrupts();
    if (prev + _maxIntervalMicros < now) {
        _RPM = 0;
    }
    return _RPM;
}

#endif