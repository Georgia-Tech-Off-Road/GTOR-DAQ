#include "DAQ_RPM_Sensor.h"
#include "Arduino.h"
//have to get silly with file paths since I want Globals defined in the DaqPackCode
#include "../../../../05 DAQ Pack/DaqPackCode/Globals.h"


RPMSensor :: RPMSensor(uint8_t pin, uint16_t numTeeth, uint32_t minExpectedRPM, uint32_t maxExpectedRPM) {
    //initialize private variables
    _pin = pin;
    _numTeeth = numTeeth;
    _prevMicros = microsecondsElapsed;
    _minExpectedRPM = minExpectedRPM;
    _maxExpectedRPM = maxExpectedRPM;
}

void RPMSensor :: handleInterrupt() {
    //always use microsecondsElapsed instead of micros()
    unsigned long long int now = microsecondsElapsed;
    _mostRecentInterval = now - _prevMicros;
    _prevMicros = now;
    //set boolean flag
    RPMUpdateFlag = true;
}

float RPMSensor :: calculateRPM() {
    //calculate time dif in minutes
    float timeDiff = (static_cast<float>(_mostRecentInterval) /1e6);
    //calculate time per rev
    float timePerRev = timeDiff * _numTeeth;
    //set _RPM to product
    _RPM = static_cast<float>(60/timePerRev);
    //set valueGood to true if the RPM is within a valid range
    if (_RPM > static_cast<float>(_minExpectedRPM) && _RPM < static_cast<float>(_maxExpectedRPM)) {
        _RPMValueGood = true;
        //always calculate timestamps based off microsecondsElapsed
        _lastGoodRPMValueTimeStamp = microsecondsElapsed * 1000;
    }
    return _RPM;
}

float RPMSensor :: checkRPM() {
    //always use microsecondsElapsed instead of micros()
    if (_prevMicros + 5000000 < microsecondsElapsed) {
        _RPM = 0;
    }
    return _RPM;
}

bool RPMSensor :: getRPMValueGood() {
    //Serial.printf("%d, %d, %d\n", _RPMValueGood, _lastGoodRPMValueTimeStamp, millis());
    if ((_lastGoodRPMValueTimeStamp + 30000) < millis() ) {
        _RPMValueGood = false;
    }
    return _RPMValueGood;
}


