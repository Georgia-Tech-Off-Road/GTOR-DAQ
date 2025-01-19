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

File outputFile;

bool isRecording = false;

//the index in this matches up to the index of binary data in the printf
int binaryValues[3] = {0, 0, 0};

Adafruit_ADS1115 ads;
//stores current values for sensors connected to adc0
int analogValues[4] = {0, 0, 0, 0};
//current analog sensor number being polled
int currentAnalogSensor = 0;

//creates a new instance of the BNO055 class
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);

//varaibles for data from BNO05
sensors_event_t orientationData, linearAccelData, accelerometerData;
uint8_t BNO05System, gyro, accel, mag = 0;
double quatW, quatX, quatY, quatZ;
imu::Quaternion quat;
//saves the last time data was saved 
ulong lastSaveTimeInMillis = 0;

void setup() {
  if (CrashReport) {
    /* print info (hope Serial Monitor windows is open) */
    Serial.print(CrashReport);
  }
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
  attachInterrupt(digitalPinToInterrupt(20), updateRearDiff, CHANGE); //rear diff
  attachInterrupt(digitalPinToInterrupt(21), updateFrontLeftHalleffect, CHANGE); // front left halleffect
  attachInterrupt(digitalPinToInterrupt(22), updateFrontRightHalleffect, CHANGE); // front right halleffect
  //sets up interupt pin for ADS1115 ADC (have to pullup alert pin in accordance with ADS1115 datasheet)
  pinMode(15, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(15), readAnalogValues, FALLING);
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
}
//writes data to SD card
void loop() {
  CrashReport.breadcrumb(2, 1111111);
  //try testing with an oscilloscope
  //potentially reduce print buffer time?
  //use flush?
  outputFile.printf("%llu,%lu,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d\n", now(), micros()
  , binaryValues[0], binaryValues[1], binaryValues[2], analogValues[0], analogValues[1], 
  analogValues[2], analogValues[3],orientationData.orientation.x, orientationData.orientation.y, 
  orientationData.orientation.z, linearAccelData.acceleration.x, linearAccelData.acceleration.y, linearAccelData.acceleration.z,
  accelerometerData.acceleration.x, accelerometerData.acceleration.y, accelerometerData.acceleration.z,
  quatW, quatX, quatY, quatZ, BNO05System, gyro, accel, mag);
  //CrashReport.breadcrumb(2, 2222222);
  /**orientationData.orientation.x, orientationData.orientation.y, 
  orientationData.orientation.z, linearAccelData.acceleration.x, linearAccelData.acceleration.y, linearAccelData.acceleration.z,
  accelerometerData.acceleration.x, accelerometerData.acceleration.y, accelerometerData.acceleration.z,
  quatW, quatX, quatY, quatZ, BNO05System, gyro, accel, mag**/
  //update the BNO05 data values once every 5 milliseconds (the sensor updates at about 100hz but we poll at double it to make sure we dont miss any data)
  if(millis() % 100 == 0) {
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
    bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
    bno.getCalibration(&BNO05System, &gyro, &accel, &mag);
    quat = bno.getQuat();
    quatW = quat.w();
    quatX = quat.x();
    quatY= quat.y();
    quatZ = quat.z();
  }
  CrashReport.breadcrumb(2, 3333333);
  //doing it this way instead of using interupts since it seems to be more stable
  if (digitalRead(7) && lastSaveTimeInMillis + 1000 < millis()) {
    CrashReport.breadcrumb(3, 66666666);
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
    CrashReport.breadcrumb(3, 77777777);
  }
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
  CrashReport.breadcrumb(2, 44444444);
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
  CrashReport.breadcrumb(2, 55555555);
}

