#ifndef TC_TEMP_CONVERT_H
#define TC_TEMP_CONVERT_H
#include <cstdint>

namespace CVTBeltTemp {

    /**
     * @brief Converts ADC value of CVTTemp sensor to temperature value (°C)
     * 
     * @param ADC_val: Value retrieved from the ADC (currently 15 bits)
     * 
     * @returns temperature of the tranfer case (°C)
     */
    inline float calculateTemp(uint16_t ADC_val) {

    }

    /**
     * @brief Converts value from the ADC to mV
     * 
     * @param ADC_val: Value retrieved from the ADC (Currently 15 bits)
     * 
     * @returns converted value in mV
     */
    inline float convertToMilliVolts(uint16_t ADC_val) {
        // Maximum value is 0111 1111 1111 1111 (15 bits) = 2^16 - 1 - 2^15
        const uint16_t MAX_ADC_VALUE = 32767;

        // Maximum voltage output from sensor is 5000 mV
        const float MAX_VOLTAGE = 5000.0f;

        // Minimu voltage output from sensor is 0 mV
        const float MIN_VOLTAGE = 0.0f;

        return static_cast<float>(ADC_val) * ((MAX_VOLTAGE - MIN_VOLTAGE) / static_cast<float>(MAX_ADC_VALUE));
    }
}

#endif