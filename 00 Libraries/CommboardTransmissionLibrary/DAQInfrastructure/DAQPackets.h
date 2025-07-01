#ifndef CMBTL_DAQ_PACKETS_H
#define CMBTL_DAQ_PACKETS_H
#include "Packets/All_Packet.h"
#include "../Packets/PacketIDMap.h"
#include "../Packets/PacketScheme.h"
#include "DAQSensors.h"

namespace cmbtl {
    namespace packet {
        const PacketInstructionsIDMap<DAQSensorDataType::NUM_SENSORS> IDMap({all_packet});

        const PacketScheme<DAQSensorDataType::NUM_SENSORS> SIMPLE_SCHEME({all_packet});
    }
}

#endif
