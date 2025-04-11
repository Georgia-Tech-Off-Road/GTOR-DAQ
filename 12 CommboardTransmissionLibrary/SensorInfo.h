#include "misc/BinaryBuffer/BinaryBuffer.h"
#include "boost/endian/arithmetic.hpp"
#include <cmath>
#ifndef CMBTL_SENSOR_INFO_H
#define CMBTL_SENSOR_INFO_H

using cmbtl::BinaryBuffer;

    
namespace cmbtl {

    //Forward declare default functions
    template<typename SV, uint32_t BIT_SIZE>
    void defaultEncode(const SV&, BinaryBuffer&);

    template<typename SV, uint32_t BIT_SIZE>
    SV defaultDecode(const BinaryBuffer&);

    template<typename SV, typename RV>
    RV defaultConvert(const SV&);

    //--------------- Define Data Types for Different Sensors ---------------------------------
    /**
     * @brief Metaprograming type to store sensor information at compile time
     * 
     * Used by SensorData to generate code that handles encoding, decoding, storing, etc. 
     * 
     * @tparam SV: Stored value of the sensor data, the data type that it will be encoded and decoded as (and consequently stored in SensorData as)
     *  Stored value should always be big_endian for the serialization to work as expected across different platforms 
     *  (that's what boost::endian is for)
     * @tparam RV: Real value of the data ex. CVTTemp is a 15 bit value so we store it in a big_uint16_t (big for big endian) 
     * but later convert to a float (more natural and usable value for calculations)
     * @tparam BIT_SIZE: The total bit size the this sensor data will take up when encoded.
     * @tparam ENCODE: Reference to a function that writes the data to a binary buffer, should write exactly BIT_SIZE bits
     * @tparam DECODE: Reference to a function that retrives the data from a binary buffer, should read exactly BIT_SIZE bits
     * @tparam CONVERT: Function that converts from the stored value (SV) to the real value (RV)
     */
    template <typename SV, 
    typename RV, 
    uint32_t BIT_SIZE,
    void (*ENCODE)(const SV &data, BinaryBuffer&) = defaultEncode<SV, BIT_SIZE>,
    SV (*DECODE)(const BinaryBuffer&) = defaultDecode<SV, BIT_SIZE>,
    RV (*CONVERT)(const SV&) = defaultConvert<SV, RV>>
    struct SensorInfo {
        using STORED_VALUE = SV;
        using REAL_VALUE = RV;
        //Numbers of bits that the SV will be stored as in the buffer
        static constexpr uint32_t ENCODED_BIT_SIZE = BIT_SIZE;
        //Takes data from data and stores it in buffer
        static constexpr void (*encode)(const SV& data, BinaryBuffer& buffer) = ENCODE;
        //Takes data from buffer and updates data
        static constexpr SV (*decode)(BinaryBuffer const &buffer) = DECODE;
        //Convert to a more natural (and probably less space efficient) data type after data is sent over.
        static constexpr RV(*convert)(const SV&) = CONVERT;
    };

    //Forward declare default functions for the bool sensor (default functions would probably work but I'm paranoid)
    void defaultBoolEncode(const bool& val, BinaryBuffer& buffer);

    bool defaultBoolDecode(BinaryBuffer const &buffer);

    bool defaultBoolConvert(const bool& val);

    using DefaultBoolSensorInfo = SensorInfo<bool, bool, 1, defaultBoolEncode, defaultBoolDecode>;


    //----------------------- DEFINE COMMONLY USED FUNCTIONS ------------------------------------
    
    /**
     * @brief Default encoding function that should work with all basic types (ints, floats, etc).
     *  MAKE SURE SV IS BIG ENDIAN!
     * 
     * @tparam SV: Type of value to encode
     * @tparam BIT_SIZE: Number of bits from val to write to the buffer
     * 
     * @param val: Data of type SV to encode
     * 
     * @param buffer: Buffer to write NUM_BITS (Starting at the LSB) from val to
     * 
     */
    template<typename SV, uint32_t BIT_SIZE>
    void defaultEncode(const SV& val, BinaryBuffer& buffer) {
        buffer.writeValue<SV>(val, BIT_SIZE);
    }

    /**
     * @brief Default decoding function that should work with all basic types (ints, floats, etc).
     *  MAKE SURE SV IS BIG ENDIAN!
     * 
     * @tparam SV: Type of value to decode
     * @tparam BIT_SIZE: Number of bits to decode from the buffer
     * 
     * @param buffer: Binary buffer to load SV data from
     * 
     * @return Data of type SV that was read from buffer.
     *  Loads from the buffer and places the bits into the returned type starting at the LSB
     */
    template<typename SV, uint32_t BIT_SIZE>
    SV defaultDecode(BinaryBuffer const &buffer) {
        return buffer.readValue<SV>(BIT_SIZE);
    }

    /**
     * @brief: Default conversion function that should work with all basic types (ints, floats, etc).all_are
     * 
     * @tparam SV: Stored value type from SensorData
     * @tparam RV: Real value type that we'd like to convert to
     * 
     * @param val: Data from SensorData
     * 
     * @return: Val converted to type RV
     */
    template<typename SV, typename RV>
    RV defaultConvert(SV const &val) {
        return static_cast<RV>(val);
    }


    /**
     * @brief: Specialization of defaultConvert when both SV and RV are of the same type
     * Any decently intelligent compiler should optimize defaultConvert(val) to val
     * 
     * @tparam T: type of the both SV and RV
     * 
     * @param val: Data that we'd like to "convert"
     * 
     * @return: "Converted" data
     * 
     */
    template<typename T>
    T defaultConvert(T const &val) {
        return val;
    }

    // ---------------- VERSIONS OF ENCODE AND DECODE FOR BOOLS ---------------------------------------------------------
    // defaultEncode and defaultDecode would proabably be fine but I'm paranoid

    void defaultBoolEncode(const bool& val, BinaryBuffer& buffer) {
        if (val) {
            buffer.writeValue<uint8_t>(1, 1);
        }  else {
            buffer.writeValue<uint8_t>(0, 1);
        }
    }

    bool defaultBoolDecode(BinaryBuffer const &buffer) {
        return buffer.readValue<uint8_t>(1) == 1 ? true : false;
    }

    // ------------------------- VERSION OF ENCODE AND DECODE FOR FLOATS ----------------------------------------------------
    //These encode and decode function reduce a float value to an int, then multiply / divide it to shift the bits
    // around an imaginary decimal position so that they can be compressed to values that are between integers
    // ex. DECIMAL_BITS = 1 allows interval of 0.5, DECIMAL_BITS = 2 allows intervals of 0.25, 3 decimal bits 0.125 etc.
    template<size_t ENCODED_BIT_SIZE, uint8_t DECIMAL_BITS>
    void floatEncode(const float& val, BinaryBuffer& buffer) {
        //Multiply val by 2^(PRECISION_FACTOR) to get extra encoded precision
        boost::endian::big_uint64_t newVal = std::round(static_cast<uint64_t>(val * std::pow(2, DECIMAL_BITS)));

        buffer.writeValue<boost::endian::big_uint64_t>(newVal, ENCODED_BIT_SIZE);
    }

    template<size_t ENCODED_BIT_SIZE, uint8_t DECIMAL_BITS>
    float floatDecode(const BinaryBuffer& buffer) {
        boost::endian::big_uint64_t val = buffer.readValue<boost::endian::big_uint64_t>(ENCODED_BIT_SIZE);

        //Divide by 2^(PRECISION_FACTOR)
        return static_cast<float>(val / std::pow(2, DECIMAL_BITS));
    }

    // ----------------------------------------- META PROGRAMMING TEMPLATES ---------------------------------------------

    //------------- Helps deduces at compile time if variadic template arguments are actually all instances of SensorInfo
    template<typename T>
    struct is_sensor_info : std::false_type {};


    //If this template can be substituted then we know that T is of type SensorInfo!
    template <typename SV, 
    typename RV, 
    uint32_t BIT_SIZE,
    void (*ENCODE)(const SV &data, BinaryBuffer&),
    SV (*DECODE)(const BinaryBuffer&),
    RV (*CONVERT)(const SV&)>
    struct is_sensor_info<cmbtl::SensorInfo<SV, RV,  BIT_SIZE, ENCODE, DECODE, CONVERT>> : std::true_type {};
    //-------------------------------------------------------------------------------------------------------------------
}
#endif