/*
 * AMT22_Arduino_Sample_Code.ino
 * Author: Simone Di Blasi
 * Date: December, 2020
 * 
 * This sample code can be used with the Arduino Uno to control the AMT22 encoder.
 * It uses SPI to control the encoder and the the Arduino UART to report back to the PC
 * via the Arduino Serial Monitor.
 * 
 * After uploading code to Arduino Uno open the open the Serial Monitor under the Tools 
 * menu and set the baud rate to 115200 to view the serial stream the position from the AMT22.
 * 
 * Arduino Pin Connections
 * SPI Chip Select Enc:     Pin  2
 * SPI MOSI                 Pin 11
 * SPI MISO                 Pin 12
 * SPI SCLK:                Pin 13
 * 
 * 
 * AMT22 Pin Connections
 * Vdd (5V):                Pin  1
 * SPI SCLK:                Pin  2
 * SPI MOSI:                Pin  3
 * GND:                     Pin  4
 * SPI MISO:                Pin  5
 * SPI Chip Select:         Pin  6
 * 
 * This code takes inspiration and is based on the code shared by CUI Devices which authorizes its diffusion and modification.
 */

#include "AMT22_lib.h"
#include "SPI.h"

/* Serial rates for UART */
#define BAUDRATE        115200


/* Define special ascii characters */
#define NEWLINE         0x0A
#define TAB             0x09

/* We will use these define macros so we can write code once compatible with 12 or 14 bit encoders */
#define RES12           12
#define RES14           14

/* SPI pins */
#define ENC             10

/*Object creation*/
AMT22* Encoder;
//tried halfing the data rate to hopefully get the data bits to line up with clocks properly
SPISettings settingsA(100000, MSBFIRST, SPI_MODE0); 

void setup(){
  //Initialize the UART serial connection for debugging
  Serial.begin(BAUDRATE);
  //Initialize the SPI communication
  SPI.begin(); //This operation only needs to be done once, not once for each Encoder object.
  //Initialize the encoder object with necessary parameters 
  Encoder = new AMT22(ENC,RES12);
  pinMode(ENC, OUTPUT);
  digitalWrite(ENC, HIGH);
}

void loop() 
{
  //create a 16 bit variable to hold the encoders position
  uint16_t currentPosition;
  //let's also create a variable where we can count how many times we've tried to obtain the position in case there are errors
  //if you want to set the zero position before beggining uncomment the following function call
  //once we enter this loop we will run forever
  uint8_t data;

  //Binary array
  bool binaryArray[16];
  while(1)
  {
    digitalWrite(ENC, LOW);
    delayMicroseconds(3);
    //SPI.beginTransaction(settingsA);
    data = SPI.transfer(0x00);
    currentPosition = data << 8;
    delayMicroseconds(3);
    //SPI.beginTransaction(settingsA);
    data = SPI.transfer(0x00);
    currentPosition |= data;
    delayMicroseconds(3);
    digitalWrite(ENC, HIGH);
    //run through the 16 bits of position and put each bit into a slot in the array so we can do the checksum calculation
    for(int i = 0; i < 16; i++) binaryArray[i] = (0x01) & (currentPosition >> (i));

    //using the equation on the datasheet we can calculate the checksums and then make sure they match what the encoder sent
    if ((binaryArray[15] == !(binaryArray[13] ^ binaryArray[11] ^ binaryArray[9] ^ binaryArray[7] ^ binaryArray[5] ^ binaryArray[3] ^ binaryArray[1])) && (binaryArray[14] == !(binaryArray[12] ^ binaryArray[10] ^ binaryArray[8] ^ binaryArray[6] ^ binaryArray[4] ^ binaryArray[2] ^ binaryArray[0])))
        currentPosition &= 0x3FFF; //we got back a good position, so just mask away the checkbits
    else
        currentPosition = 0xFFFF; //bad position

    //If the resolution is 12-bits, and wasn't 0xFFFF, then shift position, otherwise do nothing
    if (currentPosition != 0xFFFF) currentPosition = currentPosition >> 2;
    Serial.printf("%f\n", convertToDegrees(currentPosition));
    delayMicroseconds(40);
  }
}

float convertToDegrees(uint16_t value) {
  return (static_cast<float>(value) * 360) / 4095;
}


