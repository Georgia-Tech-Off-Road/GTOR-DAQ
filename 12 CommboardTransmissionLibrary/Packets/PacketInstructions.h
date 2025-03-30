#include "../misc/BinaryBuffer/BinaryBuffer.h"
#include "Sensors.h"
#ifndef CMBTL_PACKET_INSTRUCTIONS_H
#define CMBTL_PACKET_INSTRUCTIONS_H
namespace cmbtl {
    template <size_t NUM_SENSORS>
    using PacketInstructions = std::array<bool, NUM_SENSORS>;
}
#endif