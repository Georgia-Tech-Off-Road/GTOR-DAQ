#include "SensorInfo.h"
#include "../boost/endian/arithmetic.hpp"
#include "../misc/BinaryBuffer/BinaryBuffer.h"
#ifndef CMBTL_STEERING_ROTATION_H
#define CMBTL_STEERING_ROTATION_H

namespace cmbtl {
    namespace steering_rotation {
        constexpr uint32_t ENCODED_BIT_SIZE = 16;
        
        constexpr uint8_t DECIMAL_BITS = 7;

        using SV = float;
        using RV = float;

        using STEERING_ROTATION_SENSOR_INFO = SensorInfo<SV, RV, ENCODED_BIT_SIZE, 
        unsignedFloatEncode<ENCODED_BIT_SIZE, DECIMAL_BITS>, 
        unsignedFloatDecode<ENCODED_BIT_SIZE, DECIMAL_BITS>>;

    }
}

#endif