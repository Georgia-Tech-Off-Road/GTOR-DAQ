#ifndef ANALOG_SENSOR_H
#define ANALOG_SENSOR_H
#include "SensorInfo.h"
#include "boost/endian/arithmetic.hpp"
#include "BinaryBuffer.h"
#include <string>

// Generic Analog Sensor Class
namespace cmbtl {
    namespace analog_sensor {
        constexpr uint32_t ENCODED_BIT_SIZE = 16;

        using SV = boost::endian::big_uint16_t;
        using RV = uint16_t;

        template <std::string (*getName)()>
        using ANALOG_SENSOR = DefaultSensorWithName<SV, RV, ENCODED_BIT_SIZE, getName>;
    }
}
#endif