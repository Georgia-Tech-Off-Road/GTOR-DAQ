#include "Packets/PacketInstructions.h"
#include "Packets/PacketInstructionsBuilder.h"
#include "../DAQSensors.h"
#ifndef CMBTL_ALL__PACKET_H
#define CMBTL_ALL__PACKET_H
namespace cmbtl {
    namespace packet {
        const PacketInstructions<DAQSensorDataType::NUM_SENSORS> all_packet = createPacket<DAQSensorDataType::NUM_SENSORS>({
            SensorIndex::SEC,
            SensorIndex::MICRO_SEC,
            SensorIndex::RPM1,
            SensorIndex::RPM2,
            SensorIndex::RPM3,
            SensorIndex::RPM4,
            SensorIndex::RearBrakePressure,
            SensorIndex::FrontBrakePressure,
            SensorIndex::LDSFrontLeft,
            SensorIndex::LDSFrontRight,
            SensorIndex::TEENSY_TEMP
        });
    }
}
#endif