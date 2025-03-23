#include "misc/BinaryBuffer/BinaryBuffer.h"
#include "Sensors.h"
#ifndef CMBTL_PACKET_INSTRUCTIONS_H
#define CMBTL_PACKET_INSTRUCTIONS_H
namespace cmbtl {
    class PacketInstructions : BinaryBuffer {
        public:
            inline PacketInstructions() {
                BinaryBuffer(cmbtl::NUM_SENSORS);
                uint32_t i = 0;
            }
            inline uint32_t getPacketEncodedBitSize() const {
                uint32_t bit_size = 0;
                uint32_t i = 0;
                for (SensorInfo SENSOR : SENSORS) {
                    if (getBit(i) == 1) {
                        bit_size += SENSOR.ENCODED_BIT_SIZE;
                    }
                    i++;
                }
                return bit_size;
            }
    }
}
#endif