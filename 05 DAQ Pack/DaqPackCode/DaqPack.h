#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>
#include <Adafruit_ADS1X15.h>
#include <TeensyThreads.h>
#include <DAQ_AMT22.h>
#include <DAQ_RPM_Sensor.h>
#include <DAQ_Linear_Analog_Sensor.h>

#include <DAQSensors.h>
#include <DAQPackets.h>

//debug macros
#define SD_CARD_INIT_LED 3
#define ANALOG_ONE_LED 4
#define ANALOG_TWO_LED 5
#define ANALOG_THREE_LED 6
#define ANALOG_FOUR_LED 7

#define RPM_ONE_LED 8
#define RPM_TWO_LED 9
#define RPM_THREE_LED 10
#define RPM_FOUR_LED 11

#define RECORDING_LED 12


// Define number of teeth in one place for easy modification
#define NUM_TESTING_TEETH 22

// Define min and max expected RPM values
#define MIN_EXPECTED_RPM 0
#define MAX_EXPECTED_RPM 10000

#define BAUD 230400

#define serialMonitor Serial

//declare non setup functions
time_t getTeensy3Time();

cmbtl::DAQSensorDataType DAQData;

//enums for AUXDAQ ports
enum AUXDAQ_Ports {
  RPM1 = 20,
  RPM2 = 37,
  RPM3 = 14,
  RPM4 = 16,
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
};
//sensor constants
enum Sensor_Constants {
  RDTEETH = 14,
  ENGTEETH = 1,
  FRTEETH = 4,
  FLTEETH = 4
};

//function to test all debug LEDs
inline void flashBang() {
  digitalWrite(RECORDING_LED, HIGH);
  digitalWrite(RPM_ONE_LED, HIGH);
  digitalWrite(RPM_TWO_LED, HIGH);
  digitalWrite(RPM_THREE_LED, HIGH);
  digitalWrite(RPM_FOUR_LED, HIGH);
  digitalWrite(ANALOG_ONE_LED, HIGH);
  digitalWrite(ANALOG_TWO_LED, HIGH);
  digitalWrite(ANALOG_THREE_LED, HIGH);
  digitalWrite(ANALOG_FOUR_LED, HIGH);
  digitalWrite(SD_CARD_INIT_LED, HIGH);
  delay(5000);
  digitalWrite(RECORDING_LED, LOW);
  digitalWrite(RPM_ONE_LED, LOW);
  digitalWrite(RPM_TWO_LED, LOW);
  digitalWrite(RPM_THREE_LED, LOW);
  digitalWrite(RPM_FOUR_LED, LOW);
  digitalWrite(ANALOG_ONE_LED, LOW);
  digitalWrite(ANALOG_TWO_LED, LOW);
  digitalWrite(ANALOG_THREE_LED, LOW);
  digitalWrite(ANALOG_FOUR_LED, LOW);
  digitalWrite(SD_CARD_INIT_LED, LOW);
}

//outputFile
File outputFile;

//flag for if recording
bool isRecording = false;

//saves the last time data was saved 
ulong lastSaveTimeInMillis = 0;

//saves the last time auto save
ulong autoSaveTimeMillis = 0;

//led pinmode declarations
inline void initDebugLEDs() {
  pinMode(SD_CARD_INIT_LED, OUTPUT);
  digitalWrite(SD_CARD_INIT_LED, 0);
  pinMode(ANALOG_ONE_LED, OUTPUT);
  digitalWrite(ANALOG_ONE_LED, 0);
  pinMode(ANALOG_TWO_LED, OUTPUT);
  digitalWrite(ANALOG_TWO_LED, 0);
  pinMode(ANALOG_THREE_LED, OUTPUT);
  digitalWrite(ANALOG_THREE_LED, 0);
  pinMode(ANALOG_FOUR_LED, OUTPUT);
  digitalWrite(ANALOG_FOUR_LED, 0);
  pinMode(RPM_ONE_LED, OUTPUT);
  digitalWrite(RPM_ONE_LED, 0);
  pinMode(RPM_TWO_LED, OUTPUT);
  digitalWrite(RPM_TWO_LED, 0);
  pinMode(RPM_THREE_LED, OUTPUT);
  digitalWrite(RPM_THREE_LED, 0);
  pinMode(RPM_FOUR_LED, OUTPUT);
  digitalWrite(RPM_FOUR_LED, 0);
  pinMode(RECORDING_LED, OUTPUT);
  digitalWrite(RECORDING_LED, 0);
}

//function to intialize dataStruct values
inline void initDataStructValues() {
  DAQData.setData<cmbtl::SEC>(now());
  DAQData.setData<cmbtl::MICRO_SEC>(micros());
  DAQData.setData<cmbtl::RPM1>(0);
  DAQData.setData<cmbtl::RPM2>(0);
  DAQData.setData<cmbtl::RPM3>(0);
  DAQData.setData<cmbtl::RPM4>(0);
  DAQData.setData<cmbtl::RearBrakePressure>(0);
  DAQData.setData<cmbtl::FrontBrakePressure>(0);
  DAQData.setData<cmbtl::LDSFrontLeft>(0);
  DAQData.setData<cmbtl::LDSFrontRight>(0);
}


//method to setup outFile/SD card
inline void setUpSD() {
  int SDCARDINIT = SD.begin(BUILTIN_SDCARD);
  Serial.printf("SD Card init status: %d\n",SDCARDINIT);
  if(!SDCARDINIT) {
    Serial.printf("SD FAILED TO INIT\n");
    digitalWrite(SD_CARD_INIT_LED, LOW);
  } else {
    Serial.printf("SD INITIALIZED\n");
    digitalWrite(SD_CARD_INIT_LED, HIGH);
  }
  delay(500);
  String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second())+".txt";
  Serial.println(time.c_str());
  outputFile = SD.open(time.c_str(),  FILE_WRITE);
  //add bracket at beginning to make it a list
  outputFile.printf("[\n");
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