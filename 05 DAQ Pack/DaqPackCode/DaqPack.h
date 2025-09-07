#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>
#include <Adafruit_ADS1X15.h>
#include <TeensyThreads.h>
#include <DAQ_AMT22.h>
#include <DAQ_RPM_Sensor.h>

#include <DAQSensors.h>
#include <DAQPackets.h>


#define BAUD 230400

#define serialMonitor Serial

//declare all setup functions
inline void initDataStructValues();
inline void setUpSD();
inline void initializeADS1();
void updateAnalogValueFlag1();

//declare non setup functions
time_t getTeensy3Time();

//declare DaqPackCode.ino functions
void dataAquisitionAndSavingLoop();

// TODO: Replace with DAQ Sensor Data
//struct for sensor data
struct {
  unsigned long long int seconds;
  unsigned long int micros;
  float RPMs[4];
  int analogValues1[4];
  float teensyTemp;
} dataStruct;

//enums for AUXDAQ ports
enum AUXDAQ_Ports {
  RPM1 = 20,
  RPM2 = 16,
  RPM3 = 14,
  RPM4 = 37,
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
  RDTEETH = 3,
  ENGTEETH = 3,
  FRTEETH = 3,
  FLTEETH = 3
};

//outputFile
File outputFile;

//flag for if recording
bool isRecording = false;

//saves the last time data was saved 
ulong lastSaveTimeInMillis = 0;

//saves the last time auto save
ulong autoSaveTimeMillis = 0;

//TODO: Change to use new data struct
//function to intialize dataStruct values
inline void initDataStructValues() {
  dataStruct.seconds = now();
  dataStruct.micros = micros();
  dataStruct.RPMs[0] = 0;
  dataStruct.RPMs[1] = 0;
  dataStruct.RPMs[2] = 0;
  dataStruct.RPMs[3] = 0;
  dataStruct.analogValues1[0] = 0;
  dataStruct.analogValues1[1] = 0;
  dataStruct.analogValues1[2] = 0;
  dataStruct.analogValues1[3] = 0;
}


// TODO: Change file extension
//method to setup outFile/SD card
inline void setUpSD() {
  SD.begin(BUILTIN_SDCARD);
  delay(500);
  String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second())+".bin";
  Serial.println(time.c_str());
  outputFile = SD.open(time.c_str(),  FILE_WRITE);
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