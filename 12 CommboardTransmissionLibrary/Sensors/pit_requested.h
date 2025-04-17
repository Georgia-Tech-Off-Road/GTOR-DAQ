#include "../SensorInfo.h"
#ifndef CMBTL_PIT_REQUESTED_H
#define CMBTL_PIT_REQUESTED_H
namespace cmbtl {
    namespace pit_requested {

        void serializeToJSON(const bool& convertedVal, std::stringstream& ss) {
            ss << "pit_requested: " << (convertedVal ? "true" : "false");
        }

        using PIT_REQUESTED_SENSOR_INFO = DefaultBoolSensorInfo<serializeToJSON>;
    }
}
#endif