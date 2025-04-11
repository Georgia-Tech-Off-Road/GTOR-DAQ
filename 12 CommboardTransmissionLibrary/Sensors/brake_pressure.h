#include "../SensorInfo.h"
#include "../misc/BinaryBuffer/BinaryBuffer.h"
#include "../boost/endian/arithmetic.hpp"
#include <cstdint>

#ifndef CMBLT_BRAKE_PRESSURE_H
#define CMBTL_BRAKE_PRESSURE_H

namespace cmbtl {
    namespace brake_pressure {
        constexpr uint32_t ENCODED_BIT_SIZE = 12;

        using SV = boost::endian::big_uint32_t;
        using RV = boost::endian::big_float32_t;

        //Two should be the exact same
        using BRAKE_PRESSURE_FRONT_SENSOR_INFO = SensorInfo<SV, RV, ENCODED_BIT_SIZE>;
        using BRAKE_PRESSURE_BACK_SENSOR_INFO = SensorInfo<SV, RV, ENCODED_BIT_SIZE>;
    }
}

#endif