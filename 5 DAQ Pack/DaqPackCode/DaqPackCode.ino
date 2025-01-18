//Note: May be able to go faster by modifying core Teensy 4.1 code to reduce the amount of averages that it takes when doing an analog read
//Note: Also could maybe decrease bit resolution
//Note: Should get an external powersupply to get better VCC stability
//sd card stuff

//try to change this to use interupts instead of constantly checking each pin (there's apparently a seperate piece of hardware that geenrates the interupt signal so using interupts should save clock cycles)
#include "SD.h"
#include "SPI.h"
//time stuff
#include <TimeLib.h>
#include <Adafruit_ADS1X15.h>
#include <Wire.h>

#define BAUD 230400

#define serialMonitor Serial

File outputFile;

bool isRecording = false;

//the index in this matches up to the index of binary data in the printf
int binaryValues[3] = {0, 0, 0};

Adafruit_ADS1115 ads;
//stores current values for sensors connected to adc0
int analogValues[4] = {0, 0, 0, 0};
//current analog sensor number being polled
int currentAnalogSensor = 0;

int lastSaveTimeInSeconds = 0;

void setup() {
  // set the Time library to use Teensy 3.0's RTC to keep time
  pinMode(8, OUTPUT); //white LED (powered on)
  Serial.begin(115200);
  setSyncProvider(getTeensy3Time);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  serialMonitor.begin(BAUD);
  SD.begin(BUILTIN_SDCARD);
  delay(500);
  String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
  Serial.println(time.c_str());
  outputFile = SD.open(time.c_str(),  FILE_WRITE);
  //sets top leds to output
  pinMode(9, OUTPUT); //red LED (recording)
  digitalWrite(8, HIGH); //turn on white LED
  pinMode(7, INPUT_PULLDOWN); //pull down input for record stop/start button (NEEDED TO MAKE IT WORK)
  //set the values in the digital value array to their initial values (likely doesn't matter but feels better to do it this way)
  binaryValues[0] = digitalRead(20);
  binaryValues[1] = digitalRead(21);
  binaryValues[2] = digitalRead(22);
  //sets up interupts for binary values
  attachInterrupt(digitalPinToInterrupt(7), changeRecordingState, CHANGE); //record button
  attachInterrupt(digitalPinToInterrupt(20), updateRearDiff, CHANGE); //rear diff
  attachInterrupt(digitalPinToInterrupt(21), updateFrontLeftHalleffect, CHANGE); // front left halleffect
  attachInterrupt(digitalPinToInterrupt(22), updateFrontRightHalleffect, CHANGE); // front right halleffect
  //sets up interupt pin for ADS1115 ADC (has to pullup alert pin in accordance with ADS1115 datasheet)
  pinMode(15, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(15), readAnalogValues, FALLING);
  //configure ADC for continuous operation 
  ads.begin();
  //set data rate to max 
  ads.setDataRate(RATE_ADS1115_860SPS);
  pinMode(17, INPUT_PULLDOWN); // front brake pressure
  pinMode(16, INPUT_PULLDOWN); //rear brake pressure
  isRecording = true;
  digitalWrite(9, HIGH); //turn on red LED
  //start first ADC reading to begin the cycle
  ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
}
//writes data to SD card
void loop() {
  //try testing with an oscilloscope
  //potentially reduce print buffer time?
  //use flush?
  outputFile.printf("%llu,%lu,%d,%d,%d,%d,%d,%d,%d\n", now(), micros()
  , binaryValues[0], binaryValues[1], binaryValues[2], analogValues[0], analogValues[1], analogValues[2], analogValues[3]);
  //Serial.printf("%llu,%lu,%d,%d,%d,%d,%d,%d,%d\n", now(), micros()
  //, binaryValues[0], binaryValues[1], binaryValues[2], analogValues[0], analogValues[1], analogValues[2], analogValues[3]);
}
//method needed to get time
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}
//I think you need a seperate one for some reason :(
void updateRearDiff() {
  binaryValues[0] = !binaryValues[0];
}
void updateFrontLeftHalleffect() {
  binaryValues[1] = !binaryValues[1];
}
void updateFrontRightHalleffect() {
  binaryValues[2] = !binaryValues[2];
}
void readAnalogValues() {
  switch (currentAnalogSensor) {
      case 0:
        analogValues[0] =  ads.getLastConversionResults();
        currentAnalogSensor = 1;
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_1, false);
        break;
      case 1:
        analogValues[1] =  ads.getLastConversionResults();
        currentAnalogSensor = 2;
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_2, false);
        break;
      case 2:
        analogValues[2] =  ads.getLastConversionResults();
        currentAnalogSensor = 3;
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_3, false);
        break;
      case 3:
        analogValues[3] =  ads.getLastConversionResults();
        currentAnalogSensor = 0;
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
        break;
  }
}

void changeRecordingState() {
  if(lastSaveTimeInSeconds + 1 > int(second())) {
    return;
  }
  if(isRecording == true) {
    while(digitalRead(7) == 1) {
      Serial.println("RELEASE BUTTON");
    }
    outputFile.println(now());
    outputFile.close();
    digitalWrite(9, LOW); //turn off red LED
    Serial.println("Data Recording Stopped");
     
    isRecording = false;
  }
  else {
    while(digitalRead(7) == 1) {
      Serial.println("RELEASE BUTTON");
    }
    String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
    Serial.println(time.c_str());
    //turn on red LED
    digitalWrite(9, HIGH);
    outputFile = SD.open(time.c_str(),  FILE_WRITE);
    outputFile.println(now());
    isRecording = true;
  }
  lastSaveTimeInSeconds = int(second());
}
