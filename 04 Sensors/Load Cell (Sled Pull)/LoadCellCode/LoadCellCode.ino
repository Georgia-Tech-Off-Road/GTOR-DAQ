#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>
#include <HX711.h>

#define calibration_factor 10000 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define DOUT  3
#define CLK  2

#define BAUD 115200

String fileName = "loadCellTest";

HX711 scale;

File outputFile;

void setup() {
  Serial.begin(BAUD);
  setSyncProvider(getTeensy3Time);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  SD.begin(BUILTIN_SDCARD);
  delay(500);
  fileName = fileName + String(day()) + "-" + String(month()) + "-"+ String(year()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second())+".csv";
  outputFile = SD.open(fileName.c_str(),  FILE_WRITE);
  scale.begin(DOUT, CLK);
  pinMode(22, INPUT_PULLDOWN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
}

void loop() {
  outputFile.printf("%llu,%llu,%lf\n", now(), micros(),
    scale.get_units());
  outputFile.flush();
}

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

