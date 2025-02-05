#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "../ConversionLibraries/ConversionFunctions.h"
//Must be above macrologger.h, decides log level, see macrologger.h for levels. Defined at compile time
#define LOG_LEVEL     DEBUG_LEVEL

//Shows the logger if to log INFO_LEVEL and DEBUG_LEVEL messages
//If VERBOSE_FLAG variable is false will only log ERROR_LEVEL messages ragardless of LOG_LEVEL value
#define VERBOSE_FLAG (verbose_flag)
#include "macrologger.h"

//The buffer for each line in the binary file will be set to this
#define LINE_BUFFER_SIZE 300

void readConfigFile(std::ifstream *cf);