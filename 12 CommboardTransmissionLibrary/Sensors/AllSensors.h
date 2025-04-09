#include "SensorInfo.h"
#include <tuple>

#include "Sensors/millisec.h"
#include "Sensors/clutch_engaged.h"
#include "Sensors/pit_requested.h"
#include "Sensors/speed.h"

#ifndef CMBTL_ALL_SENSORS_H
#define CMBTL_ALL_SENSORS_H

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

    using SensorInfoTuple = std::tuple<
    cmbtl::millisec::MILLI_SEC_SENSOR_INFO,
    cmbtl::clutch_engaged::CLUTCH_ENGAGED_SENSOR_INFO,
    cmbtl::pit_requested::PIT_REQUESTED_SENSOR_INFO>;
}
#endif