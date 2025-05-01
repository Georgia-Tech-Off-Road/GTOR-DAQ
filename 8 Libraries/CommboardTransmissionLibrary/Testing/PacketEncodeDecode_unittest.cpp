#include "../SensorData.h"
#include "gtest/gtest.h"
#include "../DAQInfrastructure/DAQSensors.h"
#include "../Packets/PacketInstructions.h"
#include "../Packets/PacketScheme.h"
#include "../Packets/PacketInstructionsBuilder.h"
#include "../Packets/PacketIDMap.h"
#include <iostream>
using cmbtl::SensorData;
using cmbtl::packet::PacketInstructions;
using cmbtl::packet::createPacket;

using SensorsTuple = std::tuple<cmbtl::millisec::MILLI_SEC_SENSOR_INFO>;
using SensorDataType = SensorData<SensorsTuple>; 
TEST(SensorDataPacketTests, decodePacket) {
    SensorDataType sensorData;
    PacketInstructions<SensorDataType::NUM_SENSORS> instructions;
    instructions.set(0);

    BinaryBuffer buffer(cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE);
    buffer.writeValue<cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE>(451, cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE);

    sensorData.decodePacket(instructions, buffer);

    ASSERT_EQ(sensorData.getData<0>(), 451);
}

TEST(SensorDataPacketTests, encodePacket) {
    SensorDataType sensorData;
    PacketInstructions<SensorDataType::NUM_SENSORS> instructions;
    instructions.set(0);
    sensorData.setData<0>(427);

    BinaryBuffer buffer = sensorData.encodePacket(instructions);
    
    cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE actual = buffer.readValue<cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE>(cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE);
    ASSERT_EQ(actual, 427);

    instructions.set(0, false);
    ASSERT_TRUE(sensorData.encodePacket(instructions).getCapacity() == 0);
}

TEST(SensorDataPacketTests, createPacket) {
    PacketInstructions<10> instructions = createPacket<10>({0, 2, 4, 6, 8});

    //Make sure that all even packets are set to 1
    for (int i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            ASSERT_EQ(instructions[i], 1);
        } else {
            ASSERT_EQ(instructions[i], 0);
        }
    }
}

