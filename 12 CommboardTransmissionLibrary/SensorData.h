#include "SensorInfo.h"
#include "Sensors.h"
#include "boost/mp11/algorithm.hpp"
#include "MP.h"
#include <tuple>
#include <type_traits>

#ifndef CMBTL_SENSOR_DATA_H
#define CMBTL_SENSOR_DATA_H
namespace cmbtl {

    template<class T> using extract_SV = typename T::STORED_VALUE;

    template<typename T, typename Enable = void>
    struct SensorData {
        static_assert(always_false<T>, "Template parameter: SensorsTuple must be of type std::tuple<SensorInfo<SV, RV, ENCODE, DECODE, CONVERT>...>");
    };

    template<typename SensorInfoTuple>
    struct SensorData<SensorInfoTuple, typename std::enable_if<is_sensor_info_tuple<SensorInfoTuple>::value>::type>  {
        using SensorsTuple = SensorInfoTuple;

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

    template<typename T>
    struct is_sensor_data : std::false_type {};

    template <typename... Ts>
    struct is_sensor_data<SensorData<std::tuple<Ts...>>> : std::true_type {};

}
#endif
