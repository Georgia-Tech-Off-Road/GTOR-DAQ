//Note: May be able to go faster by modifying core Teensy 4.1 code to reduce the amount of averages that it takes when doing an analog read
//Note: Also could maybe decrease bit resolution
//Note: Should get an external powersupply to get better VCC stability
//sd card stuff
#include "SD.h"
#include "SPI.h"
//time stuff
#include <TimeLib.h>

#define BAUD 230400

#define serialMonitor Serial

unsigned long previousMicros = micros();

File outputFile;

bool isRecording = false;

void setup() {
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
  String time = String(day()) + "-" + String(month()) + "-"+ String(year()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
  Serial.println(time.c_str());
  outputFile = SD.open(time.c_str(),  FILE_WRITE);
  //sets top leds to output
  pinMode(9, OUTPUT); //red LED (recording)
  digitalWrite(8, HIGH); //turn on white LED
  //pulls down signals on inputs
  pinMode(7, INPUT_PULLDOWN);
  pinMode(20, INPUT_PULLDOWN); // rear diff
  pinMode(21, INPUT_PULLDOWN); // front left halleffect
  pinMode(22, INPUT_PULLDOWN); // front right halleffect
  pinMode(17, INPUT_PULLDOWN); // front brake pressure
  pinMode(15, INPUT_PULLDOWN); //rear brake pressure
  isRecording = true;
  digitalWrite(9, HIGH); //turn on red LED
  outputFile.println(now());
}
//writes data to SD card, checks if it should stop recording, and then waits 1ms
void loop() {
  // put your main code here, to run repeatedly:
  //try testing with an oscilloscope
  //potentially reduce print buffer time?
  //use flush?
  outputFile.printf("%llu,%lu,%d,%d,%d,%d,%d\n", now(), micros()
  , digitalRead(20), digitalRead(21), digitalRead(22), analogRead(17), analogRead(15));
  //Serial.printf("%llu,%lu,%d,%d,%d,%d,%d\n", now(), micros()
  //, digitalRead(17), digitalRead(20), digitalRead(21), analogRead(22), analogRead(23));
  if (digitalRead(7) == 1) {
    if(isRecording == true) {
      while(digitalRead(7) == 1) {
        Serial.println("RELEASE BUTTON");
      }
      delay(300); //delays to prevent debounce
      outputFile.println(now());
      outputFile.close();
      digitalWrite(9, LOW); //turn off red LED
      Serial.println("Data Recording Stopped");
     
      isRecording = false;
    }
    else {
      while(digitalRead(7) == 1) {
        Serial.println("RELEASE BUTTON");
      }
      delay(300); //delays to prevent debounce
      String time = String(day()) + "-" + String(month()) + "-"+ String(year()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
      Serial.println(time.c_str());
      digitalWrite(9, HIGH);
      outputFile = SD.open(time.c_str(),  FILE_WRITE);
      outputFile.println(now());
      isRecording = true;
    }
  }
}
//method needed to get time
time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}
