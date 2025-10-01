#ifndef DAQ_LDS_H
#define DAQ_LDS_H

#include "DAQ_Linear_Analog_Sensor.h"

#include <Arduino.h>

class LDS : public Linear_Analog_Sensor {
    //everything has to be static so each instance owns all of it's own items
    public:
        LDS(uint8_t resolution, float referenceVoltage, float maximalValue, float minimalValue, float maximalVoltage, float minimalVoltage);
        float computeSensorReading(int reading) override;
};
#endif