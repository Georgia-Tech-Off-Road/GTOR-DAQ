#ifndef HallEffect_H
#define HallEffect_H
#include "Arduino.h"

class HallEffect  {
    private:
        int _pin;   
    public: 
        HallEffect(int pin);
        int getPin();
        bool getHigh();
};

#endif