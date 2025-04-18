// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>
// Modified by Richard Gemmell Oct 2019

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device connected to pins 18 and 19.
//
// Consider using the I2CDevice class instead of Wire to read a sensor.

// Created 29 March 2006

// This example code is in the public domain.


#include <Arduino.h>
#include <i2c_driver.h>
#include <i2c_driver_wire.h>
#include "SD.h"
#include <TimeLib.h>

struct {
  unsigned long long int seconds;
  unsigned long int micros;
  int binaryValues[3];
  int analogValues[4];
} dataStruct;

long long unsigned int pastTime;
int led = LED_BUILTIN;
File outputFile;

byte* rxBufferPointer;

void setup()
{
  setSyncProvider(getTeensy3Time);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  pinMode(led, OUTPUT);
  Wire.setClock(1000000);
  Wire.begin();                         // join i2c bus
  SD.begin(BUILTIN_SDCARD);
  delay(500);
  String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second())+".txt";
  outputFile = SD.open(time.c_str(),  FILE_WRITE);
  pinMode(22, INPUT_PULLUP);
  pastTime = millis();
  rxBufferPointer = Wire.get_RX_Buffer_Pointer();
}

void loop()
{
    if(Wire.requestFrom(0x40, sizeof(dataStruct))) {
      memcpy(&dataStruct, rxBufferPointer, sizeof(dataStruct));
      Serial.printf("%llu\n", dataStruct.seconds);
      //Serial.printf("%d\n", Wire.read());
      //Serial.printf("%llu\n",dataStruct.seconds);
      if (digitalRead(22) == 0) {
        outputFile.close();
        Serial.println("FileCLOSED");
      }
  }
  if (digitalRead(22) == 0) {
    outputFile.close();
    Serial.println("FileCLOSED");
  }
  delay(50);
}

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}
