#include "PacketInstructions.h"
#include <vector>
#include <initializer_list>
#ifndef CMBTL_PACKET_SCHEME_H
#define CMBTL_PACKET_SCHEME_H
namespace cmbtl {
    namespace packet {
        template<size_t NUM_SENSORS>
        /**
         * @brief Defines a "packet scheme", the definition and order in which packets are to be sent under certain circumstances.
         * 
         * Packet schemes are used to define how packets are to be sent in a scenario. 
         * For example, under minimum bandwith, one might switch to a packet scheme full of sparse (few sensors included) packets to
         * conserve data.
         * 
         * It is not the job of PacketScheme to determine how often packets are sent.
         * 
         * Stores a vector of packets in order that they are to be sent. 
         * getNext() "wraps" around to the beginning of the list to restart the packet cycle
         */
        class PacketScheme {
            private:
                // Stores the packets in order of usage
                std::vector<PacketInstructions<NUM_SENSORS>> packets;

                // Count variable
                int i;

            public:
                /**
                 * @brief Constructor for PacketScheme
                 * 
                 * Sets private variable packets to the list of packets provided
                 * 
                 * @param packets A {} enclosed list of PacketInstructions corresponding to the packets (in order) 
                 */
                PacketScheme(std::initializer_list<PacketInstructions<NUM_SENSORS>> packets) : packets(packets), i(0){};


                /**
                 * @brief returns the next packet in the sequence, wrapping if necessary
                 * 
                 * @returns The next packet in the sequence, returning to the beginning once it has reached the end
                 */
                PacketInstructions<NUM_SENSORS> getNext() {
                    if (++i >= packets.size()) i = 0;

                    return packets[i];
                }
        };
    }
}
#endif