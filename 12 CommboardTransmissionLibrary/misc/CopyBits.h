#include <cstring>
#include <bitset>
#include <assert.h>
#include <iostream>

#ifndef CMBTL_COPY_BITS_H
#define CMBTL_COPY_BITS_H
namespace cmbtl {
	namespace misc {
		//SV: Stored Value
		template <std::size_t N, typename SV>
		void copyBits(std::bitset<N>* dest, SV src) {
			assert((sizeof(src) * 8) >= N && "Your encoded value takes more bits than the size of your stored value!");
			//Clear all bits
			dest->reset();
			//Create a buffer to copy the bits directly to.
			//We can't use >> operators to shift and get bits from the SV because the >> is overloaded for
			//both big and little endian integers and floats. Basically bigEndian >> 1 and littleEndian >> 1 will shift
			//towards the least significant bit. Great for most things, but we just want to copy bits! So we need to use a buffer.
			unsigned char buffer[sizeof(src) / sizeof(char)];
			memcpy(buffer, &src, sizeof(src)); 
			for (uint16_t i = dest->size() - 1; i > 0; i--) {
				//First get which char our desired bit is in, then bit shifts to place the bit in the least significant position,
				// and finally masks out every other bit.
				int bufferIndex = (i / (sizeof(char) * 8));
				int shift_ammount = ((-i + (dest->size() - 1)) % (sizeof(char) * 8));
				uint8_t bit = buffer[bufferIndex] >> shift_ammount & 0x1;
				if (bit == 1) {
					//Set bit
					(*dest)[-i + (dest->size() - 1)] = 1;
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
}
#endif
