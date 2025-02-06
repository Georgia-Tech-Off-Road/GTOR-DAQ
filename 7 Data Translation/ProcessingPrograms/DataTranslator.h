#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "../ConversionLibraries/ConversionFunctions.h"
//Must be above macrologger.h, decides log level, see macrologger.h for levels. Defined at compile time
#define LOG_LEVEL     DEBUG_LEVEL

//Shows the logger if to log INFO_LEVEL and DEBUG_LEVEL messages
//If VERBOSE_FLAG variable is false will only log ERROR_LEVEL messages ragardless of LOG_LEVEL value
#define VERBOSE_FLAG (verbose_flag)
#include "macrologger.h"

//The buffer for each line in the binary file will be set to this
#define LINE_BUFFER_SIZE 300

//Enum for SensorType
enum DataType {digital = 0, analog = 1};

const char* getDataTypeName(DataType dt) {
    return (dt == DataType::digital ? "digital" : "analog");
}

//Define a struct that contains the basic data for each sensor
struct DAQSensor {
    int index;
    DataType dataType;
    std::string sensorName;
    int pollingRate;
    int numTeeth;
    //Given a line in the config file, read the line and initialize the Sensor
    void initializeSensor(char* line) {
        char sensorInfo[5][30];
        //Load the text before the first "," into sensorInfo[0]
        strcpy(sensorInfo[0], strtok(line, ","));
        //In bizzare design NULL tells strtok to use the previously give src, i.e line_buffer
        strcpy(sensorInfo[1], strtok(NULL, ","));
        strcpy(sensorInfo[2], strtok(NULL, ","));
        strcpy(sensorInfo[3], strtok(NULL, ","));
        strcpy(sensorInfo[4], strtok(NULL, "\n"));
        //Initialize index, convert string to int
        try {
            this->index = std::stoi(sensorInfo[0]);
        } catch (const std::exception& e) {
            LOG_ERROR("In line: %s of your config file, your index parameter %s is misformatted", line, sensorInfo[0]);
        }
        //Find data type in config file
        if (strcmp(sensorInfo[1], "digital") == 0) {
            this->dataType = DataType::digital;
        } else if (strcmp(sensorInfo[1], "analog") == 0) {
            this->dataType = DataType::analog;
        } else {
            LOG_ERROR("In line %s the data type must either be digital or analog, not %s", line, sensorInfo[1]);
        }
        
        //Initialize sensorName
        sensorName.assign(sensorInfo[2]);

        //Initialize pollingRate, convert string to int
        try {
            this->pollingRate = std::stoi(sensorInfo[3]);
        } catch (const std::exception& e) {
            LOG_ERROR("In line: %s of your config file, your pollingRate parameter %s is misformatted", line, sensorInfo[3]);
        }

        //Initialize numTeeth
        try {
            this->numTeeth = std::stoi(sensorInfo[4]);
        } catch (const std::exception& e) {
            LOG_ERROR("In line: %s of your config file, your numTeeth parameter %s is misformatted", line, sensorInfo[3]);
        }
    }

    std::string toStr() {
        const int bufferSize = 200;
        char buffer[bufferSize];

        sprintf(buffer,"|---------Sensor---------|\nName: %s\nIndex: %d\nData Type: %s\nPolling Rate: %d\nNum Teeth: %d\n\n", 
            this->sensorName.c_str(), this->index, getDataTypeName(this->dataType), this->pollingRate, this->numTeeth);
        
        return std::string(buffer);
    }
};

void readConfigFile(std::ifstream *cf);

void processCLIArgs(int argc, char *argv[]);

void openFiles();