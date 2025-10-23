#include "DAQ_Linear_Analog_Sensor.h"
#include "Arduino.h"

Linear_Analog_Sensor :: Linear_Analog_Sensor(uint8_t resolution, float referenceVoltage, float maximalValue, float minimalValue, float maximalVoltage, float minimalVoltage, float minimalReading, float maximalReading) {
    //initialize private variables
    _maximalValue = maximalValue;
    _minimalValue = minimalValue;
    _maximalVoltage = maximalVoltage;
    _minimalVoltage = minimalVoltage;
    _resolution = resolution;
    _referenceVoltage = referenceVoltage;
    _minimalReading = minimalReading;
    _maximalReading = maximalReading;
}

float Linear_Analog_Sensor :: computeVoltage(int reading) { 
    //reading is cvast to a float so we do floating pijnt division
    return ((float)reading / (pow(2,_resolution)-1)) * _referenceVoltage;
}

float Linear_Analog_Sensor :: computeSensorReading(int reading) {
    float voltage = computeVoltage(reading);
    float value = (voltage - _minimalVoltage)/(_maximalVoltage - _minimalVoltage) * (_maximalValue - _minimalValue);
    //checks that the brake pressure sensor is returning a valid value
    if (value > _minimalReading && value < _maximalReading) {
        _valueGood = true;
        _lastGoodValueTimeStamp = millis();
    }
    return value;
}

bool Linear_Analog_Sensor :: getValueGood() {
    if ((_lastGoodValueTimeStamp + 30000) - millis() < 0) {
        _valueGood = false;
    }
    return _valueGood;
}

