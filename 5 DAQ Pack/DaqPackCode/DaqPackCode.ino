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

//create struct for data packet
struct DataStruct{
  unsigned long long int seconds;
  unsigned long int micros;
  int binaryValues[3];
  int analogValues[4];
  float orientation[3];
  float acceleration[3];
  float linearAcceleration[3];
  float quaternionCoords[4];
  int calibration[4];
};

//create struct for queueNode
struct QueueNode {
  DataStruct *data;
  QueueNode *next;
};

//create struct for queue
struct Queue {
  QueueNode *head;
  QueueNode *tail;
};


//create a new queue (note may need to initialize some of the parameters of this node)
Queue Queue = {NULL, NULL};


File outputFile;

bool isRecording = false;

Adafruit_ADS1115 ads;

//current analog sensor number being polled
int currentAnalogSensor = 0;

//create an interval timer for BNO05
IntervalTimer BNO05Timer;

//create an interval timer for overal Data Collection Method
IntervalTimer dataCollectionTimer;

//creates a new instance of the BNO055 class
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);

//lists for the current analog and binary readings
int binaryValues[3];
int analogValues[4];

//variables for data from BNO05
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
  binaryValues[0] = digitalRead(20);
  binaryValues[1] = digitalRead(21);
  binaryValues[2] = digitalRead(22);
  //sets up interupts for binary values
  attachInterrupt(digitalPinToInterrupt(20), updateRearDiff, CHANGE); //rear diff
  attachInterrupt(digitalPinToInterrupt(21), updateFrontLeftHalleffect, CHANGE); // front left halleffect
  attachInterrupt(digitalPinToInterrupt(22), updateFrontRightHalleffect, CHANGE); // front right halleffect
  BNO05Timer.begin(updatBNO05Flag, 5000); //BNO05 polling flag
  dataCollectionTimer.begin(checkIfNewData, 100); //set rate of data checking
  dataCollectionTimer.priority(255); //set priority of interrupt to absolute lowest to make sure it doesn't interfere with data collection (prob have to tune this tbh)
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
  //size of is apparently computed at compile time
  if(Queue.head != NULL) {
    outputFile.write(Queue.head -> data, sizeof(DataStruct));
    QueueNode temp = *Queue.head;
    free(Queue.head -> data);
    free(Queue.head);
    Queue.head = temp.next;
  }
}
void checkIfNewData() {
  bool timeToUpdateData = false;
  if(BNO05flag) {
    updateBNO05Readings();
    BNO05flag = false;
    timeToUpdateData = true;
  }
  if (analogValueFlag) {
    readAnalogValues();
    analogValueFlag = false;
    timeToUpdateData = true;
  }
  if (digitalRead(7) && lastSaveTimeInMillis + 1000 < millis()) {
    changeRecordingState();
  }
  //set this to always read true to see how much of a perf increase the queue yields
  if (timeToUpdateData || !timeToUpdateData) {
    updateData();
  }
}

void updateData() {
  DataStruct *data = (DataStruct *)malloc(sizeof(DataStruct));
  data -> seconds = now();
  data -> micros = micros();
  data -> binaryValues[0] = binaryValues[0];
  data -> binaryValues[1] = binaryValues[1];
  data -> binaryValues[2] = binaryValues[2];
  data -> analogValues[0] = analogValues[0];
  data -> analogValues[1] = analogValues[1];
  data -> analogValues[2] = analogValues[2];
  data -> analogValues[3] = analogValues[3];
  data -> orientation[0] = orientationData.orientation.x;
  data -> orientation[1] = orientationData.orientation.y;
  data -> orientation[2] = orientationData.orientation.z;
  data -> linearAcceleration[0] = linearAccelData.acceleration.x;
  data -> linearAcceleration[1] = linearAccelData.acceleration.y;
  data -> linearAcceleration[2] = linearAccelData.acceleration.z;
  data -> acceleration[0] = accelerometerData.acceleration.x;
  data -> acceleration[1] = accelerometerData.acceleration.y;
  data -> acceleration[2] = accelerometerData.acceleration.z;
  data -> calibration[0] = BNO05System;
  data -> calibration[1] = gyro;
  data -> calibration[2] = accel;
  data -> calibration[3] = mag;
  QueueNode *queueNode = (QueueNode *)malloc(sizeof(QueueNode));
  queueNode -> data = data;
  if (Queue.head != NULL) {
    Queue.tail -> next = queueNode;
    Queue.tail = queueNode; 
  } else {
    Queue.head = queueNode;
    Queue.tail = queueNode;
  }
}

//method to update BNO05 readings
void updateBNO05Readings() {
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getCalibration(&BNO05System, &gyro, &accel, &mag);
  quat = bno.getQuat();
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
  binaryValues[0] = !binaryValues[0];
}
void updateFrontLeftHalleffect() {
  binaryValues[1] = !binaryValues[1];
}
void updateFrontRightHalleffect() {
  binaryValues[2] = !binaryValues[2];
}
void changeRecordingState() {
    noInterrupts();
    if(isRecording == true) {
      while(digitalRead(7) == 1) {
        delay(10);
      }
      while (Queue.head != NULL) {
        outputFile.write(Queue.head -> data, sizeof(DataStruct));
        QueueNode temp = *Queue.head;
        free(Queue.head -> data);
        free(Queue.head);
        Queue.head = temp.next;
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

