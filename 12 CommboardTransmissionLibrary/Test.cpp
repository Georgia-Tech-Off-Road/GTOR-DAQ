#include <iostream>
#include "Sensors.h"
#include <cstdio>
#include "misc/BinaryBuffer.h"
int main() {
	FILE* fi = fopen("test_output.dat", "wb");
	uint32_t test = 0xAAAAFFFF;
	boost::endian::big_uint32_t example = 0xAAAAFFFF;
	boost::endian::little_uint32_t example_2 = 0xAAAAFFFF;
	printf("Comparison 1: %x %xs\n", *(&example), *(&example_2));
	std::bitset<32> encoded_example;
	cmbtl::misc::copyBits(&encoded_example, example);
	std::bitset<32> encoded_example_2;
	cmbtl::misc::copyBits(&encoded_example_2, example_2);
	printf("\nComparison 2: %x %x\n", encoded_example, encoded_example_2);
}

void testBinaryBuffer() {
	
}