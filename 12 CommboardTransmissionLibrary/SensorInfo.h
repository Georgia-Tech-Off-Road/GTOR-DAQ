#include "SensorData.h"
#include "misc/BinaryBuffer/BinaryBuffer.h"
#ifndef CMBTL_SENSOR_INFO_H
#define CMBTL_SENSOR_INFO_H
namespace cmbtl {
    //--------------- Define Data Types for Different Sensors ---------------------------------
    //SV: Stored Value, value that is stored in the Data struct
    //RV: Real Value, value that we would commonly use this as. Ex. temp is stored most efficiently as a uint (we get that from the ADC),
    //      it will be encoded and decoded as a uint, but is proabably most useful as a float afterwards
    template <typename SV, typename RV>
    struct SensorInfo {
        //Numbers of bits that the SV will be stored as in the buffer
        constexpr uint32_t ENCODED_BIT_SIZE;
        //Takes data from data and stores it in buffer
        void (*encode)(SensorData& const data, BinaryBuffer& buffer);
        //Takes data from buffer and updates data
        void (*decode)(SensorData& data, BinaryBuffer& const buffer);
        //Convert to a more natural (and probably less space efficient) data type after data is sent over.
        RV(*convert)(SV);
    };
}
#endif