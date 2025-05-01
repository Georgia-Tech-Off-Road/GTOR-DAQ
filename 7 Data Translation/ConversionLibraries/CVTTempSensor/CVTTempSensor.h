#include <cmath>
#include <string>

namespace cvf {
    inline char* CVTTempSensor(char* line) {
        constexpr size_t RESULT_LENGTH = 20;
        static char res[RESULT_LENGTH];
        
        //Convert input segment to float
        float x = std::stof(line);

        //Define Coefficients
        static const float a = -1750;
        static const float b = 0.1;
        static const float c = 2213;
        //Calculate fit result (95% confidence)
        float fitResult = a * std::pow(std::log(x), b) + c;

        snprintf(res, RESULT_LENGTH, "%f", fitResult);
        return res;
    }
}
