#include <cmath>
#include <string>
#include "../../../8 Libraries/TCTemp/Convert.h"

namespace cvf {
    inline char* CVTTempSensor(char* line) {
        constexpr size_t RESULT_LENGTH = 20;
        static char res[RESULT_LENGTH];
        
        //Convert input segment to float
        int x = std::stoi(line);

        float tempValue = TCTemp::calculateTemp(x);

        snprintf(res, RESULT_LENGTH, "%f", tempValue);
        return res;
    }
}
