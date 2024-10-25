//Note: May be able to go faster by modifying core Teensy 4.1 code to reduce the amount of averages that it takes when doing an analog read
//Note: Also could maybe decrease bit resolution
//Note: Should get an external powersupply to get better VCC stability

//sd card stuff
#include "SD.h"
#include "SPI.h"
//time stuff
#include <TimeLib.h>

#define BAUD 115200

//#define serialMonitor Serial

File outputFile;

bool isRecording = false;

void setup() {
  // set the Time library to use Teensy 3.0's RTC to keep time
  Serial.begin(BAUD);
  setSyncProvider(getTeensy3Time);
  while (!Serial);  // Wait for Arduino Serial Monitor to open
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  //serialMonitor.begin(BAUD);
  SD.begin(BUILTIN_SDCARD);
  delay(500);
  String time = String(day()) + "-" + String(month()) + "-"+ String(year()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
  Serial.println(time.c_str());
  outputFile = SD.open(time.c_str(),  FILE_WRITE);
  //sets analog read precision
  analogReadRes(10);
  //reduces amount of reads averaged together to 2
  analogReadAveraging(2);
  //pulls down signals
  pinMode(17, INPUT_PULLDOWN);
  pinMode(20, INPUT_PULLDOWN);
  pinMode(21, INPUT_PULLDOWN);
  pinMode(15, INPUT_PULLDOWN);
  pinMode(22, INPUT_PULLDOWN);

  isRecording = true;
  outputFile.println(now());
}
//writes data to SD card, checks if it should stop recording, and then waits 1ms
void loop() {
  // put your main code here, to run repeatedly:
  outputFile.printf("%llu,%lu,%d,%d,%d,%d,%d\n", now(), micros(),
    analogRead(17), digitalRead(20), digitalRead(21),analogRead(15), digitalRead(22));
    //Serial.printf("%llu,%d,%d,%d\n", now(), digitalRead(17), analogRead(20), analogRead(21));
  if (digitalRead(22) == 1) {
    if(isRecording == true) {
      outputFile.println(now());
      outputFile.close();
      Serial.println("Data Recording Stopped");
      while(digitalRead(22) == 1) {
        Serial.println("RELEASE BUTTON");
      }
      isRecording = false;
    }
    else {
      while(digitalRead(22) == 1) {
        Serial.println("RELEASE BUTTON");
      }
      String time = String(day()) + "-" + String(month()) + "-"+ String(year()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
      Serial.println(time.c_str());
      outputFile = SD.open(time.c_str(),  FILE_WRITE);
      isRecording = true;
    }
  }
}
//method needed to get time
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}
