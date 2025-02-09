#include <vector>
#include <string>
#include <list>
#include "macrologger.h"

struct Time {
    uint32_t sec;
    uint32_t milli;

    Time operator-(Time other) {
        Time res;
        res.sec = sec - other.sec;
        res.milli = milli - other.milli;
        return res;
    }
};

enum DAQSensorType {digital = 0, analog = 1};

class DAQSensor {
    private: 
        int index;
        std::string sensorName;
        int pollingRate;
        int numTeeth;
        std::vector<void*> data;
    public: 
        DAQSensorType type;
    public: 
        void initializeSensor(char* line);
        std::string toStr();
        DAQSensorType getSensorType();
};

class AnalogDAQSensor : DAQSensor {
    public:
        // Converts given text input into converted format for .csv
        char* (*convert)(char*);
        // Can line convert, see convert function above!^
        virtual inline bool canLineConvert() {return true;};
};

class DigitalDAQSensor : DAQSensor {
    public:
        // Records when the bit flipped on the sensor
        std::list<Time> changeList;
        // Give the time difference between bit flips,
        // returns the converted value in text format for csv
        char* (*convertFromTimeDiff)(Time);
};