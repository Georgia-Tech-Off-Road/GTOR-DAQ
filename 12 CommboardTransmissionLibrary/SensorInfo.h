#include "misc/BinaryBuffer/BinaryBuffer.h"
#ifndef CMBTL_SENSOR_INFO_H
#define CMBTL_SENSOR_INFO_H

using cmbtl::BinaryBuffer;

    
namespace cmbtl {
    //--------------- Define Data Types for Different Sensors ---------------------------------
    //SV: Stored Value, value that is stored in the Data struct
    //RV: Real Value, value that we would commonly use this as. Ex. temp is stored most efficiently as a uint (we get that from the ADC),
    //      it will be encoded and decoded as a uint, but is proabably most useful as a float afterwards
    template <typename SV, 
    typename RV, 
    uint32_t BIT_SIZE,
    void (*ENCODE)(SV data, BinaryBuffer& buffer),
    SV (*DECODE)(const BinaryBuffer& buffer),
    RV (*CONVERT)(SV)>
    struct SensorInfo {
        using STORED_VALUE = SV;
        using REAL_VALUE = RV;
        //Numbers of bits that the SV will be stored as in the buffer
        static constexpr uint32_t ENCODED_BIT_SIZE = BIT_SIZE;
        //Takes data from data and stores it in buffer
        static constexpr void (*encode)(SV data, BinaryBuffer& buffer) = ENCODE;
        //Takes data from buffer and updates data
        static constexpr SV (*decode)(const BinaryBuffer& buffer) = DECODE;
        //Convert to a more natural (and probably less space efficient) data type after data is sent over.
        static constexpr RV(*convert)(SV) = CONVERT;
    };

    // ----------------------------------------- META PROGRAMMING TEMPLATES ---------------------------------------------

    //------------- Helps deduces at compile time if variadic template arguments are actually all instances of SensorInfo
    template<typename T>
    struct is_sensor_info : std::false_type {};


    //If this template can be substituted then we know that T is of type SensorInfo!
    template <typename SV, 
    typename RV, 
    uint32_t BIT_SIZE,
    void (*ENCODE)(SV data, BinaryBuffer& buffer),
    SV (*DECODE)(const BinaryBuffer& buffer),
    RV (*CONVERT)(SV)>
    struct is_sensor_info<cmbtl::SensorInfo<SV, RV,  BIT_SIZE, ENCODE, DECODE, CONVERT>> : std::true_type {};
    //-------------------------------------------------------------------------------------------------------------------
}
#endif