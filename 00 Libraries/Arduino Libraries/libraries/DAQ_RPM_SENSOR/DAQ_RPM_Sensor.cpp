#include "DAQ_RPM_Sensor.h"
#include "Arduino.h"

RPMSensor :: RPMSensor(uint8_t pin, uint16_t numTeeth, uint32_t minExpectedRPM, uint32_t maxExpectedRPM) {
    //initialize private variables
    _pin = pin;
    _numTeeth = numTeeth;
    _prevMicros = micros();
    _minExpectedRPM = minExpectedRPM;
    _maxExpectedRPM = maxExpectedRPM;
}

void RPMSensor :: calculateRPM() {
    //calculate time dif in minutes
    float timeDiff = (static_cast<float>((micros() - _prevMicros)) /1e6);
    //calculate time per rev
    float timePerRev = timeDiff * _numTeeth;
    //set _RPM to product
    RPM = static_cast<float>(60/timePerRev);
    //set valueGood to true if the RPM is within a valid range
    if (RPM > static_cast<float>(_minExpectedRPM) && RPM < static_cast<float>(_maxExpectedRPM)) {
        _RPMValueGood = true;
        _lastGoodRPMValueTimeStamp = millis();
    }
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

bool RPMSensor :: getRPMValueGood() {
    //Serial.printf("%d, %d, %d\n", _RPMValueGood, _lastGoodRPMValueTimeStamp, millis());
    if ((_lastGoodRPMValueTimeStamp + 30000) < millis() ) {
        _RPMValueGood = false;
    }
    return _RPMValueGood;
}


