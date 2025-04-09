#include <cstdint>
#include <cmath>
#include <string>
namespace cvf {
    inline char* CVTTempSensor(char* line) {
        //Allocate 19 characters (1 being taken by EOL) for result of conversion
        constexpr size_t RESULT_LENGTH = 20;
        static char res[RESULT_LENGTH];
        int32_t analog_value = std::stoi(line);
        double db;
        db = static_cast<double>(-1750 * std::pow(std::log(analog_value), 0.1) + 2213);
        snprintf(res, RESULT_LENGTH, "%f", db);
        return res;
    }
}
