#include "misc/BinaryBuffer/BinaryBuffer.h"
#include "Sensors.h"

#ifndef CMBTL_PACKET_INSTRUCTIONS_BUILDER_H
#define CMBTL_PACKET_INSTRUCTIONS_BUILDER_H

namespace cmbtl {

using cmbtl::SensorIndex;

    struct PacketInstructionsBuilderParams {
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


    class PacketInstructions : BinaryBuffer {
        public:
            PacketInstructions() {
                BinaryBuffer(cmbtl::NUM_SENSORS);
            }
    }

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
    // This PacketInstructionsBuilder class is just a nice way to create these instructions
    class PacketInstructionsBuilder {
        static PacketInstructions generatePacketInstructions(PacketInstructionsBuilderParams instructionInfo) {
            //Declare packet instructions
            PacketInstructions instructions;

            if (instructionInfo.include_millisec) {
                instructions.writeBit(1, SensorIndex::MILLI_SEC);
            }
            if (instructionInfo.include_clutch_engaged) {
                instructions.writeBit(1, SensorIndex::CLUTCH_ENGAGED);
            }
            if (instructionInfo.include_pit_requested) {
                instructions.writeBit(1, SensorIndex::PIT_REQUESTED);
            }
            if (instructionInfo.include_speed_x) {
                instructions.writeBit(1, SensorIndex::SPEED_X);
            }
            if (instructionInfo.include_speed_y) {
                instructions.writeBit(1, SensorIndex::SPEED_Y);
            }
            if (instructionInfo.include_speed_z) {
                instructions.writeBit(1, SensorIndex::SPEED_Z);
            }
            if (instructionInfo.include_position_x) {
                instructions.writeBit(1, SensorIndex::POSITION_X);
            }
            if (instructionInfo.include_position_y) {
                instructions.writeBit(1, SensorIndex::POSITION_Y);
            }
            if (instructionInfo.include_position_z) {
                instructions.writeBit(1, SensorIndex::POSITION_Z);
            }
            if (instructionInfo.include_brake_pressure_front) {
                instructions.writeBit(1, SensorIndex::BRAKE_PRESSURE_FRONT);
            }
            if (instructionInfo.include_brake_pressure_back) {
                instructions.writeBit(1, SensorIndex::BRAKE_PRESSURE_BRAKE);
            }
            if (instructionInfo.include_RPM) {
                instructions.writeBit(1, SensorIndex::RPM);
            }
            if (instructionInfo.include_linear_acceleration_x) {
                instructions.writeBit(1, SensorIndex::LINEAR_ACCELERATION_X);
            }
            if (instructionInfo.include_linear_acceleration_y) {
                instructions.writeBit(1, SensorIndex::LINEAR_ACCELERATION_Y);
            }
            if (instructionInfo.include_linear_acceleration_z) {
                instructions.writeBit(1, SensorIndex::LINEAR_ACCELERATION_Z);
            }
            if (instructionInfo.include_cvt_temp) {
                instructions.writeBit(1, SensorIndex::CVT_TEMP);
            }
            if (instructionInfo.include_brake_temp_front_left) {
                instructions.writeBit(1, SensorIndex::BRAKE_PRESSURE_FRONT_LEFT);
            }
            if (instructionInfo.include_brake_temp_front_right) {
                instructions.writeBit(1, SensorIndex::BRAKE_TEMP_FRONT_RIGHT);
            }
            if (instructionInfo.include_brake_temp_back) {
                instructions.writeBit(1, SensorIndex::BRAKE_TEMP_BACK);
            }
            if (instructionInfo.include_steering_rotation) {
                instructions.writeBit(1, SensorIndex::STEERING_ROTATION);
            }

            return instructions;
        }
    };
}

#endif