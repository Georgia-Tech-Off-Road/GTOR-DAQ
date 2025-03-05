// Wire Slave Sender
// by Nicholas Zambetti http://www.zambetti.com/
// Modified by Richard Gemmell Oct 2019

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI slave device
// Refer to the "Wire Master Reader" example for use with this
// To use it, connect a master to the Teensy on pins 16 and 17.
//
// Consider using the I2CRegisterSlave class instead of Wire to
// create an I2C slave.

// Created 29 March 2006

// This example code is in the public domain.

#include <Arduino.h>
#include <i2c_driver.h>
#include <i2c_driver_wire.h>
#include <TimeLib.h>
struct {
  long long unsigned seconds;
  int ree;
  double test;
} myData;


void requestEvent();

int led = LED_BUILTIN;

void setup()
{
  setSyncProvider(getTeensy3Time);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  pinMode(led, OUTPUT);
  Wire.setClock(3000000);
  Wire.begin(0x40);        // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Serial.begin(9600);
}

void loop()
{

}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  myData.ree = 60;
  myData.test = 60.9092;
  myData.seconds=now();
  Wire.write((byte *)&myData, sizeof myData); 
}
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}