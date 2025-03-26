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
}
#endif