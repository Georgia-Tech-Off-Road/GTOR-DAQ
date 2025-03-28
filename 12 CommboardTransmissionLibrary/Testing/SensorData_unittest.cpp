#include "SensorData.h"
#include "gtest/gtest.h"
#include "Sensors.h"
using cmbtl::SensorData;

TEST(SensorDataTests, Initialize) {
    SensorData<std::tuple<cmbtl::millisec::MILLI_SEC_SENSOR_INFO>> data;
}