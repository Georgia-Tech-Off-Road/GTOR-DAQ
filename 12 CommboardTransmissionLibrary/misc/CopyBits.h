#include <cstring>
#include <bitset>
#include <assert.h>

#ifndef CMBTL_COPY_BITS_H
#define CMBTL_COPY_BITS_H
namespace cmbtl::misc {
	//SV: Stored Value
	template <std::size_t N, typename SV>
	void copyBits (std::bitset<N> *dest, SV src) {
		assert((sizeof(src) * 8) >= N && "Your encoded value takes more bits than the size of your stored value!");
		//Clear all bits
		dest->reset();
		for (uint16_t i = 0; i < dest->size(); i++) {
			//ith bit in src is 1
			if (((src >> i) & 0x1) == 1) {
				//Set bit
				(*dest)[i] = 1;
			}
		}
	}
	//SV: Stored Value
	template <std::size_t N, typename SV>
	void copyBits(SV* dest, std::bitset<N> src) {
		assert(sizeof((*dest)) * 8 >= N && "Your encoded value takes more bits than the size of your stored value!");
		//Clear all bits in dest
		*dest = 0;
		for (std::size_t i = 0; i < src.size(); i++) {
			//Set bits
			*dest = (static_cast<SV>(src[i]) << i);
		}
	}
}
#endif
