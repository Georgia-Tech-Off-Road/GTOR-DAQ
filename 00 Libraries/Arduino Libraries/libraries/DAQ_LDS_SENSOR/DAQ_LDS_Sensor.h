#ifndef DAQ_RPM_SENSOR_H
#define DAQ_RPM_SENSOR_H

#include <Arduino.h>

#define MAX_SENSORS 3  // Adjust based on the number of sensors you expect

class RPMSensor {
    //everything has to be static so each instance owns all of it's own items
    public:
        LDSSensor(uint8_t pin, float length);
        //declare as volatile to allow for safe thread/interrupt access
        volatile float currentLength; // current length in centimeters
        //volatile flag to let data collection flag check (reduces num bits loaded per data save loop)
        void calculateLength(uint16_t analogReading); 
    private:
        uint8_t _pin;
        float length; // length in centimeters
};

#endif