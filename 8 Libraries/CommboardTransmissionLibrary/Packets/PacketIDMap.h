#include "PacketInstructions.h"
#include <unordered_map>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <sstream>
#ifndef CMBTL_PACKET_ID_MAP_H
#define CMBTL_PACKET_ID_MAP_H
namespace cmbtl {
    namespace packet {
    /**
     * @brief A hashmap based class that connects packet instructions with a unique ID
     * 
     * Each instruction's unique ID is useful for network requests so that 
     * it can be sent across as space efficiently as possible.
     * 
     * When using with network communication make sure both ends hold the same map.
     * When you add a instruction locally make sure that other side(s) do the same as well.
     * 
     * This class uses two hashmaps for O(1) access to either value given the other
     * 
     * @tparam NUM_SENSORS: The number of sensors that every packet instruction must contain
     */
    template <size_t NUM_SENSORS>
    class PacketInstructionsIDMap {
        private:
            std::unordered_map<PacketInstructions<NUM_SENSORS>, uint32_t> IDMap;

            std::unordered_map<uint32_t, PacketInstructions<NUM_SENSORS>> InstructionsMap;

            // Contains the ID of the last element added (or num elems - 1)
            uint32_t count;  

            inline void addID(const PacketInstructions<NUM_SENSORS>& instructions) {
                InstructionsMap[count] = instructions; 
            }

        public:
            /**
             * @brief Adds pairs of each instruction with a unique sequential ID to the hashmap
             * 
             * @param packetInstructions a {} enclosed list (std::initializer_list) of unique packets
            */ 
            inline PacketInstructionsIDMap(std::initializer_list<PacketInstructions<NUM_SENSORS>> packetInstructions) {
                // Begin assigning IDs at 0
                count = 0;

                // Assign instruction IDs in order (ex first instruction's ID is 0, second's is 1, etc.)
                for (const auto& instructions : packetInstructions) {
                    addPacketInstructions(instructions);
                }
            }

            /**
             * @brief Associates the instructions with a unique ID and adds the pair to the map, throws an exception if that packet is already present
             * 
             * @param packetInstructions: packet instructions to add to the map
             */
            inline void addPacketInstructions(const PacketInstructions<NUM_SENSORS>& packetInstructions) {
                // Throw error if packet is already present
                if (IDMap.find(packetInstructions) != IDMap.end()) {
                    std::stringstream msg;
                    msg << "Attempting to add packet instructions with index: " << count << " which is not unique (already present in map)!";
                    msg << " This will result in an unused ID number, aborting.";
                    throw std::invalid_argument(msg.str());
                } else {
                    // Insert pair to map
                    IDMap[packetInstructions] = count;

                    // Associate this ID with a packet instructions
                    addID(packetInstructions);

                    // Increment count for unique ID generation
                    count++;
                }
            }

            /**
             * @brief Returns the unique ID of the packetInstructions
             * 
             * @param packetInstructions: Instructions for which we would like to retrieve its unique associated ID
             * 
             * @return The unique ID of the packet instructions
             */
            inline uint32_t getID(const PacketInstructions<NUM_SENSORS>& packetInstructions) const {
                if (IDMap.find(packetInstructions) == IDMap.end()) {
                    std::stringstream msg;
                    msg << "The specified packet instructions:\n" << packetInstructions.to_string() << "\n\ndoes not exist!";
                    throw std::invalid_argument(msg.str());
                }

                return IDMap.at(packetInstructions);
            }

            /**
             * @brief returns the packet instructions associated with passed in ID
             * 
             * @param ID: ID of the instructions to return
             * 
             * @return: Packet instruction associated with the ID
             */
            inline const PacketInstructions<NUM_SENSORS>& getInstructionsByID(uint32_t ID) const {
                // If ID DNE
                if (InstructionsMap.find(ID) == InstructionsMap.end()) {
                    std::stringstream msg;
                    msg << "This ID number: " << ID << " is not associated with any packet instructions!";
                    throw std::invalid_argument(msg.str());
                }
                return InstructionsMap.at(ID);
            }

            /**
             * @brief Clears the hashmap
             * 
             * Use this with extreme caution. 
             * Consider that the other end of communication most hold 
             * the same pairs of packets and ID numbers as this class
             */
            inline void clear() {
                IDMap.clear();
                InstructionsMap.clear();
                count = 0;
            }

            inline size_t size() const { return IDMap.size(); }
            inline bool empty() const { return IDMap.empty(); }
        };
    }
}

#endif