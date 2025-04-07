#include <cstdint>
#include <cstdlib> 
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <bitset>
#include <memory>
#ifndef CMBTL_BINARY_BUFFER_H
#define CMBTL_BINARY_BUFFER_H
namespace cmbtl {
    class BinaryBuffer {
        private:
            std::shared_ptr<unsigned char[]> buffer;
            //Number of bits that buffer is designed to hold
            uint32_t capacity;

            //Holds the value of the index after the index of the furthest bit written
            mutable uint32_t bits_written;

            //Holds the index of the bit that should be written (or overwritten) on the next writeValue
            //For most use cases write_cursor_pos = bits_written
            mutable uint32_t write_cursor_pos;

            //Holds the value of the index after the index of the furthest bit read
            mutable uint32_t bits_read;

            //Holds the index of the bit that should read next
            //For most use cases read_cursor_pos = bits_read
            mutable uint32_t read_cursor_pos;

        public:
            //Parameter capacity: Number of BITS (NOT BYTES) the buffer should store
            inline BinaryBuffer(uint32_t capacity, bool should_clear_buffer = true) {
                resetIOValues();

                cmbtl::BinaryBuffer::capacity = capacity;

                buffer = std::shared_ptr<unsigned char[]>(new unsigned char[getSize()], std::default_delete<unsigned char[]>());

                if (should_clear_buffer) {
                    clearBuffer();
                }
            }
            //Constructs BinaryBuffer from an existing unsigned char* assuming that unsigned char* buffer
            //was created by the BinaryBuffer class
            //Parameter *buffer: unsigned char array created by the binary buffer class
            //Parameter capacity: capacity of the 
            inline BinaryBuffer (unsigned char* raw_buffer, uint32_t capacity) {
                resetIOValues();

                cmbtl::BinaryBuffer::capacity = capacity;
                cmbtl::BinaryBuffer::buffer = std::shared_ptr<unsigned char[]>(raw_buffer, [](unsigned char*){});

            }

            //Default copy constructor and assignment
            BinaryBuffer(const BinaryBuffer&) = default;
            BinaryBuffer& operator=(const BinaryBuffer&) = default;

            //Default move constructor and assignment
            BinaryBuffer(BinaryBuffer&&) = default;
            BinaryBuffer& operator=(BinaryBuffer&&) = default;

            //---------------------- GETTERS --------------------

            const unsigned char* getBuffer() {
                return buffer.get();
            }

            inline uint32_t getCapacity() const {
                return capacity;
            }
            
            inline uint32_t getBitsWritten() const {
                return bits_written;
            }
            inline uint32_t getWriteCursorPos() const {
                return write_cursor_pos;
            }

            inline uint32_t getBitsRead() const {
                return bits_read;
            }
            inline uint32_t getReadCursorPos() const {
                return read_cursor_pos;
            }

            // Returns the size of the buffer in terms of bytes. This is the number of char elements in the unsigned char* buffer array
            inline size_t getSize() const {
                return (capacity - 1) / CHAR_BIT + 1;
            }

            //------------------- SETTERS ---------------------------
            //Sets write_cursor_pos to a custom user defined index
            inline void setWriteCursorPos(uint32_t new_pos) const {
                assert(new_pos < capacity && "New cursor position must be within range [0, capacity).");
                write_cursor_pos = new_pos;
            }
            //Resets write_cursor_pos to bits_written, i.e index after the furthest bit written
            inline void resetWriteCursorPos() const {
                write_cursor_pos = bits_written;
            }

            inline void setBitsRead(uint32_t new_val) const {
                bits_read = new_val;
            }

            inline void setBitsWritten(uint32_t new_val) const {
                bits_written = new_val;
            }

            //Sets read_cursor_pos to a custom user defined index
            inline void setReadCursorPos(uint32_t new_pos) const {
                assert(new_pos < capacity && "New cursor position must be within range [0, capacity).");
                read_cursor_pos = new_pos;
            }

            //Resets read_cursor_pos to bits_read, i.e index after the furthest bit read
            inline void resetReadCursorPos() const {
                read_cursor_pos = bits_read;
            }
            //---------------- BASIC FUNCTIONS --------------------

            inline void clearBuffer() {
                memset(buffer.get(), 0, getSize());

                bits_written = 0;
                write_cursor_pos = 0;

                bits_read = 0;
                read_cursor_pos = 0;
            }

            //Note does NOT modify read_cursor_pos or bits_read
            inline uint8_t getBit(uint32_t bit_index) const {
                assert(bit_index < capacity && "Cannot retrieve a bit with index beyond capacity of binary buffer!");
                return (buffer[getBufferArrayIndex(bit_index)] >> getDistanceFromLSB(bit_index)) & 0x1;
            }

            //Does modify read_cursor_pos or bits_read
            inline uint8_t readBit() const {
                uint8_t bit = getBit(read_cursor_pos);

                //Update read_cursor_poos and bits_read
                read_cursor_pos++;
                if (read_cursor_pos > bits_read) {
                    bits_read = read_cursor_pos;
                }
                return bit;
            }

            //Does modify write_cursor_pos and bits_written
            inline void write(uint8_t bit) {
                writeBit(bit, write_cursor_pos);

                //Update write_cursor_pos and bits_written
                write_cursor_pos++;
                if (write_cursor_pos > bits_written) {
                    bits_written = write_cursor_pos;
                }
            }
            
            //Does NOT modify write_cursor_pos and bits_written
            inline void writeBit(uint8_t bit, uint32_t bit_index) {
                assert((bit == 0 || bit == 1) && "Bit value must be 0 or 1.");
                assert(bit_index < capacity && "Parameter: bit_index must be in range [0, capacity)");
                if (bit) {
                    //Write bit
                    buffer[getBufferArrayIndex(bit_index)] |= 0x1 << getDistanceFromLSB(bit_index);
                } else {
                    //Reset bit
                    buffer[getBufferArrayIndex(bit_index)] &= ~(0x1 << getDistanceFromLSB(bit_index));
                }
            }

            //---------------------- DERIVED FUNCTIONS ------------------------------
            //Assumes big endian values
            template<typename V>
            inline void writeValue(V val) {
                writeValue(val, sizeof(V) * CHAR_BIT);
            }
        
            template<typename V>
            //Writes num_bits from variable val, the range is shifted to include the rightmost (i.e LSB)
            //Assumes big endian values
            inline void writeValue(V val, uint32_t num_bits) {
                constexpr size_t val_size = sizeof(V);
                unsigned char temp_buffer[val_size];

                memcpy(temp_buffer, &val, val_size);
                for (uint32_t i = (val_size * CHAR_BIT - num_bits); i < val_size * CHAR_BIT; i++) {
                    uint8_t bit = (temp_buffer[getBufferArrayIndex(i)] >> getDistanceFromLSB(i)) & 0x01;
                    write(bit);
                }
            }

            //NOTE: This const method modifies the internal read cursor position
            //Reads value type V from the buffer.
            //Assumes big endian values
            template<typename V>
            inline V readValue() const {
                return readValue<V>(sizeof(V) * CHAR_BIT);
            }

            //NOTE: This const method modifies the internal read cursor position
            //Reads num_bits from the buffer and places then and returns a variable of type V
            //Assumes big endian values
            //This functions reads bits from least to most significant (right to left in binary)
            //If num_bits is less than the number of bits V takes up, 
            //only the num_bits rightmost bits will be stored in the returned value
            //Does modify cursor_read_pos and bits_read
            //V: Type that you would like to read from buffer
            //Parameter: num_bits, number of bits that you would like to read from the buffer.
            template<typename V>
            inline V readValue(uint32_t num_bits) const {
                constexpr size_t val_size = sizeof(V);
                unsigned char temp_buffer[val_size];

                memset(temp_buffer, 0, val_size);

                assert(num_bits <= val_size * CHAR_BIT && "Parameter: num_bits is greater than the possible size of type V!");

                for (uint32_t i = val_size * CHAR_BIT - num_bits; i < val_size * CHAR_BIT; i++) {
                    //read_cursor_pos is updated with readBit() so we need to retrieve the old value beforehand
                    temp_buffer[getBufferArrayIndex(i)] |=  readBit() << getDistanceFromLSB(i);
                }

                V val;
                memcpy(&val, temp_buffer, val_size);

                return val;
            }

            //L: Length, the number of bits that you want to be included in the returned bitset
            //Ex: getBits<12>(0) would returned bits from indices [0, 11]
            //Note: Does NOT modify read_cursor_pos and bits_read
            template<uint32_t L>
            inline std::bitset<L> getBits(uint32_t start_bit_index) const {
                assert(start_bit_index + L <= capacity && "Cannot retrive bits that are beyond the capacity of the buffer");

                //Initialize bitset
                std::bitset<L> bitset;
                for (uint32_t count = 0; count < L; count++) {
                    bitset[L - 1 - count] = getBit(start_bit_index + count);
                }
                return bitset;
            }
        
        // ---------------------- PRIVATE HELPERS ----------------------------
        
        private:
            static constexpr inline uint32_t getBufferArrayIndex(uint32_t bit_index) {
                return bit_index / CHAR_BIT;
            }
            static constexpr inline uint32_t getDistanceFromLSB(uint32_t bit_index) {
                return CHAR_BIT - 1 - (bit_index % CHAR_BIT);
            }
            inline void resetIOValues() {
                bits_written = 0;
                write_cursor_pos = 0;
                bits_read = 0;
                read_cursor_pos = 0;
            } 
    };
}
#endif