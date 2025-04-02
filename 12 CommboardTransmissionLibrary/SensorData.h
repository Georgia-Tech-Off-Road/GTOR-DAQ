#include "SensorInfo.h"
#include "Sensors.h"
#include "boost/mp11/algorithm.hpp"
#include "MP.h"
#include <tuple>
#include <exception>
#include <type_traits>

#ifndef CMBTL_SENSOR_DATA_H
#define CMBTL_SENSOR_DATA_H
namespace cmbtl {

    template<class T> using extract_SV = typename T::STORED_VALUE;

    template<typename T, typename Enable = void>
    struct SensorData {
        static_assert(always_false<T>, "Template parameter: SensorsTuple must be of type std::tuple<SensorInfo<SV, RV, ENCODE, DECODE, CONVERT>...>");
    };

    template<typename... SensorInfos>
    struct SensorData<std::tuple<SensorInfos...>, typename std::enable_if<is_sensor_info_tuple<std::tuple<SensorInfos...>>::value>::type>  {
        using SensorsTuple = std::tuple<SensorInfos...>;

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

        //Array container to allow runtime access to SensorInfo encode functions
        static constexpr std::array<void (*)(void), NUM_SENSORS> encodeFunctionTable = {SensorInfos::encode...};

        //Array container to allow runtime access to ENCODED_BIT_SIZE for each SensorInfo
        static constexpr std::array<uint32_t, NUM_SENSORS> sensorEncodeSizeTable = {SensorInfos::ENCODED_BIT_SIZE...};


        //TODO: Make nicer template substitution error messages for these functions
        //Returns the data stored at the specified index by value
        template<size_t SensorIndex>
        inline SVTypeAt<SensorIndex> getData() const {
            return std::get<SensorIndex>(data);
        }

        //Returns a reference to data (to allow modification of more complex types)
        template<size_t SensorIndex>
        inline SVTypeAt<SensorIndex>& getRefToData() {
            return std::get<SensorIndex>(data);
        }

        //Returns a reference to constant data (use case: returning larger data types that might take up too much stack space)
        template<size_t SensorIndex>
        inline SVTypeAt<SensorIndex> const & getConstRefToData() const {
            return std::get<SensorIndex>(data);
        }

        //Sets data at a specific index. Not technically necessary, always can use getRefToData()
        template<size_t SensorIndex>
        inline void setData(SVTypeAt<SensorIndex> newValue) {
            getRefToData<SensorIndex>() = newValue;
        }

        //Calls encode at SV
        template<size_t SensorIndex>
        inline void encodeData(BinaryBuffer &buffer) {
            static_assert(SensorIndex < NUM_SENSORS, "Template parameter: SensorIndex must be less than NUM_SENSORS!!!");
            SensorAt<SensorIndex>::encode(getData<SensorIndex>(), buffer);
        }

        private:
            //Runtime access to encode function of sensor infos
            inline void encodeDataRuntime(size_t sensor_index) {
                if (sensor_index > NUM_SENSORS) {
                    throw std::exception("Parameter: sensor_index must be less than NUM_SENSORS");
                }
                encodeFunctionTable[sensor_index]();
            }

            //Runtime access to size property of sensor infos
            inline uint32_t sensorEncodedBitSizeRuntime(size_t sensor_index) {
                if (sensor_index > NUM_SENSORS) {
                    throw std::exception("Parameter: sensor_index must be less than NUM_SENSORS");
                }
                return sensorEncodedBitSizeRuntime[sensor_index];
            }
    };

    template<typename T>
    struct is_sensor_data : std::false_type {};

    template <typename... Ts>
    struct is_sensor_data<SensorData<std::tuple<Ts...>>> : std::true_type {};

    
}
#endif
