#include "misc/BinaryBuffer/BinaryBuffer.h"
#include "PacketInstructions.h"
#include <array>

#ifndef CMBTL_PACKET_INSTRUCTIONS_BUILDER_H
#define CMBTL_PACKET_INSTRUCTIONS_BUILDER_H

namespace cmbtl {
    namespace packet {
        template <size_t NUM_SENSORS>
        constexpr packet::PacketInstructions<NUM_SENSORS> createPacket(std::vector<int> indices) {
            packet::PacketInstructions<NUM_SENSORS> instructions;

            for (int index : indices) {
                instructions.set(index);
            }

            return instructions;
        }
    }
}

#endif