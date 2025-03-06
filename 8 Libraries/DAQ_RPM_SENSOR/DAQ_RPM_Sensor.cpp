#include "DAQ_RPM_Sensor.h"
#include "Arduino.h"

//declare static variables (literally everything)
uint8_t RPMSensor :: _pin;
uint16_t RPMSensor :: _numTeeth;
uint32_t RPMSensor :: _prevMicros;
volatile float RPMSensor :: RPM;
volatile bool RPMSensor :: RPMUpdateFlag;

RPMSensor :: RPMSensor(uint8_t pin, uint16_t numTeeth) {
    //initialize private variables
    _pin = pin;
    _numTeeth = numTeeth;
    _prevMicros = micros();
    //attach interrupt 
    attachInterrupt(digitalPinToInterrupt(pin), RPMSensor:: calculateRPM, CHANGE);
}

void RPMSensor :: calculateRPM() {
    //calculate time dif in minutes
    uint32_t timeDiff = (micros() - _prevMicros) / (pow(10, 6));
    //calculate percentage of wheel traversed
    float percentageTraversed = 1/(_numTeeth);
    //set _RPM to product
    RPM = static_cast<float>(timeDiff) * percentageTraversed;
    //set boolean flag
    RPMUpdateFlag = true;
}