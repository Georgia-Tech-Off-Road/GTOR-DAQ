#ifndef CMBTL_SENSOR_INFO_H
#define CMBTL_SENSOR_INFO_H
namespace cmbtl {
    //--------------- Define Data Types for Different Sensors ---------------------------------
    //SV: Stored Value, value that is stored in the Data struct
    //EV: Encoded/ Compressed Value, value that data will be compressed to for networking
    //RV: Real Value, value that we would commonly use this as. Ex. temp is stored as a uint (we get that from the ADC), it will be encoded and decoded as a uint, but is proabably most useful as a float afterwards
    template <typename SV, typename EV, typename RV>
    struct SensorInfo {
        EV(*encode)(SV);
        SV(*decode)(EV);
        //Convert to a more natural (and probably less space efficient) data type after data is sent over.
        RV(*convert)(SV);
    };
}
#endif