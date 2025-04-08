#include "../SensorInfo.h"
#include "../misc/BinaryBuffer/BinaryBuffer.h"
#include "../boost/endian/arithmetic.hpp"
#include "../boost/endian/conversion.hpp"
#include <cstdint>
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

		//The millisec sensor
		using MILLI_SEC_SENSOR_INFO = cmbtl::SensorInfo<SV, RV, ENCODED_BIT_SIZE>;
	}
}
#endif