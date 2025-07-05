#include "CommBoardData.h"

using namespace cmb;

cmb::Data::Data() {
    //Sensor data info stores
    sensorDataInfo.insert(sensorDataInfo.begin() + SensorIndex::MILLI_SEC, SensorDataInfo(&cmb::Data::getMilliSec, 16));
    sensorDataInfo.insert(sensorDataInfo.begin() + SensorIndex::CLUTCH_ENGAGED, SensorDataInfo(&cmb::Data::getClutchEngaged, 1));
}

data_return_type cmb::Data::getMilliSec() {
    return static_cast<data_return_type>(millis);
}

data_return_type cmb::Data::getClutchEngaged() {
    return static_cast<data_return_type>(clutchEngaged);
}

data_return_type cmb::Data::getPitRequested() {
    return static_cast<data_return_type>(pitRequested);
}

data_return_type cmb::Data::getPositionX() {
    data_return_type return_val;
    memcpy(&return_val, &position_x, sizeof(data_return_type));
    return return_val;
}