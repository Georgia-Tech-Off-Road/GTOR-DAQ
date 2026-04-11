#include "DaqPack.h"

//create an interval timer (provides more accurate timing than microseconds)
IntervalTimer intervalMicros;

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
RPMSensor engineRPM(RPM1, ENGTEETH, MIN_EXPECTED_VALUE, MAX_EXPECTED_VALUE, MAX_RPM_INTERVAL_MICROS);
RPMSensor frontLeftRPM(RPM2, FLTEETH, MIN_EXPECTED_VALUE, MAX_EXPECTED_VALUE, MAX_RPM_INTERVAL_MICROS);
RPMSensor frontRightRPM(RPM3, FRTEETH, MIN_EXPECTED_VALUE, MAX_EXPECTED_VALUE, MAX_RPM_INTERVAL_MICROS);
RPMSensor aux1RPM(RPM4, RDTEETH, MIN_EXPECTED_VALUE, MAX_EXPECTED_VALUE, MAX_RPM_INTERVAL_MICROS);

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
  Wire.setClock(400000);
  Wire1.setClock(1000000);



  //initialize pins for LEDs, buttons, etc.
  initPins();
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
  if (setUpSD() == -1) {
    status.error_status = status.SD_ERROR;
  };

  if (initDisplay() != 0) {
    Serial.println("Failure setting up the display!");
  }

  updateStatusLEDs();
  updateStatusDisplay();


  pinMode(RPM3, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM3), frontRightRPMInterrupt, RISING);
  pinMode(RPM1, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM1), engineRPMInterrupt, RISING);
  pinMode(RPM2, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM2), frontLeftRPMInterrupt, RISING);
  pinMode(RPM4, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(RPM4), aux1RPMInterrupt, RISING);

  if (initADS1256() != 0) {
    status.error_status = status.ADS_ERROR;
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
  //set recording flag
  isRecording = true;
  //init auto save time
  autoSaveTimeMillis = millis();

  status.recording_status = status.READY_TO_RECORD;
  updateStatusLEDs();
  updateStatusDisplay();
  dataAquisitionAndSavingLoop();
}

//do nothing here
void loop() {}

//writes data to SD card
void dataAquisitionAndSavingLoop() {
  errorCheck();
  blockForButtonHold(RECORD_SAVE_BUTTON, 1000000); // Must hold the recording button for one second
  // Rapid flash the recording LED for 5 sec
  rapidFlash(RECORDING_LED, 5000);
  while(1) {
    digitalWrite(POWER_LED, HIGH);
    //updateDebugLeds();
    //update auto save time
    //check to see if save should be started/stopped
    static uint32_t holdStartMicros = 0;
    static bool tracking = false;
    bool shouldSave = false;

    if(SDCardChecker.shouldLog(microsecondsElapsed)) {
      SDCardChecker.updateLastLogTime(microsecondsElapsed);
      if (!isSDCardAccessible()) {
        emitSDError();
      }
    }

    if (digitalRead(RECORD_SAVE_BUTTON) == LOW) {
      if(!tracking) {
        holdStartMicros = safeMicrosecondsElapsed();
        tracking = true;
      } else if (safeMicrosecondsElapsed() - holdStartMicros >= 1000000) {
        shouldSave = true;
      }
    } else {
      tracking = false;
    }

    if (Serial.read() == 's') {
      shouldSave = true;
    }

    if (displayLogger.shouldLog(microsecondsElapsed)) {
      displayLogger.updateLastLogTime(microsecondsElapsed);
      updateStatusDisplay();
    }

    if (shouldSave) {
      digitalWrite(ERROR_LED, LOW);
      saveTimer.updateLastLogTime(microsecondsElapsed);

      changeRecordingState();

      status.recording_status = status.READY_TO_RECORD;
      updateStatusLEDs();
      updateStatusDisplay();

      blockForButtonHold(RECORD_SAVE_BUTTON, 1000000); // Must hold the recording button for one second
      updateStatusDisplay();
      changeRecordingState();

      rapidFlash(RECORDING_LED, 5000);

      tracking = false;
    }
    //perform flush check before data check
    if (millis() > autoSaveTimeMillis + 30000) {
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
  }
}

inline void recordNextADSValue() {
  static uint8_t index = 0;
  constexpr int PORT_LIST_LENGTH = 4; 

  // List of ADS ports to read from, should wrap around to begining
  constexpr SensorID ads1256SensorList[PORT_LIST_LENGTH] = {LDS_FRONT_LEFT, LDS_FRONT_RIGHT, LDS_REAR_LEFT, LDS_REAR_RIGHT};

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
    //outputFile.printf("]");
    if(!isSDCardAccessible()) {
      emitSDError();
    }

    if (outputFile) {
      outputFile.flush();
      outputFile.close();
    }

    Serial.printf("File closed\n");

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.printf("%s closed\n", outputFileName.c_str());
    display.display();

    delay(4000); // Delay 2 seconds
    isRecording = false;
    //signal to user that the file saved with a flashbang
    status.recording_status = status.READY_TO_RECORD;
  }
  else {
    String time = String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
    SD.mkdir(time.c_str());
    Serial.println(time.c_str());
    File structConfigFile = SD.open(String("/"+time+"/"+time+"Config.txt").c_str(), FILE_WRITE);
    structConfigFile.close();

    outputFileName = String("/"+time+"/"+time+".bin");
    outputFile = SD.open(outputFileName.c_str(),  FILE_WRITE);

    if(!outputFile) {
      emitSDError();
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.printf("Opening: %s.bin\n", time.c_str());
    display.display();
    delay(4000);

    // Reset flags
    engineRPM.RPMUpdateFlag = false;
    frontLeftRPM.RPMUpdateFlag = false;
    frontRightRPM.RPMUpdateFlag = false;
    aux1RPM.RPMUpdateFlag = false;
    teensyTempLogger.updateLastLogTime(microsecondsElapsed);
        
    isRecording = true;
    status.recording_status = status.RECORDING;
  }
}

void updateAnalogValueFlag1() {
  analogValueFlag1 = true;
}

void updateAnalogValueFlag2() {
  analogValueFlag2 = true;
}

inline void initPins() {
  pinMode(POWER_LED, OUTPUT);
  digitalWrite(POWER_LED, HIGH); // Leave at HIGH for basically forever

  pinMode(RECORDING_LED, OUTPUT);
  digitalWrite(RECORDING_LED, LOW);

  pinMode(ERROR_LED, OUTPUT);
  digitalWrite(ERROR_LED, LOW);

  pinMode(RECORD_SAVE_BUTTON, INPUT_PULLUP);
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



int initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 failed");
    return -1;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hello DAQ!");
  display.display();  // must call this to push buffer to screen
  return 0;
}

constexpr uint8_t getADSPort(SensorID sensorID) {
  switch (sensorID) {
    case LDS_FRONT_LEFT:
      return SING_4;
    case LDS_FRONT_RIGHT:
      return SING_6;
    case LDS_REAR_LEFT:
      return SING_5;
    case LDS_REAR_RIGHT:
      return SING_7;
    default:
      return -1;
  }
}

void blockForButtonHold(int buttonPin, uint32_t holdMicroseconds) {
  uint32_t heldMicroseconds = 0;
  uint32_t lastHeldTime = 0;
  uint32_t debounceMicros = 1000;
  bool firstHold = true;
  while(true) {
    if (heldMicroseconds >= holdMicroseconds) {
      onButtonHeldConfirmed();
      return;
    }
    if (digitalRead(buttonPin) == LOW) {
      uint32_t currMicros = safeMicrosecondsElapsed();
      if (!firstHold) {
        heldMicroseconds += currMicros - lastHeldTime;
        // Serial.println("Button held.");
      }
      firstHold = false;
      lastHeldTime = currMicros;
    } else if ((safeMicrosecondsElapsed() - lastHeldTime) < debounceMicros) { // Prevent noisy values from breaking our hold
      continue;
    }
    else {
      firstHold = true;
      heldMicroseconds = 0;
    }
  }
}

void rapidFlash(int ledPin, uint32_t durationMilliseconds) {
  uint32_t flashRate = 50;
  uint32_t startTime = millis();
  uint32_t flashCounter = 0;

  while (millis() - startTime <= durationMilliseconds) {
    delay(flashRate);
    digitalWrite(ledPin, flashCounter % 2 ? HIGH : LOW);
    flashCounter++;
  }
}

void errorFlash(int ledPin) {
  while (true) {
    digitalWrite(ledPin, LOW);
  }
}

void writePacket(SensorID id, float value) {
  if (!isRecording) return;
  DataPacket packet;
  packet.sensorID = id;
  // Get in units of sec 10^-4
  uint32_t ts = safeTimestamp();
  packet.timestamp100Micros = ts;
  packet.value = value;

  outputFile.write(&packet, sizeof(packet));
}

uint32_t safeTimestamp() {
  return safeMicrosecondsElapsed() / 100;
}

inline uint32_t safeMicrosecondsElapsed() {
  uint32_t ts;
  noInterrupts();
  ts = (uint32_t) microsecondsElapsed;
  interrupts();
  return ts;
}

void updateStatusLEDs() {
  if (status.recording_status == status.RECORDING) {
    digitalWrite(ERROR_LED, HIGH);
  } else {
    digitalWrite(ERROR_LED, LOW);
  }
}

void updateStatusDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  static uint counter = 0;


  if (status.recording_status == status.NOT_READY_TO_RECORD) {
    display.println("Recording Status: Not Ready");
  }
  else if (status.recording_status == status.READY_TO_RECORD) {
    display.println("Recording Status: Ready");
  }
  else if (status.recording_status == status.RECORDING) {
    if (counter % 4 == 0) {
      display.println("Recording Status: Recording");
    }
    else if (counter % 4 == 1) {
      display.println("Recording Status: Recording.");
    }
    else if (counter % 4 == 2) {
      display.println("Recording Status: Recording..");
    } 
    else {
      display.println("Recording Status: Recording...");
    }
  }

  if (status.error_status == status.ERROR) {
    display.setTextColor(SSD1306_WHITE);
    display.println("Error Status: ERROR");
  }
  else if (status.error_status == status.SD_ERROR) {
    display.println("Error Status: SD Card Error!");
    display.println("Please reinsert the SD card and restart");
  }
  else if (status.error_status == status.ADS_ERROR) {
    display.println("Error Status: ADS failed to initialize, restart");
  }
  else if (status.error_status == status.NO_ERROR) {
    display.setTextColor(SSD1306_WHITE);
    display.println("Error Status: None");
  }

  if (status.recording_status == status.READY_TO_RECORD && status.error_status == status.NO_ERROR) {
    display.println("To begin recording, please hold the button for atleast 1 second.");
  }

  if (status.recording_status == status.RECORDING && status.error_status == status.NO_ERROR) {
    display.printf("Output File: %s\n", outputFileName.c_str());
    display.println("Hold to close file");
  }

  display.display();
  counter++;
}

bool isSDCardAccessible() {
  File testFile = SD.open("/_healthcheck.tmp", FILE_WRITE);
  if (!testFile) {
    return false;
  }
  testFile.close();
  SD.remove("/_healthcheck.tmp");
  return true;
}

inline void errorCheck() {
  if (status.error_status != status.NO_ERROR) {
    updateStatusDisplay();
    errorFlash(ERROR_LED);
  }
}

inline void emitSDError() {
  status.error_status = status.SD_ERROR;
  updateStatusDisplay();
  errorFlash(ERROR_LED);
}

inline void emitADSError() {
  status.error_status = status.ADS_ERROR;
  updateStatusDisplay();
  errorFlash(ERROR_LED);
}

inline void onButtonHeldConfirmed() {
  digitalWrite(ERROR_LED, HIGH);
  while (digitalRead(RECORD_SAVE_BUTTON) == LOW) {} // Wait for release so we don't retrigger
  digitalWrite(ERROR_LED, LOW);
  delay(250);
}
