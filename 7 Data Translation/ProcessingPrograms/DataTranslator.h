#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "../ConversionLibraries/ConversionFunctions.h"
#include "DAQSensors.h"
//Must be above macrologger.h, decides log level, see macrologger.h for levels. Defined at compile time
#define LOG_LEVEL     DEBUG_LEVEL

//Shows the logger if to log INFO_LEVEL and DEBUG_LEVEL messages
//If VERBOSE_FLAG variable is false will only log ERROR_LEVEL messages ragardless of LOG_LEVEL value
#ifndef VERBOSE_FLAG
#define VERBOSE_FLAG (verbose_flag)
#endif
#include "macrologger.h"

//Buffer size for each line read from input file
inline constexpr int LINE_BUFFER_SIZE = 300;
//If you're confused why constexpr is used instead of const, read here: https://www.cppstories.com/2022/const-options-cpp20/

void readConfigFile(std::ifstream *cf);

void processCLIArgs(int argc, char *argv[]);

void readInputFile(std::ifstream *inf);

void openFiles();

void processInputLine(char* line_buffer);