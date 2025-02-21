#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <chrono>
#include <bits/stdc++.h>
#include "../ConversionLibraries/ConversionFunctions.h"
#include "DAQSensor.h"
//Must be above macrologger.h, decides log level, see macrologger.h for levels. Defined at compile time
#define LOG_LEVEL     ERROR_LEVEL

//Shows the logger if to log INFO_LEVEL and DEBUG_LEVEL messages
//If VERBOSE_FLAG variable is false will only log ERROR_LEVEL messages ragardless of LOG_LEVEL value
#define VERBOSE_FLAG (verbose_flag)
#include "macrologger.h"



//Buffer size for each line read from input file
inline constexpr int LINE_BUFFER_SIZE = 500;
//If you're confused why constexpr is used instead of const, read here: https://www.cppstories.com/2022/const-options-cpp20/

void readConfigFile(std::ifstream *cf);

int processCLIArgs(int argc, char *argv[], std::string *inputFileName, std::string *outputFileName, std::string *configFileName);

void convertInputFile(std::ifstream *inf, std::ofstream *of);

void openFiles(std::ifstream *in_file, std::string inputFileName, std::ofstream *out_file, std::string outputFileName, std::ifstream *config_file, std::string configFileName);

void processInputLine(char* line_buffer, std::ofstream *of);

void printHelp();


cvf::Time getTimeFromLine(char*);