#include "SensorData.h"
#include "gtest/gtest.h"
#include "../DAQInfrastructure/DAQSensors.h"
#include "Packets/PacketInstructions.h"
#include <iostream>
using cmbtl::SensorData;
using cmbtl::packet::PacketInstructions;

float testSensorConvert(const uint16_t& data) {
    return static_cast<float>(data) + 10.0f;
}

using TestSensorInfo = cmbtl::SensorInfo<uint16_t, float, 16, nullptr, nullptr, testSensorConvert>;

using SensorsTuple = std::tuple<cmbtl::millisec::MILLI_SEC_SENSOR_INFO>;
using SensorDataType = SensorData<SensorsTuple>; 

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

TEST(SensorDataTests, getRefToData) {
    SensorDataType sensorData;

    cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE& millisecRef = sensorData.getRefToData<0>();
    millisecRef = 42;

    ASSERT_EQ(sensorData.getRefToData<0>(), 42);
    ASSERT_TRUE((std::is_same<decltype(sensorData.getData<0>()), cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE>::value));
}

TEST(SensorDataTests, getConstRefToData) {
    SensorDataType sensorData;
    sensorData.setData<0>(47);
    cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE const  &constMillisecRef = sensorData.getConstRefToData<0>();

    ASSERT_EQ(constMillisecRef, 47);

}

TEST(SensorDataTests, encodeData) {
    SensorDataType sensorData;
    sensorData.setData<0>(251);
    BinaryBuffer buffer(cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE);
    sensorData.encodeSensorData<0>(buffer);
    cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE actual = buffer.readValue<cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE>(cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE);

    ASSERT_EQ(actual, 251);
}

TEST(SensorDataTests, encodedSizeTable) {
    std::array<uint32_t, SensorDataType::NUM_SENSORS> actual = SensorDataType::encodedSizeTable;
    std::array<uint32_t, SensorDataType::NUM_SENSORS> expected = {cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE};
    ASSERT_TRUE(actual == expected);
}


TEST(SensorDataTests, runtimeEncodeData) {
    SensorDataType sensorData;
    sensorData.setData<0>(251);
    BinaryBuffer buffer(cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE);
    sensorData.encodeDataRuntime(0, buffer);
    cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE actual = buffer.readValue<cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE>(cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE);

    ASSERT_EQ(actual, 251);
}

TEST(SensorDataTests, runtimeDecodeData) {
    SensorDataType sensorData;
    BinaryBuffer buffer(cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE);
    buffer.writeValue<cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE>(359, cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE);
    sensorData.decodeSensorData<0>(buffer);
    cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE actual = sensorData.getData<0>();
    ASSERT_EQ(actual, 359);
}


TEST(SensorDataTests, runtimeEncodeAndDecodeData) {
    SensorDataType sensorData;
    BinaryBuffer buffer(cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE);

    sensorData.setData<0>(789);
    sensorData.encodeDataRuntime(0, buffer);
    
    //Change value so we know if decode actually works
    sensorData.setData<0>(0);

    sensorData.decodeDataRuntime(0, buffer);

    ASSERT_EQ(sensorData.getData<0>(), 789);
}

TEST(SensorDataTests, convertData) {
    SensorData<std::tuple<cmbtl::millisec::MILLI_SEC_SENSOR_INFO, TestSensorInfo>> sensorData;

    sensorData.setData<0>(32);
    sensorData.setData<1>(32);

    std::tuple<uint32_t, float> convertedData = sensorData.convertData();

    ASSERT_EQ(std::get<0>(convertedData), 32);
    ASSERT_EQ(std::get<1>(convertedData), 42.0f);
}