#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>
#include <Adafruit_ADS1X15.h>
#include <i2c_driver.h>
#include <i2c_driver_wire.h>
#include <TeensyThreads.h>
#include <AMT22.h>
#include <DAQ_RPM_Sensor.h>

#define BAUD 230400

#define serialMonitor Serial

//declare all setup functions
inline void initDataStructValues();
inline void initializeI2CCommLink();
inline void initializeSPI();
inline void setupPinModes();
inline void setUpSD();
inline void initializeThreads();
inline void turnOnLEDS();

//declare non setup functions
inline void requestEvent();
time_t getTeensy3Time();

//declare DaqPackCode.ino functions
void dataAquisitionAndSavingLoop();
void updateAMT22Reading();


//struct for sensor data
struct {
  unsigned long long int seconds;
  unsigned long int micros;
  float RPMs[3];
  int analogValues[4];
  float steeringPosition;
} dataStruct;

//enums for AUXDAQ ports
enum AUXDAQ_Ports {
  RPM1 = 20,
  RPM2 = 21,
  RPM3 = 21,
  LDS1 = 22,
  LDS2 = 23,
  SPIO = 24,
  SPIT = 25,
  I2C1 = 26,
  I2C2 = 27,
  STG1 = 28,
  STG2 = 29,
  STG3 = 30,
  STG4 = 31,
  AIN1 = 32,
  AIN2 = 33,
  AIN3 = 34,
  PWRL = 8,
  RECL = 9
};
//sensor constants
enum Sensor_Constants {
  AMT22RES = 12,
  RDTEETH = 33,
  FRTEETH = 33,
  FLTEETH = 34
};

//outputFile
File outputFile;

//thread ids
int mainThread;
int AMT22PositionThread;

//flag for if recording
bool isRecording = false;

//saves the last time data was saved 
ulong lastSaveTimeInMillis = 0;

//function to intialize dataStruct values
inline void initDataStructValues() {
  dataStruct.seconds = now();
  dataStruct.micros = micros();
  dataStruct.RPMs[0] = 0;
  dataStruct.RPMs[1] = 0;
  dataStruct.RPMs[2] = 0;
  dataStruct.analogValues[0] = 0;
  dataStruct.analogValues[1] = 0;
  dataStruct.analogValues[2] = 0;
  dataStruct.analogValues[3] = 0;
  dataStruct.steeringPosition = 0;
}

//function to initialize I2C link between data collection board and comms board
inline void initializeI2CCommLink() {
  Wire.setClock(1000000);
  Wire.begin(0x40);
  Wire.onRequest(requestEvent);
}

//function to handle I2C requests from comms board (runs on interrupt so WILL PREVENT DATA AQUISITION WHILE RUNNING)
void requestEvent()
{
  Wire.write((byte *)&dataStruct, sizeof(dataStruct)); 
}

//function to initialize all necessary SPI busses/ports
inline void initializeSPI() {
  SPI.begin();
}

//function to setup pinModes
inline void setupPinModes() {
  //setup power and recording LEDs as outputs
  pinMode(PWRL, OUTPUT);
  pinMode(RECL, OUTPUT);
  //setup save button as input
  pinMode(7, INPUT_PULLDOWN);
}

//method to setup outFile/SD card
inline void setUpSD() {
  SD.begin(BUILTIN_SDCARD);
  delay(500);
  String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second())+".bin";
  Serial.println(time.c_str());
  outputFile = SD.open(time.c_str(),  FILE_WRITE);
}

//function to initialize TeensyThreads
inline void initializeThreads() {
  //set thread slices
  threads.setSliceMicros(100);
  //add a thread for the main dataAquisitionAndSavingLoop
  mainThread = threads.addThread(dataAquisitionAndSavingLoop);
  //add a thread for steering position sensor
  AMT22PositionThread = threads.addThread(updateAMT22Reading);
}

//function to turn on power/recording LEDs
inline void turnOnLEDS() {
  //turn on power LED
  digitalWrite(PWRL, HIGH);
  //turn on recording LED
  digitalWrite(RECL, HIGH);
}

//time setup function
inline void setupTeensyTime() {
  setSyncProvider(getTeensy3Time);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
}

//method needed to get time
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}