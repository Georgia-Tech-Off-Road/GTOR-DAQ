/*
 Code for Engine Dyno based off HX711 code from Adafruit. I believe it uses software based i2c so it's a bit slower than it could be but 
 it doesn't really matter.
*/

#include "HX711.h"

#define DOUT  18
#define CLK  19

HX711 scale;

volatile float RPM;

int numTeethInGear = 30;

uint32_t prevMicros = 0;

float calibration_factor = -7050; //-7050 worked for my 440lb max scale setup

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);

  //interrupt for RPM
  pinMode(4, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(4), updateRPM, RISING);
  //interrupt for Tare 
  pinMode(5, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(5), tare, FALLING);
  setCalibrationFactor();

}

void loop() {
  Serial.printf("RPM: %f  Load: %.1f lbs\n", RPM, scale.get_units());
  checkRPM();
}


void setCalibrationFactor() {
  Serial.println("Remove all weight from scale");
  delay(100);
  scale.tare();
  while (true) {
    scale.set_scale(calibration_factor); //Adjust to this calibration factor
    Serial.println("After readings begin, place known weight on scale");
    Serial.println("Press + or a to increase calibration factor");
    Serial.println("Press - or z to decrease calibration factor");
    Serial.println("Press ~ to proceed to engine dyno program");
    Serial.print("Reading: ");
    Serial.print(scale.get_units(), 1);
    Serial.print(" lbs"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
    Serial.print(" calibration_factor: ");
    Serial.print(calibration_factor);
    Serial.println();
    if(Serial.available())
    {
      char temp = Serial.read();
      if(temp == '+' || temp == 'a')
        calibration_factor += 10;
      else if(temp == '-' || temp == 'z')
        calibration_factor -= 10;
      else if(temp =='~') 
        break;
    }
  }
}

void updateRPM() {
  //calculate time dif in minutes
  float timeDiff = (static_cast<float>((micros() - prevMicros)) /1e6);
  //calculate time per rev
  float timePerRev = timeDiff * numTeethInGear;
  //set _RPM to product
  RPM = static_cast<float>(60/timePerRev);
  //update _prevMicros
  prevMicros = micros();
}

void tare() {
  scale.tare();
}

void checkRPM() {
    if (prevMicros + 500000 < micros()) {
        RPM = 0;
    }
    return RPM;
}
