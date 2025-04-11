#include "../SensorInfo.h"
#include "../boost/endian/arithmetic.hpp"
#include "../misc/BinaryBuffer/BinaryBuffer.h"
#ifndef CMBTL_CVT_TEMP_H
#define CMBTL_CVT_TEMP_H

namespace cmbtl {
    namespace cvt_temp {
        constexpr uint32_t ENCODED_BIT_SIZE = 10;

        using SV = boost::endian::big_uint16_t;
        using RV = uint16_t;

        using CVT_TEMP_SENSOR_INFO = SensorInfo<SV, RV, ENCODED_BIT_SIZE>;
    }
}

#endif