#include "DaqPack.h"

//intialize ADCs
Adafruit_ADS1115 ads1;

//initialize analog value flags
volatile bool analogValueFlag1 = false;

//current analog sensor number being polled
int currentAnalogSensor1 = 0;

//slowdown code
ulong lastMicros = 0;


//initialize RPM sensors
RPMSensor engineRPM(RPM1, ENGTEETH);
RPMSensor frontLeft(RPM2, FLTEETH);
RPMSensor frontRight(RPM3, FRTEETH);
RPMSensor aux1(RPM3, FRTEETH);


void setup() {
  //init temp monitor
  tempmon_init();
  //start tempmon
  tempmon_Start();
  //set up time 
  setupTeensyTime();
  //initiliaze serial monitor
  serialMonitor.begin(BAUD);
  //perform outFile init
  setUpSD();

  //configure save pin
  pinMode(0, INPUT_PULLUP);

  //set up ADC interrupts
  pinMode(40, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(40), updateAnalogValueFlag1, FALLING);

  pinMode(RPM3, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM3), frontRightInterrupt, RISING);
  pinMode(RPM1, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM1), engineRPMInterrupt, RISING);
  pinMode(RPM2, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM2), frontLeftInterrupt, RISING);
  pinMode(RPM4, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM4), aux1Interrupt, RISING);
  //configure ADCs
  ads1.begin(0x48, &Wire);
  ads1.setDataRate(RATE_ADS1115_64SPS);
  ads1.setGain(GAIN_TWOTHIRDS);

  //zero out all data fields
  initDataStructValues();
  //set recording flag
  isRecording = true;
  //start ADCs
  ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
  dataAquisitionAndSavingLoop();
}

//do nothing here
void loop(){}

//writes data to SD card
void dataAquisitionAndSavingLoop() {
  while(1) {
    delay(10);
    dataStruct.seconds = now();
    dataStruct.micros = micros();
    dataStruct.teensyTemp = tempmonGetTemp();
    //size of is apparently computed at compile time
    if (isRecording) {
      outputFile.write(&dataStruct, sizeof(dataStruct));
      Serial.printf("%f,%f,%f,%f\n",dataStruct.RPMs[0], dataStruct.RPMs[1], dataStruct.RPMs[2],dataStruct.RPMs[3]);
    }
    //check for RPM updates
    if (engineRPM.RPMUpdateFlag) {
      Serial.printf("Engine\n");
      dataStruct.RPMs[0] = engineRPM.RPM;
      engineRPM.RPMUpdateFlag = false;
    } else {
      dataStruct.RPMs[0] = engineRPM.checkRPM();
    }
    if (frontLeft.RPMUpdateFlag) {
      Serial.printf("frontLeft\n");
      dataStruct.RPMs[1] = frontLeft.RPM;
      frontLeft.RPMUpdateFlag = false;
    } else {
      dataStruct.RPMs[1] = frontLeft.checkRPM();
    }
    if (frontRight.RPMUpdateFlag) {
      Serial.printf("frontRight\n");
      dataStruct.RPMs[2] = frontRight.RPM;
      frontRight.RPMUpdateFlag = false;
    } else {
      dataStruct.RPMs[2] = frontRight.checkRPM();
    }
    if (aux1.RPMUpdateFlag) {
      Serial.printf("aux1\n");
      dataStruct.RPMs[3] = aux1.RPM;
      aux1.RPMUpdateFlag = false;
    } else {
      dataStruct.RPMs[3] = aux1.checkRPM();
    }
    if (analogValueFlag1) {
      readAnalogValues1();
      analogValueFlag1 = false;
    }
    if (!digitalRead(0) && lastSaveTimeInMillis + 2000 < millis()) {
      changeRecordingState();
    }
    if (millis() > autoSaveTimeMillis + 300000) {
      outputFile.flush();
    }
    while (lastMicros + 50 > micros()) {
    }
  }
}

//changes recording state and saves file
void changeRecordingState() {
  //suspend position thread to prevent it from interfering in saving process
  noInterrupts();
  if(isRecording == true) {
    while(digitalRead(7) == 0) {
    }
    outputFile.close();
    Serial.printf("File saved!\n");
    isRecording = false;
  }
  else {
    while(digitalRead(7) == 0) {
    }
    String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second()+".bin");
    Serial.println(time.c_str());
    //turn on red LED
    outputFile = SD.open(time.c_str(),  FILE_WRITE);
    Serial.printf("New recording started!\n");
    isRecording = true;
  }
  lastSaveTimeInMillis = millis();
  interrupts();   
}

void updateAnalogValueFlag1() {
  analogValueFlag1 = true;
}


void readAnalogValues1() {
  switch (currentAnalogSensor1) {
      case 0:
        dataStruct.analogValues1[0] =  ads1.getLastConversionResults();
        currentAnalogSensor1 = 1;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_1, false);
        break;
      case 1:
        dataStruct.analogValues1[1] =  ads1.getLastConversionResults();
        currentAnalogSensor1 = 2;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_2, false);
        break;
      case 2:
        dataStruct.analogValues1[2] =  ads1.getLastConversionResults();
        currentAnalogSensor1 = 3;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_3, false);
        break;
      case 3:
        dataStruct.analogValues1[3] =  ads1.getLastConversionResults();
        currentAnalogSensor1 = 0;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
        break;
  }
}

void engineRPMInterrupt() {
  engineRPM.calculateRPM();
}

void frontLeftInterrupt() {
  frontLeft.calculateRPM();
}

void frontRightInterrupt() {
  frontRight.calculateRPM();
}

void aux1Interrupt() {
  aux1.calculateRPM();
}