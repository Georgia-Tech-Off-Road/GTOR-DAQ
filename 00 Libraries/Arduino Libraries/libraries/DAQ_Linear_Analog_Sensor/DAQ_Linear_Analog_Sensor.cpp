#include "DAQ_Linear_Analog_Sensor.h"
#include "Arduino.h"
#include <TimeLib.h>

Linear_Analog_Sensor :: Linear_Analog_Sensor(uint8_t resolution, float referenceVoltage, float maximalValue, float minimalValue, float maximalVoltage, float minimalVoltage) {
    //initialize private variables
    _maximalValue = maximalValue;
    _minimalValue = minimalValue;
    _maximalVoltage = maximalVoltage;
    _minimalVoltage = minimalVoltage;
    _resolution = resolution;
    _referenceVoltage = referenceVoltage;
    _recentRecording = false;
    _previousRecordingMS = 0;
}

float Linear_Analog_Sensor :: computeVoltage(int reading) { 
    //reading is cvast to a float so we do floating pijnt division
    return ((float)reading / (pow(2,_resolution)-1)) * _referenceVoltage;
}

float Linear_Analog_Sensor :: computeSensorReading(int reading) {
    // Update if recording is recent or not
    updateRecentRecording();
    _previousRecordingMS = currentRecordingMS;
    float voltage = computeVoltage(reading);
    float value = (voltage - _minimalVoltage)/(_maximalVoltage - _minimalVoltage) * (_maximalValue - _minimalValue);
    return value;
}

bool Linear_Analog_Sensor::hasRecentRecording() {
    return _recentRecording;
}

void updateRecentRecording() {
    _recentRecording = false;
    uint32_t currentRecordingMS = millis();
    if (currentRecordingMS - _previousRecordingMS <= DAQ_ANALOG_SENSOR_RECENT_RECORDING_TIME) {
        _recentRecording = true;
    }
}