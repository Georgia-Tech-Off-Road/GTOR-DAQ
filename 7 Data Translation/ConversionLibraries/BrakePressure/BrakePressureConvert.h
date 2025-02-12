#include <cstdint>
#include <string>
//Allocate 2 characters (1 being taken by EOL) for result of conversion
inline constexpr size_t RESULT_LENGTH = 13;

inline char* convertBrakePressure(char* line) {
    int32_t analog_value = std::stoi(line);
    double db;
    db = static_cast<double>(50+(((analog_value/32767)*4.096-.5)/4)*7950);
    char* res = new char[RESULT_LENGTH];
    snprintf(res, RESULT_LENGTH, "%f", db);
    return res;
}