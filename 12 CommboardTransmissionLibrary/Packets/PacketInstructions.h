#include "../misc/BinaryBuffer/BinaryBuffer.h"
#include "../boost/mp11/list.hpp"
#include "../boost/mp11/algorithm.hpp"
#include "Sensors.h"
#include "../MP.h"
#ifndef CMBTL_PACKET_INSTRUCTIONS_H
#define CMBTL_PACKET_INSTRUCTIONS_H
namespace cmbtl {
    template <bool... values>
    using PacketInstructions = boost::mp11::mp_list_c<bool, values...>;

    template<typename SensorInfo>
    using ExtractEncodedBitsize = boost::mp11::mp_int<SensorInfo::ENCODED_BIT_SIZE>;

    template<class instruction, typename BitSize>
    using ConditionalAdd = boost::mp11::mp_if<
     instruction,
     BitSize,
     boost::mp11::mp_int<0>
     >;

    template<typename Instructions, typename SensorInfoTuple>
    struct EncodedPacketBitSize {};

    template<bool... values, typename... SensorInfos>
    struct EncodedPacketBitSize <PacketInstructions<values...>, std::tuple<SensorInfos...>>  {

        //Make sure that values... and SensorInfos... have the same number of elems
        static_assert(sizeof...(values) == sizeof...(SensorInfos),
        "Number of instructions must match number of sensors!");

        //Create a list of each sensor's bitsize by transform the SensorInfos...
        using BitSizes = boost::mp11::mp_transform<
        ExtractEncodedBitsize,
        boost::mp11::mp_list<SensorInfos...>
        >;

        using InstructionList = PacketInstructions<values...>;

        using ConditionalBitSizes = boost::mp11::mp_transform<
        ConditionalAdd,
        InstructionList,
        BitSizes
        >;

        //Fold conditional bit sizes to get the sum
        static constexpr uint32_t value = boost::mp11::mp_fold<
        ConditionalBitSizes,
        boost::mp11::mp_int<0>,
        boost::mp11::mp_plus
        >::value;
    };
}
#endif