#include "Globals.h"
#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>
#include <Adafruit_ADS1X15.h>
#include <ADS1256.h>

#include <TeensyThreads.h>
#include <DAQ_AMT22.h>
#include <DAQ_RPM_Sensor.h>
#include <DAQ_Linear_Analog_Sensor.h>

#include <DAQSensors.h>

enum SensorID : uint8_t {
    ENGINE_RPM      = 0,
    FRONT_LEFT_RPM  = 1,
    FRONT_RIGHT_RPM = 2,
    AUX_RPM         = 3,
    REAR_BRAKE_PRES = 4,
    FRONT_BRAKE_PRES= 5,
    LDS_FRONT_RIGHT = 6,
    LDS_FRONT_LEFT  = 7,
    LDS_REAR_RIGHT  = 8,
    LDS_REAR_LEFT   = 9,
    CVT_TEMP        = 10,
    REAR_TC_TEMP    = 11,
    TEENSY_TEMP     = 12
};

struct __attribute__((packed)) DataPacket {
    uint8_t sensorID;
    // Record in units of 100 micros
    uint32_t timestamp100Micros;
    float value;
};

struct SensorLogger {
    uint32_t intervalMicros;
    uint64_t lastLogTimeMicros;
    
    bool shouldLog(uint64_t currentTimeMicros) {
        return currentTimeMicros >= lastLogTimeMicros + intervalMicros;
    }
    
    void updateLastLogTime(uint64_t currentTimeMicros) {
        lastLogTimeMicros = currentTimeMicros;
    }
};

// Log once a second
struct SensorLogger teensyTempLogger = {1000000UL, 0};

// Log once a second
struct SensorLogger debugLogger = {1000000UL / 50, 0};

// Save once a minute
struct SensorLogger saveTimer = {1000000UL * 60, 0};

// Write a packet to the output file
void writePacket(SensorID id, float value);

//length of lds when hanging in inches
#define HANG_TRAVEL 7.1365

//ADC Reference voltage enum
#define ADC_REFERENCE_VOLTAGE 6.144

//LDS max travel
#define LDS_MAX_TRAVEL 7.87402

//ADS Settings
#define ADC_RESOLUTION 15 //Delete once adafruit ADS is gone
#define DRDY 22
#define RESET 23
#define SYNC 24
#define CS 10
#define V_REF 5.0

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

//actually init the global variable for this 
volatile unsigned long long microsecondsElapsed = 0;
//delcare function for intervalTimer
void intervalTimerFunction() {
  microsecondsElapsed += 5;
}

//declare non setup functions
time_t getTeensy3Time();

//enums for AUXDAQ ports
enum AUXDAQ_Ports {
  RPM1 = 15,
  RPM2 = 37,
  RPM3 = 20,
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
  ENGTEETH = 8,
  FRTEETH = 12,
  FLTEETH = 12
};

//function to test all debug LEDs
inline void flashBang(int timeInMillis, int frontBackOrAll) {
  //choose which lights to set high (used to communicate info for LDS calibration and whatnot)
  if(frontBackOrAll != 1) {
    digitalWrite(RECORDING_LED, HIGH);
    digitalWrite(RPM_ONE_LED, HIGH);
    digitalWrite(RPM_TWO_LED, HIGH);
    digitalWrite(RPM_THREE_LED, HIGH);
    digitalWrite(RPM_FOUR_LED, HIGH);
  }
  if (frontBackOrAll != 0) {
    digitalWrite(ANALOG_ONE_LED, HIGH);
    digitalWrite(ANALOG_TWO_LED, HIGH);
    digitalWrite(ANALOG_THREE_LED, HIGH);
    digitalWrite(ANALOG_FOUR_LED, HIGH);
    digitalWrite(SD_CARD_INIT_LED, HIGH);
  }
  delay(timeInMillis);
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

//DAQ sensor data struct (used for local logging and potential wireless packet formats)
cmbtl::DAQSensorDataType DAQData;

ADS1256 ads1256(DRDY, RESET, SYNC, CS, V_REF, &SPI); //DRDY, RESET, SYNC(PDWN), CS, VREF(float).
//outputFile
File outputFile;

//flag for if recording
bool isRecording = false;

//saves the last time data was saved 
volatile long long unsigned lastSaveTimeInMillis = 0;

//saves the last time auto save
ulong autoSaveTimeMillis = 0;

//function to calibrate LDSs
Linear_Analog_Sensor* createCalibratedLDSSensor(int ADCPortNumber, Adafruit_ADS1115 * adc, int frontOrBack) {
  Serial.printf("Beginning calibration!\n");
  //flash all lights to show that it wants the car lifted
  //flashBang(10000, frontOrBack);
  //read the data from the adc port
  //(We read five numbers and then average them)
  float hangVoltage = 0.0;
  for (int i =0; i < 200; i +=1) {
    hangVoltage += ((adc -> readADC_SingleEnded(ADCPortNumber))/(pow(2, ADC_RESOLUTION)-1)) * ADC_REFERENCE_VOLTAGE;
  }
  hangVoltage = hangVoltage/200;
  Serial.printf("Hang Voltage Computed! %f\n", hangVoltage);
  //flash all lights to communicate that it finished the current LDS
  //flashBang(1000, frontOrBack);
  //according to the data sheet these sensors are at 0V at 0mm and 5V at 200mm we recalibrate by assuming linearity and then creating a new slope, we may end up with the inability to measure at extreme low or high points but that doesn't really matter for us since its impossible for the lds to do that)
  //200 mm is 7.87402 inches

  //now we must find a second point to compare with, all the LDSs have a mark at 1 inch below hang which you push the car down to and hold it at (it should really just rest there)
  //longer flash bang to give more time
  //flashBang(10000, frontOrBack);

  float slightlySettledVoltage = 0.0;
  for (int i =0; i < 200; i +=1) {
    slightlySettledVoltage += ((adc -> readADC_SingleEnded(ADCPortNumber))/(pow(2, ADC_RESOLUTION)-1)) * ADC_REFERENCE_VOLTAGE;
  }
  slightlySettledVoltage = slightlySettledVoltage/200;

  Serial.printf("Second Voltage Computed! %f\n", slightlySettledVoltage);
  //solve for slope 
  float slope = (HANG_TRAVEL - (HANG_TRAVEL - 1))/(hangVoltage - slightlySettledVoltage);
  Serial.printf("Computed slope: %f\n", slope);

  //solve for y intercept via hang travel point(minimum value at 0V)
  float yIntercept = HANG_TRAVEL - slope * hangVoltage;
  Serial.printf("Computed yIntercept: %f\n", yIntercept);
  
  //solve for maximum value we should see based on this modified function
  float maximumValue = slope * 5 + yIntercept;
  Serial.printf("Computed maximum Value: %f\n", maximumValue);

  //define the LDS
  Linear_Analog_Sensor* LDSToReturn = new Linear_Analog_Sensor(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, maximumValue, yIntercept, 5, 0, 0, 7.87402);
  
  //return it
  return LDSToReturn;
}

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


//method to setup outFile/SD card
inline void setUpSD() {
  int SDCARDINIT = SD.begin(BUILTIN_SDCARD);
  Serial.printf("SD Card init status: %d\n",SDCARDINIT);
  if(!SDCARDINIT) {
    Serial.printf("SD FAILED TO INIT\n");
    digitalWrite(SD_CARD_INIT_LED, LOW);
  } else {
    Serial.printf("SD INITIALIZED\n");
    //digitalWrite(SD_CARD_INIT_LED, HIGH);
  }
  delay(500);
  String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
  SD.mkdir(time.c_str());
  Serial.println(time.c_str());
  outputFile = SD.open(String("/"+time+"/"+time+".bin").c_str(),  FILE_WRITE);
}

inline void recordNextADSValue();

//time setup function
inline void setupTeensyTime() {
  setSyncProvider(getTeensy3Time);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
}

// Returns -1 if failure, 0 if success
int initADS1256();

uint32_t safeTimestamp();

//method needed to get time
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}