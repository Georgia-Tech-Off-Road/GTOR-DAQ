#include "Globals.h"
#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>
#include <ADS1256.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <TeensyThreads.h>
#include <DAQ_AMT22.h>
#include <DAQ_RPM_Sensor.h>
#include <DAQ_Linear_Analog_Sensor.h>

#include <DAQSensors.h>

enum SensorID : uint8_t {
    ENGINE_RPM      = 0,
    FRONT_LEFT_RPM  = 1,
    FRONT_RIGHT_RPM = 2,
    REAR_RPM        = 3,
    REAR_BRAKE_PRES = 4,
    FRONT_BRAKE_PRES= 5,
    LDS_FRONT_RIGHT = 6,
    LDS_FRONT_LEFT  = 7,
    LDS_REAR_RIGHT  = 8,
    LDS_REAR_LEFT   = 9,
    CVT_TEMP        = 10,
    REAR_TC_TEMP    = 11,
    TEENSY_TEMP     = 12,
};

struct Status {
  enum recordingStatus {NOT_READY_TO_RECORD = 0, READY_TO_RECORD, RECORDING};
  int recording_status;
  enum errorStatus {NO_ERROR = 0, ERROR, SD_ERROR, ADS_ERROR};
  int error_status;
} status;

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


// Write a packet to the output file
void writePacket(SensorID id, float value);

//length of lds when hanging in inches
#define HANG_TRAVEL 7.1365

//ADC Reference voltage enum
#define ADC_REFERENCE_VOLTAGE 6.144

//LDS max travel
#define LDS_MAX_TRAVEL 7.87402

//ADS Settings
#define ADC_RESOLUTION 24
#define DRDY_PIN 22
#define ADS_RESET_PIN 23
#define ADS_SYNC_PIN 24
#define ADS_CS_PIN 10
#define V_REF 5.0

// LEDs
#define RECORDING_LED 35
#define ERROR_LED 40
#define POWER_LED 40

// Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64  // or 32, depending on your display
#define OLED_ADDR 0x3C 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// Buttons
#define RECORD_SAVE_BUTTON 34
#define BUTTON_HOLD_DURATION 1000000

// How often to flash (in milliseconds)
#define FLASH_RATE 400

// Define number of teeth in one place for easy modification
#define NUM_TESTING_TEETH 22

// Define min and max expected RPM values
#define MIN_EXPECTED_RPM 0
#define MAX_EXPECTED_RPM 10000
#define MAX_RPM_INTERVAL_MICROS 100000 // Allow up to 100 ms between hall effect readings before we invalidate the data (return 0)

#define BAUD 230400

#define serialMonitor Serial

// Log once a second
struct SensorLogger teensyTempLogger = {1000000UL, 0};

// Log once a second
struct SensorLogger debugLogger = {1000000UL / 50, 0};

// Display every half second
struct SensorLogger displayLogger = {1000000UL / 2, 0};

// Save once a minute
struct SensorLogger saveTimer = {1000000UL * 60, 0};

struct SensorLogger SDCardChecker = {1000000UL * 30, 0}; // Check every 30 sec to see if SD card is still responding


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
  RDTEETH = 69,
  ENGTEETH = 1,
  FRTEETH = 30,
  FLTEETH = 30
};

//DAQ sensor data struct (used for local logging and potential wireless packet formats)
cmbtl::DAQSensorDataType DAQData;

ADS1256 ads1256(DRDY_PIN, ADS_RESET_PIN, ADS_SYNC_PIN, ADS_CS_PIN, V_REF, &SPI); //DRDY, RESET, SYNC(PDWN), CS, VREF(float).
//outputFile
File outputFile;
String outputFileName;

//flag for if recording
bool isRecording = false;

//saves the last time data was saved
volatile long long unsigned lastSaveTimeInMillis = 0;

//saves the last time auto save
ulong autoSaveTimeMillis = 0;

/*
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
*/

//led pinmode declarations
inline void initPins();


//method to setup outFile/SD card
inline int setUpSD() {
  int SDCARDINIT = SD.begin(BUILTIN_SDCARD);
  Serial.printf("SD Card init status: %d\n",SDCARDINIT);
  if(!SDCARDINIT) {
    Serial.printf("SD FAILED TO INIT\n");
    return -1;
  } else {
    Serial.printf("SD INITIALIZED\n");
  }
  delay(500);
  String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
  SD.mkdir(time.c_str());
  Serial.println(time.c_str());

  outputFileName = String("/"+time+"/"+time+".bin");
  outputFile = SD.open(outputFileName.c_str(),  FILE_WRITE);

  return 0;
}

constexpr uint8_t getADSPort(SensorID sensorID);

inline void recordNextADSValue();

/*
 * Returns if the specified buttonPin is held for holdMicroseconds
*/
void blockForButtonHold(int buttonPin, uint32_t holdMicroseconds);

void rapidFlash(int ledPin, uint32_t durationMilliseconds);

void errorFlash();

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

int initDisplay();

uint32_t safeTimestamp();

inline uint32_t safeMicrosecondsElapsed();

//method needed to get time
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

void updateStatusLEDs();

void updateStatusDisplay();

bool isSDCardAccessible();

inline void emitSDError();

inline void emitADSError();

inline void errorCheck();

inline void onButtonHeldConfirmed();