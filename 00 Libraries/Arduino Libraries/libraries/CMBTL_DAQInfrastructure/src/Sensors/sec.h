#ifndef CMBTL_SECS_H
#define CMBTL_SECS_H

// Include Headers
#include "SensorInfo.h"
#include "BinaryBuffer.h"
#include "boost/endian/arithmetic.hpp"
#include <cstdint>
#include <string>
#include <sstream>

namespace cmbtl {
    namespace sec {
        // Number of bits that the sec sensor will need to store
        constexpr uint32_t ENCODED_BIT_SIZE = 64;

        // Stored value of the sec sensor
        using SV = boost::endian::big_uint64_t;

        // 'Real' value of the sec sensor
        using RV = uint64_t;

        inline void serializeTOJSON(const RV& convertedVal, std::stringstream &ss) {
            ss << "\"sec\": " << convertedVal;
        }

        // The sec sensor
        using SEC_SENSOR_INFO = DefaultSensor<SV, RV, ENCODED_BIT_SIZE, serializeTOJSON>;
    }
}

#endif