#include "DaqPack.h"

//intialize ADCs
Adafruit_ADS1115 ads1;
Adafruit_ADS1115 ads2;

//initialize analog value flags
volatile bool analogValueFlag1 = false;
volatile bool analogValueFlag2 = false;

//current analog sensor number being polled
int currentAnalogSensor1 = 0;
int currentAnalogSensor2 = 0;

//slowdown code
ulong lastMicros = 0;

//initialize AMT22 sensor
AMT22 steeringPositionSensor(10, AMT22RES);

//initialize RPM sensors
/**RPMSensor engineRPM(RPM1, ENGTEETH);
RPMSensor frontLeft(RPM2, FLTEETH);
RPMSensor frontRight(RPM3, FRTEETH);**/


void setup() {
  //init temp monitor
  tempmon_init();
  //start tempmon
  tempmon_Start();
  //set up time 
  setupTeensyTime();
  //initiliaze serial monitor
  serialMonitor.begin(BAUD);
  //setup pin modes
  setupPinModes();
  //perform outFile init
  setUpSD();

  //set up ADC interrupts
  pinMode(40, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(40), updateAnalogValueFlag1, FALLING);
  pinMode(41, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(41), updateAnalogValueFlag2, FALLING);

  pinMode(RPM3, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM3), frontRightInterrupt, RISING);
  pinMode(RPM1, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM1), engineRPMInterrupt, RISING);
  pinMode(RPM2, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM2), frontLeftInterrupt, RISING);

  //configure ADCs
  ads1.begin(0x48, &Wire);
  ads1.setDataRate(RATE_ADS1115_64SPS);
  ads1.setGain(GAIN_TWOTHIRDS);

  ads2.begin(0x49, &Wire);
  ads2.setDataRate(RATE_ADS1115_64SPS);
  ads2.setGain(GAIN_TWOTHIRDS);

  //zero out all data fields
  initDataStructValues();
  //initialize I2C link
  //initializeI2CCommLink();
  //initialize SPI busses/ports
  initializeSPI();
  //turn on diagnostic LEDS
  turnOnLEDS();
  //set recording flag
  isRecording = true;
  //start ADCs
  ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
  ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
  dataAquisitionAndSavingLoop();
  //setup execution threads
  //initializeThreads();
}

//do nothing here
void loop(){}

//writes data to SD card
void dataAquisitionAndSavingLoop() {
  while(1) {
    dataStruct.seconds = now();
    dataStruct.micros = micros();
    dataStruct.teensyTemp = tempmonGetTemp();
    //size of is apparently computed at compile time
    if (isRecording) {
      outputFile.write(&dataStruct, sizeof(dataStruct));
    }
    //check for RPM updates
    /**if (engineRPM.RPMUpdateFlag) {
      dataStruct.RPMs[0] = engineRPM.RPM;
      engineRPM.RPMUpdateFlag = false;
    } else {
      dataStruct.RPMs[0] = engineRPM.checkRPM();
    }
    if (frontLeft.RPMUpdateFlag) {
      dataStruct.RPMs[1] = frontLeft.RPM;
      frontLeft.RPMUpdateFlag = false;
    } else {
      dataStruct.RPMs[1] = frontLeft.checkRPM();
    }
    if (frontRight.RPMUpdateFlag) {
      dataStruct.RPMs[2] = frontRight.RPM;
      frontRight.RPMUpdateFlag = false;
    } else {
      dataStruct.RPMs[2] = frontRight.checkRPM();
    }
    **/
    if (analogValueFlag1) {
      readAnalogValues1();
      analogValueFlag1 = false;
    }
    if (analogValueFlag2) {
      readAnalogValues2();
      analogValueFlag2 = false;
    }
    if(steeringPositionSensor.steeringPositionUpdateFlag) {
      dataStruct.steeringPosition = steeringPositionSensor.steeringPosition;
      steeringPositionSensor.steeringPositionUpdateFlag = false;
    }
    if (digitalRead(7) && lastSaveTimeInMillis + 2000 < millis()) {
      changeRecordingState();
    }
    if (millis() > autoSaveTimeMillis + 300000) {
      outputFile.flush();
    }
    while (lastMicros + 50 > micros()) {
    }
  }
}

//gets steering data
void updateAMT22Reading() {
  while(1) {
    steeringPositionSensor.getPosition();
    threads.delay(10);
  }
}
//changes recording state and saves file
void changeRecordingState() {
  //suspend position thread to prevent it from interfering in saving process
  threads.suspend(AMT22PositionThread);
  noInterrupts();
  if(isRecording == true) {
    while(digitalRead(7) == 1) {
    }
    outputFile.close();
    digitalWrite(9, LOW); //turn off red LED
    isRecording = false;
  }
  else {
    while(digitalRead(7) == 1) {
    }
    String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second()+".bin");
    Serial.println(time.c_str());
    //turn on red LED
    digitalWrite(9, HIGH);
    outputFile = SD.open(time.c_str(),  FILE_WRITE);
    isRecording = true;
  }
  lastSaveTimeInMillis = millis();
  interrupts();   
  threads.restart(AMT22PositionThread); 
}

void updateAnalogValueFlag1() {
  analogValueFlag1 = true;
}

void updateAnalogValueFlag2() {
  analogValueFlag2 = true;
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
void readAnalogValues2() {
  switch (currentAnalogSensor2) {
      case 0:
        dataStruct.analogValues2[0] =  ads2.getLastConversionResults();
        currentAnalogSensor2 = 1;
        ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_1, false);
        break;
      case 1:
        dataStruct.analogValues2[1] =  ads2.getLastConversionResults();
        currentAnalogSensor2 = 2;
        ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_2, false);
        break;
      case 2:
        dataStruct.analogValues2[2] =  ads2.getLastConversionResults();
        currentAnalogSensor2 = 3;
        ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_3, false);
        break;
      case 3:
        dataStruct.analogValues2[3] =  ads2.getLastConversionResults();
        currentAnalogSensor2 = 0;
        ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
        break;
  }
}

void engineRPMInterrupt() {
  //engineRPM.calculateRPM();
}

void frontLeftInterrupt() {
  //frontLeft.calculateRPM();
}

void frontRightInterrupt() {
  //frontRight.calculateRPM();
}