#include "DAQ_RPM_Sensor.h"
#include "Arduino.h"

RPMSensor :: RPMSensor(uint8_t pin, uint16_t numTeeth) {
    //initialize private variables
    _pin = pin;
    _numTeeth = numTeeth;
    _prevMicros = micros();
}

void RPMSensor :: calculateRPM() {
    //calculate time dif in minutes
    float timeDiff = (static_cast<float>((micros() - _prevMicros)) /1e6);
    //calculate time per rev
    float timePerRev = timeDiff * _numTeeth;
    //set _RPM to product
    RPM = static_cast<float>(60/timePerRev);
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