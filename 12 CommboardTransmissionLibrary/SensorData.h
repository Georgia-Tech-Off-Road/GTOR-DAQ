#include "SensorInfo.h"
#include "Sensors.h"
#include <tuple>
#include <type_traits>

#ifndef CMBTL_SENSOR_DATA_H
#define CMBTL_SENSOR_DATA_H
namespace cmbtl {

    //------------- Helps deduces at compile time if variadic template arguments are actually all instances of SensorInfo
    template<typename T>
    struct is_sensor_info : std::false_type {};

    template<typename SV, typename RV>
    struct is_sensor_info<cmbtl::SensorInfo<SV, RV>> : std::true_type {};
    //----------------------------------------------------------------------------------------------------------------------
    template <typename SensorIndicesWrapper, typename... CTSensors>
    struct SensorData {

        template<size_t N>
        using CompileSensorAt = typename std::tuple_element<N, std::tuple<CTSensors...>>::type;

        template<size_t N>
        using SensorInfoTypeAt = decltype(CompileSensorAt<N>::value);
    
        //Template to deduce stored value type
        template<size_t N>
        using SensorSV = typename SensorInfoTypeAt<N>::STORED_VALUE;

        //Template to deduce real value type (See SensorInfo.h for more info on both of these)
        template<size_t N>
        using SensorRV = typename SensorInfoTypeAt<N>::REAL_VALUE;

        //Time since data aquisition began
        SensorSV<0> millisec;
        //Is clutch engaged or not?
        unsigned int clutchEngaged : 1;
        //Pit stop requested?
        unsigned int pitRequested : 1;

        //Speed X
        float speed_x;
        //Speed Y
        float speed_y;
        //Speed Z
        float speed_z;

        //Position X
        float position_x;
        //Position Y
        float position_y;
        //Position Z
        float position_z;

        unsigned int brake_pressure_front : 12;
        unsigned int brake_pressure_back : 12;

        unsigned int RPM : 14;

        //Linear Accelerations
        float linear__acceleration_x;
        float linear_acceleration_y;
        float linear_acceleartion_z;

        //Continuously Variable Transmission Temp
        unsigned int cvt_temp : 12;

        //Brake Temperature (PLEASE RENAME THESE!!!!!!)
        //TODO: Rename brake temperatures
        unsigned int break_temp_front_left : 12;
        unsigned int break_temp_front_right : 12;
        unsigned int break_temp_back : 12;

        //Steering wheel rotation rotation
        unsigned int steering_rotation : 12;
    };
}
#endif
