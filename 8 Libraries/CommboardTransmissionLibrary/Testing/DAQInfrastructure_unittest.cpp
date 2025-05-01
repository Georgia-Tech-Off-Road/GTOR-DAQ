#include "../SensorData.h"
#include "gtest/gtest.h"
#include "../DAQInfrastructure/DAQSensors.h"
#include "../DAQInfrastructure/DAQPackets.h"
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

// MILLISEC TESTS
TEST(DAQSensorDataPacketTests, encodeDecodeCycle_Millisec) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::MILLI_SEC);
    sensorData1.setData<SensorIndex::MILLI_SEC>(1234);

    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify data was preserved through encode/decode cycle
    ASSERT_EQ(sensorData2.getData<SensorIndex::MILLI_SEC>(), 1234);
    ASSERT_EQ(sensorData1.getData<SensorIndex::MILLI_SEC>(), sensorData2.getData<SensorIndex::MILLI_SEC>());
}

TEST(DAQSensorDataPacketTests, encodePacket_Millisec) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::MILLI_SEC);
    sensorData.setData<SensorIndex::MILLI_SEC>(1234);

    BinaryBuffer buffer = sensorData.encodePacket(instructions);
    
    cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE actual = 
        buffer.readValue<cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE>(
            cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE);
    ASSERT_EQ(actual, 1234);
}

TEST(DAQSensorDataPacketTests, decodePacket_Millisec) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::MILLI_SEC);

    BinaryBuffer buffer(cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE);
    buffer.writeValue<cmbtl::millisec::MILLI_SEC_SENSOR_INFO::STORED_VALUE>(
        2345, cmbtl::millisec::MILLI_SEC_SENSOR_INFO::ENCODED_BIT_SIZE);

    sensorData.decodePacket(instructions, buffer);

    ASSERT_EQ(sensorData.getData<SensorIndex::MILLI_SEC>(), 2345);
}

// CLUTCH_ENGAGED TESTS
TEST(DAQSensorDataPacketTests, encodeDecodeCycle_ClutchEngaged) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::CLUTCH_ENGAGED);
    sensorData1.setData<SensorIndex::CLUTCH_ENGAGED>(true);

    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify data was preserved through encode/decode cycle
    ASSERT_TRUE(sensorData2.getData<SensorIndex::CLUTCH_ENGAGED>());
    ASSERT_EQ(sensorData1.getData<SensorIndex::CLUTCH_ENGAGED>(), sensorData2.getData<SensorIndex::CLUTCH_ENGAGED>());
}

TEST(DAQSensorDataPacketTests, encodePacket_ClutchEngaged) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::CLUTCH_ENGAGED);
    sensorData.setData<SensorIndex::CLUTCH_ENGAGED>(true);

    BinaryBuffer buffer = sensorData.encodePacket(instructions);
    
    bool actual = buffer.readValue<uint8_t>(1) == 1;
    ASSERT_TRUE(actual);
}

TEST(DAQSensorDataPacketTests, decodePacket_ClutchEngaged) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::CLUTCH_ENGAGED);

    BinaryBuffer buffer(1);
    buffer.writeValue<uint8_t>(1, 1);

    sensorData.decodePacket(instructions, buffer);

    ASSERT_TRUE(sensorData.getData<SensorIndex::CLUTCH_ENGAGED>());
}

// PIT_REQUESTED TESTS
TEST(DAQSensorDataPacketTests, encodeDecodeCycle_PitRequested) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::PIT_REQUESTED);
    sensorData1.setData<SensorIndex::PIT_REQUESTED>(true);

    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify data was preserved through encode/decode cycle
    ASSERT_TRUE(sensorData2.getData<SensorIndex::PIT_REQUESTED>());
    ASSERT_EQ(sensorData1.getData<SensorIndex::PIT_REQUESTED>(), sensorData2.getData<SensorIndex::PIT_REQUESTED>());
}

TEST(DAQSensorDataPacketTests, encodePacket_PitRequested) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::PIT_REQUESTED);
    sensorData.setData<SensorIndex::PIT_REQUESTED>(true);

    BinaryBuffer buffer = sensorData.encodePacket(instructions);
    
    bool actual = buffer.readValue<uint8_t>(1) == 1;
    ASSERT_TRUE(actual);
}

TEST(DAQSensorDataPacketTests, decodePacket_PitRequested) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::PIT_REQUESTED);

    BinaryBuffer buffer(1);
    buffer.writeValue<uint8_t>(1, 1);

    sensorData.decodePacket(instructions, buffer);

    ASSERT_TRUE(sensorData.getData<SensorIndex::PIT_REQUESTED>());
}

// SPEED TESTS
TEST(DAQSensorDataPacketTests, encodeDecodeCycle_Speed) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::SPEED);
    
    cmbtl::speed::Speed testSpeed;
    testSpeed.x = 12.5;
    testSpeed.y = 23.7;
    testSpeed.z = 34.9;
    
    sensorData1.setData<SensorIndex::SPEED>(testSpeed);

    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify data was preserved through encode/decode cycle
    cmbtl::speed::Speed decodedSpeed = sensorData2.getData<SensorIndex::SPEED>();
    ASSERT_FLOAT_EQ(decodedSpeed.x, 12.5);
    ASSERT_FLOAT_EQ(decodedSpeed.y, 23.7);
    ASSERT_FLOAT_EQ(decodedSpeed.z, 34.9);
    
    // Compare original and decoded structs
    cmbtl::speed::Speed originalSpeed = sensorData1.getData<SensorIndex::SPEED>();
    ASSERT_FLOAT_EQ(originalSpeed.x, decodedSpeed.x);
    ASSERT_FLOAT_EQ(originalSpeed.y, decodedSpeed.y);
    ASSERT_FLOAT_EQ(originalSpeed.z, decodedSpeed.z);
}

TEST(DAQSensorDataPacketTests, encodePacket_Speed) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::SPEED);
    
    cmbtl::speed::Speed testSpeed;
    testSpeed.x = 12.5;
    testSpeed.y = 23.7;
    testSpeed.z = 34.9;
    
    sensorData.setData<SensorIndex::SPEED>(testSpeed);

    BinaryBuffer buffer = sensorData.encodePacket(instructions);
    
    cmbtl::speed::Speed actual;
    actual.x = buffer.readValue<cmbtl::speed::speed_axis_type>(cmbtl::speed::AXIS_ENCODED_BIT_SIZE);
    actual.y = buffer.readValue<cmbtl::speed::speed_axis_type>(cmbtl::speed::AXIS_ENCODED_BIT_SIZE);
    actual.z = buffer.readValue<cmbtl::speed::speed_axis_type>(cmbtl::speed::AXIS_ENCODED_BIT_SIZE);
    
    ASSERT_FLOAT_EQ(actual.x, 12.5);
    ASSERT_FLOAT_EQ(actual.y, 23.7);
    ASSERT_FLOAT_EQ(actual.z, 34.9);
}

TEST(DAQSensorDataPacketTests, decodePacket_Speed) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::SPEED);

    BinaryBuffer buffer(cmbtl::speed::ENCODED_BIT_SIZE);
    buffer.writeValue<cmbtl::speed::speed_axis_type>(45.6, cmbtl::speed::AXIS_ENCODED_BIT_SIZE);
    buffer.writeValue<cmbtl::speed::speed_axis_type>(56.7, cmbtl::speed::AXIS_ENCODED_BIT_SIZE);
    buffer.writeValue<cmbtl::speed::speed_axis_type>(67.8, cmbtl::speed::AXIS_ENCODED_BIT_SIZE);

    sensorData.decodePacket(instructions, buffer);

    cmbtl::speed::Speed actual = sensorData.getData<SensorIndex::SPEED>();
    ASSERT_FLOAT_EQ(actual.x, 45.6);
    ASSERT_FLOAT_EQ(actual.y, 56.7);
    ASSERT_FLOAT_EQ(actual.z, 67.8);
}

// POSITION TESTS
TEST(DAQSensorDataPacketTests, encodeDecodeCycle_Position) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::POSITION);
    
    cmbtl::position::Position testPosition;
    testPosition.x = 123.45;
    testPosition.y = 234.56;
    testPosition.z = 345.67;
    
    sensorData1.setData<SensorIndex::POSITION>(testPosition);

    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify data was preserved through encode/decode cycle
    cmbtl::position::Position decodedPosition = sensorData2.getData<SensorIndex::POSITION>();
    ASSERT_FLOAT_EQ(decodedPosition.x, 123.45);
    ASSERT_FLOAT_EQ(decodedPosition.y, 234.56);
    ASSERT_FLOAT_EQ(decodedPosition.z, 345.67);
    
    // Compare original and decoded structs
    cmbtl::position::Position originalPosition = sensorData1.getData<SensorIndex::POSITION>();
    ASSERT_FLOAT_EQ(originalPosition.x, decodedPosition.x);
    ASSERT_FLOAT_EQ(originalPosition.y, decodedPosition.y);
    ASSERT_FLOAT_EQ(originalPosition.z, decodedPosition.z);
}

TEST(DAQSensorDataPacketTests, encodePacket_Position) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::POSITION);
    
    cmbtl::position::Position testPosition;
    testPosition.x = 123.45;
    testPosition.y = 234.56;
    testPosition.z = 345.67;
    
    sensorData.setData<SensorIndex::POSITION>(testPosition);

    BinaryBuffer buffer = sensorData.encodePacket(instructions);
    
    cmbtl::position::Position actual;
    actual.x = buffer.readValue<cmbtl::position::position_axis_type>(cmbtl::position::AXIS_BIT_SIZE);
    actual.y = buffer.readValue<cmbtl::position::position_axis_type>(cmbtl::position::AXIS_BIT_SIZE);
    actual.z = buffer.readValue<cmbtl::position::position_axis_type>(cmbtl::position::AXIS_BIT_SIZE);
    
    ASSERT_FLOAT_EQ(actual.x, 123.45);
    ASSERT_FLOAT_EQ(actual.y, 234.56);
    ASSERT_FLOAT_EQ(actual.z, 345.67);
}

TEST(DAQSensorDataPacketTests, decodePacket_Position) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::POSITION);

    BinaryBuffer buffer(cmbtl::position::ENCODED_BIT_SIZE);
    buffer.writeValue<cmbtl::position::position_axis_type>(456.78, cmbtl::position::AXIS_BIT_SIZE);
    buffer.writeValue<cmbtl::position::position_axis_type>(567.89, cmbtl::position::AXIS_BIT_SIZE);
    buffer.writeValue<cmbtl::position::position_axis_type>(678.90, cmbtl::position::AXIS_BIT_SIZE);

    sensorData.decodePacket(instructions, buffer);

    cmbtl::position::Position actual = sensorData.getData<SensorIndex::POSITION>();
    ASSERT_FLOAT_EQ(actual.x, 456.78);
    ASSERT_FLOAT_EQ(actual.y, 567.89);
    ASSERT_FLOAT_EQ(actual.z, 678.90);
}

// BRAKE_PRESSURE TESTS
TEST(DAQSensorDataPacketTests, encodeDecodeCycle_BrakePressure) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::BRAKE_PRESSURE);
    
    // Create a brake pressure struct with front and rear values
    cmbtl::brake_pressure::BrakePressure testPressure;
    testPressure.front = 789;
    testPressure.rear = 901;
    
    sensorData1.setData<SensorIndex::BRAKE_PRESSURE>(testPressure);

    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify data was preserved through encode/decode cycle
    cmbtl::brake_pressure::BrakePressure decodedPressure = sensorData2.getData<SensorIndex::BRAKE_PRESSURE>();
    ASSERT_EQ(decodedPressure.front, 789);
    ASSERT_EQ(decodedPressure.rear, 901);
    
    // Compare original and decoded structs
    cmbtl::brake_pressure::BrakePressure originalPressure = sensorData1.getData<SensorIndex::BRAKE_PRESSURE>();
    ASSERT_EQ(originalPressure.front, decodedPressure.front);
    ASSERT_EQ(originalPressure.rear, decodedPressure.rear);
}

TEST(DAQSensorDataPacketTests, encodePacket_BrakePressure) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::BRAKE_PRESSURE);
    
    // Create and set brake pressure struct
    cmbtl::brake_pressure::BrakePressure testPressure;
    testPressure.front = 1234;
    testPressure.rear = 2345;
    
    sensorData.setData<SensorIndex::BRAKE_PRESSURE>(testPressure);

    BinaryBuffer buffer = sensorData.encodePacket(instructions);
    
    // Read values directly from buffer and verify
    cmbtl::brake_pressure::AXIS_SV frontValue = buffer.readValue<cmbtl::brake_pressure::AXIS_SV>(
        cmbtl::brake_pressure::AXIS_ENCODED_BIT_SIZE);
    cmbtl::brake_pressure::AXIS_SV rearValue = buffer.readValue<cmbtl::brake_pressure::AXIS_SV>(
        cmbtl::brake_pressure::AXIS_ENCODED_BIT_SIZE);
    
    ASSERT_EQ(frontValue, 1234);
    ASSERT_EQ(rearValue, 2345);
}

TEST(DAQSensorDataPacketTests, decodePacket_BrakePressure) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::BRAKE_PRESSURE);

    BinaryBuffer buffer(cmbtl::brake_pressure::ENCODED_BIT_SIZE);
    
    // Write brake pressure values to the buffer
    buffer.writeValue<cmbtl::brake_pressure::AXIS_SV>(3456, cmbtl::brake_pressure::AXIS_ENCODED_BIT_SIZE);
    buffer.writeValue<cmbtl::brake_pressure::AXIS_SV>(4095, cmbtl::brake_pressure::AXIS_ENCODED_BIT_SIZE);

    sensorData.decodePacket(instructions, buffer);

    // Verify the decoded struct
    cmbtl::brake_pressure::BrakePressure decodedPressure = sensorData.getData<SensorIndex::BRAKE_PRESSURE>();
    
    ASSERT_EQ(decodedPressure.front, 3456);
    ASSERT_EQ(decodedPressure.rear, 4095);
}

// Test multiple sensors including brake pressure
TEST(DAQSensorDataPacketTests, encodeDecodeCycle_WithBrakePressure) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    
    // Set multiple sensors to be encoded including brake pressure
    instructions.set(SensorIndex::MILLI_SEC);
    instructions.set(SensorIndex::BRAKE_PRESSURE);
    instructions.set(SensorIndex::CLUTCH_ENGAGED);
    
    // Set values for each sensor
    sensorData1.setData<SensorIndex::MILLI_SEC>(5000);
    
    cmbtl::brake_pressure::BrakePressure testPressure;
    testPressure.front = 1200;
    testPressure.rear = 1800;
    sensorData1.setData<SensorIndex::BRAKE_PRESSURE>(testPressure);
    
    sensorData1.setData<SensorIndex::CLUTCH_ENGAGED>(true);
    
    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify all values were preserved
    ASSERT_EQ(sensorData2.getData<SensorIndex::MILLI_SEC>(), 5000);
    
    cmbtl::brake_pressure::BrakePressure decodedPressure = sensorData2.getData<SensorIndex::BRAKE_PRESSURE>();
    ASSERT_EQ(decodedPressure.front, 1200);
    ASSERT_EQ(decodedPressure.rear, 1800);
    
    ASSERT_TRUE(sensorData2.getData<SensorIndex::CLUTCH_ENGAGED>());
    
    // Verify all data matches between instances
    ASSERT_EQ(sensorData1.getData<SensorIndex::MILLI_SEC>(), sensorData2.getData<SensorIndex::MILLI_SEC>());
    
    cmbtl::brake_pressure::BrakePressure originalPressure = sensorData1.getData<SensorIndex::BRAKE_PRESSURE>();
    ASSERT_EQ(originalPressure.front, decodedPressure.front);
    ASSERT_EQ(originalPressure.rear, decodedPressure.rear);
    
    ASSERT_EQ(sensorData1.getData<SensorIndex::CLUTCH_ENGAGED>(), sensorData2.getData<SensorIndex::CLUTCH_ENGAGED>());
}

// RPM TESTS
TEST(DAQSensorDataPacketTests, encodeDecodeCycle_RPM) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::RPM);
    
    // Create a new RPM struct with all three values
    cmbtl::rpm::RPM testRPM;
    testRPM.left = 2500.375f;
    testRPM.right = 2600.5f;
    testRPM.rear = 2700.625f;
    
    sensorData1.setData<SensorIndex::RPM>(testRPM);

    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify data was preserved through encode/decode cycle
    cmbtl::rpm::RPM decodedRPM = sensorData2.getData<SensorIndex::RPM>();
    ASSERT_FLOAT_EQ(decodedRPM.left, 2500.375f);
    ASSERT_FLOAT_EQ(decodedRPM.right, 2600.5f);
    ASSERT_FLOAT_EQ(decodedRPM.rear, 2700.625f);
    
    // Compare the original and decoded RPM structs
    cmbtl::rpm::RPM originalRPM = sensorData1.getData<SensorIndex::RPM>();
    ASSERT_FLOAT_EQ(originalRPM.left, decodedRPM.left);
    ASSERT_FLOAT_EQ(originalRPM.right, decodedRPM.right);
    ASSERT_FLOAT_EQ(originalRPM.rear, decodedRPM.rear);
}

TEST(DAQSensorDataPacketTests, encodePacket_RPM) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::RPM);
    
    // Create and set RPM struct with all three values
    cmbtl::rpm::RPM testRPM;
    testRPM.left = 3000.25f;
    testRPM.right = 3100.375f;
    testRPM.rear = 3200.5f;
    
    sensorData.setData<SensorIndex::RPM>(testRPM);

    BinaryBuffer buffer = sensorData.encodePacket(instructions);
    
    // Read values directly from buffer and verify
    float leftRPM = unsignedFloatDecode<cmbtl::rpm::AXIS_ENCODED_BIT_SIZE, cmbtl::rpm::DECIMAL_BITS>(buffer);
    float rightRPM = unsignedFloatDecode<cmbtl::rpm::AXIS_ENCODED_BIT_SIZE, cmbtl::rpm::DECIMAL_BITS>(buffer);
    float rearRPM = unsignedFloatDecode<cmbtl::rpm::AXIS_ENCODED_BIT_SIZE, cmbtl::rpm::DECIMAL_BITS>(buffer);
    
    ASSERT_FLOAT_EQ(leftRPM, 3000.25f);
    ASSERT_FLOAT_EQ(rightRPM, 3100.375f);
    ASSERT_FLOAT_EQ(rearRPM, 3200.5f);
}

TEST(DAQSensorDataPacketTests, decodePacket_RPM) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::RPM);

    BinaryBuffer buffer(cmbtl::rpm::ENCODED_BIT_SIZE);
    
    // Write RPM values for each axis to the buffer
    unsignedFloatEncode<cmbtl::rpm::AXIS_ENCODED_BIT_SIZE, cmbtl::rpm::DECIMAL_BITS>(4000.25f, buffer);
    unsignedFloatEncode<cmbtl::rpm::AXIS_ENCODED_BIT_SIZE, cmbtl::rpm::DECIMAL_BITS>(4100.375f, buffer);
    unsignedFloatEncode<cmbtl::rpm::AXIS_ENCODED_BIT_SIZE, cmbtl::rpm::DECIMAL_BITS>(4200.5f, buffer);

    sensorData.decodePacket(instructions, buffer);

    // Verify the decoded RPM struct
    cmbtl::rpm::RPM decodedRPM = sensorData.getData<SensorIndex::RPM>();
    
    ASSERT_FLOAT_EQ(decodedRPM.left, 4000.25f);
    ASSERT_FLOAT_EQ(decodedRPM.right, 4100.375f);
    ASSERT_FLOAT_EQ(decodedRPM.rear, 4200.5f);
}

// Test multiple sensors together
TEST(DAQSensorDataPacketTests, encodeDecodeCycle_MultipleSensors) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    
    // Set multiple sensors to be encoded
    instructions.set(SensorIndex::MILLI_SEC);
    instructions.set(SensorIndex::CLUTCH_ENGAGED);
    instructions.set(SensorIndex::POSITION);
    
    // Set values for each sensor
    sensorData1.setData<SensorIndex::MILLI_SEC>(5000);
    sensorData1.setData<SensorIndex::CLUTCH_ENGAGED>(true);
    
    cmbtl::position::Position testPosition;
    testPosition.x = 123.45;
    testPosition.y = 234.56;
    testPosition.z = 345.67;
    sensorData1.setData<SensorIndex::POSITION>(testPosition);
    
    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify all values were preserved
    ASSERT_EQ(sensorData2.getData<SensorIndex::MILLI_SEC>(), 5000);
    ASSERT_TRUE(sensorData2.getData<SensorIndex::CLUTCH_ENGAGED>());
    
    cmbtl::position::Position decodedPosition = sensorData2.getData<SensorIndex::POSITION>();
    ASSERT_FLOAT_EQ(decodedPosition.x, 123.45);
    ASSERT_FLOAT_EQ(decodedPosition.y, 234.56);
    ASSERT_FLOAT_EQ(decodedPosition.z, 345.67);
    
    // Verify all data matches between instances
    ASSERT_EQ(sensorData1.getData<SensorIndex::MILLI_SEC>(), sensorData2.getData<SensorIndex::MILLI_SEC>());
    ASSERT_EQ(sensorData1.getData<SensorIndex::CLUTCH_ENGAGED>(), sensorData2.getData<SensorIndex::CLUTCH_ENGAGED>());
    
    cmbtl::position::Position originalPosition = sensorData1.getData<SensorIndex::POSITION>();
    ASSERT_FLOAT_EQ(originalPosition.x, decodedPosition.x);
    ASSERT_FLOAT_EQ(originalPosition.y, decodedPosition.y);
    ASSERT_FLOAT_EQ(originalPosition.z, decodedPosition.z);
}

TEST(DAQSensorDataPacketTests, encodePacket_MultipleSensors) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    
    // Set multiple sensors to be encoded
    instructions.set(SensorIndex::MILLI_SEC);
    instructions.set(SensorIndex::CLUTCH_ENGAGED);
    
    // Set values for each sensor
    sensorData.setData<SensorIndex::MILLI_SEC>(5000);
    sensorData.setData<SensorIndex::CLUTCH_ENGAGED>(true);
    
    // Encode to buffer
    BinaryBuffer buffer = sensorData.encodePacket(instructions);
    
    // Read values back and verify
    cmbtl::millisec::SV millisec = buffer.readValue<cmbtl::millisec::SV>(
        cmbtl::millisec::ENCODED_BIT_SIZE);
    ASSERT_EQ(millisec, 5000);
    
    bool clutchEngaged = buffer.readValue<uint8_t>(1) == 1;
    ASSERT_TRUE(clutchEngaged);
}

TEST(DAQSensorDataPacketTests, decodePacket_MultipleSensors) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    
    // Set multiple sensors to be decoded
    instructions.set(SensorIndex::PIT_REQUESTED);
    instructions.set(SensorIndex::SPEED);
    
    // Calculate total buffer size
    uint32_t totalSize = 1 + // PIT_REQUESTED (bool)
                     cmbtl::speed::ENCODED_BIT_SIZE;
    
    // Create and fill buffer
    BinaryBuffer buffer(totalSize);
    buffer.writeValue<uint8_t>(1, 1); // PIT_REQUESTED = true
    
    // Write speed
    buffer.writeValue<cmbtl::speed::speed_axis_type>(10.5, cmbtl::speed::AXIS_ENCODED_BIT_SIZE);
    buffer.writeValue<cmbtl::speed::speed_axis_type>(20.5, cmbtl::speed::AXIS_ENCODED_BIT_SIZE);
    buffer.writeValue<cmbtl::speed::speed_axis_type>(30.5, cmbtl::speed::AXIS_ENCODED_BIT_SIZE);
    
    // Decode the packet
    sensorData.decodePacket(instructions, buffer);
    
    // Verify values were correctly decoded
    ASSERT_TRUE(sensorData.getData<SensorIndex::PIT_REQUESTED>());
    
    cmbtl::speed::Speed speed = sensorData.getData<SensorIndex::SPEED>();
    ASSERT_FLOAT_EQ(speed.x, 10.5);
    ASSERT_FLOAT_EQ(speed.y, 20.5);
    ASSERT_FLOAT_EQ(speed.z, 30.5);
}

// LINEAR_ACCELERATION TESTS
TEST(DAQSensorDataPacketTests, encodePacket_LinearAcceleration) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::LINEAR_ACCELERATION);
    
    // Create and set acceleration with values for all three axes
    cmbtl::linear_acceleration::linear_acceleration testAccel;
    testAccel.x = 2.5f;
    testAccel.y = 1.75f;
    testAccel.z = 3.25f;
    
    sensorData.setData<SensorIndex::LINEAR_ACCELERATION>(testAccel);

    BinaryBuffer buffer = sensorData.encodePacket(instructions);
    
    // Calculate expected encoded values based on DECIMAL_BITS
    const uint64_t expectedX = static_cast<uint64_t>(std::round(2.5f * std::pow(2, cmbtl::linear_acceleration::DECIMAL_BITS)));
    const uint64_t expectedY = static_cast<uint64_t>(std::round(1.75f * std::pow(2, cmbtl::linear_acceleration::DECIMAL_BITS)));
    const uint64_t expectedZ = static_cast<uint64_t>(std::round(3.25f * std::pow(2, cmbtl::linear_acceleration::DECIMAL_BITS)));
    
    // Read and verify each component
    boost::endian::big_uint64_t xValue = buffer.readValue<boost::endian::big_uint64_t>(cmbtl::linear_acceleration::AXIS_BIT_SIZE);
    boost::endian::big_uint64_t yValue = buffer.readValue<boost::endian::big_uint64_t>(cmbtl::linear_acceleration::AXIS_BIT_SIZE);
    boost::endian::big_uint64_t zValue = buffer.readValue<boost::endian::big_uint64_t>(cmbtl::linear_acceleration::AXIS_BIT_SIZE);
    
    ASSERT_EQ(xValue, expectedX);
    ASSERT_EQ(yValue, expectedY);
    ASSERT_EQ(zValue, expectedZ);
}

TEST(DAQSensorDataPacketTests, decodePacket_LinearAcceleration) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::LINEAR_ACCELERATION);

    BinaryBuffer buffer(cmbtl::linear_acceleration::ENCODED_BIT_SIZE);
    
    // Encode acceleration values manually
    const float xAccel = 1.5f;
    const float yAccel = -2.25f;
    const float zAccel = 3.375f;
    
    const uint64_t encodedX = std::round(xAccel * std::pow(2, cmbtl::linear_acceleration::DECIMAL_BITS));
    const uint64_t encodedY = std::round(yAccel * std::pow(2, cmbtl::linear_acceleration::DECIMAL_BITS));
    const uint64_t encodedZ = std::round(zAccel * std::pow(2, cmbtl::linear_acceleration::DECIMAL_BITS));
    
    buffer.writeValue<boost::endian::big_uint64_t>(encodedX, cmbtl::linear_acceleration::AXIS_BIT_SIZE);
    buffer.writeValue<boost::endian::big_uint64_t>(encodedY, cmbtl::linear_acceleration::AXIS_BIT_SIZE);
    buffer.writeValue<boost::endian::big_uint64_t>(encodedZ, cmbtl::linear_acceleration::AXIS_BIT_SIZE);

    sensorData.decodePacket(instructions, buffer);

    cmbtl::linear_acceleration::linear_acceleration decodedAccel = sensorData.getData<SensorIndex::LINEAR_ACCELERATION>();
    
    // Calculate expected decoded values
    const float expectedX = encodedX / std::pow(2, cmbtl::linear_acceleration::DECIMAL_BITS);
    const float expectedY = encodedY / std::pow(2, cmbtl::linear_acceleration::DECIMAL_BITS);
    const float expectedZ = encodedZ / std::pow(2, cmbtl::linear_acceleration::DECIMAL_BITS);
    
    ASSERT_FLOAT_EQ(decodedAccel.x, expectedX);
    ASSERT_FLOAT_EQ(decodedAccel.y, expectedY);
    ASSERT_FLOAT_EQ(decodedAccel.z, expectedZ);
}

TEST(DAQSensorDataPacketTests, encodeDecodeCycle_LinearAcceleration) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::LINEAR_ACCELERATION);
    
    // Create acceleration values that are exactly representable with current DECIMAL_BITS
    // For DECIMAL_BITS = 6, the precision is 1/64 = 0.015625
    const float precision = 1.0f / std::pow(2, cmbtl::linear_acceleration::DECIMAL_BITS);
    
    cmbtl::linear_acceleration::linear_acceleration testAccel;
    testAccel.x = 4.0f + 8.0f * precision;  // 4.125 = 4 + 8/64
    testAccel.y = 2.0f - 4.0f * precision; // 2.0625 = 2 - 4/64
    testAccel.z = 3.0f + 32.0f * precision; // 3.5 = 3 + 32/64
    
    sensorData1.setData<SensorIndex::LINEAR_ACCELERATION>(testAccel);

    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify data was preserved through encode/decode cycle
    cmbtl::linear_acceleration::linear_acceleration decodedAccel = sensorData2.getData<SensorIndex::LINEAR_ACCELERATION>();
    
    ASSERT_FLOAT_EQ(decodedAccel.x, testAccel.x);
    ASSERT_FLOAT_EQ(decodedAccel.y, testAccel.y);
    ASSERT_FLOAT_EQ(decodedAccel.z, testAccel.z);
    
    // Test with values that aren't exactly representable with DECIMAL_BITS precision
    cmbtl::linear_acceleration::linear_acceleration testAccel2;
    testAccel2.x = 5.1234f;
    testAccel2.y = 3.7654f;
    testAccel2.z = 2.9876f;
    
    sensorData1.setData<SensorIndex::LINEAR_ACCELERATION>(testAccel2);
    
    buffer = sensorData1.encodePacket(instructions);
    sensorData2.decodePacket(instructions, buffer);
    
    decodedAccel = sensorData2.getData<SensorIndex::LINEAR_ACCELERATION>();
    
    // Verify the values are within one precision unit of the original
    ASSERT_NEAR(decodedAccel.x, testAccel2.x, precision);
    ASSERT_NEAR(decodedAccel.y, testAccel2.y, precision);
    ASSERT_NEAR(decodedAccel.z, testAccel2.z, precision);
}

// BRAKE_TEMP TESTS
TEST(DAQSensorDataPacketTests, encodeDecodeCycle_BrakeTemp) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::BRAKE_TEMP);
    
    // Create a brake temp struct with all three values
    cmbtl::brake_temp::BrakeTemp testTemp;
    testTemp.front_left = 450;
    testTemp.front_right = 475;
    testTemp.rear = 500;
    
    sensorData1.setData<SensorIndex::BRAKE_TEMP>(testTemp);

    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify data was preserved through encode/decode cycle
    cmbtl::brake_temp::BrakeTemp decodedTemp = sensorData2.getData<SensorIndex::BRAKE_TEMP>();
    ASSERT_EQ(decodedTemp.front_left, 450);
    ASSERT_EQ(decodedTemp.front_right, 475);
    ASSERT_EQ(decodedTemp.rear, 500);
    
    // Compare original and decoded structs
    cmbtl::brake_temp::BrakeTemp originalTemp = sensorData1.getData<SensorIndex::BRAKE_TEMP>();
    ASSERT_EQ(originalTemp.front_left, decodedTemp.front_left);
    ASSERT_EQ(originalTemp.front_right, decodedTemp.front_right);
    ASSERT_EQ(originalTemp.rear, decodedTemp.rear);
}

TEST(DAQSensorDataPacketTests, encodePacket_BrakeTemp) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::BRAKE_TEMP);
    
    // Create and set brake temp struct
    cmbtl::brake_temp::BrakeTemp testTemp;
    testTemp.front_left = 525;
    testTemp.front_right = 550;
    testTemp.rear = 575;
    
    sensorData.setData<SensorIndex::BRAKE_TEMP>(testTemp);

    BinaryBuffer buffer = sensorData.encodePacket(instructions);
    
    // Read values directly from buffer and verify
    cmbtl::brake_temp::AXIS_SV frontLeftValue = buffer.readValue<cmbtl::brake_temp::AXIS_SV>(
        cmbtl::brake_temp::AXIS_ENCODED_BIT_SIZE);
    cmbtl::brake_temp::AXIS_SV frontRightValue = buffer.readValue<cmbtl::brake_temp::AXIS_SV>(
        cmbtl::brake_temp::AXIS_ENCODED_BIT_SIZE);
    cmbtl::brake_temp::AXIS_SV rearValue = buffer.readValue<cmbtl::brake_temp::AXIS_SV>(
        cmbtl::brake_temp::AXIS_ENCODED_BIT_SIZE);
    
    ASSERT_EQ(frontLeftValue, 525);
    ASSERT_EQ(frontRightValue, 550);
    ASSERT_EQ(rearValue, 575);
}

TEST(DAQSensorDataPacketTests, decodePacket_BrakeTemp) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::BRAKE_TEMP);

    BinaryBuffer buffer(cmbtl::brake_temp::ENCODED_BIT_SIZE);
    
    // Write brake temp values to the buffer
    buffer.writeValue<cmbtl::brake_temp::AXIS_SV>(600, cmbtl::brake_temp::AXIS_ENCODED_BIT_SIZE);
    buffer.writeValue<cmbtl::brake_temp::AXIS_SV>(625, cmbtl::brake_temp::AXIS_ENCODED_BIT_SIZE);
    buffer.writeValue<cmbtl::brake_temp::AXIS_SV>(650, cmbtl::brake_temp::AXIS_ENCODED_BIT_SIZE);

    sensorData.decodePacket(instructions, buffer);

    // Verify the decoded struct
    cmbtl::brake_temp::BrakeTemp decodedTemp = sensorData.getData<SensorIndex::BRAKE_TEMP>();
    
    ASSERT_EQ(decodedTemp.front_left, 600);
    ASSERT_EQ(decodedTemp.front_right, 625);
    ASSERT_EQ(decodedTemp.rear, 650);
}

// Test multiple temperature sensors including brake temp with other sensors
TEST(DAQSensorDataPacketTests, encodeDecodeCycle_WithBrakeTemp) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    
    // Set multiple sensors to be encoded
    instructions.set(SensorIndex::MILLI_SEC);
    instructions.set(SensorIndex::BRAKE_TEMP);
    instructions.set(SensorIndex::CVT_TEMP);
    
    // Set values for each sensor
    sensorData1.setData<SensorIndex::MILLI_SEC>(4000);
    
    cmbtl::brake_temp::BrakeTemp testTemp;
    testTemp.front_left = 675;
    testTemp.front_right = 700;
    testTemp.rear = 725;
    sensorData1.setData<SensorIndex::BRAKE_TEMP>(testTemp);
    
    sensorData1.setData<SensorIndex::CVT_TEMP>(800);
    
    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify all values were preserved
    ASSERT_EQ(sensorData2.getData<SensorIndex::MILLI_SEC>(), 4000);
    
    cmbtl::brake_temp::BrakeTemp decodedTemp = sensorData2.getData<SensorIndex::BRAKE_TEMP>();
    ASSERT_EQ(decodedTemp.front_left, 675);
    ASSERT_EQ(decodedTemp.front_right, 700);
    ASSERT_EQ(decodedTemp.rear, 725);
    
    ASSERT_EQ(sensorData2.getData<SensorIndex::CVT_TEMP>(), 800);
    
    // Verify all data matches between instances
    ASSERT_EQ(sensorData1.getData<SensorIndex::MILLI_SEC>(), sensorData2.getData<SensorIndex::MILLI_SEC>());
    
    cmbtl::brake_temp::BrakeTemp originalTemp = sensorData1.getData<SensorIndex::BRAKE_TEMP>();
    ASSERT_EQ(originalTemp.front_left, decodedTemp.front_left);
    ASSERT_EQ(originalTemp.front_right, decodedTemp.front_right);
    ASSERT_EQ(originalTemp.rear, decodedTemp.rear);
    
    ASSERT_EQ(sensorData1.getData<SensorIndex::CVT_TEMP>(), sensorData2.getData<SensorIndex::CVT_TEMP>());
}

// CVT_TEMP TESTS
TEST(DAQSensorDataPacketTests, encodePacket_CVTTemp) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::CVT_TEMP);
    sensorData.setData<SensorIndex::CVT_TEMP>(789);

    BinaryBuffer buffer = sensorData.encodePacket(instructions);
    
    cmbtl::cvt_temp::SV actual = 
        buffer.readValue<cmbtl::cvt_temp::SV>(cmbtl::cvt_temp::ENCODED_BIT_SIZE);
    ASSERT_EQ(actual, 789);
}

TEST(DAQSensorDataPacketTests, decodePacket_CVTTemp) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::CVT_TEMP);

    BinaryBuffer buffer(cmbtl::cvt_temp::ENCODED_BIT_SIZE);
    buffer.writeValue<cmbtl::cvt_temp::SV>(851, cmbtl::cvt_temp::ENCODED_BIT_SIZE);

    sensorData.decodePacket(instructions, buffer);

    ASSERT_EQ(sensorData.getData<SensorIndex::CVT_TEMP>(), 851);
}

TEST(DAQSensorDataPacketTests, encodeDecodeCycle_CVTTemp) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::CVT_TEMP);
    sensorData1.setData<SensorIndex::CVT_TEMP>(923);

    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify data was preserved through encode/decode cycle
    ASSERT_EQ(sensorData2.getData<SensorIndex::CVT_TEMP>(), 923);
    ASSERT_EQ(sensorData1.getData<SensorIndex::CVT_TEMP>(), sensorData2.getData<SensorIndex::CVT_TEMP>());
}

// Test multiple temperature sensors together
TEST(DAQSensorDataPacketTests, encodeDecodeCycle_MultipleTempSensors) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    
    // Set multiple temperature sensors to be encoded
    instructions.set(SensorIndex::BRAKE_TEMP);
    instructions.set(SensorIndex::CVT_TEMP);
    
    // Set values for each sensor
    sensorData1.setData<SensorIndex::BRAKE_TEMP>({550, 575, 660});
    sensorData1.setData<SensorIndex::CVT_TEMP>(725);
    
    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify all values were preserved
    ASSERT_EQ(sensorData2.getData<SensorIndex::BRAKE_TEMP>().front_left, 550);
    ASSERT_EQ(sensorData2.getData<SensorIndex::BRAKE_TEMP>().front_right, 575);
    ASSERT_EQ(sensorData2.getData<SensorIndex::BRAKE_TEMP>().rear, 660);
    ASSERT_EQ(sensorData2.getData<SensorIndex::CVT_TEMP>(), 725);
    
    // Verify all data matches between instances
    ASSERT_EQ(sensorData1.getData<SensorIndex::BRAKE_TEMP>().front_left, sensorData2.getData<SensorIndex::BRAKE_TEMP>().front_left);
    ASSERT_EQ(sensorData1.getData<SensorIndex::BRAKE_TEMP>().front_right, sensorData2.getData<SensorIndex::BRAKE_TEMP>().front_right);
    ASSERT_EQ(sensorData1.getData<SensorIndex::BRAKE_TEMP>().rear, sensorData2.getData<SensorIndex::BRAKE_TEMP>().rear);
    ASSERT_EQ(sensorData1.getData<SensorIndex::CVT_TEMP>(), sensorData2.getData<SensorIndex::CVT_TEMP>());
}

// STEERING_ROTATION TESTS
TEST(DAQSensorDataPacketTests, encodePacket_SteeringRotation) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::STEERING_ROTATION);
    
    // Set a steering rotation value in degrees (between 0 and 360)
    sensorData.setData<SensorIndex::STEERING_ROTATION>(135.5f);

    BinaryBuffer buffer = sensorData.encodePacket(instructions);
    
    // Note: For steering rotation, we need to be aware of the DECIMAL_BITS precision
    const float encodedValue = 135.5f;
    const float decodedValue = buffer.readValue<boost::endian::big_uint32_t>(cmbtl::steering_rotation::ENCODED_BIT_SIZE) / std::pow(2, cmbtl::steering_rotation::DECIMAL_BITS);
    
    // We use ASSERT_FLOAT_EQ because we're comparing floating-point values
    ASSERT_FLOAT_EQ(decodedValue, encodedValue);
}

TEST(DAQSensorDataPacketTests, decodePacket_SteeringRotation) {
    DAQSensorDataType sensorData;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::STEERING_ROTATION);

    BinaryBuffer buffer(cmbtl::steering_rotation::ENCODED_BIT_SIZE);
    
    // Write a steering rotation value to the buffer (between 0 and 360 degrees)
    const float steeringValue = 270.25f;

    boost::endian::big_uint64_t val = static_cast<uint64_t>(std::round(steeringValue * std::pow(2, cmbtl::steering_rotation::DECIMAL_BITS)));
    buffer.writeValue<boost::endian::big_uint64_t>(val, cmbtl::steering_rotation::ENCODED_BIT_SIZE);

    sensorData.decodePacket(instructions, buffer);
    
    // Get the decoded value
    float decodedValue = sensorData.getData<SensorIndex::STEERING_ROTATION>();
    
    // Compare with expected value
    ASSERT_FLOAT_EQ(decodedValue, steeringValue);
}

TEST(DAQSensorDataPacketTests, encodeDecodeCycle_SteeringRotation) {
    // First instance for encoding
    DAQSensorDataType sensorData1;
    PacketInstructions<DAQSensorDataType::NUM_SENSORS> instructions;
    instructions.set(SensorIndex::STEERING_ROTATION);
    
    // Create a value that should be exactly representable with current DECIMAL_BITS
    // For DECIMAL_BITS = 7, the precision is 1/128 = 0.0078125
    const float precision = 1.0f / std::pow(2, cmbtl::steering_rotation::DECIMAL_BITS);
    
    // 180.125 = 180 + 16/128 (degrees)
    const float exactValue = 180.0f + 16.0f * precision;
    sensorData1.setData<SensorIndex::STEERING_ROTATION>(exactValue);

    // Encode to buffer
    BinaryBuffer buffer = sensorData1.encodePacket(instructions);
    
    // Create second instance and decode the buffer
    DAQSensorDataType sensorData2;
    sensorData2.decodePacket(instructions, buffer);
    
    // Verify data was preserved through encode/decode cycle
    float decodedValue = sensorData2.getData<SensorIndex::STEERING_ROTATION>();
    ASSERT_FLOAT_EQ(decodedValue, exactValue);
    
    // Test with a value that's not exactly representable with DECIMAL_BITS precision
    // Still within 0-360 degrees range
    const float inexactValue = 320.1234f;
    sensorData1.setData<SensorIndex::STEERING_ROTATION>(inexactValue);
    
    buffer = sensorData1.encodePacket(instructions);
    sensorData2.decodePacket(instructions, buffer);
    
    decodedValue = sensorData2.getData<SensorIndex::STEERING_ROTATION>();
    
    // The value should be within one precision unit of the original
    ASSERT_NEAR(decodedValue, inexactValue, precision);
    
    // Alternative test that calculates the expected rounded value
    // Calculate expected rounded value based on DECIMAL_BITS
    const uint64_t encodedRoundedValue = std::round(inexactValue * std::pow(2, cmbtl::steering_rotation::DECIMAL_BITS));
    const float expectedRoundedValue = encodedRoundedValue / std::pow(2, cmbtl::steering_rotation::DECIMAL_BITS);
    
    // Compare with the actual decoded value
    ASSERT_FLOAT_EQ(decodedValue, expectedRoundedValue);
}

TEST (DAQJSONSerializationTests, serializeToJSON) {
    DAQSensorDataType sensorData;

    sensorData.setData<SensorIndex::SPEED>({2.043f, 5.044f, 2.043345f});

    std::cout << std::endl;
    std::cout << sensorData.serializeDataToJSON();
    std::cout << std::endl;
}