#include "BinaryBuffer/BinaryBuffer.h"
#include "../../SensorInfo.h"
#include "boost/endian/arithmetic.hpp"
#include <cstdint>
#ifndef CMBTL_POSITION_H
#define CMBTL_POSITION_H
namespace cmbtl {
    namespace position {
        // x, y, z
        constexpr uint32_t AXIS_COUNT = 3;

        //Bit size needed per axis
        constexpr uint32_t AXIS_BIT_SIZE = 32;

        // Total encoded bit size for the entire position struct
        constexpr uint32_t ENCODED_BIT_SIZE = AXIS_COUNT * AXIS_BIT_SIZE;

        using position_axis_type = boost::endian::big_float32_t;

        struct Position {
            position_axis_type x;
            position_axis_type y;
            position_axis_type z;
        };

        inline void encodePosition(const Position& val, BinaryBuffer& buffer) {
            buffer.writeValue<position_axis_type>(val.x, AXIS_BIT_SIZE);
            buffer.writeValue<position_axis_type>(val.y, AXIS_BIT_SIZE);
            buffer.writeValue<position_axis_type>(val.z, AXIS_BIT_SIZE);
        }

        inline Position decodePosition(const BinaryBuffer& buffer) {
            Position position;

            position.x = buffer.readValue<position_axis_type>(AXIS_BIT_SIZE);
            position.y = buffer.readValue<position_axis_type>(AXIS_BIT_SIZE);
            position.z = buffer.readValue<position_axis_type>(AXIS_BIT_SIZE);

            return position;
        }

        inline void serializeToJSON(const Position& val, std::stringstream& ss) {
            ss << "\"position\": {\"x\": " << val.x << ", " << "\"y\": " << val.y << ", " << "\"z\": " << val.z << "}";
        }

        using POSITION_SENSOR_INFO = SensorInfo<Position, Position, ENCODED_BIT_SIZE, encodePosition, decodePosition, defaultConvert<Position, Position>, serializeToJSON>;

    }
}
#endif