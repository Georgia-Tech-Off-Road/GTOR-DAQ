#include "SensorData.h"
#include "gtest/gtest.h"
#include "DAQSensors.h"
#include "DAQPackets.h"
#include "Packets/PacketInstructions.h"
#include <iostream>

using cmbtl::SensorData;
using cmbtl::packet::PacketInstructions;
using cmbtl::DAQSensorDataType;
using cmbtl::SensorIndex;
using cmbtl::unsignedFloatEncode;
using cmbtl::unsignedFloatDecode;
// Tests specifically of implementations that DAQ will use

// * Disclaimer: these tests were created by AI but reviewed and modified by a human
TEST (DAQJSONSerializationTests, serializeToJSON) {
    DAQSensorDataType sensorData;
    std::cout << std::endl;
    std::cout << sensorData.serializeDataToJSON();
    std::cout << std::endl;
}