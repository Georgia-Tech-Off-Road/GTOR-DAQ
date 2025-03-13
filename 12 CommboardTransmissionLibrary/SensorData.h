#include "Sensors.h"
#ifndef CMBTL_SENSOR_DATA_H
#define CMBTL_SENSOR_DATA_H
namespace cmbtl {
    struct SensorData {
        //Time since data aquisition began
        cmbtl::millisec::SV millisec;
        //Is clutch engaged or not?
        unsigned int clutchEngaged : 1;
        //Pit stop requested?
        unsigned int pitRequested : 1;

        //Speed X
        float speed_x;
        //Speed Y
        float speed_y;
        //Speed Z
        float speed_z;

        //Position X
        float position_x;
        //Position Y
        float position_y;
        //Position Z
        float position_z;

        unsigned int brake_pressure_front : 12;
        unsigned int brake_pressure_back : 12;

        unsigned int RPM : 14;

        //Linear Accelerations
        float linear__acceleration_x;
        float linear_acceleration_y;
        float linear_acceleartion_z;

        //Continuously Variable Transmission Temp
        unsigned int cvt_temp : 12;

        //Brake Temperature (PLEASE RENAME THESE!!!!!!)
        //TODO: Rename brake temperatures
        unsigned int break_temp_front_left : 12;
        unsigned int break_temp_front_right : 12;
        unsigned int break_temp_back : 12;

        //Steering wheel rotation rotation
        unsigned int steering_rotation : 12;
    };
}
#endif
