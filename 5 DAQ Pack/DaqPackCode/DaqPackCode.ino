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
#include <i2c_driver.h>
#include <i2c_driver_wire.h>
//threading library that allows all of our code to split off into seperate things
#include <TeensyThreads.h>
#include <AMT22.h>
#include <DAQ_RPM_Sensor.h>

#define BAUD 230400

#define serialMonitor Serial


struct {
  unsigned long long int seconds;
  unsigned long int micros;
  float RPMs[3];
  int analogValues[4];
  float steeringPosition;
} dataStruct;

File outputFile;

//declare variables for thread ids
int mainThread;
int AMT22PositionThread;

bool isRecording = false;

Adafruit_ADS1115 ads;

//current analog sensor number being polled
int currentAnalogSensor = 0;

volatile bool analogValueFlag = false;

//saves the last time data was saved 
ulong lastSaveTimeInMillis = 0;

//initialize AMT22 sensor
AMT22 steeringPositionSensor(10, 12);

//initialize RPM sensors (tooth nums are placeholders atm)
RPMSensor rearDiff(21, 33);
RPMSensor frontLeft(20, 33);
RPMSensor frontRight(22, 33);

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
  dataStruct.RPMs[0] = 0;
  dataStruct.RPMs[1] = 0;
  dataStruct.RPMs[2] = 0;
  //sets up interupt pin for ADS1115 ADC (have to pullup alert pin in accordance with ADS1115 datasheet)
  pinMode(15, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(15), updateAnalogValueFlag, FALLING);
  //set up ADC on seperate bus to hopefully make them play nice
  ads.begin(ADS1X15_ADDRESS, &Wire1);
  //set data rate to max 
  ads.setDataRate(RATE_ADS1115_128SPS);
  //verify that gain is set to two thirds (+-4.096)
  ads.setGain(GAIN_ONE);
  isRecording = true;
  digitalWrite(9, HIGH); //turn on red LED
  //start first ADC reading to begin the cycle
  ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
  //set thread slices
  threads.setSliceMicros(10);
  //add a thread for the main dataAquisitionAndSavingLoop
  mainThread = threads.addThread(dataAquisitionAndSavingLoop);
  //add a thread for steering position sensor
  AMT22PositionThread = threads.addThread(updateAMT22Reading);
  //have data board communicated with controller/transmitter board
  Wire.setClock(1000000);
  Wire.begin(0x40);
  Wire.onRequest(requestEvent);
  SPI.begin();
}
//do nothing here
void loop(){}
//writes data to SD card
void dataAquisitionAndSavingLoop() {
  while(1) {
    dataStruct.seconds = now();
    dataStruct.micros = micros();
    //size of is apparently computed at compile time
    outputFile.write(&dataStruct, sizeof(dataStruct));
    //check for RPM updates
    if (rearDiff.RPMUpdateFlag) {
      dataStruct.RPMs[0] = rearDiff.RPM;
      rearDiff.RPMUpdateFlag = false;
    }
    if (frontLeft.RPMUpdateFlag) {
      dataStruct.RPMs[1] = frontLeft.RPM;
      frontLeft.RPMUpdateFlag = false;
    }
    if (frontRight.RPMUpdateFlag) {
      dataStruct.RPMs[2] = frontRight.RPM;
      frontRight.RPMUpdateFlag = false;
    }
    if(steeringPositionSensor.steeringPositionUpdateFlag) {
      dataStruct.steeringPosition = steeringPositionSensor.steeringPosition;
      steeringPositionSensor.steeringPositionUpdateFlag = false;
    }
    if (analogValueFlag) {
      readAnalogValues();
      analogValueFlag = false;
    }
    if (digitalRead(7) && lastSaveTimeInMillis + 1000 < millis()) {
      changeRecordingState();
    }
  }
}
//method needed to get time
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}
void updateAnalogValueFlag() {
  analogValueFlag = true;
}
void updateAMT22Reading() {
  //get steering data
  while(1) {
    steeringPositionSensor.getPosition();
    threads.delay(1);
  }
}
void changeRecordingState() {
    noInterrupts();
    if(isRecording == true) {
      while(digitalRead(7) == 1) {
        delay(10);
        Serial.println("Button Engaged");
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

void requestEvent()
{
  Wire.write((byte *)&dataStruct, sizeof(dataStruct)); 
}