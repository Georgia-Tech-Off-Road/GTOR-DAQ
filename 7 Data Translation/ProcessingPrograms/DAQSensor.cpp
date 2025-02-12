#include "DAQSensor.h"

DAQSensor::DAQSensor() {
    //Initialize the function maps
    convertFunctionMap = {
        {"brake", cvf::convertBrakePressure},
        {"time", cvf::convertTime}
    };
    convertDigitalFunctionMap = {
        {"RPM", cvf::convertRPM}
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

    //Assign sensor data type
    sensorType = strToSensorType(sensorInfo[1]);
    
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
    snprintf(buffer, bufferSize, "|---------Sensor---------|\nName: %s\nSensor Type: %s\nIndex: %d\nPolling Rate: %d\nNum Teeth: %d\n\n", 
        this->sensorName.c_str(), sensorTypeToStr(this->sensorType), this->index, this->pollingRate, this->numTeeth);
    return std::string(buffer);
}

char* DAQSensor::convertLine(char* line, Time *time) {
    if (sensorType == DAQSensorType::analog) {
        return convert(line);
    } else {
        return convertDigital(line, time);
    }
}

void DAQSensor::assignConvertFunctionsFromName(std::string name) {
    //Assign analog functions
    if(sensorType == DAQSensorType::analog) {
        for(auto pair = convertFunctionMap.begin(); pair != convertFunctionMap.end(); ++pair) {
            if (name.find(pair->first) != std::string::npos) {
                convert = convertFunctionMap[pair->first];
                return;
            }
        }
        char msg[400];
        sprintf(msg, "%s is not a known sensor token. Here is a list of valid sensor tokens:\n", name.c_str());
        for (auto pair : convertFunctionMap) {
            strcat(msg, pair.first.c_str());
            strcat(msg, "\n");
        }
        strcat(msg, "The name of sensor only needs to include one of these tokens, it does not have to match this token exactly!");
        throw std::invalid_argument(msg);
    }
    //Now assign digital functions
    else {
        for(auto pair = convertDigitalFunctionMap.begin(); pair != convertDigitalFunctionMap.end(); ++pair) {
            if (name.find(pair->first) != std::string::npos) {
                convertDigital = convertDigitalFunctionMap[pair->first];
                return;
            }
        }
        char msg[400];
        sprintf(msg, "%s is not a known sensor token. Here is a list of valid sensor tokens:\n", name.c_str());
        for (auto pair : convertDigitalFunctionMap) {
            strcat(msg, pair.first.c_str());
            strcat(msg, "\n");
        }
        strcat(msg, "The name of sensor only needs to include one of these tokens, it does not have to match this token exactly!");
        throw std::invalid_argument(msg);
    }
}

std::string DAQSensor::getSensorName() {
    return sensorName;
}