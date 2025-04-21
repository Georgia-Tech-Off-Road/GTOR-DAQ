#include "../../SensorInfo.h"
#include "../../boost/endian/arithmetic.hpp"
#include "../../misc/BinaryBuffer/BinaryBuffer.h"
#include <string>
#include <sstream>
#ifndef CMBTL_CVT_TEMP_H
#define CMBTL_CVT_TEMP_H

namespace cmbtl {
    namespace cvt_temp {
        constexpr uint32_t ENCODED_BIT_SIZE = 10;

        using SV = boost::endian::big_uint16_t;
        using RV = uint16_t;

        inline void serializeToJSON(const RV& convertedVal, std::stringstream& ss) {
            ss << "\"CVTTemp: \"" << convertedVal;
        }

        using CVT_TEMP_SENSOR_INFO = DefaultSensor<SV, RV, ENCODED_BIT_SIZE, serializeToJSON>;
    }
}

#endif