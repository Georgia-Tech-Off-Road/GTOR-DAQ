#pragma once
#include <vector>
#include <string>
#include <list>
#include <unordered_map>
#include <stdexcept>
#include "macrologger.h"
#include "../ConversionLibraries/ConversionFunctions.h"

using namespace::cvf;

enum DAQSensorType {digital = 0, analog = 1};
inline DAQSensorType strToSensorType(char* str) {
    if (strcmp(str, "analog") == 0) {
        return DAQSensorType::analog;
    }
    else if (strcmp(str, "digital") == 0) {
        return DAQSensorType::digital;
    }
    else {
        throw std::invalid_argument("Sensor type must either be digital or analog!");
    }
}

inline const char* sensorTypeToStr(DAQSensorType type) {
    if (type == DAQSensorType::analog) {
        return "analog";
    } else {
        return "digital";
    }
}
class DAQSensor {
    private: 
        int index;
        std::string sensorName;
        int pollingRate;
        int numTeeth;
        std::vector<void*> data;

        //Function Maps define what token in the config file should be assigned to which convert function
        std::unordered_map<std::string, char* (*)(char*)> convertFunctionMap;
        std::unordered_map<std::string, char* (*)(char*, Time*)> convertDigitalFunctionMap;

        // Analog sensor only properties, should be left null for digital sensors
        // Converts given text input into converted format for .csv
        char* (*convert)(char*);
        char* (*convertDigital)(char*, Time*);

        //Digital sensor only properties, should be left null for analog sensor
        // Records when the bit flipped on the sensor
        std::list<Time> changeList;
        // Give the time difference between bit flips,
        // returns the converted value in text format for csv
        char* (*convertFromTimeDiff)(Time);

        void assignConvertFunctionsFromName(std::string name);
    public: 
        DAQSensorType sensorType;
    public:
        DAQSensor();
        void initializeSensor(char* line);
        char* convertLine(char* line, cvf::Time*);
        std::string toStr();
        DAQSensorType getSensorType();
        std::string getSensorName();
};
