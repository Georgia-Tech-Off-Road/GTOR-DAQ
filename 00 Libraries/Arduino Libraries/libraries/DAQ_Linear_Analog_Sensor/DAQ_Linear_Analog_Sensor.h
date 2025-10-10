#ifndef DAQ_Linear_Analog_Sensor_H
#define DAQ_Linear_Analog_Sensor_H

#include "DAQ_Linear_Analog_Sensor.h"

#include <Arduino.h>

class Linear_Analog_Sensor {
    //everything has to be static so each instance owns all of it's own items
    public:
        Linear_Analog_Sensor(uint8_t resolution, float referenceVoltage, float maximalValue, float minimalValue, float maximalVoltage, float minimalVoltage);
        float computeVoltage(int reading);
        float computeSensorReading(int reading);
    private:
        uint8_t _resolution;
        float _referenceVoltage;
        float _maximalValue;
        float _minimalValue;
        float _maximalVoltage;
        float _minimalVoltage;
        
};

#endif