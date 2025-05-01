#include <cstdint>
#include <string>

namespace cvf {
    inline char* convertBrakePressure(char* line) {
        //Allocate 19 characters (1 being taken by EOL) for result of conversion
        constexpr size_t RESULT_LENGTH = 20;
        static char res[RESULT_LENGTH];
        int32_t analog_value = std::stoi(line);
        double db;
        db = static_cast<double>(50+((((analog_value/32767.0))*4.096-0.5)/4.0)*1950.0);
        snprintf(res, RESULT_LENGTH, "%f", db);
        return res;
    }
}