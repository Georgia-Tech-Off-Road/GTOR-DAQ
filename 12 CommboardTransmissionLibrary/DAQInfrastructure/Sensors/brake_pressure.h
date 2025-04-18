#include "../../SensorInfo.h"
#include "../../misc/BinaryBuffer/BinaryBuffer.h"
#include "../../boost/endian/arithmetic.hpp"
#include <string>
#include <sstream>
#include <cstdint>

#ifndef CMBLT_BRAKE_PRESSURE_H
#define CMBTL_BRAKE_PRESSURE_H

namespace cmbtl {
    namespace brake_pressure {
        constexpr uint32_t ENCODED_BIT_SIZE = 12;

        using SV = boost::endian::big_uint32_t;
        using RV = boost::endian::big_float32_t;

        void serializeFrontToJSON(const RV& convertedVal, std::stringstream& ss) {
            ss << "\"brakePressureFront\": " << convertedVal;
        }

        void serializeBackToJSON(const RV& convertedVal, std::stringstream& ss) {
            ss << "\"brakePressureBack\": " << convertedVal;
        }

        //Two should be the exact same
        using BRAKE_PRESSURE_FRONT_SENSOR_INFO = DefaultSensor<SV, RV, ENCODED_BIT_SIZE, serializeFrontToJSON>;
        using BRAKE_PRESSURE_BACK_SENSOR_INFO = DefaultSensor<SV, RV, ENCODED_BIT_SIZE, serializeBackToJSON>;
    }
}

#endif