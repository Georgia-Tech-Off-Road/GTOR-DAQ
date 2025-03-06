#include "DaqPack.h"

//initialize AMT22 sensor
AMT22 steeringPositionSensor(10, AMT22RES);

//initialize RPM sensors=
RPMSensor rearDiff(RPM1, RDTEETH);
RPMSensor frontLeft(RPM2, FLTEETH);
RPMSensor frontRight(RPM3, FRTEETH);

void setup() {
  //set up time 
  setupTeensyTime();
  //initiliaze serial monitor
  serialMonitor.begin(BAUD);
  //perform outFile init
  setUpSD();
  //zero out all data fields
  initDataStructValues();
  //initialize I2C link
  initializeI2CCommLink();
  //initialize SPI busses/ports
  initializeSPI();
  //setup execution threads
  initializeThreads();
  //turn on diagnostic LEDS
  turnOnLEDS();
  //set recording flag
  isRecording = true;
}

//do nothing here
void loop(){}

//writes data to SD card
void dataAquisitionAndSavingLoop() {
  while(1) {
    dataStruct.seconds = now();
    dataStruct.micros = micros();
    //size of is apparently computed at compile time
    outputFile.write(&dataStruct, sizeof(dataStruct));
    //check for RPM updates
    if (rearDiff.RPMUpdateFlag) {
      dataStruct.RPMs[0] = rearDiff.RPM;
      rearDiff.RPMUpdateFlag = false;
    }
    if (frontLeft.RPMUpdateFlag) {
      dataStruct.RPMs[1] = frontLeft.RPM;
      frontLeft.RPMUpdateFlag = false;
    }
    if (frontRight.RPMUpdateFlag) {
      dataStruct.RPMs[2] = frontRight.RPM;
      frontRight.RPMUpdateFlag = false;
    }
    if(steeringPositionSensor.steeringPositionUpdateFlag) {
      dataStruct.steeringPosition = steeringPositionSensor.steeringPosition;
      steeringPositionSensor.steeringPositionUpdateFlag = false;
    }
    if (digitalRead(7) && lastSaveTimeInMillis + 1000 < millis()) {
      changeRecordingState();
    }
  }
}

//gets steering data
void updateAMT22Reading() {
  while(1) {
    steeringPositionSensor.getPosition();
    threads.delay(1);
  }
}

//changes recording state and saves file
void changeRecordingState() {
    noInterrupts();
    if(isRecording == true) {
      while(digitalRead(7) == 1) {
        delay(10);
        Serial.println("Button Engaged");
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
    interrupts();    
}