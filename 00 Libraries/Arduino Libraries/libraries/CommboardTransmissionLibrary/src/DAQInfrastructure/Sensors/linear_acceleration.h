#include "BinaryBuffer.h"
#include "boost/endian/arithmetic.hpp"
#include "../../SensorInfo.h"
#include <string>
#include <sstream>

namespace cmbtl {
    namespace linear_acceleration {
        
        constexpr uint32_t AXIS_BIT_SIZE = 20;

        constexpr uint32_t AXES_COUNT = 3;

        //Total encoded size of the entire struct
        constexpr uint32_t ENCODED_BIT_SIZE = AXES_COUNT * AXIS_BIT_SIZE;

        constexpr uint32_t DECIMAL_BITS = 6;

        using axis_type = float;

        struct linear_acceleration {
            axis_type x;
            axis_type y;
            axis_type z;
        };

        using SV = linear_acceleration;

        using RV = linear_acceleration;

        void accelerationEncode(const SV& val, BinaryBuffer& buffer) {
            unsignedFloatEncode<AXIS_BIT_SIZE, DECIMAL_BITS>(val.x, buffer);
            unsignedFloatEncode<AXIS_BIT_SIZE, DECIMAL_BITS>(val.y, buffer);
            unsignedFloatEncode<AXIS_BIT_SIZE, DECIMAL_BITS>(val.z, buffer);
        }

        SV decodeAcceleration (const BinaryBuffer& buffer) {
            linear_acceleration val;

            val.x = unsignedFloatDecode<AXIS_BIT_SIZE, DECIMAL_BITS>(buffer);
            val.y = unsignedFloatDecode<AXIS_BIT_SIZE, DECIMAL_BITS>(buffer);
            val.z = unsignedFloatDecode<AXIS_BIT_SIZE, DECIMAL_BITS>(buffer);

            return val;
        }

        void serializeToJSON(const RV& val, std::stringstream& ss) {
            ss << "\"linearAcceleration\": {\"x\": " << val.x << ", " << "\"y\": " << val.y << ", " << "\"z\": " << val.z << "}";
        }

        using LINEAR_ACCELERATION_SENSOR_INFO = SensorInfo<SV, RV, ENCODED_BIT_SIZE, accelerationEncode, decodeAcceleration, defaultConvert<SV, RV>, serializeToJSON>;

    }
}