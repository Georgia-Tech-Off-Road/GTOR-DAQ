#include <utility>
#include <vector>
#include "SensorData.h"
#include "Sensors.h"
#include "MP.h"
#include "Packets/PacketInstructions.h"
#include "boost/mp11/list.hpp"
#include "misc/BinaryBuffer/BinaryBuffer.h"

#ifndef CMBTL_PACKET_ENCODE_DECODE_H
#define CMBTL_PACKET_ENCODE_DECODE_H


namespace cmbtl {

    template<typename I, typename T, typename = void>
    inline std::pair<char*, size_t> createPacket(const T&) {
        static_assert(always_false<T>::value, "Template parameter: SensorDataType must be of type SensorData<T...>");
    }

    template<
    typename Instructions, 
    typename SensorDataType, 
    >
    inline std::pair<char*, size_t> createPacket<
    Instructions,
    SensorDataType,
    std::enable_if<is_sensor_data<SensorDataType>::value && boost::mp11::mp_is_list<Instructions>::value>::type
    >(SensorDataType const &data) {
        static_assert(std::tuple_Size<Instructions> == data::NUM_SENSORS);

        //How many bits the buffer should store, calculated from the packet instructions
        constexpr uint32_t bit_size = getTotalBitSize();

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