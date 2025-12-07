#include "DAQSensor.h"

DAQSensor::DAQSensor() {
    //Initialize the function maps
    convertFunctionMap = {
        {"brake", cvf::convertBrakePressure},
        {"time", cvf::copy},
        {"RPM", cvf::copy},
        {"transfer", cvf::CVTTempSensor}
    };
}

// Given a line in the config file, read the line and initialize the Sensor
void DAQSensor::initializeSensor(char* line) {
    char sensorInfo[5][30];
    // Load the text before the first "," into sensorInfo[0]
    strcpy(sensorInfo[0], strtok(line, ","));
    // In bizzare design NULL tells strtok to use the previously give src, 
    // i.e line_buffer
    strcpy(sensorInfo[1], strtok(NULL, ","));
    strcpy(sensorInfo[2], strtok(NULL, ","));
    strcpy(sensorInfo[3], strtok(NULL, ","));
    strcpy(sensorInfo[4], strtok(NULL, "\n"));
    // Initialize index, convert string to int
    try {
        this->index = std::stoi(sensorInfo[0]);
    } catch (const std::exception& e) {
        LOG_ERROR("In line: %s of your config file, your index parameter %s is misformatted", line, sensorInfo[0]);
    }

    //Assign sensor data type. NO LONGER USED. ONLY HAVE Analog Sensors Currently
    //sensorType = strToSensorType(sensorInfo[1]);
    
    // Initialize sensorName
    sensorName.assign(sensorInfo[2]);

    //Assign dependent values from name (conversion function, etc.)
    assignConvertFunctionsFromName(sensorName);

    // Initialize pollingRate, convert string to int
    try {
        this->pollingRate = std::stoi(sensorInfo[3]);
    } catch (const std::exception& e) {
        LOG_ERROR("In line: %s of your config file, your pollingRate parameter %s is misformatted", line, sensorInfo[3]);
    }

    // Initialize numTeeth
    try {
        this->numTeeth = std::stoi(sensorInfo[4]);
    } catch (const std::exception& e) {
        LOG_ERROR("In line: %s of your config file, your numTeeth parameter %s is misformatted", line, sensorInfo[3]);
    }
}

std::string DAQSensor::toStr() {
    constexpr int bufferSize = 200;
    char buffer[bufferSize];
    snprintf(buffer, bufferSize, "|---------Sensor---------|\nName: %s\nIndex: %d\nPolling Rate: %d\nNum Teeth: %d\n\n", 
        this->sensorName.c_str(), this->index, this->pollingRate, this->numTeeth);
    return std::string(buffer);
}

char* DAQSensor::convertLineSegment(char* line, Time *time) {
    return convert(line);
}

void DAQSensor::assignConvertFunctionsFromName(std::string name) {
        bool foundConvertFunction;
        for (auto pair = convertFunctionMap.begin(); pair != convertFunctionMap.end(); ++pair) {
            if (name.find(pair->first) != std::string::npos) {
                printf("Found conversion function for: %s sensor!\n", pair->first.c_str());
                convert = convertFunctionMap[pair->first];
                foundConvertFunction = true;
                return;
            }
        }
        char msg[400];
        printf("Could not find a custom conversion function for %s: will just copy data\n", name.c_str());
        //Literally will just copy and paste value
        convert=cvf::copy;
}

std::string DAQSensor::getSensorName() {
    return sensorName;
}
