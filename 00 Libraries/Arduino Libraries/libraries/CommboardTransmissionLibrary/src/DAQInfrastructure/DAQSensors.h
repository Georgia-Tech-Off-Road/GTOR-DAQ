#ifndef CMBTL_DAQ_SENSORS_H
#define CMBTL_DAQ_SENSORS_H

#include "../SensorInfo.h"
#include "../SensorData.h"
#include <tuple>

#include "Sensors/millisec.h"
#include "Sensors/clutch_engaged.h"
#include "Sensors/pit_requested.h"
#include "Sensors/speed.h"
#include "Sensors/position.h"
#include "Sensors/brake_pressure.h"
#include "Sensors/RPM.h"
#include "Sensors/linear_acceleration.h"
#include "Sensors/CVT_temp.h"
#include "Sensors/brake_temp.h"
#include "Sensors/steering_rotation.h"

using std::array;
namespace cmbtl {
    //--------------- Define Sensor Indexes ----------------------------------------------------
    enum SensorIndex {
        MILLI_SEC,
        CLUTCH_ENGAGED,
        PIT_REQUESTED,
        SPEED,
        POSITION,
        BRAKE_PRESSURE,
        RPM,
        LINEAR_ACCELERATION,
        CVT_TEMP,
        BRAKE_TEMP,
        STEERING_ROTATION,
        //COUNT IS NOT A REAL SENSOR, it is used to determine how many elements are in this enum
        //COUNT SHOULD ALWAYS BE THE LAST ELEMENT
        COUNT,
    };

    // The order that the sensor appear in the enum and this tuple MUST MUST BE THE SAME
    using SensorInfoTuple = std::tuple<
    cmbtl::millisec::MILLI_SEC_SENSOR_INFO,
    cmbtl::clutch_engaged::CLUTCH_ENGAGED_SENSOR_INFO,
    cmbtl::pit_requested::PIT_REQUESTED_SENSOR_INFO,
    cmbtl::speed::SPEED_SENSOR_INFO,
    cmbtl::position::POSITION_SENSOR_INFO,
    cmbtl::brake_pressure::BRAKE_PRESSURE_SENSOR_INFO,
    cmbtl::rpm::RPM_SENSOR_INFO,
    cmbtl::linear_acceleration::LINEAR_ACCELERATION_SENSOR_INFO,
    cmbtl::cvt_temp::CVT_TEMP_SENSOR_INFO,
    cmbtl::brake_temp::BRAKE_TEMP_SENSOR_INFO,
    cmbtl::steering_rotation::STEERING_ROTATION_SENSOR_INFO>;

    using DAQSensorDataType = SensorData<SensorInfoTuple>;

}
#endif