#include "../../SensorInfo.h"
#include <cstdint>
#include <string>
#include <sstream>

#ifndef CMBTL_CLUTCH_ENGAGED_H
#define CMBTL_CLUTCH_ENGAGED_H
namespace cmbtl {
    namespace clutch_engaged {

        inline void serializeToJSON(const bool& convertedVal, std::stringstream& ss) {
            ss << "\"clutchEngaged\": " << (convertedVal ? "true" : "false");
        }

        using CLUTCH_ENGAGED_SENSOR_INFO = DefaultBoolSensorInfo<serializeToJSON>;
    }
}
#endif