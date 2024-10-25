#include "HallEffect.h"
#include "Arduino.h"

HallEffect :: HallEffect (int pin) {
    _pin = pin;
}
bool HallEffect :: getHigh () {
    return digitalRead(_pin);
}
int HallEffect :: getPin () {
    return _pin;
}
