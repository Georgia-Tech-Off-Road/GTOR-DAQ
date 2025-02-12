#include <cstdint>
#ifndef TIME_H
#define TIME_H
struct Time {
    uint32_t sec;
    uint32_t milli;
    Time operator-(Time other) {
        Time res;
        res.sec = sec - other.sec;
        res.milli = milli - other.milli;
        return res;
    }
    std::string toStr() {
        char buffer[200];
        sprintf(buffer, "Sec: %f\nMillisec:%f", sec, milli);
        return std::string(buffer);
    }
};

#endif