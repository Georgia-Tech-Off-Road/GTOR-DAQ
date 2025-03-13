#include "../SensorInfo.h"
#include "../misc/CopyBits.h"
#include "../boost/endian/arithmetic.hpp"
#include "../boost/endian/conversion.hpp"
#include <cstdint>
#include <bitset>
#ifndef CMBTL_MILLIS_H
#define CMBTL_MILLIS_H
namespace cmbtl {
	namespace millisec {
		//Number of bits that its encoded value should store
		constexpr size_t ENCODED_NUM_BITS = 26;
		//Stored value of millisec in sensor data struct
		typedef boost::endian::big_uint32_t millisecSV;
		//Encoded bitset
		typedef std::bitset<ENCODED_NUM_BITS> millisecEV;
		//'Real Value' of millisec (same as stored)
		typedef uint32_t millisecRV;

		inline millisecEV encodeMillisec(millisecSV stored_val) {
			//Initialize var
			millisecEV encoded_val;
			cmbtl::misc::copyBits(&encoded_val, stored_val);
			return encoded_val;
		}

		inline millisecSV decodeMillisec(millisecEV encoded_val) {
			//Initialize var
			millisecSV stored_val;
			cmbtl::misc::copyBits(&stored_val, encoded_val);
			return stored_val;
		}
		//Do no conversion, SV and RV already same type
		inline millisecRV convert(millisecSV stored_val) {
			return boost::endian::big_to_native(stored_val);
		}

		//The millisec sensor
		constexpr cmbtl::SensorInfo<millisecSV, millisecEV, millisecRV> MILLISEC_SENSOR = {encodeMillisec, decodeMillisec, convert};
	}
}
#endif