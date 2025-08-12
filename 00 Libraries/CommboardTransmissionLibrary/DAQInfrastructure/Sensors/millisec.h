#include "../../SensorInfo.h"
#include "BinaryBuffer.h"
#include "boost/endian/arithmetic.hpp"
#include <cstdint>
#include <string>
#include <sstream>
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

		inline void serializeToJSON(const RV& convertedVal, std::stringstream& ss) {
			ss << "\"millisec\": " << convertedVal;
		}

		//The millisec sensor
		using MILLI_SEC_SENSOR_INFO = DefaultSensor<SV, RV, ENCODED_BIT_SIZE, serializeToJSON>;
	}
}
#endif