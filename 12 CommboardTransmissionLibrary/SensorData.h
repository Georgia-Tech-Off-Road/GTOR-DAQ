#include "SensorInfo.h"
#include "Sensors.h"
#include "boost/mp11/algorithm.hpp"
#include <tuple>
#include <type_traits>

#ifndef CMBTL_SENSOR_DATA_H
#define CMBTL_SENSOR_DATA_H
namespace cmbtl {

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
    //----------------------------------------------------------------------------------------------------------------------

    //------------- Helps to check if all elements in Ts.. match a certain type

    //Template declaration
    template<template<typename> class F, typename... Ts>
    struct all_are;

    //Base case, empty parameter pack Ts...
    template<template<typename> class F>
    struct all_are<F> : std::true_type {};

    //Parameters
    //F: Metafunction (like is_sensor_info) that checks whether T matches a certain type
    //
    //Recursively expands parameter pack Ts...
    template<template<typename> class F, typename T, typename... Ts>
    struct all_are<F, T, Ts...> : std::integral_constant<bool, F<T>::value && all_are<F,Ts...>::value> {};

    //----------------------------------------------------------------------------------------------------------------------

    //------------- Deduces whether a typename T is a tuple where each element has a type of SensorInfo
    template<typename T, typename = void>
    struct is_sensor_info_tuple : std::false_type {};

    template<typename... Ts>
    struct is_sensor_info_tuple<std::tuple<Ts...>, typename std::enable_if<all_are<is_sensor_info, Ts...>::value>::type> : std::true_type {};
    //----------------------------------------------------------------------------------------------------------------------
    template<typename>
    constexpr bool always_false = false;

    template<class T> using extract_SV = typename T::STORED_VALUE;

    template<typename T, typename Enable = void>
    struct SensorData {
        static_assert(always_false<T>, "Template parameter: SensorsTuple must be of type std::tuple<SensorInfo<SV, RV, ENCODE, DECODE, CONVERT>...>");
    };

    template<typename SensorsTuple>
    struct SensorData<SensorsTuple, typename std::enable_if<is_sensor_info_tuple<SensorsTuple>::value>::type>  {

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

        //Returns the data stored at the specified index by value
        template<size_t SensorIndex>
        inline SVTypeAt<SensorIndex> getData() const {
            return std::get<SensorIndex>(data);
        }

        //Returns a reference to data (to allow modification of more complex types)
        template<size_t SensorIndex>
        inline typename std::enable_if<SensorIndex < NUM_SENSORS, SVTypeAt<SensorIndex>&>::type getRefToData() {
            return std::get<SensorIndex>(data);
        }

        //Returns a reference to constant data (use case: returning larger data types that might take up too much stack space)
        template<size_t SensorIndex>
        inline typename std::enable_if<SensorIndex < NUM_SENSORS, SVTypeAt<SensorIndex> const &>::type getConstRefToData() const {
            return std::get<SensorIndex>(data);
        }

        //Sets data at a specific index. Not technically necessary, always can use getRefToData()
        template<size_t SensorIndex>
        inline typename std::enable_if<SensorIndex < NUM_SENSORS, void>::type setData(SVTypeAt<SensorIndex> newValue) {
            getRefToData<SensorIndex>() = newValue;
        }

    };
}
#endif
