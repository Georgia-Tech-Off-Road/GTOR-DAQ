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
  bool binaryArray[16];           //after receiving the position we will populate this array and use it for calculating the checksum
  //if you want to set the zero position before beggining uncomment the following function call
  //Encoder->setZeroSPI();
  uint8_t data;
  //once we enter this loop we will run forever
  while(1)
  {
    digitalWrite(ENC, LOW);
    delay(0.004);
    data = SPI.transfer(0x00);
    delay(0.004);
    digitalWrite(ENC, HIGH);
    Serial.printf("%d\n", data);
    delay(200);
  }
}
