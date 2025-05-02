#ifndef TC_TEMP_CONVERT_H
#define TC_TEMP_CONVERT_H
#include <functional>
#include <cstdint>
#include <cmath>

namespace TCTemp {
    inline float calculateR2(float V_s, float V_out, float R_1) {
        float R_2 = ((V_out / V_s) * R_1) / (1 - (V_out / V_s));

        return R_2;
    }

    inline float calculateTemp(float V_out) {
        // Define constants
        const float V_S = 5;
        const float R_1 = 2200;

        float R_2 = calculateR2(V_S, V_out, R_1);
 
        // Define coefficients
        const int a = -1750;
        const int b = 0.1;
        const int c = 2212;

        float temp = a * std::pow(std::log(R_2), static_cast<float>(b)) + c;

        return temp;
    }
}

#endif