#include "DAQSensors.h"

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
    
    // Initialize sensorName
    sensorName.assign(sensorInfo[2]);

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
    inline constexpr int bufferSize = 200;
    char buffer[bufferSize];
    snprintf(buffer, bufferSize, "|---------Sensor---------|\nName: %s\nIndex: %d\nPolling Rate: %d\nNum Teeth: %d\n\n", 
        this->sensorName.c_str(), this->index, this->pollingRate, this->numTeeth);
    return std::string(buffer);
}

// Analog Sensor Class!!!!

