#include "DaqPack.h"


// Intialize ADCs
Adafruit_ADS1115 ads1;
Adafruit_ADS1115 ads2;

// Initialize analog value flags
volatile bool analogValueFlag1 = false;
volatile bool analogValueFlag2 = false;

// Current analog sensor number being polled
int currentAnalogSensor1 = 0;
int currentAnalogSensor2 = 0;

//general rpm definitions for now
#define MIN_EXPECTED_VALUE 0
#define MAX_EXPECTED_VALUE 2000

//initialize RPM sensors
RPMSensor engineRPM(RPM1, ENGTEETH, MIN_EXPECTED_VALUE, MAX_EXPECTED_VALUE);
RPMSensor frontLeftRPM(RPM2, FLTEETH, MIN_EXPECTED_VALUE, MAX_EXPECTED_VALUE);
RPMSensor frontRightRPM(RPM3, FRTEETH, MIN_EXPECTED_VALUE, MAX_EXPECTED_VALUE);
RPMSensor aux1RPM(RPM3, RDTEETH, MIN_EXPECTED_VALUE, MAX_EXPECTED_VALUE);

//dont need to calibrate the brake pressures at start up I don't think
Linear_Analog_Sensor rearBrakePressure(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 2000, 0, 4.5, 0.5, 0, 2000);
Linear_Analog_Sensor frontBrakePressure(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 2000, 0, 4.5, 0.5, 0, 2000);

//create pointers so we can reference these globaly
//Linear_Analog_Sensor * LDSFrontLeft;
//Linear_Analog_Sensor * LDSFrontRight;

//create LDSs
Linear_Analog_Sensor LDSFrontRight(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 7.87402, 0, 5.0, 0.0, 0.0, 200);
Linear_Analog_Sensor LDSFrontLeft(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 7.87402, 0, 5.0, 0.0, 0.0, 200);
Linear_Analog_Sensor LDSRearLeft(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 7.87402, 0, 5.0, 0.0, 0.0, 200);
Linear_Analog_Sensor LDSRearRight(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 7.87402, 0, 5.0, 0.0, 0.0, 200);

//create temp sensors
Linear_Analog_Sensor CVTTemp(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 200, 0, 5.0, 0.0, 0.0, 200);
Linear_Analog_Sensor RearTransferCaseTemp(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 200, 0, 5.0, 0.0, 0.0, 200);


void setup() {
  //crank up i2c clocks
  Wire.setClock(1000000);
  Wire1.setClock(1000000);
  //initialize debug leds
  initDebugLEDs();
  //test leds
  //flashBang(5000, 2);
  initDataStructValues();
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

  pinMode(41, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(41), updateAnalogValueFlag2, FALLING);

  pinMode(RPM3, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM3), frontRightRPMInterrupt, RISING);
  pinMode(RPM1, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM1), engineRPMInterrupt, RISING);
  pinMode(RPM2, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM2), frontLeftRPMInterrupt, RISING);
  pinMode(RPM4, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM4), aux1RPMInterrupt, RISING);
  //configure ADCs
  ads1.begin(0x48, &Wire);
  ads2.begin(0x48, &Wire1);
  //make sure it sets it right
  while(ads1.getDataRate() != RATE_ADS1115_860SPS) {
    ads1.setDataRate(RATE_ADS1115_860SPS);
  }
  ads1.setGain(GAIN_TWOTHIRDS);

  while(ads2.getDataRate() != RATE_ADS1115_860SPS) {
    ads2.setDataRate(RATE_ADS1115_860SPS);
  }
  ads2.setGain(GAIN_TWOTHIRDS);

  //calibrate the LDSs
  //LDSFrontLeft = createCalibratedLDSSensor(2, &ads1, 0);
  //LDSFrontRight = createCalibratedLDSSensor(3, &ads1, 0);
  //delay to give calibrators time to get to the back of the car
  //delay(10000);
  //*LDSRearLeft = createCalibratedLDSSensor(2, &ads2, 1);
  //*LDSRearRight = createCalibratedLDSSensor(3, &ads2, 1);
  //final delay to let you read off all the calibrated values
  delay(7000);
  //zero out all data fields
  initDataStructValues();
  //set recording flag
  isRecording = true;
  //init auto save time
  autoSaveTimeMillis = millis();
  //start ADCs
  ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
  ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
  dataAquisitionAndSavingLoop();
}

//do nothing here
void loop(){}

//update debug LEDs
void updateDebugLeds() {
  digitalWrite(RECORDING_LED, isRecording ? HIGH : LOW);
  digitalWrite(RPM_ONE_LED, engineRPM.getRPMValueGood() ? HIGH : LOW);
  digitalWrite(RPM_TWO_LED, frontLeftRPM.getRPMValueGood() ? HIGH : LOW);
  digitalWrite(RPM_THREE_LED, frontRightRPM.getRPMValueGood() ? HIGH : LOW);
  digitalWrite(RPM_FOUR_LED, aux1RPM.getRPMValueGood() ? HIGH : LOW);
  digitalWrite(ANALOG_ONE_LED, rearBrakePressure.getValueGood() ? HIGH : LOW);
  digitalWrite(ANALOG_TWO_LED, frontBrakePressure.getValueGood() ? HIGH : LOW);
  digitalWrite(ANALOG_THREE_LED, LDSFrontLeft.getValueGood() ? HIGH : LOW);
  digitalWrite(ANALOG_FOUR_LED, LDSFrontRight.getValueGood() ? HIGH : LOW);
  //SD debug is handled by the file creation code
  //power LED is always on whenever the box is on
  
}


//writes data to SD card
void dataAquisitionAndSavingLoop() {
  bool firstEntry = true;
  while(1) {  
    //updateDebugLeds();
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
    DAQData.setData<cmbtl::SEC>(now());
    dataStruct.sec = now(); 
    DAQData.setData<cmbtl::MICRO_SEC>(micros());
    dataStruct.microsec = micros();
    DAQData.setData<cmbtl::TEENSY_TEMP>(tempmonGetTemp());
    dataStruct.teensyTemp = tempmonGetTemp();

    //size of is apparently computed at compile time
    if (isRecording) {
      outputFile.write(&dataStruct, sizeof(dataStruct));
      // TODO: Check with andrew
      /**if (firstEntry) {
        outputFile.print(DAQData.serializeDataToJSON().c_str());
        firstEntry=false;
      } else {
        outputFile.print(",");
        outputFile.print(DAQData.serializeDataToJSON().c_str());
      }
      **/
      //Serial.printf("%s", DAQData.serializeDataToJSON().c_str());
    }
    //check for RPM updates (we still use the individual flags as they enable us to reset RPM to 0 after a certain amount of time goes by (prevents hanging at like 5000 or whatev))
    if (engineRPM.RPMUpdateFlag) {
      DAQData.setData<cmbtl::RPM1>(engineRPM.RPM);
      dataStruct.engineRPM = engineRPM.RPM;
      engineRPM.RPMUpdateFlag = false;
    } else {
      DAQData.setData<cmbtl::RPM1>(engineRPM.checkRPM());
      dataStruct.engineRPM = engineRPM.checkRPM();
    }
    if (frontLeftRPM.RPMUpdateFlag) {
      DAQData.setData<cmbtl::RPM2>(frontLeftRPM.RPM);
      dataStruct.frontLeftRPM = frontLeftRPM.RPM;
      frontLeftRPM.RPMUpdateFlag = false;
    } else {
      DAQData.setData<cmbtl::RPM2>(frontLeftRPM.checkRPM());
      dataStruct.frontLeftRPM = frontLeftRPM.checkRPM();
    }
    if (frontRightRPM.RPMUpdateFlag) {
      DAQData.setData<cmbtl::RPM3>(frontRightRPM.RPM);
      dataStruct.frontRightRPM = frontRightRPM.RPM;
      frontRightRPM.RPMUpdateFlag = false;
    } else {
      DAQData.setData<cmbtl::RPM3>(frontRightRPM.checkRPM());
      dataStruct.frontRightRPM = frontRightRPM.checkRPM();
    }
    if (aux1RPM.RPMUpdateFlag) {
      DAQData.setData<cmbtl::RPM4>(aux1RPM.RPM);
      dataStruct.aux1RPM = aux1RPM.RPM;
      aux1RPM.RPMUpdateFlag = false;
    } else {
      DAQData.setData<cmbtl::RPM4>(aux1RPM.checkRPM());
      dataStruct.aux1RPM = aux1RPM.checkRPM();
    }
    //this is still called from within this while loop so an interrupt isnt calling a function
    if (analogValueFlag1) {
      readAnalogValues1();
      analogValueFlag1 = false;
    }
    if (analogValueFlag2) {
      readAnalogValues2();
      analogValueFlag2 = false;
    }
  }
}

//changes recording state and saves file
void changeRecordingState() {
  if(isRecording == true) {
    while(digitalRead(0) == 1) {
      delay(5);
    }
    //outputFile.printf("]");
    outputFile.flush();
    outputFile.close();

    Serial.printf("File closed\n");
    isRecording = false;
  }
  else {
    while(digitalRead(0) == 1) {
      delay(5);
    }
    //generate a random number ssince RTC isnt working
    randomSeed(analogRead(A0));
    int randomNumber = random(999999);
    String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
    SD.mkdir(time.c_str());
    Serial.println(time.c_str());
    File structConfigFile = SD.open(String("/"+time+"/"+time+"Config.txt").c_str(), FILE_WRITE);
    writeStructData(structConfigFile);
    structConfigFile.close();
    outputFile = SD.open(String("/"+time+"/"+time+".bin").c_str(),  FILE_WRITE);
    //check if file creation worked and if so turn on the light
    //outputFile.printf("[\n");
    isRecording = true;
  }
  lastSaveTimeInMillis = millis();
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
        DAQData.setData<cmbtl::RearBrakePressure>(rearBrakePressure.computeSensorReading(ads1.getLastConversionResults()));
        dataStruct.rearBrakePressure = rearBrakePressure.computeSensorReading(ads1.getLastConversionResults());
        currentAnalogSensor1 = 1;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_1, false);
        break;
      case 1:
        DAQData.setData<cmbtl::FrontBrakePressure>(frontBrakePressure.computeSensorReading(ads1.getLastConversionResults()));
        dataStruct.frontBrakePressure = frontBrakePressure.computeSensorReading(ads1.getLastConversionResults());
        currentAnalogSensor1 = 2;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_2, false);
        break;
      case 2:
        DAQData.setData<cmbtl::LDSFrontRight>(LDSFrontRight.computeSensorReading(ads1.getLastConversionResults()));
        dataStruct.LDSFrontRight = LDSFrontRight.computeSensorReading(ads1.getLastConversionResults());
        currentAnalogSensor1 = 3;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_3, false);
        break;
      case 3:
        DAQData.setData<cmbtl::LDSFrontLeft>(LDSFrontLeft.computeSensorReading(ads1.getLastConversionResults()));
        dataStruct.LDSFrontLeft = LDSFrontLeft.computeSensorReading(ads1.getLastConversionResults());
        currentAnalogSensor1 = 0;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
        break;
  }
}

void readAnalogValues2() {
  switch (currentAnalogSensor2) {
      case 0:
        DAQData.setData<cmbtl::LDSRearRight>(LDSRearRight.computeSensorReading(ads2.getLastConversionResults()));
        dataStruct.LDSRearRight = LDSRearRight.computeSensorReading(ads2.getLastConversionResults());
        currentAnalogSensor2 = 1;
        ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_1, false);
        break;
      case 1:
        DAQData.setData<cmbtl::LDSRearLeft>(LDSRearLeft.computeSensorReading(ads2.getLastConversionResults()));
        dataStruct.LDSRearLeft = LDSRearLeft.computeSensorReading(ads2.getLastConversionResults());
        currentAnalogSensor2 = 2;
        ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_2, false);
        break;
      case 2:
        DAQData.setData<cmbtl::CVTTemp>(CVTTemp.computeSensorReading(ads2.getLastConversionResults()));
        dataStruct.CVTTemp = CVTTemp.computeSensorReading(ads2.getLastConversionResults());
        currentAnalogSensor2 = 3;
        ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_3, false);
        break;
      case 3:
        DAQData.setData<cmbtl::RearTransferCaseTemp>(RearTransferCaseTemp.computeSensorReading(ads2.getLastConversionResults()));
        dataStruct.RearTransferCaseTemp = RearTransferCaseTemp.computeSensorReading(ads2.getLastConversionResults());
        currentAnalogSensor2 = 0;
        ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
        break;
  }
}

void engineRPMInterrupt() {
  engineRPM.calculateRPM();
}

void frontLeftRPMInterrupt() {
  frontLeftRPM.calculateRPM();
}

void frontRightRPMInterrupt() {
  frontRightRPM.calculateRPM();
}

void aux1RPMInterrupt() {
  aux1RPM.calculateRPM();
}
