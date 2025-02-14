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
        snprintf(buffer, 200, "Sec: %d\nMillisec:%d", sec, milli);
        return std::string(buffer);
    }
};

#endif