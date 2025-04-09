#include "SensorInfo.h"
#include "Sensors/millisec.h"
#include <array>
#ifndef CMBTL_SENSORS_H
#define CMBTL_SENSORS_H

using std::array;
namespace cmbtl {
    //--------------- Define Sensor Indexes ----------------------------------------------------
    enum SensorIndex {
        MILLI_SEC,
        CLUTCH_ENGAGED,
        PIT_REQUESTED,
        SPEED_X,
        SPEED_Y,
        SPEED_Z,
        POSITION_X,
        POSITION_Y,
        POSITION_Z,
        BRAKE_PRESSURE_FRONT,
        BRAKE_PRESSURE_BACK,
        RPM,
        LINEAR_ACCELERATION_X,
        LINEAR_ACCELERATION_Y,
        LINEAR_ACCELERATION_Z,
        CVT_TEMP,
        BRAKE_TEMP_FRONT_LEFT,
        BRAKE_TEMP_FRONT_RIGHT,
        BRAKE_TEMP_BACK,
        STEERING_ROTATION,
        //COUNT IS NOT A REAL SENSOR, it is used to determine how many elements are in this enum
        //COUNT SHOULD ALWAYS BE THE LAST ELEMENT
        COUNT,
    };

    using sensor_index = size_t;

    template <sensor_index milli_sec,
    sensor_index clutch_engaged,
    sensor_index pit_requested,
    sensor_index speed_x,
    sensor_index speed_y,
    sensor_index speed_z,
    sensor_index position_x,
    sensor_index position_y,
    sensor_index position_z,
    sensor_index brake_pressure_front,
    sensor_index brake_pressure_back,
    sensor_index rpm,
    sensor_index linear_acceleration_x,
    sensor_index linear_acceleration_y,
    sensor_index linear_acceleration_z,
    sensor_index cvt_temp,
    sensor_index brake_temp_front_left,
    sensor_index brake_temp_front_right,
    sensor_index brake_temp_back,
    sensor_index steering_rotation>
    struct SensorIndices {
        static constexpr sensor_index MILLI_SEC = milli_sec;
        static constexpr sensor_index CLUTCH_ENGAGED = clutch_engaged;
        static constexpr sensor_index PIT_REQUESTED = pit_requested;
        static constexpr sensor_index SPEED_X = speed_x;
        static constexpr sensor_index SPEED_Y = speed_y;
        static constexpr sensor_index SPEED_Z = speed_z;
        static constexpr sensor_index POSITION_X = position_x;
        static constexpr sensor_index POSITION_Y = position_y;
        static constexpr sensor_index POSITION_Z = position_z;
        static constexpr sensor_index BRAKE_PRESSURE_FRONT = brake_pressure_front;
        static constexpr sensor_index BRAKE_PRESSURE_BACK = brake_pressure_back;
        static constexpr sensor_index RPM = rpm;
        static constexpr sensor_index LINEAR_ACCELERATION_X = linear_acceleration_x;
        static constexpr sensor_index LINEAR_ACCELERATION_Y = linear_acceleration_y;
        static constexpr sensor_index LINEAR_ACCELERATION_Z = linear_acceleration_z;
        static constexpr sensor_index CVT_TEMP = cvt_temp;
        static constexpr sensor_index BRAKE_TEMP_FRONT_LEFT = brake_temp_front_left;
        static constexpr sensor_index BRAKE_TEMP_FRONT_RIGHT = brake_temp_front_right;
        static constexpr sensor_index BRAKE_TEMP_BACK = brake_temp_back;
        static constexpr sensor_index STEERING_ROTATION = steering_rotation;

        constexpr size_t getNumActiveSensors() const {
            size_t count = 0;
            if(MILLI_SEC >= 0) { count++; }
            if(CLUTCH_ENGAGED >= 0) { count++; }
            if(PIT_REQUESTED >= 0) { count++; }
            if(SPEED_X >= 0) { count++; }
            if(SPEED_Y >= 0) { count++; }
            if(SPEED_Z >= 0) { count++; }
            if(POSITION_X >= 0) { count++; }
            if(POSITION_Y >= 0) { count++; }
            if(POSITION_Z >= 0) { count++; }
            if(BRAKE_PRESSURE_FRONT >= 0) { count++; }
            if(BRAKE_PRESSURE_BACK >= 0) { count++; }
            if(RPM >= 0) { count++; }
            if(LINEAR_ACCELERATION_X >= 0) { count++; }
            if(LINEAR_ACCELERATION_Y >= 0) { count++; }
            if(LINEAR_ACCELERATION_Z >= 0) { count++; }
            if(CVT_TEMP >= 0) { count++; }
            if(BRAKE_TEMP_FRONT_LEFT >= 0) { count++; }
            if(BRAKE_TEMP_FRONT_RIGHT >= 0) { count++; }
            if(BRAKE_TEMP_BACK >= 0) { count++; }
            if(STEERING_ROTATION >= 0) { count++; }
            return count;
        }

// Returns true if there are no conflicts (same index) among all variables with valid indices (>= 0)
constexpr bool validMap() const {
        // Create an array to track which indices are used
        // This approach assumes indices are within a reasonable range (e.g., 0-100)
        constexpr size_t MAX_SENSORS = 100;
        bool used_indices[MAX_SENSORS] = {false};
        
        // Check each sensor index
        if (MILLI_SEC >= 0) {
            if (used_indices[MILLI_SEC]) return false;
            used_indices[MILLI_SEC] = true;
        }
        
        if (CLUTCH_ENGAGED >= 0) {
            if (used_indices[CLUTCH_ENGAGED]) return false;
            used_indices[CLUTCH_ENGAGED] = true;
        }
        
        if (PIT_REQUESTED >= 0) {
            if (used_indices[PIT_REQUESTED]) return false;
            used_indices[PIT_REQUESTED] = true;
        }
        
        if (SPEED_X >= 0) {
            if (used_indices[SPEED_X]) return false;
            used_indices[SPEED_X] = true;
        }
        
        if (SPEED_Y >= 0) {
            if (used_indices[SPEED_Y]) return false;
            used_indices[SPEED_Y] = true;
        }
        
        if (SPEED_Z >= 0) {
            if (used_indices[SPEED_Z]) return false;
            used_indices[SPEED_Z] = true;
        }
        
        if (POSITION_X >= 0) {
            if (used_indices[POSITION_X]) return false;
            used_indices[POSITION_X] = true;
        }
        
        if (POSITION_Y >= 0) {
            if (used_indices[POSITION_Y]) return false;
            used_indices[POSITION_Y] = true;
        }
        
        if (POSITION_Z >= 0) {
            if (used_indices[POSITION_Z]) return false;
            used_indices[POSITION_Z] = true;
        }
        
        if (BRAKE_PRESSURE_FRONT >= 0) {
            if (used_indices[BRAKE_PRESSURE_FRONT]) return false;
            used_indices[BRAKE_PRESSURE_FRONT] = true;
        }
        
        if (BRAKE_PRESSURE_BACK >= 0) {
            if (used_indices[BRAKE_PRESSURE_BACK]) return false;
            used_indices[BRAKE_PRESSURE_BACK] = true;
        }
        
        if (RPM >= 0) {
            if (used_indices[RPM]) return false;
            used_indices[RPM] = true;
        }
        
        if (LINEAR_ACCELERATION_X >= 0) {
            if (used_indices[LINEAR_ACCELERATION_X]) return false;
            used_indices[LINEAR_ACCELERATION_X] = true;
        }
        
        if (LINEAR_ACCELERATION_Y >= 0) {
            if (used_indices[LINEAR_ACCELERATION_Y]) return false;
            used_indices[LINEAR_ACCELERATION_Y] = true;
        }
        
        if (LINEAR_ACCELERATION_Z >= 0) {
            if (used_indices[LINEAR_ACCELERATION_Z]) return false;
            used_indices[LINEAR_ACCELERATION_Z] = true;
        }
        
        if (CVT_TEMP >= 0) {
            if (used_indices[CVT_TEMP]) return false;
            used_indices[CVT_TEMP] = true;
        }
        
        if (BRAKE_TEMP_FRONT_LEFT >= 0) {
            if (used_indices[BRAKE_TEMP_FRONT_LEFT]) return false;
            used_indices[BRAKE_TEMP_FRONT_LEFT] = true;
        }
        
        if (BRAKE_TEMP_FRONT_RIGHT >= 0) {
            if (used_indices[BRAKE_TEMP_FRONT_RIGHT]) return false;
            used_indices[BRAKE_TEMP_FRONT_RIGHT] = true;
        }
        
        if (BRAKE_TEMP_BACK >= 0) {
            if (used_indices[BRAKE_TEMP_BACK]) return false;
            used_indices[BRAKE_TEMP_BACK] = true;
        }
        
        if (STEERING_ROTATION >= 0) {
            if (used_indices[STEERING_ROTATION]) return false;
            used_indices[STEERING_ROTATION] = true;
        }
        
        // If we reach here, no conflicts were found
        return true;
    }
};

using SENSOR_INDICES = SensorIndices<SensorIndex::MILLI_SEC,
    SensorIndex::CLUTCH_ENGAGED,
    SensorIndex::PIT_REQUESTED,
    SensorIndex::SPEED_X,
    SensorIndex::SPEED_Y,
    SensorIndex::SPEED_Z,
    SensorIndex::POSITION_X,
    SensorIndex::POSITION_Y,
    SensorIndex::POSITION_Z,
    SensorIndex::BRAKE_PRESSURE_FRONT,
    SensorIndex::BRAKE_PRESSURE_BACK,
    SensorIndex::RPM,
    SensorIndex::LINEAR_ACCELERATION_X,
    SensorIndex::LINEAR_ACCELERATION_Y,
    SensorIndex::LINEAR_ACCELERATION_Z,
    SensorIndex::CVT_TEMP,
    SensorIndex::BRAKE_TEMP_FRONT_LEFT,
    SensorIndex::BRAKE_TEMP_FRONT_RIGHT,
    SensorIndex::BRAKE_TEMP_BACK,
    SensorIndex::STEERING_ROTATION>;
}
#endif