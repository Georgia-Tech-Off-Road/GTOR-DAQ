#include "SensorData.h"
#include "gtest/gtest.h"
#include "Sensors.h"
using cmbtl::SensorData;

TEST(SensorDataTests, Initialize) {
    SensorData<cmbtl::millisec::MILLI_SEC_SENSOR_INFO> data;
}