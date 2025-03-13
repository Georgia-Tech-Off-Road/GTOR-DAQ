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
		typedef boost::endian::little_uint32_t SV;
		//Encoded bitset
		typedef std::bitset<ENCODED_NUM_BITS> EV;
		//'Real Value' of millisec (same as stored)
		typedef uint32_t RV;

		inline EV encodeMillisec(SV stored_val) {
			//Initialize var
			EV encoded_val;
			cmbtl::misc::copyBits(&encoded_val, stored_val);
			return encoded_val;
		}

		inline SV decodeMillisec(EV encoded_val) {
			//Initialize var
			SV stored_val;
			cmbtl::misc::copyBits(&stored_val, encoded_val);
			return stored_val;
		}
		//Do no conversion, SV and RV already same type
		inline RV convert(SV stored_val) {
			uint32_t real_value = stored_val;
			return real_value;
		}

		//The millisec sensor
		constexpr cmbtl::SensorInfo<SV, EV, RV> MILLISEC_SENSOR = {encodeMillisec, decodeMillisec, convert};
	}
}
#endif