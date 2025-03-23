#include <utility>
#include <vector>
#include "SensorData.h"
#include "Sensors.h"
#include "Packets/PacketInstructions.h"
#include "misc/BinaryBuffer/BinaryBuffer.h"

#ifndef CMBTL_PACKET_ENCODE_DECODE_H
#define CMBTL_PACKET_ENCODE_DECODE_H

namespace cmbtl {
    inline std::pair<char*, size_t> createPacket(SensorData& const data, const PacketInstructions instructions) {
        //How many bits the buffer should store, calculated from the packet instructions
        uint32_t bit_size = instructions.getPacketEncodedBitSize();

        //Create a binary buffer
        BinaryBuffer buffer(bit_size);

        uint32_t i = 0;
        for (SensorInfo SENSOR : SENSORS) {
            if (instructions.getBit(i) == 1) {
                SENSOR.encode(data, buffer);
            }
            i++;
        }
        //Size of the buffer, this time in bytes
        size_t buffer_size = buffer.getSize();
        return std::make_pair(buffer.getBuffer(), buffer_size);
    }
}

#endif