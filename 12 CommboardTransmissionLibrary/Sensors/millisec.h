#include "SensorInfo.h"
#include "misc/BinaryBuffer/BinaryBuffer.h"
#include "boost/endian/arithmetic.hpp"
#include "boost/endian/conversion.hpp"
#include <cstdint>
#include <bitset>
#ifndef CMBTL_MILLIS_H
#define CMBTL_MILLIS_H
namespace cmbtl {
	namespace millisec {
		//Number of bits that its encoded value should store
		constexpr uint32_t ENCODED_BIT_SIZE = 26;

		//Stored value of millisec in sensor data struct
		using SV = boost::endian::big_uint32_t;

		//'Real Value' of millisec
		using RV = uint32_t;

		inline void encodeMillisec(SV value, BinaryBuffer& buffer) {
			buffer.writeValue(value, ENCODED_BIT_SIZE);
		}

		inline SV decodeMillisec(const BinaryBuffer& buffer) {
			return buffer.readValue<SV>(ENCODED_BIT_SIZE);
		}

		inline RV convert(SV stored_val) {
			uint32_t real_value = stored_val;
			return real_value;
		}

		//The millisec sensor
		using MILLI_SEC_SENSOR_INFO = cmbtl::SensorInfo<SV, RV, ENCODED_BIT_SIZE, encodeMillisec, decodeMillisec, convert>;
	}
}
#endif