#include "BinaryBuffer.h"
#include "gtest/gtest.h"
#include "boost/endian/arithmetic.hpp"

using BinaryBuffer = cmbtl::BinaryBuffer;

using namespace boost::endian;

//--------------- CONSTRUCTOR TESTS --------------------
TEST(BinaryBufferTests, Initialize) {
    BinaryBuffer buffer(8);
    ASSERT_EQ(buffer.getCapacity(), 8);
    ASSERT_EQ(buffer.getBitsWritten(), 0);
    ASSERT_EQ(buffer.getBitsRead(), 0);
    ASSERT_EQ(buffer.getWriteCursorPos(), 0);
    ASSERT_EQ(buffer.getReadCursorPos(), 0);
    ASSERT_EQ(buffer.getSize(), 1);
    unsigned char temp[] = {0x0};
    ASSERT_EQ(*(buffer.getBuffer()), *temp);
}

TEST(BinaryBufferTests, InitializeLessThan8Bits) {
    BinaryBuffer buffer(1);
    ASSERT_EQ(buffer.getCapacity(), 1);
    ASSERT_EQ(buffer.getBitsWritten(), 0);
    ASSERT_EQ(buffer.getBitsRead(), 0);
    ASSERT_EQ(buffer.getWriteCursorPos(), 0);
    ASSERT_EQ(buffer.getReadCursorPos(), 0);
    ASSERT_EQ(buffer.getSize(), 1);
    unsigned char temp[] = {0x0};
    ASSERT_EQ(*(buffer.getBuffer()), *temp);
}

TEST(BinaryBufferTests, InitializeMoreThan8Bits) {
    BinaryBuffer buffer(9);
    ASSERT_EQ(buffer.getCapacity(), 9);
    ASSERT_EQ(buffer.getBitsWritten(), 0);
    ASSERT_EQ(buffer.getBitsRead(), 0);
    ASSERT_EQ(buffer.getWriteCursorPos(), 0);
    ASSERT_EQ(buffer.getReadCursorPos(), 0);
    ASSERT_EQ(buffer.getSize(), 2);
    unsigned char temp[] = {0x0, 0x0};
    const unsigned char* rb = buffer.getBuffer();
    ASSERT_EQ(rb[0], temp[0]);
    ASSERT_EQ(rb[1], temp[1]);
}

TEST(BinaryBufferTests, InitializeManyBits) {
    BinaryBuffer buffer(64);
    ASSERT_EQ(buffer.getCapacity(), 64);
    ASSERT_EQ(buffer.getBitsWritten(), 0);
    ASSERT_EQ(buffer.getBitsRead(), 0);
    ASSERT_EQ(buffer.getWriteCursorPos(), 0);
    ASSERT_EQ(buffer.getReadCursorPos(), 0);
    ASSERT_EQ(buffer.getSize(), 8);
    unsigned char temp[] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    const unsigned char* rb = buffer.getBuffer();
    for(int i = 0; i < 8; i++) {
        ASSERT_EQ(rb[i], temp[i]);
    }
}

// ------------------------- BASIC FUNCTIONS TESTS -------------------------
//The unit tests in this section were written by AI but were reviewed and when needed, modified by the author

//---------------- TEST clearBuffer() -----------------------
TEST(BinaryBufferTests, ClearBuffer) {
    BinaryBuffer buffer(32);
    
    // Write some bits
    buffer.write(1);
    buffer.write(1);
    buffer.write(0);
    buffer.write(1);
    
    // Read some bits
    buffer.readBit();
    buffer.readBit();
    
    ASSERT_EQ(buffer.getBitsWritten(), 4);
    ASSERT_EQ(buffer.getWriteCursorPos(), 4);
    ASSERT_EQ(buffer.getBitsRead(), 2);
    ASSERT_EQ(buffer.getReadCursorPos(), 2);
    
    // Clear the buffer
    buffer.clearBuffer();
    
    // Verify everything is reset
    ASSERT_EQ(buffer.getBitsWritten(), 0);
    ASSERT_EQ(buffer.getWriteCursorPos(), 0);
    ASSERT_EQ(buffer.getBitsRead(), 0);
    ASSERT_EQ(buffer.getReadCursorPos(), 0);
    
    // Check buffer contents
    const unsigned char* buf = buffer.getBuffer();
    for (size_t i = 0; i < buffer.getSize(); i++) {
        ASSERT_EQ(buf[i], 0);
    }
}

//---------------- TEST getBit() -----------------------
TEST(BinaryBufferTests, GetBit) {
    BinaryBuffer buffer(16);
    
    // Write pattern 1010 0001 to first byte
    buffer.writeBit(1, 0);
    buffer.writeBit(0, 1);
    buffer.writeBit(1, 2);
    buffer.writeBit(0, 3);
    buffer.writeBit(0, 4);
    buffer.writeBit(0, 5);
    buffer.writeBit(0, 6);
    buffer.writeBit(1, 7);
    
    // Check bits
    ASSERT_EQ(buffer.getBit(0), 1);
    ASSERT_EQ(buffer.getBit(1), 0);
    ASSERT_EQ(buffer.getBit(2), 1);
    ASSERT_EQ(buffer.getBit(3), 0);
    ASSERT_EQ(buffer.getBit(4), 0);
    ASSERT_EQ(buffer.getBit(5), 0);
    ASSERT_EQ(buffer.getBit(6), 0);
    ASSERT_EQ(buffer.getBit(7), 1);
    
    // Check bits in second byte (should all be 0)
    for (int i = 8; i < 16; i++) {
        ASSERT_EQ(buffer.getBit(i), 0);
    }
    
    // Verify cursor positions unchanged
    ASSERT_EQ(buffer.getBitsWritten(), 0);
    ASSERT_EQ(buffer.getWriteCursorPos(), 0);
    ASSERT_EQ(buffer.getBitsRead(), 0);
    ASSERT_EQ(buffer.getReadCursorPos(), 0);
}

//---------------- TEST readBit() -----------------------
TEST(BinaryBufferTests, ReadBit) {
    BinaryBuffer buffer(16);
    
    // Write pattern 1010 0001 1111 0000 to buffer
    buffer.writeBit(1, 0);
    buffer.writeBit(0, 1);
    buffer.writeBit(1, 2);
    buffer.writeBit(0, 3);
    buffer.writeBit(0, 4);
    buffer.writeBit(0, 5);
    buffer.writeBit(0, 6);
    buffer.writeBit(1, 7);
    buffer.writeBit(1, 8);
    buffer.writeBit(1, 9);
    buffer.writeBit(1, 10);
    buffer.writeBit(1, 11);
    buffer.writeBit(0, 12);
    buffer.writeBit(0, 13);
    buffer.writeBit(0, 14);
    buffer.writeBit(0, 15);
    
    // Read and verify bits
    ASSERT_EQ(buffer.readBit(), 1);
    ASSERT_EQ(buffer.getReadCursorPos(), 1);
    ASSERT_EQ(buffer.getBitsRead(), 1);
    
    ASSERT_EQ(buffer.readBit(), 0);
    ASSERT_EQ(buffer.getReadCursorPos(), 2);
    ASSERT_EQ(buffer.getBitsRead(), 2);
    
    // Skip ahead using setReadCursorPos
    buffer.setReadCursorPos(8);
    
    ASSERT_EQ(buffer.readBit(), 1);
    ASSERT_EQ(buffer.getReadCursorPos(), 9);
    ASSERT_EQ(buffer.getBitsRead(), 9);
    
    // Read multiple bits
    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(buffer.readBit(), 1);
    }
    
    ASSERT_EQ(buffer.getReadCursorPos(), 12);
    ASSERT_EQ(buffer.getBitsRead(), 12);
    
    // Read remaining bits
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(buffer.readBit(), 0);
    }
    
    ASSERT_EQ(buffer.getReadCursorPos(), 16);
    ASSERT_EQ(buffer.getBitsRead(), 16);
}

//---------------- TEST write(uint8_t bit) -----------------------
TEST(BinaryBufferTests, WriteSingleBitWithCursor) {
    BinaryBuffer buffer(16);
    
    // Write alternating bits
    buffer.write(1);
    buffer.write(0);
    buffer.write(1);
    buffer.write(0);
    
    // Check bits were written
    ASSERT_EQ(buffer.getBit(0), 1);
    ASSERT_EQ(buffer.getBit(1), 0);
    ASSERT_EQ(buffer.getBit(2), 1);
    ASSERT_EQ(buffer.getBit(3), 0);
    
    // Check cursor position and bits_written updated correctly
    ASSERT_EQ(buffer.getBitsWritten(), 4);
    ASSERT_EQ(buffer.getWriteCursorPos(), 4);
    
    // Reset cursor and overwrite
    buffer.setWriteCursorPos(1);
    buffer.write(1);
    
    // Check bit was overwritten
    ASSERT_EQ(buffer.getBit(1), 1);
    
    // Check cursor advanced
    ASSERT_EQ(buffer.getWriteCursorPos(), 2);
    // bits_written should still be 4 since we didn't write past the end
    ASSERT_EQ(buffer.getBitsWritten(), 4);
    
    // Now write past the previous end
    buffer.setWriteCursorPos(4);
    buffer.write(1);
    
    // Check cursor and bits_written updated
    ASSERT_EQ(buffer.getWriteCursorPos(), 5);
    ASSERT_EQ(buffer.getBitsWritten(), 5);
    
    // Write remaining bits to fill buffer
    for (int i = 5; i < 16; i++) {
        buffer.write(i % 2);
    }
    
    // Check final positions
    ASSERT_EQ(buffer.getWriteCursorPos(), 16);
    ASSERT_EQ(buffer.getBitsWritten(), 16);
    
    // Verify all bits were written correctly
    ASSERT_EQ(buffer.getBit(0), 1);
    ASSERT_EQ(buffer.getBit(1), 1); // This was overwritten
    ASSERT_EQ(buffer.getBit(2), 1);
    ASSERT_EQ(buffer.getBit(3), 0);
    ASSERT_EQ(buffer.getBit(4), 1);
    
    // Check the alternating pattern from position 5 onwards
    for (int i = 5; i < 16; i++) {
        ASSERT_EQ(buffer.getBit(i), i % 2);
    }
}

//---------------- TEST writeBit() -----------------------
TEST(BinaryBufferTests, WriteBit) {
    BinaryBuffer buffer(16);
    
    // Write bits at specific positions
    buffer.writeBit(1, 0);
    buffer.writeBit(1, 7);
    buffer.writeBit(1, 8);
    buffer.writeBit(1, 15);
    
    // Verify bits at those positions
    ASSERT_EQ(buffer.getBit(0), 1);
    ASSERT_EQ(buffer.getBit(7), 1);
    ASSERT_EQ(buffer.getBit(8), 1);
    ASSERT_EQ(buffer.getBit(15), 1);
    
    // Verify bits at other positions are still 0
    for (int i = 1; i < 7; i++) {
        ASSERT_EQ(buffer.getBit(i), 0);
    }
    for (int i = 9; i < 15; i++) {
        ASSERT_EQ(buffer.getBit(i), 0);
    }
    
    // Verify cursor positions unchanged
    ASSERT_EQ(buffer.getBitsWritten(), 0);
    ASSERT_EQ(buffer.getWriteCursorPos(), 0);
    
    // Overwrite some bits
    buffer.writeBit(0, 0);
    buffer.writeBit(0, 15);
    
    // Verify overwritten bits
    ASSERT_EQ(buffer.getBit(0), 0);
    ASSERT_EQ(buffer.getBit(15), 0);
}

//---------------- TEST cursor positions -----------------------
TEST(BinaryBufferTests, CursorPositions) {
    BinaryBuffer buffer(32);
    
    // Write and advance cursor
    buffer.write(1);
    buffer.write(0);
    buffer.write(1);
    
    ASSERT_EQ(buffer.getBitsWritten(), 3);
    ASSERT_EQ(buffer.getWriteCursorPos(), 3);
    
    // Read and advance cursor
    ASSERT_EQ(buffer.readBit(), 1);
    ASSERT_EQ(buffer.readBit(), 0);
    
    ASSERT_EQ(buffer.getBitsRead(), 2);
    ASSERT_EQ(buffer.getReadCursorPos(), 2);
    
    // Reset write cursor
    buffer.resetWriteCursorPos();
    ASSERT_EQ(buffer.getWriteCursorPos(), 3);  // No change since it was already at end
    
    // Set write cursor to custom position
    buffer.setWriteCursorPos(1);
    ASSERT_EQ(buffer.getWriteCursorPos(), 1);
    
    // Overwrite and check advancement
    buffer.write(1);
    ASSERT_EQ(buffer.getBit(1), 1);
    ASSERT_EQ(buffer.getWriteCursorPos(), 2);
    ASSERT_EQ(buffer.getBitsWritten(), 3);  // Total bits written still 3
    
    // Reset read cursor
    buffer.resetReadCursorPos();
    ASSERT_EQ(buffer.getReadCursorPos(), 2);  // No change since it was already at end
    
    // Set read cursor to custom position
    buffer.setReadCursorPos(0);
    ASSERT_EQ(buffer.getReadCursorPos(), 0);
    
    // Read again from beginning
    ASSERT_EQ(buffer.readBit(), 1);
    ASSERT_EQ(buffer.readBit(), 1);  // This was overwritten from 0 to 1
    ASSERT_EQ(buffer.readBit(), 1);
    
    ASSERT_EQ(buffer.getBitsRead(), 3);
    ASSERT_EQ(buffer.getReadCursorPos(), 3);
}

//------------------------------------- DERIVED FUNCTIONS -------------------------------------------
//The unit tests in this section were written by AI but were reviewed and when needed, modified by the author

//---------------- TEST writeValue/readValue with big_int32_t -----------------------
TEST(BinaryBufferTests, WriteReadBigInt32) {
    BinaryBuffer buffer(128);
    
    // Test value (arbitrary 32-bit signed int)
    big_int32_t original_value = -123456789;
    
    // Write full value
    buffer.writeValue(original_value);
    
    // Reset read cursor
    buffer.setReadCursorPos(0);
    
    // Read full value
    big_int32_t read_value = buffer.readValue<big_int32_t>();
    
    // Verify
    ASSERT_EQ(read_value, original_value);
    ASSERT_EQ(buffer.getBitsWritten(), 32);
    ASSERT_EQ(buffer.getBitsRead(), 32);
}

TEST(BinaryBufferTests, WriteReadBigInt32Partial) {
    BinaryBuffer buffer(128);
    
    // Test values with specific bit patterns
    big_int32_t value1 = 0xF0F0F0F0; // 11110000111100001111000011110000
    big_int32_t value2 = 0x12345678; // 00010010001101000101011001111000
    
    // Write only 16 bits of value1 (rightmost/LSB bits)
    buffer.writeValue(value1, 16);
    
    // Reset read cursor
    buffer.setReadCursorPos(0);
    
    // Read 16 bits back
    big_int32_t read_value1 = buffer.readValue<big_int32_t>(16);
    
    // Should get only the 16 LSBs (0xF0F0)
    ASSERT_EQ(read_value1, static_cast<big_int32_t>(0xF0F0));
    ASSERT_EQ(buffer.getBitsWritten(), 16);
    ASSERT_EQ(buffer.getBitsRead(), 16);
    
    // Write only 8 bits of value2
    buffer.setWriteCursorPos(16);
    buffer.writeValue(value2, 8);
    
    // Reset read cursor
    buffer.setReadCursorPos(16);
    
    // Read 8 bits back
    big_int32_t read_value2 = buffer.readValue<big_int32_t>(8);
    
    // Should get only the 8 LSBs (0x78)
    ASSERT_EQ(read_value2, static_cast<big_int32_t>(0x78));
    ASSERT_EQ(buffer.getBitsWritten(), 24);
    ASSERT_EQ(buffer.getBitsRead(), 24);
}

//---------------- TEST writeValue/readValue with big_int64_t -----------------------
TEST(BinaryBufferTests, WriteReadBigInt64) {
    BinaryBuffer buffer(128);
    
    // Test value (arbitrary 64-bit signed int)
    big_int64_t original_value = -9223372036854775807LL; // close to min 64-bit int
    
    // Write full value
    buffer.writeValue(original_value);
    
    // Reset read cursor
    buffer.setReadCursorPos(0);
    
    // Read full value
    big_int64_t read_value = buffer.readValue<big_int64_t>();
    
    // Verify
    ASSERT_EQ(read_value, original_value);
    ASSERT_EQ(buffer.getBitsWritten(), 64);
    ASSERT_EQ(buffer.getBitsRead(), 64);
}

TEST(BinaryBufferTests, WriteReadBigInt64Partial) {
    BinaryBuffer buffer(128);
    
    // Test with specific bit pattern
    big_int64_t value = 0x0123456789ABCDEFLL;
    
    // Write only 32 bits (4 bytes) of value
    buffer.writeValue(value, 32);
    
    // Reset read cursor
    buffer.setReadCursorPos(0);
    
    // Read 32 bits back
    big_int64_t read_value = buffer.readValue<big_int64_t>(32);
    
    // Should get only the 32 LSBs (0x89ABCDEF)
    ASSERT_EQ(read_value, static_cast<big_int64_t>(0x89ABCDEF));
    ASSERT_EQ(buffer.getBitsWritten(), 32);
    ASSERT_EQ(buffer.getBitsRead(), 32);
    
    // Test with fewer bits
    buffer.setWriteCursorPos(32);
    buffer.writeValue(value, 24);
    
    buffer.setReadCursorPos(32);
    big_int64_t read_value2 = buffer.readValue<big_int64_t>(24);
    
    // Should get only the 24 LSBs (0xCDEF + the high byte of 0xAB)
    ASSERT_EQ(read_value2, static_cast<big_int64_t>(0xABCDEF));
    ASSERT_EQ(buffer.getBitsWritten(), 56);
    ASSERT_EQ(buffer.getBitsRead(), 56);
}

// Custom bit field structure for testing
struct PackedData {
    uint32_t field1 : 4;  // 4 bits
    uint32_t field2 : 8;  // 8 bits 
    uint32_t field3 : 12; // 12 bits
    uint32_t field4 : 8;  // 8 bits
    // Total: 32 bits
};

//---------------- TEST writeValue/readValue with custom bit field structure -----------------------
TEST(BinaryBufferTests, WriteReadCustomBitFields) {
    BinaryBuffer buffer(128);
    
    // Create a custom bit field structure
    PackedData data;
    data.field1 = 0xF;        // 1111 (4 bits)
    data.field2 = 0xAA;       // 10101010 (8 bits)
    data.field3 = 0xBCD;      // 101111001101 (12 bits)
    data.field4 = 0x12;       // 00010010 (8 bits)
    
    // Write the full structure
    buffer.writeValue(data);
    
    // Reset read cursor
    buffer.setReadCursorPos(0);
    
    // Read full structure back
    PackedData read_data = buffer.readValue<PackedData>();
    
    // Verify each field
    ASSERT_EQ(read_data.field1, data.field1);
    ASSERT_EQ(read_data.field2, data.field2);
    ASSERT_EQ(read_data.field3, data.field3);
    ASSERT_EQ(read_data.field4, data.field4);
    ASSERT_EQ(buffer.getBitsWritten(), 32);
    ASSERT_EQ(buffer.getBitsRead(), 32);
}

//---------------- TEST Multiple Reads/Writes with Different Types -----------------------
TEST(BinaryBufferTests, MultipleWriteReadsMixedTypes) {
    BinaryBuffer buffer(256);
    
    // Write multiple values of different types
    big_int32_t int32_val = 0x12345678;
    big_int64_t int64_val = 0x0123456789ABCDEFLL;
    
    // Write all values with different bit lengths
    buffer.writeValue(int32_val);          // 32 bits
    buffer.writeValue(int64_val, 48);      // 48 bits
    
    ASSERT_EQ(buffer.getBitsWritten(), 32 + 48);
    
    // Read all values back
    buffer.setReadCursorPos(0);
    
    big_int32_t read_int32 = buffer.readValue<big_int32_t>();
    ASSERT_EQ(read_int32, int32_val);
    
    big_int64_t read_int64 = buffer.readValue<big_int64_t>(48);
    // Extract the 48 LSBs of int64_val for comparison
    big_int64_t expected48 = int64_val & ((1LL << 48) - 1);
    ASSERT_EQ(read_int64, expected48);
    
    ASSERT_EQ(buffer.getBitsRead(), 32 + 48);
}

//---------------- TEST getBits<L>() -----------------------
TEST(BinaryBufferTests, GetBitsSmall) {
    BinaryBuffer buffer(64);
    
    // Write a known pattern: 0xA5 = 10100101
    buffer.writeBit(1, 0);
    buffer.writeBit(0, 1);
    buffer.writeBit(1, 2);
    buffer.writeBit(0, 3);
    buffer.writeBit(0, 4);
    buffer.writeBit(1, 5);
    buffer.writeBit(0, 6);
    buffer.writeBit(1, 7);
    
    // Get all 8 bits
    std::bitset<8> bits8 = buffer.getBits<8>(0);
    ASSERT_EQ(bits8.to_ulong(), 0xA5);
    
    // Get 4 bits from different positions
    std::bitset<4> bits4_0 = buffer.getBits<4>(0); // Should be 1010
    ASSERT_EQ(bits4_0.to_ulong(), 0xA);
    
    std::bitset<4> bits4_4 = buffer.getBits<4>(4); // Should be 0101
    ASSERT_EQ(bits4_4.to_ulong(), 0x5);
    
    // Get individual bits
    std::bitset<1> bit0 = buffer.getBits<1>(0);
    std::bitset<1> bit1 = buffer.getBits<1>(1);
    ASSERT_EQ(bit0.to_ulong(), 1);
    ASSERT_EQ(bit1.to_ulong(), 0);
    
    // Verify cursor positions remain unchanged
    ASSERT_EQ(buffer.getBitsWritten(), 0);
    ASSERT_EQ(buffer.getBitsRead(), 0);
}

TEST(BinaryBufferTests, GetBitsCrossByteBoundary) {
    BinaryBuffer buffer(32);
    
    // Write pattern to first two bytes: 0xF0 0x0F = 11110000 00001111
    for (int i = 0; i < 4; i++) buffer.writeBit(1, i);
    for (int i = 4; i < 12; i++) buffer.writeBit(0, i);
    for (int i = 12; i < 16; i++) buffer.writeBit(1, i);
    
    // Get 8 bits across byte boundary
    std::bitset<8> cross_bits = buffer.getBits<8>(4);
    ASSERT_EQ(cross_bits.to_ulong(), 0x00); // All zeros
    
    // Get 12 bits spanning all the way
    std::bitset<12> wide_bits = buffer.getBits<12>(2);
    ASSERT_EQ(wide_bits.to_ulong(), 0xC03); // 110000000011
}

TEST(BinaryBufferTests, GetBitsLarge) {
    BinaryBuffer buffer(128);
    
    // Write a 64-bit pattern
    uint64_t pattern = 0x0123456789ABCDEFULL;
    
    for (int i = 0; i < 64; i++) {
        buffer.writeBit((pattern >> (63-i)) & 1, i);
    }
    
    // Get all 64 bits
    std::bitset<64> bits64 = buffer.getBits<64>(0);
    ASSERT_EQ(bits64.to_ullong(), pattern);
    
    // Get 32 high bits
    std::bitset<32> bits32_high = buffer.getBits<32>(0);
    ASSERT_EQ(bits32_high.to_ulong(), 0x01234567);
    
    // Get 32 low bits
    std::bitset<32> bits32_low = buffer.getBits<32>(32);
    ASSERT_EQ(bits32_low.to_ulong(), 0x89ABCDEF);
    
    // Get middle 32 bits
    std::bitset<32> bits32_mid = buffer.getBits<32>(16);
    ASSERT_EQ(bits32_mid.to_ulong(), 0x456789AB);
    
    // Get various smaller chunks
    std::bitset<16> bits16_1 = buffer.getBits<16>(8);
    std::bitset<16> bits16_2 = buffer.getBits<16>(24);
    std::bitset<16> bits16_3 = buffer.getBits<16>(40);
    
    ASSERT_EQ(bits16_1.to_ulong(), 0x2345);
    ASSERT_EQ(bits16_2.to_ulong(), 0x6789);
    ASSERT_EQ(bits16_3.to_ulong(), 0xABCD);
}

//---------------- TEST Repeated Partial Reads/Writes -----------------------
TEST(BinaryBufferTests, RepeatedPartialWrites) {
    BinaryBuffer buffer(64);
    
    // Write multiple values with specific bit lengths
    big_uint16_t val1 = 0xABCD;
    big_uint16_t val2 = 0x1234;
    big_uint32_t val3 = 0xFEDCBA98;
    
    // Write them in pieces
    buffer.writeValue(val1, 8);  // Write only the bottom 8 bits (0xCD)
    buffer.writeValue(val2, 12); // Write 12 bits (0x234)
    buffer.writeValue(val3, 4);  // Write only bottom 4 bits (0x8)
    
    // Should have written 8 + 12 + 4 = 24 bits
    ASSERT_EQ(buffer.getBitsWritten(), 24);
    
    // Check individual bits using getBits
    std::bitset<8> bits1 = buffer.getBits<8>(0);
    std::bitset<12> bits2 = buffer.getBits<12>(8);
    std::bitset<4> bits3 = buffer.getBits<4>(20);
    
    ASSERT_EQ(bits1.to_ulong(), 0xCD);
    ASSERT_EQ(bits2.to_ulong(), 0x234);
    ASSERT_EQ(bits3.to_ulong(), 0x8);
    
    // Now read them back with readValue
    buffer.setReadCursorPos(0);
    
    uint8_t read1 = buffer.readValue<big_uint8_t>(8);
    uint16_t read2 = buffer.readValue<big_uint16_t>(12);
    uint8_t read3 = buffer.readValue<big_uint8_t>(4);
    
    ASSERT_EQ(read1, 0xCD);
    ASSERT_EQ(read2, 0x234);
    ASSERT_EQ(read3, 0x8);
    ASSERT_EQ(buffer.getBitsRead(), 24);
}

TEST(BinaryBufferTests, InterleavedReadsWrites) {
    BinaryBuffer buffer(128);
    
    // Interleave writes and reads
    big_uint8_t val1 = 0x3F;    // 00111111
    big_uint16_t val2 = 0x8001; // 1000000000000001
    big_uint32_t val3 = 0xAA55; // 10101010 01010101
    
    // Write first value
    buffer.writeValue(val1, 6); // Write 6 bits (0x3F & 0x3F = 0x3F = 111111)
    
    // Read it back
    buffer.setReadCursorPos(0);
    uint8_t read1 = buffer.readValue<big_uint8_t>(6);
    ASSERT_EQ(read1, 0x3F);
    
    // Write second value after the first
    buffer.setWriteCursorPos(6);
    buffer.writeValue(val2, 10); // Write 10 bits (0x8001 & 0x3FF = 0x001 = 0000000001)
    
    // Read it back
    buffer.setReadCursorPos(6);
    uint16_t read2 = buffer.readValue<big_uint16_t>(10);
    ASSERT_EQ(read2, 0x001);
    
    // Write third value after both
    buffer.setWriteCursorPos(16);
    buffer.writeValue(val3, 14); // Write 14 bits (0xAA55 & 0x3FFF = 0x2A55 = 10101001010101)
    
    // Read it back
    buffer.setReadCursorPos(16);
    uint16_t read3 = buffer.readValue<big_uint16_t>(14);
    ASSERT_EQ(read3, 0x2A55);
    
    // Total bits written/read
    ASSERT_EQ(buffer.getBitsWritten(), 30);
    ASSERT_EQ(buffer.getBitsRead(), 30);
    
    // Now read everything in one go
    buffer.setReadCursorPos(0);
    uint32_t all_bits = buffer.readValue<big_uint32_t>(30);
    
    // Calculate expected value:
    // 6 bits of val1 (0x3F = 111111)
    // 10 bits of val2 (0x001 = 0000000001)
    // 14 bits of val3 (0x2A55 = 10101001010101)
    // Combined: 111111 0000000001 10101001010101
    uint32_t expected = (0x3F << 24) | (0x001 << 14) | 0x2A55;
    
    ASSERT_EQ(all_bits, expected);
}

TEST(BinaryBufferTests, OverlappingWritesReads) {
    BinaryBuffer buffer(64);
    
    // Fill buffer with a pattern
    for (int i = 0; i < 32; i++) {
        buffer.writeBit(i % 2, i); // Alternating 0,1
    }
    
    // Read 8-bit chunks with overlap
    buffer.setReadCursorPos(0);
    uint8_t chunk1 = buffer.readValue<big_uint8_t>(8); // Bits 0-7
    
    buffer.setReadCursorPos(4);
    uint8_t chunk2 = buffer.readValue<big_uint8_t>(8); // Bits 4-11
    
    buffer.setReadCursorPos(8);
    uint8_t chunk3 = buffer.readValue<big_uint8_t>(8); // Bits 8-15
    
    // Expected values:
    // chunk1: 01010101 = 0x55
    // chunk2: 01010101 = 0x55
    // chunk3: 01010101 = 0x55
    ASSERT_EQ(chunk1, 0x55);
    ASSERT_EQ(chunk2, 0x55);
    ASSERT_EQ(chunk3, 0x55);
    
    // Now write overlapping data
    buffer.setWriteCursorPos(4);
    buffer.writeValue<big_uint16_t>(0xFFFF, 8); // Write 8 bits of 1s at position 4
    
    // Read and check
    buffer.setReadCursorPos(0);
    uint16_t result = buffer.readValue<big_uint16_t>(16);
    
    // Original: 0101 0101 0101 0101
    // After write: 0101 1111 1111 0101
    // So first 16 bits should be: 0x5FF5
    ASSERT_EQ(result, 0x5FF5);
}

TEST(BinaryBufferTests, MultipleDifferentSizeWrites) {
    BinaryBuffer buffer(128);
    
    // Test writing multiple values with different sizes
    big_uint8_t val8 = 0xA5;       // 10100101
    big_uint16_t val16 = 0xB33F;    // 1011001100111111
    big_uint32_t val32 = 0xC7D28E44; // 11000111110100101000111001000100
    
    // Write them all with different bit counts
    buffer.writeValue(val8, 6);    // 6 bits (101001)
    buffer.writeValue(val16, 10);  // 10 bits (0011111111)
    buffer.writeValue(val32, 14);  // 14 bits (00111001000100)
    
    // Total bits written: 6 + 10 + 14 = 30
    ASSERT_EQ(buffer.getBitsWritten(), 30);
    
    // Now read them back in different sized chunks
    buffer.setReadCursorPos(0);
    
    // Read first 8 bits (should span first value and part of second)
    uint8_t read8_1 = buffer.readValue<big_uint8_t>(8);
    
    // Next 12 bits (should span rest of second value and part of third)
    uint16_t read16 = buffer.readValue<big_uint16_t>(12);
    
    // Last 10 bits (rest of third value)
    uint16_t read10 = buffer.readValue<big_uint16_t>(10);
    
    // Calculate expected values
    // First 8 bits: 6 bits of val8 (100101) + 2 bits of val16 (11)
    uint8_t expected8_1 = 0b10010111;
    
    // Next 12 bits: 8 remaining bits of val16 (00111111) + 4 bits of val32 (0011)
    uint16_t expected16 = 0b001111110011;
    
    // Last 10 bits: remaining 10 bits of val32 (1001000100)
    uint16_t expected10 = 0b1001000100;
    
    ASSERT_EQ(read8_1, expected8_1);
    ASSERT_EQ(read16, expected16);
    ASSERT_EQ(read10, expected10);
}
