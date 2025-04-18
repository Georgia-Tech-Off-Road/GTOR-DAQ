#include "../../SensorInfo.h"
#include "../../boost/endian/arithmetic.hpp"
#include "../../misc/BinaryBuffer/BinaryBuffer.h"

#ifndef CMBTL_BRAKE_TEMP_H
#define CMBTL_BRAKE_TEMP_H

namespace cmbtl {
    namespace brake_temp {
        constexpr uint32_t ENCODED_BIT_SIZE = 16;

        using SV = boost::endian::big_uint16_t;
        using RV = uint16_t;

        using BRAKE_TEMP_FRONT_LEFT_SENSOR_INFO = SensorInfo<SV, RV, ENCODED_BIT_SIZE>;
        using BRAKE_TEMP_FRONT_RIGHT_SENSOR_INFO = SensorInfo<SV, RV, ENCODED_BIT_SIZE>;
        using BRAKE_TEMP_BACK_SENSOR_INFO = SensorInfo<SV, RV, ENCODED_BIT_SIZE>;
    }
}

#endif