#include "SensorInfo.h"
#include "Sensors/AllSensors.h"
#include "boost/mp11/algorithm.hpp"
#include "MP.h"
#include "Packets/PacketInstructions.h"
#include <tuple>
#include <sstream>
#include <string>
#include <exception>
#include <type_traits>
#include <stdexcept>
#include <memory>

#ifndef CMBTL_SENSOR_DATA_H
#define CMBTL_SENSOR_DATA_H
namespace cmbtl {


    template<class T> using extract_SV = typename T::STORED_VALUE;

    template<typename T, typename Enable = void>
    struct SensorData {
        static_assert(always_false<T>, "Template parameter: SensorsTuple must be of type std::tuple<SensorInfo<SV, RV, BIT_SIZE, ENCODE, DECODE, CONVERT>...>");
    };

    template<typename... SensorInfos>
    struct SensorData<std::tuple<SensorInfos...>, typename std::enable_if<is_sensor_info_tuple<std::tuple<SensorInfos...>>::value>::type>  {

        using SensorsTuple = std::tuple<SensorInfos...>;

        static constexpr size_t NUM_SENSORS = std::tuple_size<SensorsTuple>::value;

        template<size_t N>
        using SensorAt = typename std::tuple_element<N, SensorsTuple>::type;

        template<size_t N>
        using SVTypeAt = typename SensorAt<N>::STORED_VALUE;
    
        template<size_t N>
        using RVTypeAt = typename SensorAt<N>::REAL_VALUE;

        
        // First apply the transform to get the types
        using SVTupleType = boost::mp11::mp_transform<extract_SV, SensorsTuple>;

        SVTupleType data;
        //Array container to allow runtime access to SensorInfo encode functions
        const std::array<void (SensorData::*)(BinaryBuffer&), NUM_SENSORS> encodeFunctionTable;

        const std::array<void (SensorData::*)(BinaryBuffer const &), NUM_SENSORS> decodeFunctionTable;

        //Create an array of the encoded bit size values for runtime access
        template<class T>
        struct create_encode_bit_size_arr {
            static constexpr std::array<uint32_t, NUM_SENSORS> value = {};
        };

        template<size_t... I>
        struct create_encode_bit_size_arr<boost::mp11::index_sequence<I...>> {
            static constexpr std::array<uint32_t, NUM_SENSORS> value = {(SensorAt<I>::ENCODED_BIT_SIZE)...};
        };

        //Array container to allow runtime access to ENCODED_BIT_SIZE for each SensorInfo
        static constexpr std::array<uint32_t, NUM_SENSORS> encodedSizeTable = create_encode_bit_size_arr<boost::mp11::make_index_sequence<NUM_SENSORS>>::value;


        //TODO: Make nicer template substitution error messages for these functions
        //Returns the data stored at the specified index by value
        template<size_t SensorIndex>
        inline SVTypeAt<SensorIndex> getData() const {
            return std::get<SensorIndex>(data);
        }

        //Returns a reference to data (to allow modification of more complex types)
        template<size_t SensorIndex>
        inline SVTypeAt<SensorIndex>& getRefToData() {
            return std::get<SensorIndex>(data);
        }

        //Returns a reference to constant data (use case: returning larger data types that might take up too much stack space)
        template<size_t SensorIndex>
        inline SVTypeAt<SensorIndex> const & getConstRefToData() const {
            return std::get<SensorIndex>(data);
        }

        //Sets data at a specific index. Not technically necessary, always can use getRefToData()
        template<size_t SensorIndex>
        inline void setData(SVTypeAt<SensorIndex> newValue) {
            getRefToData<SensorIndex>() = newValue;
        }

        //Calls encode at SV
        template<size_t SensorIndex>
        inline void encodeSensorData(BinaryBuffer &buffer) {
            static_assert(SensorIndex < NUM_SENSORS, "Template parameter: SensorIndex must be less than NUM_SENSORS!!!");
            SensorAt<SensorIndex>::encode(getData<SensorIndex>(), buffer);
        }

        //Updates data<SensorIndex> from value in binary buffer by calling SensorAt<SensorIndex>::decode
        template<size_t SensorIndex>
        inline void decodeSensorData(BinaryBuffer const &buffer) {
            static_assert(SensorIndex < NUM_SENSORS, "Template parameter: SensorIndex must be less than NUM_SENSORS!!!");
            setData<SensorIndex>(SensorAt<SensorIndex>::decode(buffer));
        }

        template<size_t InstructionsSensorCount>
        inline BinaryBuffer encodePacket(PacketInstructions<InstructionsSensorCount> const &instructions) {
            if (InstructionsSensorCount > NUM_SENSORS) {
                std::ostringstream err;
                err << "The number of sensors specified in parameter instructions: " 
                << instructions.to_string() << " (" << instructions.size() << ")" 
                << std::endl
                << "is greater than NUM_SENSORS of SensorData: " << NUM_SENSORS;
                throw std::invalid_argument(err.str());
            }
            //Calculate total number of bits needed for the packet
            const uint32_t total_num_encoded_bits = packetEncodedBitSize(instructions);

            //Experimentally (with unit tests) clearBuffer() takes a looong time (~150 ms) for no apparent reason
            // luckily (because we will write over every bit in buffer anyway) we set should_clear_buffer to false in the constructor
            BinaryBuffer buffer(total_num_encoded_bits, false);

            //Iterate through instructions
            for (size_t i = 0; i < instructions.size(); i++) {
                if (instructions[i] == true) {
                    //If instructions[SensorIndex] is true, then we call encodeDataRuntime(sensorIndex, buffer)
                    encodeDataRuntime(i, buffer);
                }
            }

            return buffer;
        }

        template<size_t InstructionsSensorCount>
        inline void decodePacket(PacketInstructions<InstructionsSensorCount> const &instructions, BinaryBuffer const &buffer) {
            if (InstructionsSensorCount > NUM_SENSORS) {
                std::ostringstream err;
                err << "The number of sensors specified in parameter instructions: " 
                << instructions.to_string() << " (" << instructions.size() << ")" 
                << std::endl
                << "is greater than NUM_SENSORS of SensorData: " << NUM_SENSORS;
                throw std::invalid_argument(err.str());
            }

            //Calculate total number of bits needed for the packet
            const uint32_t total_num_encoded_bits = packetEncodedBitSize(instructions);

            if (total_num_encoded_bits != buffer.getCapacity() - buffer.getReadCursorPos()) {
                std::ostringstream err;
                err << "The calculated bit size of packet (from SensorInfos): " << total_num_encoded_bits
                << ". Does not equal the capacity of function parameter buffer: " << buffer.getCapacity();
                throw std::invalid_argument(err.str());
            }

            //Iterate through instructions
            for (size_t i = 0; i < instructions.size(); i++) {
                if (instructions[i] == true) {
                    decodeDataRuntime(i, buffer);
                }
            }
        }

        SensorData() :
        encodeFunctionTable(createEncodeDataTable(boost::mp11::make_index_sequence<NUM_SENSORS>{})),
        decodeFunctionTable(createDecodeDataTable(boost::mp11::make_index_sequence<NUM_SENSORS>{}))
        {};

        //TODO: Make private again after testing
        public:
            //Runtime access to encode function of sensor infos
            inline void encodeDataRuntime(size_t sensor_index, BinaryBuffer &buffer) {
                if (sensor_index >= NUM_SENSORS) {
                    throw std::invalid_argument("Parameter: sensor_index must be less than NUM_SENSORS");
                }
                //Call encodeSensorData
                (this->*encodeFunctionTable[sensor_index])(buffer);
            }

            inline void decodeDataRuntime(size_t sensor_index, BinaryBuffer const &buffer) {
                if (sensor_index >= NUM_SENSORS) {
                    throw std::invalid_argument("Parameter: sensor_index must be less than NUM_SENSORS");
                }
                //Call decodeSensorData
                (this->*decodeFunctionTable[sensor_index])(buffer);
            }

            template<size_t InstructionsSensorCount>
            static inline uint32_t const packetEncodedBitSize(PacketInstructions<InstructionsSensorCount> const &instructions) { 
                uint32_t bit_size = 0;
                for (int i = 0; i < instructions.size(); i++) {
                    if (instructions[i] == true) {
                        bit_size += encodedSizeTable.at(i);
                    }
                }
                return bit_size;
            }

            //Yes this could be done at compile time, but the solution in C++11 is... nasty
            //So we will just bite the bullet and instantiate it at runtime (simpler and easier to maintain)
            template<size_t... Is>
            static constexpr inline std::array<void (SensorData::*)(BinaryBuffer&), NUM_SENSORS> createEncodeDataTable(boost::mp11::index_sequence<Is...>) {
                return {&SensorData::template encodeSensorData<Is>...};
            }

            //Similar scenario to createEncodeDataTable
            template<size_t ... Is>
            static constexpr inline std::array<void (SensorData::*)(BinaryBuffer const &), NUM_SENSORS> createDecodeDataTable(boost::mp11::index_sequence<Is...>) {
                return {&SensorData::template decodeSensorData<Is>...};
            }
    };

    template<typename T>
    struct is_sensor_data : std::false_type {};

    template <typename... Ts>
    struct is_sensor_data<SensorData<std::tuple<Ts...>>> : std::true_type {};

    
}
#endif
