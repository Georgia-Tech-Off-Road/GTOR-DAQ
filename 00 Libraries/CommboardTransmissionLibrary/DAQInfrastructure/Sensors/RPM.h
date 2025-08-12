#include "../../SensorInfo.h"
#include "boost/endian/arithmetic.hpp"
#include "BinaryBuffer.h"
#include <cmath>
#include <string>
#include <sstream>

#ifndef CMBLT_RPM_H
#define CMBTL_RPM_H
namespace cmbtl {
    namespace rpm {
        using cmbtl::unsignedFloatDecode;
        using cmbtl::unsignedFloatEncode;
        constexpr uint32_t AXIS_ENCODED_BIT_SIZE = 16;

        constexpr uint8_t DECIMAL_BITS = 3;

        constexpr uint32_t ENCODED_BIT_SIZE = AXIS_ENCODED_BIT_SIZE * 3;

        using AXIS_SV = float;
        using AXIS_RV = float;

        struct RPM {
            AXIS_SV left;
            AXIS_SV right;
            AXIS_RV rear;
        };

        using SV = RPM;
        using RV = RPM;

        inline void encodeRPM(const SV& data, BinaryBuffer& buffer) {
            unsignedFloatEncode<AXIS_ENCODED_BIT_SIZE, DECIMAL_BITS>(data.left, buffer);
            unsignedFloatEncode<AXIS_ENCODED_BIT_SIZE, DECIMAL_BITS>(data.right, buffer);
            unsignedFloatEncode<AXIS_ENCODED_BIT_SIZE, DECIMAL_BITS>(data.rear, buffer);
        }

        inline SV decodeRPM(const BinaryBuffer& buffer) {
            SV data;

            data.left = unsignedFloatDecode<AXIS_ENCODED_BIT_SIZE, DECIMAL_BITS>(buffer);
            data.right = unsignedFloatDecode<AXIS_ENCODED_BIT_SIZE, DECIMAL_BITS>(buffer);
            data.rear = unsignedFloatDecode<AXIS_ENCODED_BIT_SIZE, DECIMAL_BITS>(buffer);

            return data;
        }

        inline void serializeToJSON(const RV& convertedVal, std::stringstream& ss) {
            ss << "\"RPM\": " << "{\"left\": " << convertedVal.left << ", ";
            ss << "\"right\": " << convertedVal.right << ", ";
            ss << "\"rear\": " << convertedVal.rear << "}";
        }



        using RPM_SENSOR_INFO = SensorInfo<SV, RV, ENCODED_BIT_SIZE, encodeRPM, decodeRPM, defaultConvert<SV, RV>, serializeToJSON>;
    }
}

#endif 