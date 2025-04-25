#include "../Time/Time.h"

namespace cvf {
<<<<<<< HEAD
    inline char* convertRPM(char *line, Time *time) {
        static float prevRPM = 0;
        Time prevTimeStamp;
        return "0";
=======

    // No implementation currently needed
    inline char* convertRPM(char *line, Time *time) {
        return line;
>>>>>>> b1196b1a (Fixed cvf namespace problems, modified CVTTemp convert)
    }
}