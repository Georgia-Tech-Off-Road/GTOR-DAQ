#include "SensorData.h"
#include "gtest/gtest.h"
#include "Sensors.h"
#include <iostream>
using cmbtl::SensorData;

using SensorsTuple = std::tuple<cmbtl::millisec::MILLI_SEC_SENSOR_INFO>;
using SensorDataType = SensorData<SensorsTuple>; 

template<typename T> struct ShowType;

TEST(SensorDataTests, Initialize) {
    SensorData<std::tuple<cmbtl::millisec::MILLI_SEC_SENSOR_INFO>> sensorData;
}

TEST(SensorDataTests, SensorAt) {
    ASSERT_TRUE((std::is_same<typename SensorDataType::SensorAt<0>, cmbtl::millisec::MILLI_SEC_SENSOR_INFO>::value));
}

TEST(SensorDataTests, SVTypeAt) {
    ASSERT_TRUE((std::is_same<typename SensorDataType::SVTypeAt<0>, cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE>::value));
}

TEST(SensorDataTests, SVTupleType) {
    ASSERT_TRUE((std::is_same<std::tuple<cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE>, 
        SensorDataType::SVTupleType>::value));
}

TEST(SensorDataTests, getData) {
    SensorDataType sensorData;

    sensorData.setData<0>(35);

    ASSERT_EQ(sensorData.getData<0>(), 35);
    ASSERT_TRUE((std::is_same<decltype(sensorData.getData<0>()),cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE>::value));

    
}