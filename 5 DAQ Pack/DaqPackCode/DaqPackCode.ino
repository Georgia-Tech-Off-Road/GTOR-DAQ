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
#include <Adafruit_BNO055.h>

#define BAUD 230400

#define serialMonitor Serial

struct {
  unsigned long long int seconds;
  unsigned long int micros;
  int binaryValues[3];
  int analogValues[4];
  float orientation[3];
  float acceleration[3];
  float linearAcceleration[3];
  float quaternionCoords[4];
  int calibration[4];
} dataStruct;


File outputFile;

bool isRecording = false;

Adafruit_ADS1115 ads;

//current analog sensor number being polled
int currentAnalogSensor = 0;

//create an interval timer for BNO05
IntervalTimer BNO05Timer;

//creates a new instance of the BNO055 class
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);

//varaibles for data from BNO05
sensors_event_t orientationData, linearAccelData, accelerometerData;
uint8_t BNO05System, gyro, accel, mag = 0;
imu::Quaternion quat;

volatile bool analogValueFlag = false;
volatile bool BNO05flag = false;

//saves the last time data was saved 
ulong lastSaveTimeInMillis = 0;

void setup() {
  pinMode(8, OUTPUT); //white LED (powered on)
  Serial.begin(115200);
  //set up time stuff for rtc
  setSyncProvider(getTeensy3Time);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  serialMonitor.begin(BAUD);
  SD.begin(BUILTIN_SDCARD);
  delay(500);
  String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second())+".bin";
  Serial.println(time.c_str());
  outputFile = SD.open(time.c_str(),  FILE_WRITE);
  //sets top leds to output
  pinMode(9, OUTPUT); //red LED (recording)
  digitalWrite(8, HIGH); //turn on white LED
  pinMode(7, INPUT_PULLDOWN); //pull down input for record stop/start button (NEEDED TO MAKE IT WORK)
  //set the values in the digital value array to their initial values (likely doesn't matter but feels better to do it this way)
  dataStruct.binaryValues[0] = digitalRead(20);
  dataStruct.binaryValues[1] = digitalRead(21);
  dataStruct.binaryValues[2] = digitalRead(22);
  //sets up interupts for binary values
  attachInterrupt(digitalPinToInterrupt(20), updateRearDiff, CHANGE); //rear diff
  attachInterrupt(digitalPinToInterrupt(21), updateFrontLeftHalleffect, CHANGE); // front left halleffect
  attachInterrupt(digitalPinToInterrupt(22), updateFrontRightHalleffect, CHANGE); // front right halleffect
  BNO05Timer.begin(updatBNO05Flag, 5000); //BNO05 polling flag
  //sets up interupt pin for ADS1115 ADC (have to pullup alert pin in accordance with ADS1115 datasheet)
  pinMode(15, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(15), updateAnalogValueFlag, FALLING);
  //set up ADC on seperate bus to hopefully make them play nice
  ads.begin(ADS1X15_ADDRESS, &Wire1);
  //set data rate to max 
  ads.setDataRate(RATE_ADS1115_860SPS);
  isRecording = true;
  digitalWrite(9, HIGH); //turn on red LED
  //start the BNO05
  bno.begin();
  //start first ADC reading to begin the cycle
  ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
  updateBNO05Readings();
}
//writes data to SD card
void loop() {
  dataStruct.seconds = now();
  dataStruct.micros = micros();
  //size of is apparently computed at compile time
  outputFile.write(&dataStruct, sizeof(dataStruct));
  if(BNO05flag) {
    updateBNO05Readings();
    BNO05flag = false;
  }
  if (analogValueFlag) {
    readAnalogValues();
    analogValueFlag = false;
  }
  if (digitalRead(7) && lastSaveTimeInMillis + 1000 < millis()) {
    changeRecordingState();
  }

}
//method to update BNO05 readings
void updateBNO05Readings() {
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getCalibration(&BNO05System, &gyro, &accel, &mag);
  quat = bno.getQuat();
  dataStruct.orientation[0] = orientationData.orientation.x;
  dataStruct.orientation[1] = orientationData.orientation.y;
  dataStruct.orientation[2] = orientationData.orientation.z;
  dataStruct.linearAcceleration[0] = linearAccelData.acceleration.x;
  dataStruct.linearAcceleration[1] = linearAccelData.acceleration.y;
  dataStruct.linearAcceleration[2] = linearAccelData.acceleration.z;
  dataStruct.acceleration[0] = accelerometerData.acceleration.x;
  dataStruct.acceleration[1] = accelerometerData.acceleration.y;
  dataStruct.acceleration[2] = accelerometerData.acceleration.z;
  dataStruct.calibration[0] = BNO05System;
  dataStruct.calibration[1] = gyro;
  dataStruct.calibration[2] = accel;
  dataStruct.calibration[3] = mag;
}
//method needed to get time
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}
void updateAnalogValueFlag() {
  analogValueFlag = true;
}
void updatBNO05Flag() {
  BNO05flag = true;
}
void updateRearDiff() {
  dataStruct.binaryValues[0] = !dataStruct.binaryValues[0];
}
void updateFrontLeftHalleffect() {
  dataStruct.binaryValues[1] = !dataStruct.binaryValues[1];
}
void updateFrontRightHalleffect() {
  dataStruct.binaryValues[2] = !dataStruct.binaryValues[2];
}
void changeRecordingState() {
    noInterrupts();
    if(isRecording == true) {
      while(digitalRead(7) == 1) {
        delay(10);
      }
      outputFile.close();
      digitalWrite(9, LOW); //turn off red LED
      Serial.println("Data Recording Stopped");
      isRecording = false;
    }
    else {
      while(digitalRead(7) == 1) {
        delay(10);
      }
      String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
      Serial.println(time.c_str());
      //turn on red LED
      digitalWrite(9, HIGH);
      outputFile = SD.open(time.c_str(),  FILE_WRITE);
      isRecording = true;
    }
    lastSaveTimeInMillis = millis();
    interrupts();    
}
void readAnalogValues() {
  switch (currentAnalogSensor) {
      case 0:
        dataStruct.analogValues[0] =  ads.getLastConversionResults();
        currentAnalogSensor = 1;
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_1, false);
        break;
      case 1:
        dataStruct.analogValues[1] =  ads.getLastConversionResults();
        currentAnalogSensor = 2;
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_2, false);
        break;
      case 2:
        dataStruct.analogValues[2] =  ads.getLastConversionResults();
        currentAnalogSensor = 3;
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_3, false);
        break;
      case 3:
        dataStruct.analogValues[3] =  ads.getLastConversionResults();
        currentAnalogSensor = 0;
        ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
        break;
  }
}

