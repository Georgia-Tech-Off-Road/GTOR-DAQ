#pragma once
#include <vector>
#include <string>
#include <list>
#include <unordered_map>
#include <stdexcept>
#include "macrologger.h"
#include "../ConversionLibraries/ConversionFunctions.h"

using namespace::cvf;

class DAQSensor {
    private: 
        int index;
        std::string sensorName;
        int pollingRate;
        int numTeeth;
        std::vector<void*> data;

        //Function Maps define what token in the config file should be assigned to which convert function
        std::unordered_map<std::string, char* (*)(char*)> convertFunctionMap;

        //Return the converted value in text format given an input segment
        char* (*convert)(char*);

        void assignConvertFunctionsFromName(std::string name);
    public: 
    public:
        DAQSensor();
        void initializeSensor(char* line);
        char* convertLineSegment(char* line, cvf::Time*);
        std::string toStr();
        std::string getSensorName();
};
