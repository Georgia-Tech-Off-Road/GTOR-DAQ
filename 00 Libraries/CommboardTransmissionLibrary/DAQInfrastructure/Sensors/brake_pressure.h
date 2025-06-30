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

        constexpr uint32_t AXIS_ENCODED_BIT_SIZE = 12;

        using AXIS_SV = boost::endian::big_uint32_t;

        constexpr uint32_t ENCODED_BIT_SIZE = AXIS_ENCODED_BIT_SIZE * 2;

        struct BrakePressure {
            AXIS_SV front;
            AXIS_SV rear;
        };

        using SV = BrakePressure;
        using RV = BrakePressure;

        inline void encodeBrakePressure(const SV& data, BinaryBuffer& buffer) {
            buffer.writeValue<AXIS_SV>(data.front, AXIS_ENCODED_BIT_SIZE);
            buffer.writeValue<AXIS_SV>(data.rear, AXIS_ENCODED_BIT_SIZE);
        }

        inline SV decodeBrakePressure(const BinaryBuffer& buffer) {
            BrakePressure data;

            data.front = buffer.readValue<AXIS_SV>(AXIS_ENCODED_BIT_SIZE);
            data.rear = buffer.readValue<AXIS_SV>(AXIS_ENCODED_BIT_SIZE);

            return data;
        }

        inline void serializeToJSON(const RV& val, std::stringstream& ss) {
            ss << "\"brakePressure\": {";
            ss <<  "\"front\": " << val.front << ", ";
            ss << "\"rear\": " << val.rear;
            ss << "}";
        }
        //Two should be the exact same
        using BRAKE_PRESSURE_SENSOR_INFO = SensorInfo<SV, RV, ENCODED_BIT_SIZE, encodeBrakePressure, decodeBrakePressure, defaultConvert<SV, RV>, serializeToJSON>;
    }
}

#endif