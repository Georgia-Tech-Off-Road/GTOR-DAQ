#include "SensorInfo.h"
#ifndef PACKET_INSTRUCTIONS_H
#define PACKET_INSTRUCTIONS_H
struct PacketBuilderParams {
    //Include millisec should always be true
    bool include_millisec = true;
    bool include_clutch_engaged = false;
    bool include_pit_requested = false;

    bool include_speed_x = false;
    bool include_speed_y = false;
    bool include_speed_z = false;

    bool include_position_x = false;
    bool include_position_y = false;
    bool include_position_z = false;

    bool include_brake_pressure_front = false;
    bool include_brake_pressure_back = false;
    bool include_RPM = false;

    bool include_linear_acceleration_x = false;
    bool include_linear_acceleration_y = false;
    bool include_linear_acceleration_z = false;

    bool include_cvt_temp = false;
    bool include_brake_temp_front_left = false;
    bool include_brake_temp_front_right = false;
    bool include_brake_temp_back = false;
    bool include_steering_rotation = false;
};
//Packet Instructions work like this:
//Imagine that we have four sensors with the following indices:
//Millisec: 0
//RPM: 1
//Speed: 2
//Brake Pressure: 3
//The following instruction code:
//Instruction Code: 0101
//Would mean
// 1.) Include Millisec data in the final packet  (index 0, should always be true)
// 2.) Do not include RPM (index 1)
// 3.) Do include speed (index 2)
// 4.) And finally, do not include brake pressure (index 3)
// This PacketBuilder class is just a nice way to create these instructions
class PacketBuilder {
    unsigned int packetInstructions : NUM_SENSORS;
    PacketBuilder(PacketBuilderParams packetInfo) {
        //Should be this already, do it just to be sure
        packetInstructions = 0x0;
        if (packetInfo.include_millisec) {
            setBit(SensorIndex::MILLI_SEC);
        }
        if (packetInfo.include_clutch_engaged) {
            setBit(SensorIndex::CLUTCH_ENGAGED);
        }
        if (packetInfo.include_pit_requested) {
            setBit(SensorIndex::PIT_REQUESTED);
        }
        if (packetInfo.include_speed_x) {
            setBit(SensorIndex::SPEED_X);
        }
        if (packetInfo.include_speed_y) {
            setBit(SensorIndex::SPEED_Y);
        }
        if (packetInfo.include_speed_z) {
            setBit(SensorIndex::SPEED_Z);
        }
        if (packetInfo.include_position_x) {
            setBit(SensorIndex::POSITION_X);
        }
        if (packetInfo.include_position_y) {
            setBit(SensorIndex::POSITION_Y);
        }
        if (packetInfo.include_position_z) {
            setBit(SensorIndex::POSITION_Z);
        }
        if (packetInfo.include_brake_pressure_front) {
            setBit(SensorIndex::BRAKE_PRESSURE_FRONT);
        }
        if (packetInfo.include_brake_pressure_back) {
            setBit(SensorIndex::BRAKE_PRESSURE_BACK);
        }
        if (packetInfo.include_RPM) {
            setBit(SensorIndex::RPM);
        }
        if (packetInfo.include_linear_acceleration_x) {
            setBit(SensorIndex::LINEAR_ACCELERATION_X);
        }
        if (packetInfo.include_linear_acceleration_y) {
            setBit(SensorIndex::LINEAR_ACCELERATION_Y);
        }
        if (packetInfo.include_linear_acceleration_z) {
            setBit(SensorIndex::LINEAR_ACCELERATION_Z);
        }
        if (packetInfo.include_cvt_temp) {
            setBit(SensorIndex::CVT_TEMP);
        }
        if (packetInfo.include_brake_temp_front_left) {
            setBit(SensorIndex::BRAKE_TEMP_FRONT_LEFT);
        }
        if (packetInfo.include_brake_temp_front_right) {
            setBit(SensorIndex::BRAKE_TEMP_FRONT_RIGHT);
        }
        if (packetInfo.include_brake_temp_back) {
            setBit(SensorIndex::BRAKE_TEMP_BACK);
        }
        if (packetInfo.include_steering_rotation) {
            setBit(SensorIndex::STEERING_ROTATION);
        }
    }
    //Sets the bit at the specified index to 1
    void setBit(int index) {
        packetInstructions = packetInstructions | ((packetInstructions & 0x1) << index);
    }
};
#endif