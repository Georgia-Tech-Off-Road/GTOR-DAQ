#include <cstdint>
#include <string>
//Allocate 19 characters (1 being taken by EOL) for result of conversion
inline constexpr size_t RESULT_LENGTH = 20;

inline char* convertBrakePressure(char* line) {
    static char res[RESULT_LENGTH];
    int32_t analog_value = std::stoi(line);
    double db;
    db = static_cast<double>(-1750 * pow(log(x), 0.1) + 2213);
    snprintf(res, RESULT_LENGTH, "%f", db);
    return res;
}
