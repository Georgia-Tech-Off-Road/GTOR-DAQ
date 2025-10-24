#ifndef RPM_SENSOR_H
#define RPM_SENSOR_H
#include "SensorInfo.h"
#include "boost/endian/arithmetic.hpp"
#include "BinaryBuffer.h"
#include <string>

// Generic RPM Sensor Class
namespace cmbtl {
    namespace rpm_sensor {
        constexpr uint32_t ENCODED_BIT_SIZE = 32;

        using SV = boost::endian::big_uint32_t;
        using RV = uint32_t;

        template<std::string (*getName)()>
        using RPM_SENSOR = DefaultSensorWithName<SV, RV, ENCODED_BIT_SIZE, getName>;
    }
}
#endif