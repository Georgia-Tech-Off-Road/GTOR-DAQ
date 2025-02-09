#include "BrakePressureConvert.h"
//
char* convertBrakePressure(char* line) {
    int32_t analog_value = std::stoi(line);
    double db;
    db = static_cast<double>(50+(((analog_value/32767)*4.096-.5)/4)*7950);
    char* res = new char[RESULT_LENGTH];
    snprintf(res, RESULT_LENGTH, "%f", db);
    return res;
}