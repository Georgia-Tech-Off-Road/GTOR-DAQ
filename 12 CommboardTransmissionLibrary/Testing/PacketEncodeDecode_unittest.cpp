#include "gtest/gtest.h"
#include "PacketEncodeDecode.h"

TEST(PacketInstructionsTest, EncodedPacketBitSize) {
    using SensorInfoTuple = std::tuple<cmbtl::millisec::MILLI_SEC_SENSOR_INFO>;

    uint32_t packetSize = cmbtl::EncodedPacketBitSize<cmbtl::PacketInstructions<true>, SensorInfoTuple>::value;
    ASSERT_EQ(packetSize, 26);
}