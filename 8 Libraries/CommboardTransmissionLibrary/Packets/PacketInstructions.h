#include <bitset>
#ifndef CMBTL_PACKET_INSTRUCTIONS_H
#define CMBTL_PACKET_INSTRUCTIONS_H
namespace cmbtl {
    namespace packet {
        template<size_t NUM_SENSORS>
        using PacketInstructions = std::bitset<NUM_SENSORS>;
    }
}
#endif