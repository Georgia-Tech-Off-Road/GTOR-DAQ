#include <AMT22.h>
#include <TeensyThreads.h>
#include <SPI.h>
#include <Arduino.h>

//functions that are involved with threads have to be declared volatile

AMT22 :: AMT22(uint8_t cs, uint8_t resolution) {
    //designate cs pin as an output
    pinMode(cs, OUTPUT);
    //raise cs line (AMT22 is active low cs)
    digitalWrite(cs, HIGH);
    _cs = cs;
    _resolution = resolution;
}

//public function to get position
float AMT22 :: getPosition() volatile
{
    return transmitOpCode(0x00);
}

//public function to zero sensor position
float AMT22 :: zeroPosition() {
    return transmitOpCode(0x70);
}

//public function to reset sensor
float AMT22 :: resetAMT22() {
    return transmitOpCode(0x60);
}

//local fucntion that returns position and sends whatever opCode is requested (returned degrees on a reset/zero can be used for any software needs in main program)
float AMT22 :: transmitOpCode(int opCode) volatile
{
    //create a binary array to make the checksum computation easier
    bool binaryArray[16];
    //integer value to be returned from sensor
    uint16_t currentPosition;
    //lower cs line
    digitalWrite(_cs, LOW);
    //have thread sleep for one millisecond (minimum time) (have to do this because AMT22 needs a 3 microscecond minimum wait between each packet and cs line going high and low )
    threads.delay(1);
    //get first half of data
    currentPosition = SPI.transfer(0x00);
    //shift first packet into upper 8 bits
    currentPosition <<= 8;
    //have thread sleep for one millisecond (minimum time) (have to do this because AMT22 needs a 3 microscecond minimum wait between each packet and cs line going high and low )
    threads.delay(1);
    //get second half of data
    currentPosition |= SPI.transfer(opCode);
    //have thread sleep for one millisecond (minimum time) (have to do this because AMT22 needs a 3 microscecond minimum wait between each packet and cs line going high and low )
    threads.delay(1);
    //raise cs line again
    digitalWrite(_cs, HIGH);
    //transfer integer respresentation of position to boolean array
    for(int i = 0; i < 16; i++) binaryArray[i] = (0x01) & (currentPosition >> (i));
    //evaluate the checksums and return all 1s if we got bad data
    if ((binaryArray[15] == !(binaryArray[13] ^ binaryArray[11] ^ binaryArray[9] ^ binaryArray[7] ^ binaryArray[5] ^ binaryArray[3] ^ binaryArray[1])) && (binaryArray[14] == !(binaryArray[12] ^ binaryArray[10] ^ binaryArray[8] ^ binaryArray[6] ^ binaryArray[4] ^ binaryArray[2] ^ binaryArray[0])))
        currentPosition &= 0x3FFF; //we got back a good position, so just mask away the checkbits
    else
        currentPosition = 0xFFFF; //bad position
    //If data is valid return degree representation, if not return all 1s
    if (currentPosition != 0xFFFF && _resolution == 12){
        currentPosition = currentPosition >> 2;
        return (static_cast<float>(currentPosition) * 360) / 4095;
    }  else if (currentPosition != 0xFFFF && _resolution == 14) {
        return (static_cast<float>(currentPosition) * 360) / 16383;
    }
    return static_cast<float>(currentPosition);
}