#include "DAQ_LDS_Sensor.h"
#include "Arduino.h"

LDSSensor :: LDSSensor(uint8_t pin, float length) {
    //initialize private variables
    _pin = pin;
    _length = length;
}

void LDSSensor :: calculateLength(yuint16_t analogReading) {
    //calculate length
    length = (analogReading / (pow(2,15)-1)) * _length;
}