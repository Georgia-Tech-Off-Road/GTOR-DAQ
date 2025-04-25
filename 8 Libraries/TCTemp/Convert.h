#ifndef TC_TEMP_CONVERT_H
#define TC_TEMP_CONVERT_H
#include <cstdint>
#include <cmath>

namespace TCTemp {

    float calculateR2(float V_s, float V_out, float R_1) {
        float R_2 = R_1 / ((V_s / V_out) - 1);

        return R_2;
    }

    float calculateTemp(uint16_t ADC_val) {

        float R_2 = calculateR2(ADC_val);

        // Define coefficients
        const int a = -1750;
        const int b = 0.1;
        const int c = 2212;

        float temp = a * std::pow(std::log(R_2), static_cast<float>(b)) + c;

        return temp;
    }
}

#endif