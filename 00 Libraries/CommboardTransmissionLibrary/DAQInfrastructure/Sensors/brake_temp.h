#include "../../SensorInfo.h"
#include "boost/endian/arithmetic.hpp"
#include "BinaryBuffer.h"
#include <string>
#include <sstream>

#ifndef CMBTL_BRAKE_TEMP_H
#define CMBTL_BRAKE_TEMP_H

namespace cmbtl {
    namespace brake_temp {
        constexpr uint32_t AXIS_ENCODED_BIT_SIZE = 16;

        using AXIS_SV = boost::endian::big_uint16_t;

        constexpr uint32_t ENCODED_BIT_SIZE = AXIS_ENCODED_BIT_SIZE * 3;

        struct BrakeTemp {
            AXIS_SV front_left;
            AXIS_SV front_right;
            AXIS_SV rear;
        };

        using SV = BrakeTemp;
        using RV = BrakeTemp;

        inline void encodeBrakeTemp(const SV& val, BinaryBuffer& buffer) {
            buffer.writeValue<AXIS_SV>(val.front_left, AXIS_ENCODED_BIT_SIZE);
            buffer.writeValue<AXIS_SV>(val.front_right, AXIS_ENCODED_BIT_SIZE);
            buffer.writeValue<AXIS_SV>(val.rear, AXIS_ENCODED_BIT_SIZE);
        }

        inline SV decodeBrakeTemp(const BinaryBuffer& buffer) {
            BrakeTemp data;

            data.front_left = buffer.readValue<AXIS_SV>(AXIS_ENCODED_BIT_SIZE);
            data.front_right = buffer.readValue<AXIS_SV>(AXIS_ENCODED_BIT_SIZE);
            data.rear = buffer.readValue<AXIS_SV>(AXIS_ENCODED_BIT_SIZE);

            return data;
        }

        inline void serializeToJSON(const RV& val, std::stringstream& ss) {
            ss << "\"brakeTemp\": " << "{";
            ss << "\"frontLeft\": " << val.front_left << ", ";
            ss << "\"frontRight\": " << val.front_right << ", ";
            ss << "\"rear\": " << val.rear;
            ss << "}";
        }

        using BRAKE_TEMP_SENSOR_INFO = SensorInfo<SV, RV, ENCODED_BIT_SIZE, encodeBrakeTemp, decodeBrakeTemp, defaultConvert<SV, RV>, serializeToJSON>;
    }
}

#endif