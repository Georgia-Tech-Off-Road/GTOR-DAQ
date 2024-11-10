#include "SD.h"
#include <TimeLib.h>
#include "HX711.h"

#define calibration_factor 10000 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define DOUT  3
#define CLK  2

#define BAUD 115200


HX711 scale;

File outputFile;

bool isRecording = false;

void setup() {
  setSyncProvider(getTeensy3Time);
  while (!Serial);  // Wait for Arduino Serial Monitor to open
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  SD.begin(BUILTIN_SDCARD);
  Serial.begin(BAUD);
  delay(500);
  String time = String(day()) + "-" + String(month()) + "-"+ String(year()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
  outputFile = SD.open(time.c_str(),  FILE_WRITE);
  scale.begin(DOUT, CLK);
  pinMode(22, INPUT_PULLDOWN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
  isRecording = true;
}

void loop() {
  outputFile.printf("%llu,%lu,%lf lbs\n", now(), micros(),
    scale.get_units());
  Serial.println(scale.get_units());
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

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

