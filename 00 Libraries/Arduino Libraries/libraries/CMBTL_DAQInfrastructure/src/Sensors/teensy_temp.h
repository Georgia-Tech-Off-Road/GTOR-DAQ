#ifndef CMBTL_TEENSY_TEMP_H
#define CMBTL_TEENSY_TEMP_H
#include "SensorInfo.h"
#include "boost/endian/arithmetic.hpp"
#include "BinaryBuffer.h"
#include <string>
namespace cmbtl {
    namespace teensy_temp {
        constexpr uint32_t ENCODED_BIT_SIZE = 32;

        using SV = boost::endian::big_float32_t;
        using RV = float;

        std::string getName() {
            return "teensy_temp";
        }

        using TEENSY_TEMP_SENSOR_INFO = DefaultSensorWithName<SV, RV, ENCODED_BIT_SIZE, getName>;
    }
}
#endif