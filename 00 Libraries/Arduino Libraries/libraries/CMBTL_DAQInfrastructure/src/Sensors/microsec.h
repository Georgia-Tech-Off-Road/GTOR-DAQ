#include "SensorInfo.h"
#include "BinaryBuffer.h"
#include "boost/endian/arithmetic.hpp"
#include <cstdint>
#include <string>
#include <sstream>
#ifndef CMBTL_MICROS_H
#define CMBTL_MICROS_H
namespace cmbtl {
	namespace microsec {
		//Number of bits that its encoded value should store
		constexpr uint32_t ENCODED_BIT_SIZE = 64;

		//Stored value of millisec in sensor data struct
		using SV = boost::endian::big_uint64_t;

		//'Real Value' of millisec
		using RV = uint64_t;

		inline void serializeToJSON(const RV& convertedVal, std::stringstream& ss) {
			ss << "\"microsec\": " << convertedVal;
		}

		//The millisec sensor
		using MICRO_SEC_SENSOR_INFO = DefaultSensor<SV, RV, ENCODED_BIT_SIZE, serializeToJSON>;
	}
}
#endif