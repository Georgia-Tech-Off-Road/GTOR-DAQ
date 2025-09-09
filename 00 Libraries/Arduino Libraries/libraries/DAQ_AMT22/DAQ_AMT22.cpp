#include <DAQ_AMT22.h>

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
void AMT22 :: getPosition() volatile
{
    transmitOpCode(0x00);
}

//public function to zero sensor position
void AMT22 :: zeroPosition() {
    transmitOpCode(0x70);
}

//public function to reset sensor
void AMT22 :: resetAMT22() {
    transmitOpCode(0x60);
}

//local fucntion that returns position and sends whatever opCode is requested (returned degrees on a reset/zero can be used for any software needs in main program)
void AMT22 :: transmitOpCode(int opCode) volatile
{
    //create a binary array to make the checksum computation easier
    bool binaryArray[16];
    //integer value to be returned from sensor
    uint16_t currentPosition;
    //lower cs line
    digitalWrite(_cs, LOW);
    //have threads yield for 30 microseconds (have to do this because AMT22 needs a 3 microscecond minimum wait between each packet and cs line going high and low plus lot of buffer)
    delayMicroseconds(3);
    //get first half of data
    currentPosition = SPI.transfer(0x00);
    //shift first packet into upper 8 bits
    currentPosition <<= 8;
    //have threads yield for 30 microseconds (have to do this because AMT22 needs a 3 microscecond minimum wait between each packet and cs line going high and low plus lot of buffer)
    delayMicroseconds(3);
    //get second half of data
    currentPosition |= SPI.transfer(opCode);
    //have threads yield for 30 microseconds (have to do this because AMT22 needs a 3 microscecond minimum wait between each packet and cs line going high and low plus lot of buffer)
    delayMicroseconds(3);
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
        steeringPosition =  (static_cast<float>(currentPosition) * 360) / 4095;
    }  else if (currentPosition != 0xFFFF && _resolution == 14) {
        steeringPosition =  (static_cast<float>(currentPosition) * 360) / 16383;
    } else {
        //set steeringPosition variable
        steeringPosition = static_cast<float>(currentPosition);
    }
    //set flag
    steeringPositionUpdateFlag = true;
}