#include "../../Packets/PacketInstructions.h"
#include "../../Packets/PacketInstructionsBuilder.h"
#include "../DAQSensors.h"
#ifndef CMBTL_ALL__PACKET_H
#define CMBTL_ALL__PACKET_H
namespace cmbtl {
    namespace packet {
        const PacketInstructions<DAQSensorDataType::NUM_SENSORS> all_packet = createPacket<DAQSensorDataType::NUM_SENSORS>({
            SensorIndex::MILLI_SEC, 
            SensorIndex::CLUTCH_ENGAGED,
            SensorIndex::PIT_REQUESTED,
            SensorIndex::SPEED,
            SensorIndex::POSITION,
            SensorIndex::BRAKE_PRESSURE,
            SensorIndex::RPM,
            SensorIndex::LINEAR_ACCELERATION,
            SensorIndex::CVT_TEMP,
            SensorIndex::BRAKE_TEMP,
            SensorIndex::STEERING_ROTATION
        });
    }
}
#endif