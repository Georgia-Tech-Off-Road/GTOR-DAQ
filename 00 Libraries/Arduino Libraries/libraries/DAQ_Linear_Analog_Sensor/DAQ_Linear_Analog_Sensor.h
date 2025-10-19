#ifndef DAQ_Linear_Analog_Sensor_H
#define DAQ_Linear_Analog_Sensor_H

// Defines a time difference that defines if there has been a recent recording
#define DAQ_ANALOG_SENSOR_RECENT_RECORDING_TIME 10

#include "DAQ_Linear_Analog_Sensor.h"

#include <Arduino.h>

class Linear_Analog_Sensor {
    //everything has to be static so each instance owns all of it's own items
    public:
        Linear_Analog_Sensor(uint8_t resolution, float referenceVoltage, float maximalValue, float minimalValue, float maximalVoltage, float minimalVoltage);
        float computeVoltage(int reading);
        float computeSensorReading(int reading);
        bool hasRecentRecording();
    private:
        uint8_t _resolution;
        float _referenceVoltage;
        float _maximalValue;
        float _minimalValue;
        float _maximalVoltage;
        float _minimalVoltage;
        // Debug boolean if sensor reading has been computed recently based on
        bool _recentRecording;
        // Time of previous recording in millisec
        uint32_t previousRecordingMS;

        void updateRecentRecording();

        
};

#endif