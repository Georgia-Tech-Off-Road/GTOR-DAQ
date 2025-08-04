#include "boost/endian/arithmetic.hpp"
#include "BinaryBuffer/BinaryBuffer.h"
#include "../../SensorInfo.h"
#include <cstdint>
#include <string>
#include <sstream>

#ifndef CMBTL_SPEED_H
#define CMBTL_SPEED_H
namespace cmbtl {
    namespace speed {
        using speed_axis_type = boost::endian::big_float32_t;

        //x, y, z
        constexpr uint32_t AXES_COUNT = 3;

        constexpr uint32_t AXIS_ENCODED_BIT_SIZE = 32;

        //Total size of the entire Speed data structure
        constexpr uint32_t ENCODED_BIT_SIZE = AXIS_ENCODED_BIT_SIZE * AXES_COUNT;

        struct Speed {
            speed_axis_type x;
            speed_axis_type y;
            speed_axis_type z;
        };
        static_assert(sizeof(Speed) == AXES_COUNT * sizeof(speed_axis_type), "Struct sizing is not correct!");

        inline void encodeSpeed(const Speed &val, BinaryBuffer &buffer) {
            buffer.writeValue<speed_axis_type>(val.x, AXIS_ENCODED_BIT_SIZE);
            buffer.writeValue<speed_axis_type>(val.y, AXIS_ENCODED_BIT_SIZE);
            buffer.writeValue<speed_axis_type>(val.z, AXIS_ENCODED_BIT_SIZE);
        }

        inline Speed decodeSpeed(const BinaryBuffer &buffer) {
            Speed data;
            data.x = buffer.readValue<speed_axis_type>(AXIS_ENCODED_BIT_SIZE);
            data.y = buffer.readValue<speed_axis_type>(AXIS_ENCODED_BIT_SIZE);
            data.z = buffer.readValue<speed_axis_type>(AXIS_ENCODED_BIT_SIZE);

            return data;
        }

        inline void serializeToJSON(const Speed& val, std::stringstream& ss) {
            ss << "\"speed\": {\"x\": " << val.x << ", " << "\"y\": " << val.y << ", " << "\"z\": " << val.z << "}";
        }

        using SPEED_SENSOR_INFO = SensorInfo<Speed, Speed, ENCODED_BIT_SIZE, encodeSpeed, decodeSpeed, defaultConvert<Speed, Speed>, serializeToJSON>;
    }
}
#endif