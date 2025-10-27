#ifndef LINEAR_ANALOG_SENSOR_H
#define LINEAR_ANALOG_SENSOR_H
#include "SensorInfo.h"
#include "boost/endian/arithmetic.hpp"
#include "BinaryBuffer.h"
#include <string>

// Generic RPM Sensor Class
namespace cmbtl {
    namespace linear_analog_sensor {
        constexpr uint32_t ENCODED_BIT_SIZE = 32;

        using SV = boost::endian::big_float32_t;
        using RV = float;

        template<std::string (*getName)()>
        using Linear_Analog_Sensor = DefaultSensorWithName<SV, RV, ENCODED_BIT_SIZE, getName>;
    }
}
#endif