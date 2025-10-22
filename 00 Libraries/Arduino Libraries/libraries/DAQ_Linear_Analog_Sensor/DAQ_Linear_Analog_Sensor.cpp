#include "DAQ_Linear_Analog_Sensor.h"
#include "Arduino.h"

Linear_Analog_Sensor :: Linear_Analog_Sensor(uint8_t resolution, float referenceVoltage, float maximalValue, float minimalValue, float maximalVoltage, float minimalVoltage) {
    //initialize private variables
    _maximalValue = maximalValue;
    _minimalValue = minimalValue;
    _maximalVoltage = maximalVoltage;
    _minimalVoltage = minimalVoltage;
    _resolution = resolution;
    _referenceVoltage = referenceVoltage;
}

float Linear_Analog_Sensor :: computeVoltage(int reading) { 
    //reading is cvast to a float so we do floating pijnt division
    return ((float)reading / (pow(2,_resolution)-1)) * _referenceVoltage;
}

float Linear_Analog_Sensor :: computeSensorReading(int reading) {
    float voltage = computeVoltage(reading);
    float value = (voltage - _minimalVoltage)/(_maximalVoltage - _minimalVoltage) * (_maximalValue - _minimalValue);
    return value;
    //checks that the brake pressure sensor is returning a valid value
    BPSValueGood = value > 0 && value < 2000;
}

bool Linear_Analog_Sensor :: getBPSValueGood() {
    return BPSValueGood;
}

