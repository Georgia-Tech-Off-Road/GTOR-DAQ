#include "misc/BinaryBuffer/BinaryBuffer.h"
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
    //SV: Stored Value, value that is stored in the Data struct
    //RV: Real Value, value that we would commonly use this as. Ex. temp is stored most efficiently as a uint (we get that from the ADC),
    //      it will be encoded and decoded as a uint, but is proabably most useful as a float afterwards
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

    //Forward declare functions for a 

    void defaultBoolEncode(const bool& val, BinaryBuffer& buffer);

    bool defaultBoolDecode(BinaryBuffer const &buffer);

    bool defaultBoolConvert(const bool& val);

    using DefaultBoolSensorInfo = SensorInfo<bool, bool, 1, defaultBoolEncode, defaultBoolDecode, defaultBoolConvert>;


    //----------------------- DEFINE COMMONLY USED FUNCTIONS ------------------------------------
    
    template<typename SV, uint32_t BIT_SIZE>
    void defaultEncode(const SV& val, BinaryBuffer& buffer) {
        buffer.writeValue<SV>(val, BIT_SIZE);
    }

    template<typename SV, uint32_t BIT_SIZE>
    SV defaultDecode(BinaryBuffer const &buffer) {
        return buffer.readValue<SV>(BIT_SIZE);
    }

    template<typename SV, typename RV>
    RV defaultConvert(SV const &val) {
        return static_cast<RV>(val);
    }

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

    bool defaultBoolConvert(const bool& val) {
        return val;
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