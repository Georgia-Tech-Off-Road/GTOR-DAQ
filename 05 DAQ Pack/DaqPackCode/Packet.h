enum SensorID : uint8_t {
    ENGINE_RPM      = 0,
    FRONT_LEFT_RPM  = 1,
    FRONT_RIGHT_RPM = 2,
    AUX_RPM         = 3,
    REAR_BRAKE_PRES = 4,
    FRONT_BRAKE_PRES= 5,
    LDS_FRONT_RIGHT = 6,
    LDS_FRONT_LEFT  = 7,
    LDS_REAR_RIGHT  = 8,
    LDS_REAR_LEFT   = 9,
    CVT_TEMP        = 10,
    REAR_TC_TEMP    = 11,
    TEENSY_TEMP     = 12
};

struct __attribute__((packed)) DataPacket {
    uint8_t sync;
	uint8_t sensorID;
    // Unit is in units of 10^-4 sec
	uint32_t timestamp100Micros;
	float value;
}

