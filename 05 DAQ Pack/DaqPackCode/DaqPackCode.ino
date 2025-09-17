#include "DaqPack.h"

// Intialize ADCs
Adafruit_ADS1115 ads1;

// Initialize analog value flags
volatile bool analogValueFlag1 = false;

// Current analog sensor number being polled
int currentAnalogSensor1 = 0;

//bool to check for if data was actually updated
volatile bool dataUpdated = false;

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
  //init auto save time
  autoSaveTimeMillis = millis();
  //start ADCs
  ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
  dataAquisitionAndSavingLoop();
}

//do nothing here
void loop(){}

//writes data to SD card
void dataAquisitionAndSavingLoop() {
  bool firstEntry = true;
  while(1) {  
    //update auto save time
    autoSaveTimeMillis = millis();
    //check to see if save should be started/stopped
     if (!digitalRead(0) && lastSaveTimeInMillis + 2000 < millis()) {
      changeRecordingState();
    }
    //perform flush check before data check
    if (millis() > autoSaveTimeMillis + 300000) {
      outputFile.flush();
    }
    if (!dataUpdated) {
      //write code here to yield time to other subprograms
      continue;
    }
    DAQData.setData<cmbtl::SEC>(now()); 
    DAQData.setData<cmbtl::MICRO_SEC>(micros());
    DAQData.setData<cmbtl::TEENSY_TEMP>(tempmonGetTemp());

    //size of is apparently computed at compile time
    if (isRecording) {
      // TODO: Check with andrew
      if (firstEntry) {
        outputFile.printf("%s", DAQData.serializeDataToJSON().c_str());
        firstEntry=false;
      } else {
        outputFile.printf(",%s", DAQData.serializeDataToJSON().c_str());
      }
      //Serial.printf("%s", DAQData.serializeDataToJSON().c_str());
    }
    //check for RPM updates (we still use the individual flags as they enable us to reset RPM to 0 after a certain amount of time goes by (prevents hanging at like 5000 or whatev))
    if (engineRPM.RPMUpdateFlag) {
      DAQData.setData<cmbtl::RPM1>(engineRPM.RPM);
      engineRPM.RPMUpdateFlag = false;
    } else {
      DAQData.setData<cmbtl::RPM1>(engineRPM.checkRPM());
    }
    if (frontLeft.RPMUpdateFlag) {
      DAQData.setData<cmbtl::RPM2>(frontLeft.RPM);
      frontLeft.RPMUpdateFlag = false;
    } else {
      DAQData.setData<cmbtl::RPM2>(frontLeft.checkRPM());
    }
    if (frontRight.RPMUpdateFlag) {
      DAQData.setData<cmbtl::RPM3>(frontRight.RPM);
      frontRight.RPMUpdateFlag = false;
    } else {
      DAQData.setData<cmbtl::RPM3>(frontRight.checkRPM());
    }
    if (aux1.RPMUpdateFlag) {
      DAQData.setData<cmbtl::RPM4>(aux1.RPM);
      aux1.RPMUpdateFlag = false;
    } else {
      DAQData.setData<cmbtl::RPM4>(aux1.checkRPM());
    }
    //this is still called from within this while loop so an interrupt isnt calling a function
    if (analogValueFlag1) {
      readAnalogValues1();
      analogValueFlag1 = false;
    }
    //reset dataUpdated
    dataUpdated = false;
  }
}

//changes recording state and saves file
void changeRecordingState() {
  //suspend position thread to prevent it from interfering in saving process
  if(isRecording == true) {
    while(digitalRead(40) == 0) {
      delay(5);
    }
    outputFile.printf("]");
    outputFile.flush();
    outputFile.close();

    Serial.printf("File closed\n");
    isRecording = false;
  }
  else {
    // TODO: Ask Andrew
    while(digitalRead(40) == 0) {
      delay(5);
    }
    String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second())+".txt";
    Serial.println(time.c_str());
    //turn on red LED
    outputFile = SD.open(time.c_str(),  FILE_WRITE);
    outputFile.printf("[\n");
    isRecording = true;
  }
  lastSaveTimeInMillis = millis();
}

void updateAnalogValueFlag1() {
  dataUpdated = true;
  analogValueFlag1 = true;
}


void readAnalogValues1() {
  switch (currentAnalogSensor1) {
      case 0:
        DAQData.setData<cmbtl::Analog1>(ads1.getLastConversionResults());
        currentAnalogSensor1 = 1;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_1, false);
        break;
      case 1:
        DAQData.setData<cmbtl::Analog2>(ads1.getLastConversionResults());
        currentAnalogSensor1 = 2;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_2, false);
        break;
      case 2:
        DAQData.setData<cmbtl::Analog3>(ads1.getLastConversionResults());
        currentAnalogSensor1 = 3;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_3, false);
        break;
      case 3:
        DAQData.setData<cmbtl::Analog4>(ads1.getLastConversionResults());
        currentAnalogSensor1 = 0;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
        break;
  }
}

void engineRPMInterrupt() {
  engineRPM.calculateRPM();
  dataUpdated = true;
}

void frontLeftInterrupt() {
  frontLeft.calculateRPM();
  dataUpdated = true;
}

void frontRightInterrupt() {
  frontRight.calculateRPM();
  dataUpdated = true;
}

void aux1Interrupt() {
  aux1.calculateRPM();
  dataUpdated = true;
}