#include <tuple>

//Metaprogramming helpers
#ifndef CMBTL_MP_H
#define CMBTL_MP_H

namespace cmbtl {
    template<typename>
    constexpr bool always_false = false;

    //------------- Helps to check if all elements in Ts.. match a certain type
    //Template declaration
    template<template<typename> class F, typename... Ts>
    struct all_are;

    //Base case, empty parameter pack Ts...
    template<template<typename> class F>
    struct all_are<F> : std::true_type {};

    //Parameters
    //F: Metafunction (like is_sensor_info) that checks whether T matches a certain type
    //Recursively expands parameter pack Ts...
    template<template<typename> class F, typename T, typename... Ts>
    struct all_are<F, T, Ts...> : std::integral_constant<bool, F<T>::value && all_are<F,Ts...>::value> {};

    #ifdef CMBTL_SENSOR_INFO_H
    //------------- Deduces whether a typename T is a tuple where each element has a type of SensorInfo
    template<typename T, typename = void>
    struct is_sensor_info_tuple : std::false_type {};

    template<typename... Ts>
    struct is_sensor_info_tuple<std::tuple<Ts...>, typename std::enable_if<all_are<is_sensor_info, Ts...>::value>::type> : std::true_type {};
    //----------------------------------------------------------------------------------------------------------------------
    #endif
}    

#endif