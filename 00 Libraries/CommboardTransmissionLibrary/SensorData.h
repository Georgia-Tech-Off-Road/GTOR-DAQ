#include "SensorInfo.h"
#include "boost_mp11_algorithm.hpp"
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


    // ------------------------------------------ TEMPLATE METAPROGRAMMING ----------------------------------------
    template<class T> using extract_SV = typename T::STORED_VALUE;

    template<class T> using extract_RV = typename T::REAL_VALUE;

    /**
     * @brief Stores and handles serialization of the sensor data specified in the tuple provided by template parameter.
     * 
     * Uses metaprogramming to automate much of the boilerplate serialization code. 
     * 
     * @tparam SensorInfoTuple: A tuple of SensorInfo<...>  types where <...> are the provided arguments that describe each sensor's capabiilities.
     * See SensorInfo.h for more details.
     * 
     * @tparam Enable: Used for metaprogramming, ignore
     */
    template<typename SensorInfoTuple, typename Enable = void>
    struct SensorData {
        static_assert(always_false<SensorInfoTuple>, "Template parameter: SensorsTuple must be of type std::tuple<SensorInfo<SV, RV, BIT_SIZE, ENCODE, DECODE, CONVERT>...>");
    };

    // ------------------------------------------------------------------------------------------------------------

    /**
     * @brief The definition of SensorData when we've confirmed that SensorInfoTuple is of type std::tuple<SensorInfo<...>...>
     */
    template<typename... SensorInfos>
    struct SensorData<std::tuple<SensorInfos...>, typename std::enable_if<is_sensor_info_tuple<std::tuple<SensorInfos...>>::value>::type>  {
        // Name declaration for ease of use
        using SensorsTuple = std::tuple<SensorInfos...>;

        // Number of sensors passed in via template parameter SensorInfoTuple
        static constexpr size_t NUM_SENSORS = std::tuple_size<SensorsTuple>::value;

        /**
         * Metaprogramming type that retrieves the type value of the sensor at index N
         * 
         * @tparam N: Index of the sensor, zero indiced
         */
        template<size_t N>
        using SensorAt = typename std::tuple_element<N, SensorsTuple>::type;

        /**
         * @brief Metaprogramming type that retrieves the stored value of the sensor at index N
         * 
         * @tparam N: Index of the sensor, zero indiced
         */
        template<size_t N>
        using SVTypeAt = typename SensorAt<N>::STORED_VALUE;
    
        /**
         * @brief Metaprogramming type that retrieves the real value of the sensor at index N
         * 
         * @tparam N: Index of the sensor, zero indiced
         */
        template<size_t N>
        using RVTypeAt = typename SensorAt<N>::REAL_VALUE;

        
        /**
         * @brief Extracts the stored value from each sensor to create a tuple of stored values
         */
        using SVTupleType = boost::mp11::mp_transform<extract_SV, SensorsTuple>;

        /**
         * @brief Extracts the real value from each sensor to create a tuple of real values
         */
        using RVTupleType = boost::mp11::mp_transform<extract_RV, SensorsTuple>;

        /***
         * @brief Stores the data from each sensor. VIP variable.
         */
        SVTupleType data;

        //Array of pointers to each sensor's encode function to allow runtime access
        const std::array<void (SensorData::*)(BinaryBuffer&), NUM_SENSORS> encodeFunctionTable;

        // Array of pointers to each sensor's decode function to allow runtime access
        const std::array<void (SensorData::*)(BinaryBuffer const &), NUM_SENSORS> decodeFunctionTable;

        // Metaprogramming type that create an array of the encoded bit size values to allow for (easy) runtime access
        // Default version when provided with any type
        template<class T>
        struct create_encode_bit_size_arr {
            static constexpr std::array<uint32_t, NUM_SENSORS> value = {};
        };

        // Metaprogramming type that create an array of the encoded bit size values to allow for (easy) runtime access
        // Specialization that actually works
        template<size_t... I>
        struct create_encode_bit_size_arr<boost::mp11::index_sequence<I...>> {
            static constexpr std::array<uint32_t, NUM_SENSORS> value = {(SensorAt<I>::ENCODED_BIT_SIZE)...};
        };

        // Array container to allow for (easy) runtime access to ENCODED_BIT_SIZE from each sensor
        static constexpr std::array<uint32_t, NUM_SENSORS> encodedSizeTable = create_encode_bit_size_arr<boost::mp11::make_index_sequence<NUM_SENSORS>>::value;
        
        /**
         * @brief Returns data stored at index: SensorIndex
         * 
         * If a variable is declare as follows: @n
         *  SensorData<std::tuple<SensorInfo<big_uint32_t, big_uint32_t, 32>, SensorInfo<bool, bool, 1>> data; @n
         *  Then data.get<1>() returns a boolean, and data.get<0>() returns a big_uint32_t.
         * 
         * @tparam SensorIndex: Index to retrieve data from. @n
         * This index corresponds to the order that SensorInfos are passed into SensorData.
         * 
         * @return Current sensor data of type SensorInfo::STORED_VALUE (Corresponding to order passed into std::tuple<SensorInfo...>).
         */
        template<size_t SensorIndex>
        inline SVTypeAt<SensorIndex> getData() const {
            return std::get<SensorIndex>(data);
        }

        /**
         * @brief Identical to SensorData::getData() but returns a reference to the data
         * 
         * Because this method returns by reference it allows for modification of the data via the returned value
         * 
         * @tparam SensorIndex: Index of the data
         * 
         * @return Reference to the data at SensorIndex
         */
        template<size_t SensorIndex>
        inline SVTypeAt<SensorIndex>& getRefToData() {
            return std::get<SensorIndex>(data);
        }

        /**
         * @brief Identical to SensorData::getData() but returns a constant reference to data instead of by value
         * 
         * This is (most likely) a pointless function (most compilers optimize from pass by value to reference).
         * Nonetheless it is provided. 
         * 
         * @tparam SensorIndex: Index of the data
         * 
         * @return A const reference to data at SensorIndex
         */
        template<size_t SensorIndex>
        inline SVTypeAt<SensorIndex> const & getConstRefToData() const {
            return std::get<SensorIndex>(data);
        }


        /**
         * @brief Sets the data at index SensorIndex to newValue
         * 
         * Uses getRefToData() to set the data at the specified index to value newValue
         * 
         * @tparam SensorIndex: Index to retrieve data from
         * 
         * @param newValue: New sensor value
         * 
         */
        template<size_t SensorIndex>
        inline void setData(SVTypeAt<SensorIndex> newValue) {
            getRefToData<SensorIndex>() = newValue;
        }

        /**
         * @brief Encodes the data of the sensor at index SensorIndex to the buffer
         * 
         * @param buffer: Buffer to encode data to
         * 
         * @tparam SensorIndex: Index of the sensor to encode data from
         */
        template<size_t SensorIndex>
        inline void encodeSensorData(BinaryBuffer &buffer) {
            static_assert(SensorIndex < NUM_SENSORS, "Template parameter: SensorIndex must be less than NUM_SENSORS!!!");
            SensorAt<SensorIndex>::encode(getData<SensorIndex>(), buffer);
        }

        /**
         * @brief Decodes and updates the data of the sensor at index SensorIndex from buffer
         * 
         * @param buffer: Buffer to retrieve data from
         * 
         * @tparam SensorIndex: Index of the sensor to update data
         */
        template<size_t SensorIndex>
        inline void decodeSensorData(BinaryBuffer const &buffer) {
            static_assert(SensorIndex < NUM_SENSORS, "Template parameter: SensorIndex must be less than NUM_SENSORS!!!");
            setData<SensorIndex>(SensorAt<SensorIndex>::decode(buffer));
        }

        /**
         * @brief Encodes a packet to a buffer given instructions
         * 
         * @param instructions: Packet instructions for which sensors to encode
         * 
         * @tparam InstructionsSensorCount: Total number of sensors that the instructions contain (included AND excluded)
         * 
         * @return A BinaryBuffer which contains the encoded data
         */
        template<size_t InstructionsSensorCount>
        inline BinaryBuffer encodePacket(packet::PacketInstructions<InstructionsSensorCount> const &instructions) {
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

        /**
         * @brief Decodes a packet from a buffer given instructions, updates sensor data
         * 
         * @param instructions: Instructions on which sensor to decode
         * Must be same as was called with encodePacket() to function properly
         * 
         * @param buffer: A binary buffer which contains the encoded data that we wish to decode
         * 
         * @tparam InstructionsSensorCount: Number of sensors that the instructions contain (included AND excluded)
         * 
         */
        template<size_t InstructionsSensorCount>
        inline void decodePacket(packet::PacketInstructions<InstructionsSensorCount> const &instructions, BinaryBuffer const &buffer) {
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

        /**
         * @brief converts the elements in data to their "real values" (see SensorInfo.h for a definition)
         * 
         * @returns A tuple of the sensor data converted to their real values (see SensorInfo.h)
         */
        inline RVTupleType convertData() const {
            return convertDataImpl(boost::mp11::make_index_sequence<NUM_SENSORS>{});
        }

        /**
         * @brief Serializes  data to JSON format
         * Serializes all current data to JSON format by calling each sensor's serializeToJSON() (See SensorInfo.h)
         */
        std::string serializeDataToJSON() const {
            std::stringstream ss;
            ss << "{" << "\n";
            serializeDataToJSONImpl(boost::mp11::make_index_sequence<NUM_SENSORS>{}, ss);
            ss << "}";

            return ss.str().c_str();

        }

        /**
         * @brief Return the data at index N converted to its "real value"
         */
        template <size_t N>
        inline RVTypeAt<N> convertedDataAt() const {
            static_assert(N < NUM_SENSORS, "Template parameter: N must be less than NUM_SENSORS!");
            return SensorAt<N>::convert(getData<N>());
        }

        /**
         * @brief Constructor which initializes crucial variables to default values
         */
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
            static inline uint32_t const packetEncodedBitSize(packet::PacketInstructions<InstructionsSensorCount> const &instructions) { 
                uint32_t bit_size = 0;
                for (int i = 0; i < instructions.size(); i++) {
                    if (instructions[i] == true) {
                        bit_size += encodedSizeTable.at(i);
                    }
                }
                return bit_size;
            }

            template <size_t... Is>
            inline RVTupleType convertDataImpl(boost::mp11::index_sequence<Is...>) const {
                return std::make_tuple(convertedDataAt<Is>()...);
            }

            template<size_t... Is>
            inline void serializeDataToJSONImpl(boost::mp11::index_sequence<Is...>, std::stringstream& ss) const {
                //Dummy array to call methods
                int dummy[] = {(seralizeSensorToJSON<Is>(convertedDataAt<Is>(), ss, !(Is < NUM_SENSORS - 1)), 0)...};
                (void)dummy;
            }

            /**
             * Calls serialiseToJSON on sensor N
             */
            template<size_t N>
            inline void seralizeSensorToJSON(RVTypeAt<N> convertedData, std::stringstream& ss, bool isFinal) const {
                ss << "\t";
                SensorAt<N>::serializeToJSON(convertedData, ss);
                if (!isFinal) {
                    ss << ",";
                } 
                ss << "\n";
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
