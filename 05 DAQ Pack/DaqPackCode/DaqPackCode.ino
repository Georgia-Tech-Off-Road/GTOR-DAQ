#include "DaqPack.h"

//create an interval timer (provides more accurate timing than microseconds)
IntervalTimer intervalMicros;

// Intialize ADCs
Adafruit_ADS1115 ads1;
Adafruit_ADS1115 ads2;

// Initialize analog value flags
volatile bool analogValueFlag1 = false;
volatile bool analogValueFlag2 = false;

//initialize save flag 
volatile bool saveFlag = false;

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
RPMSensor aux1RPM(RPM4, RDTEETH, MIN_EXPECTED_VALUE, MAX_EXPECTED_VALUE);

//dont need to calibrate the brake pressures at start up I don't think
Linear_Analog_Sensor rearBrakePressure(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 2000, 0, 4.5, 0.5, 0, 2000);
Linear_Analog_Sensor frontBrakePressure(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 2000, 0, 4.5, 0.5, 0, 2000);

//create pointers so we can reference these globaly
//Linear_Analog_Sensor * LDSFrontLeft;
//Linear_Analog_Sensor * LDSFrontRight;

//create LDSs
Linear_Analog_Sensor LDSFrontRight(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 7.87402, 0, 5.0, 0.0, 0.0, 10);
Linear_Analog_Sensor LDSFrontLeft(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 7.87402, 0, 5.0, 0.0, 0.0, 10);
Linear_Analog_Sensor LDSRearLeft(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 7.87402, 0, 5.0, 0.0, 0.0, 10);
Linear_Analog_Sensor LDSRearRight(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 7.87402, 0, 5.0, 0.0, 0.0, 10);

//create temp sensors
Linear_Analog_Sensor CVTTemp(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 200, 0, 5.0, 0.0, 0.0, 200);
Linear_Analog_Sensor RearTransferCaseTemp(ADC_RESOLUTION, ADC_REFERENCE_VOLTAGE, 200, 0, 5.0, 0.0, 0.0, 200);

static_assert(sizeof(DataPacket) == 9, "DataPacket size mismatch - check packing");

void setup() {
  //crank up i2c clocks
  Wire.setClock(1000000);
  Wire1.setClock(1000000);

  //initialize debug leds
  initDebugLEDs();
  //test leds
  //flashBang(5000, 2);
  //initialize micros interval timer (updates every 5 microseconds, means our effective maximum polling rate would be 500kHz)
  intervalMicros.begin(&intervalTimerFunction, 5);
  //set the interupt priority to be higher than the default priority of 128 for the attachInterrupts ones (we don't want our time drifting)
  intervalMicros.priority(112);
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

  //set up save interrupt
  attachInterrupt(digitalPinToInterrupt(0), saveInterrupt, CHANGE);

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

  if (initADS1256() != 0) {
    status.error_status = status.ERROR;
    Serial.println("Failure setting up ADS1256! See errors above^^^");
    while(true);
  }
  Serial.println("ADS1256 setup successful!");

  //calibrate the LDSs
  //LDSFrontLeft = createCalibratedLDSSensor(2, &ads1, 0);
  //LDSFrontRight = createCalibratedLDSSensor(3, &ads1, 0);
  //delay to give calibrators time to get to the back of the car
  //delay(10000);
  //*LDSRearLeft = createCalibratedLDSSensor(2, &ads2, 1);
  //*LDSRearRight = createCalibratedLDSSensor(3, &ads2, 1);
  //final delay to let you read off all the calibrated values
  delay(7000);
  //set recording flag
  isRecording = true;
  //init auto save time
  autoSaveTimeMillis = millis();
  //start ADCs
  ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
  ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);

  status.recording_status = status.READY_TO_RECORD;
  updateStatusLEDs();
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
  while(1) {
    //updateDebugLeds();
    //update auto save time
    //check to see if save should be started/stopped
     if (saveFlag /* || saveTimer.shouldLog(microsecondsElapsed) */ || Serial.read() == 's') {
      saveTimer.updateLastLogTime(microsecondsElapsed);
      changeRecordingState();
      saveFlag = false;
    }
    //perform flush check before data check
    if (millis() > autoSaveTimeMillis + 300000) {
      outputFile.flush();
      autoSaveTimeMillis = millis();
    }

    //size of is apparently computed at compile time
    if (isRecording) {
      status.recording_status = status.RECORDING;
      // Only write packet if sufficient time has passed
      DAQData.setData<cmbtl::SensorIndex::MICRO_SEC>(microsecondsElapsed);
      DAQData.setData<cmbtl::SensorIndex::SEC>(microsecondsElapsed / 1000000);
      
      if (debugLogger.shouldLog(microsecondsElapsed)) {
        Serial.println(DAQData.serializeDataToJSON().c_str());
        debugLogger.updateLastLogTime(microsecondsElapsed);
      }

      recordNextADSValue();

      if(teensyTempLogger.shouldLog(microsecondsElapsed)) {
        float temp = tempmonGetTemp();
        DAQData.setData<cmbtl::SensorIndex::TEENSY_TEMP>(temp);
        writePacket(SensorID::TEENSY_TEMP, temp);
        teensyTempLogger.updateLastLogTime(microsecondsElapsed);
      }
      //check for RPM updates (we still use the individual flags as they enable us to reset RPM to 0 after a certain amount of time goes by (prevents hanging at like 5000 or whatev))
      if (engineRPM.RPMUpdateFlag) {
        float value = (float) engineRPM.calculateRPM();
        DAQData.setData<cmbtl::SensorIndex::RPM1>((uint32_t)value);
        writePacket(SensorID::ENGINE_RPM, value);
        engineRPM.RPMUpdateFlag = false;
      } else {
        engineRPM.calculateRPM();
      }
      if (frontLeftRPM.RPMUpdateFlag) {
        float value = (float) frontLeftRPM.calculateRPM();
        DAQData.setData<cmbtl::SensorIndex::RPM2>((uint32_t)value);
        writePacket(SensorID::FRONT_LEFT_RPM, value);
        frontLeftRPM.RPMUpdateFlag = false;
      } else {
        frontLeftRPM.checkRPM();
      }
      if (frontRightRPM.RPMUpdateFlag) {
        float value = frontRightRPM.calculateRPM();
        DAQData.setData<cmbtl::SensorIndex::RPM3>((uint32_t)value);
        writePacket(SensorID::FRONT_RIGHT_RPM, value);
        frontRightRPM.RPMUpdateFlag = false;
      } else {
        frontRightRPM.checkRPM();
      }
      if (aux1RPM.RPMUpdateFlag) {
        float value = aux1RPM.calculateRPM();
        DAQData.setData<cmbtl::SensorIndex::RPM4>((uint32_t)value);
        writePacket(SensorID::AUX_RPM, value);
        aux1RPM.RPMUpdateFlag = false;
      } else {
        aux1RPM.checkRPM();
      }
      //Serial.printf("%s", DAQData.serializeDataToJSON().c_str());
      updateStatusLEDs();
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

inline void recordNextADSValue() {
  static uint8_t index = 0;
  constexpr int PORT_LIST_LENGTH = 4; 

  // List of ADS ports to read from, should wrap around to begining
  constexpr uint8_t ads1256SensorList[PORT_LIST_LENGTH] = {LDS_FRONT_LEFT, LDS_FRONT_RIGHT, LDS_REAR_LEFT, LDS_REAR_RIGHT};

  SensorID nextSensor = ads1256SensorList[index];
  uint8_t nextADSPort = getADSPort(nextSensor);
  if (nextSensor == LDS_FRONT_LEFT) {
    long result = ads1256.readSinglePort(nextADSPort);
    float value = LDSFrontLeft.computeSensorReading(result);
    writePacket(SensorID::LDS_FRONT_LEFT, value);
    DAQData.setData<cmbtl::SensorIndex::LDSFrontLeft>(value);
  } else if (nextSensor == LDS_FRONT_RIGHT) {
    long result = ads1256.readSinglePort(nextADSPort);
    float value = LDSFrontRight.computeSensorReading(result);
    writePacket(SensorID::LDS_FRONT_RIGHT, value);
    DAQData.setData<cmbtl::SensorIndex::LDSFrontRight>(value);
  } else if (nextSensor == LDS_REAR_LEFT) {
    long result = ads1256.readSinglePort(nextADSPort);
    float value = LDSRearLeft.computeSensorReading(result);
    writePacket(SensorID::LDS_REAR_LEFT, value);
    DAQData.setData<cmbtl::SensorIndex::LDSRearLeft>(value);
  } else if (nextSensor == LDS_REAR_RIGHT) {
    long result = ads1256.readSinglePort(nextADSPort);
    float value = LDSRearRight.computeSensorReading(result);
    writePacket(SensorID::LDS_REAR_RIGHT, value);
    DAQData.setData<cmbtl::SensorIndex::LDSRearRight>(value);
  } else {
    Serial.printf("recordNextADSValue(): Cannot read from unknown sensor with ID %d", nextSensor);
    status.error_status = status.ERROR;
  }

  index = (index + 1) % PORT_LIST_LENGTH; // Loop back to start
}
//changes recording state and saves file
void changeRecordingState() {
  if(isRecording == true) {
    while(!digitalRead(0)) {
      flashBang(10, 0);
    }
    //outputFile.printf("]");
    outputFile.flush();
    outputFile.close();

    Serial.printf("File closed\n");
    isRecording = false;
    //signal to user that the file saved with a flashbang
    flashBang(1000, 2);
  }
  else {
    while(!digitalRead(0)) {
      flashBang(10, 1);
    }
    String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
    SD.mkdir(time.c_str());
    Serial.println(time.c_str());
    File structConfigFile = SD.open(String("/"+time+"/"+time+"Config.txt").c_str(), FILE_WRITE);
    structConfigFile.close();
    outputFile = SD.open(String("/"+time+"/"+time+".bin").c_str(),  FILE_WRITE);

    // Reset flags
    engineRPM.RPMUpdateFlag = false;
    frontLeftRPM.RPMUpdateFlag = false;
    frontRightRPM.RPMUpdateFlag = false;
    aux1RPM.RPMUpdateFlag = false;
    teensyTempLogger.updateLastLogTime(microsecondsElapsed);
        
    isRecording = true;
  }
}

void updateAnalogValueFlag1() {
  analogValueFlag1 = true;
}

void updateAnalogValueFlag2() {
  analogValueFlag2 = true;
}


void readAnalogValues1() {
  switch (currentAnalogSensor1) {
    case 0: {
      float value = (float) rearBrakePressure.computeSensorReading(ads1.getLastConversionResults());
      DAQData.setData<cmbtl::SensorIndex::RearBrakePressure>(value);
      writePacket(SensorID::REAR_BRAKE_PRES, value);
      currentAnalogSensor1 = 1;
      ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_1, false);
      break;
    }
    case 1: {
      float value = (float) frontBrakePressure.computeSensorReading(ads1.getLastConversionResults());
      DAQData.setData<cmbtl::SensorIndex::FrontBrakePressure>(value);
      writePacket(SensorID::FRONT_BRAKE_PRES, value);
      currentAnalogSensor1 = 2;
      ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_2, false);
      break;
    }
    case 2: {
      float value = (float) LDSFrontRight.computeSensorReading(ads1.getLastConversionResults());
      DAQData.setData<cmbtl::SensorIndex::LDSFrontRight>(value);
      writePacket(SensorID::LDS_FRONT_RIGHT, value);
      currentAnalogSensor1 = 3;
      ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_3, false);
      break;
    }
    case 3: {
      float value = (float) LDSFrontLeft.computeSensorReading(ads1.getLastConversionResults());
      DAQData.setData<cmbtl::SensorIndex::LDSFrontLeft>(value);
      writePacket(SensorID::LDS_FRONT_LEFT, value);
      currentAnalogSensor1 = 0;
      ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
      break;
    }
  }
}

void readAnalogValues2() {
  switch (currentAnalogSensor2) {
    case 0: {
      float value = (float) LDSRearRight.computeSensorReading(ads2.getLastConversionResults());
      DAQData.setData<cmbtl::SensorIndex::LDSRearRight>(value);
      writePacket(SensorID::LDS_REAR_RIGHT, value);
      currentAnalogSensor2 = 1;
      ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_1, false);
      break;
    }
    case 1: {
      float value = (float) LDSRearLeft.computeSensorReading(ads2.getLastConversionResults());
      DAQData.setData<cmbtl::SensorIndex::LDSRearLeft>(value);
      writePacket(SensorID::LDS_REAR_LEFT, value);
      currentAnalogSensor2 = 2;
      ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_2, false);
      break;
    }
    case 2: {
      float value = (float) CVTTemp.computeSensorReading(ads2.getLastConversionResults());
      DAQData.setData<cmbtl::SensorIndex::CVTTemp>(value);
      writePacket(SensorID::CVT_TEMP, value);
      currentAnalogSensor2 = 3;
      ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_3, false);
      break;
    }
    case 3: {
      float value = RearTransferCaseTemp.computeSensorReading(ads2.getLastConversionResults());
      DAQData.setData<cmbtl::SensorIndex::RearTransferCaseTemp>(value);
      writePacket(SensorID::REAR_TC_TEMP, value);
      currentAnalogSensor2 = 0;
      ads2.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
      break;
    }
  }
}

void engineRPMInterrupt() {
  engineRPM.handleInterrupt();
}

void frontLeftRPMInterrupt() {
  frontLeftRPM.handleInterrupt();
}

void frontRightRPMInterrupt() {
  frontRightRPM.handleInterrupt();
}

void aux1RPMInterrupt() {
  aux1RPM.handleInterrupt();
}

void saveInterrupt(){
  //debounce button, the first interrupt will fire, the rest won't (microsecondElapsed is set to a higher priority so it will interrupt this to keep updating the counter)
  if ((microsecondsElapsed / 1000) > lastSaveTimeInMillis + 2000) {
    //read the digital pin to figure out if it went high or low
    if (!digitalRead(0) && saveFlag == false) {
      saveFlag = true;
    } else if (digitalRead(0) && saveFlag == true){
      saveFlag = false;
    }
    //used as a debouncer, the interrupts will finish in order since they have the same priority
    lastSaveTimeInMillis = microsecondsElapsed / 1000;
  }
}

int initADS1256() {
  Serial.println("Setting up ADS1256...");

  ads1256.InitializeADC();

  uint8_t pgaVal = PGA_1;
  uint8_t muxVal = SING_1;
  uint8_t drateVal = DRATE_1000SPS;

  // Set PGA 
  ads1256.setPGA(pgaVal);

  //Set input channels
  ads1256.setMUX(muxVal);

  //Set DRATE
  ads1256.setDRATE(drateVal);

  //Read back the above 3 values to check if the writing was succesful
  Serial.print("PGA: ");
  uint8_t pgaResult = ads1256.getPGA();
  Serial.println(pgaResult);
  delay(100);
  //--
  Serial.print("MUX: ");
  uint8_t muxResult = ads1256.readRegister(MUX_REG);
  Serial.println(muxResult);
  delay(100);
  //--
  Serial.print("DRATE: ");
  uint8_t drateResult = ads1256.readRegister(DRATE_REG);
  Serial.println(drateResult);
  delay(100);

  // --- Error Handling ---
  bool error = false;
  if (pgaVal != pgaResult) {
    Serial.println("Unable to correctly set PGA value");
    error = true;
  }
  if (muxVal != muxResult) {
    Serial.println("Unable to correctly set mux value");
    error = true;
  }
  if (drateVal != drateResult) {
    Serial.println("Unable to correctly set drate value");
    error = true;
  }

  // Failure
  if (error)
    return -1;

  // Success
  return 0;
}

constexpr uint8_t getADSPort(SensorID sensorID) {
  switch (sensorID) {
    case LDS_FRONT_LEFT:
      return SING_4;
    case LDS_FRONT_RIGHT:
      return SING_5;
    case LDS_REAR_LEFT:
      return SING_6;
    case LDS_REAR_RIGHT:
      return SING_7;
    default:
      return -1;
  }
}

void writePacket(SensorID id, float value) {
  if (!isRecording) return;
  DataPacket packet;
  packet.sensorID = id;
  // Get in units of sec 10^-4
  uint32_t ts = safeTimestamp();
  packet.timestamp100Micros = (uint32_t) (ts / 100);
  packet.value = value;

  outputFile.write(&packet, sizeof(packet));
}

uint32_t safeTimestamp() {
  uint32_t ts;
  noInterrupts();
  ts = (uint32_t)(microsecondsElapsed / 100);
  interrupts();
  return ts;
}

void updateStatusLEDs() {
  static uint recording_counter = 0;
  static uint last_record_time = 0;
  if (status.recording_status == status.NOT_READY_TO_RECORD) {
    digitalWrite(RECORDING_LED, LOW);
  }
  else if (status.recording_status == status.READY_TO_RECORD) {
    digitalWrite(RECORDING_LED, HIGH);
  }
  else if (status.recording_status == status.RECORDING) {
    if (last_record_time + FLASH_RATE < millis()) {
      digitalWrite(RECORDING_LED, ++recording_counter % 2 ? HIGH : LOW);
      last_record_time = millis();
    }
  }

  if (status.error_status == status.ERROR) {
    digitalWrite(ERROR_LED, HIGH);
  }
  else if (status.error_status == status.NO_ERROR) {
    digitalWrite(ERROR_LED, LOW);
  }
}
