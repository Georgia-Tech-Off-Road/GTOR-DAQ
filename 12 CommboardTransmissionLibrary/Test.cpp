#include <iostream>
#include "Sensors.h"
#include <cstdio>
int main() {
	boost::endian::big_uint32_t example = 0x01020304;
	boost::endian::little_uint32_t example_2 = 0x01020304;
	printf("Comparison: %x %x\n", example, example_2);
	std::bitset<32> encoded_example;
	cmbtl::misc::copyBits(&encoded_example, example);
	std::bitset<32> encoded_example_2;
	cmbtl::misc::copyBits(&encoded_example_2, example_2);
	printf("\nComparison 2: %x %x\n", encoded_example, encoded_example_2);
}