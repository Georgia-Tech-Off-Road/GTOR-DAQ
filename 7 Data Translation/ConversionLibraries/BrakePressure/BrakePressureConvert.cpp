#include <BrakePressureConvert.h>

//
double convertBrakePressure(int64_t analogue_value) {
    return (double)(50+(((analogue_value/32767)*4.096-.5)/4)*7950);
}