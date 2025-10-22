#include "DAQ_RPM_Sensor.h"
#include "Arduino.h"

RPMSensor :: RPMSensor(uint8_t pin, uint16_t numTeeth, uint8_t _minExpectedRPM, uint8_t _maxExpectedRPM) {
    //initialize private variables
    _pin = pin;
    _numTeeth = numTeeth;
    _prevMicros = micros();
    _minExpectedRPM = _minExpectedRPM;
    _maxExpectedRPM = _maxExpectedRPM;
}

void RPMSensor :: calculateRPM() {
    //calculate time dif in minutes
    float timeDiff = (static_cast<float>((micros() - _prevMicros)) /1e6);
    //calculate time per rev
    float timePerRev = timeDiff * _numTeeth;
    //set _RPM to product
    RPM = static_cast<float>(60/timePerRev);
    //set valueGood to true if the RPM is within a valid range
    RPMValueGood = RPM > _minExpectedRPM && RPM < _maxExpectedRPM;
    //set boolean flag
    RPMUpdateFlag = true;
    //update _prevMicros
    _prevMicros = micros();
}

float RPMSensor :: checkRPM() {
    if (_prevMicros + 5000000 < micros()) {
        RPM = 0;
    }
    return RPM;
}

void RPMSensor :: getRPMValueGood() {
    return RPMValueGood
}


