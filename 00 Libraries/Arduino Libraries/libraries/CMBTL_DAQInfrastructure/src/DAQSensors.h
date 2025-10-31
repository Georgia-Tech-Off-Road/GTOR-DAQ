#ifndef CMBTL_DAQ_SENSORS_H
#define CMBTL_DAQ_SENSORS_H

#include "SensorInfo.h"
#include "SensorData.h"
#include <tuple>

#include "Sensors/sec.h"
#include "Sensors/microsec.h"

#include "Sensors/RPM_Sensor.h"
#include "Sensors/Analog_Sensor.h"
#include "Sensors/Linear_Analog_Sensor.h"

#include "Sensors/teensy_temp.h"

using std::array;
namespace cmbtl {
    //--------------- Define Sensor Indexes ----------------------------------------------------
    enum SensorIndex {
        SEC,
        MICRO_SEC,
        RPM1,
        RPM2,
        RPM3,
        RPM4,
        RearBrakePressure,
        FrontBrakePressure,
        LDSFrontRight,
        LDSFrontLeft,
        LDSRearRight,
        LDSRearLeft,
        CVTTemp,
        RearTransferCaseTemp,
        TEENSY_TEMP
    };

    // Define RPM Sensors
    std::string RPM1GetName() { return "rpm1"; }
    std::string RPM2GetName() { return "rpm2"; }
    std::string RPM3GetName() { return "rpm3"; }
    std::string RPM4GetName() { return "rpm4"; }

    using RPM1_SENSOR_INFO = cmbtl::rpm_sensor::RPM_SENSOR<RPM1GetName>;
    using RPM2_SENSOR_INFO = cmbtl::rpm_sensor::RPM_SENSOR<RPM2GetName>;
    using RPM3_SENSOR_INFO = cmbtl::rpm_sensor::RPM_SENSOR<RPM3GetName>;
    using RPM4_SENSOR_INFO = cmbtl::rpm_sensor::RPM_SENSOR<RPM4GetName>;

    // Define Analog Sensors
    std::string RearBrakePressureGetName() { return "RearBrakePressure"; }
    std::string FrontBrakePressureGetName() { return "FrontBrakePressure"; }
    std::string LDSFrontRightGetName() { return "LDSFrontRight"; }
    std::string LDSFrontLeftGetName() { return "LDSFrontLeft"; }
    std::string LDSRearRightGetName() { return "LDSRearRight"; }
    std::string LDSRearLeftGetName() { return "LDSRearLeft"; }
    std::string CVTTempGetName() { return "CVTTemp"; }
    std::string RearTransferCaseTempGetName() { return "RearTransferCaseTemp"; }

    using RearBrakePressure_SENSOR_INFO = cmbtl::linear_analog_sensor::Linear_Analog_Sensor<RearBrakePressureGetName>;
    using FrontBrakePressur_SENSOR_INFO = cmbtl::linear_analog_sensor::Linear_Analog_Sensor<FrontBrakePressureGetName>;
    using LDSFrontRight_SENSOR_INFO = cmbtl::linear_analog_sensor::Linear_Analog_Sensor<LDSFrontRightGetName>;
    using LDSFrontLeft_SENSOR_INFO = cmbtl::linear_analog_sensor::Linear_Analog_Sensor<LDSFrontLeftGetName>;
    using LDSRearRight_SENSOR_INFO = cmbtl::linear_analog_sensor::Linear_Analog_Sensor<LDSRearRightGetName>;
    using LDSRearLeft_SENSOR_INFO = cmbtl::linear_analog_sensor::Linear_Analog_Sensor<LDSRearLeftGetName>;
    using CVTTemp_SENSOR_INFO = cmbtl::linear_analog_sensor::Linear_Analog_Sensor<CVTTempGetName>;
    using RearTransferCaseTemp_SENSOR_INFO = cmbtl::linear_analog_sensor::Linear_Analog_Sensor<RearTransferCaseTempGetName>;
    

    // The order that the sensor appear in the enum and this tuple MUST MUST BE THE SAME
    using SensorInfoTuple = std::tuple<
    cmbtl::sec::SEC_SENSOR_INFO,
    cmbtl::microsec::MICRO_SEC_SENSOR_INFO,
    RPM1_SENSOR_INFO,
    RPM2_SENSOR_INFO,
    RPM3_SENSOR_INFO,
    RPM4_SENSOR_INFO,
    RearBrakePressure_SENSOR_INFO,
    FrontBrakePressur_SENSOR_INFO,
    LDSFrontRight_SENSOR_INFO,
    LDSFrontLeft_SENSOR_INFO,
    LDSRearRight_SENSOR_INFO,
    LDSRearLeft_SENSOR_INFO,
    CVTTemp_SENSOR_INFO,
    RearTransferCaseTemp_SENSOR_INFO,
    cmbtl::teensy_temp::TEENSY_TEMP_SENSOR_INFO
    >;

    using DAQSensorDataType = SensorData<SensorInfoTuple>;

}
#endif