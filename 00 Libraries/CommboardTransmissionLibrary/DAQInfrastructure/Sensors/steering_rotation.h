#include "../../SensorInfo.h"
#include "boost/endian/arithmetic.hpp"
#include "BinaryBuffer/BinaryBuffer.h"
#include <string>
#include <sstream>
#ifndef CMBTL_STEERING_ROTATION_H
#define CMBTL_STEERING_ROTATION_H

namespace cmbtl {
    namespace steering_rotation {
        constexpr uint32_t ENCODED_BIT_SIZE = 16;
        
        constexpr uint8_t DECIMAL_BITS = 7;

        using SV = float;
        using RV = float;

        void serializeToJSON(const RV& convertedVal, std::stringstream& ss) {
            ss << "\"steeringAngle\": " << convertedVal;
        }

        using STEERING_ROTATION_SENSOR_INFO = SensorInfo<SV, RV, ENCODED_BIT_SIZE, 
        unsignedFloatEncode<ENCODED_BIT_SIZE, DECIMAL_BITS>, 
        unsignedFloatDecode<ENCODED_BIT_SIZE, DECIMAL_BITS>,
        defaultConvert<SV, RV>,
        serializeToJSON>;

    }
}

#endif