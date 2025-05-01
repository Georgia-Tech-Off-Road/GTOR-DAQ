#ifndef TC_TEMP_CONVERT_H
#define TC_TEMP_CONVERT_H
#include <cstdint>
#include <cmath>

namespace TCTemp {

    /**
     * @brief Converts value from the ADC to mV
     * 
     * @param ADC_val: Value retrieved from the ADC (Currently 15 bits)
     * 
     * @returns converted value in mV
     */
    inline float convertToVolts(uint16_t ADC_val) {
        // Maximum value is 0111 1111 1111 1111 (15 bits) = 2^16 - 1 - 2^15
        const uint16_t MAX_ADC_VALUE = 32767;

        // Maximum voltage output from sensor is 5 V
        const float MAX_VOLTAGE = 6.144f;

        // Minimu voltage output from sensor is 0 V
        const float MIN_VOLTAGE = 0.0f;

        return static_cast<float>(ADC_val) * ((MAX_VOLTAGE - MIN_VOLTAGE) / static_cast<float>(MAX_ADC_VALUE));
    }

    inline float calculateR2(float V_s, float V_out, float R_1) {
        float R_2 = ((V_out / V_s) * R_1) / (1 - (V_out / V_s));

        return R_2;
    }

    inline float calculateTemp(uint16_t ADC_val) {

        // Define constants
        const float V_S = 5;
        const float R_1 = 2200;
        
        float V_out = convertToVolts(ADC_val);
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