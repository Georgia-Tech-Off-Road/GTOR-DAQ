
#include <Arduino.h>
#include <SD.h>
#include <stdio.h>
#include <string.h>

// ==========================================
// USER SETTINGS
// ==========================================

// Number of connected sensors (1 to 4)
const int NUM_SENSORS = 4;
const int SENSOR_PINS[4] = {A0, A1, A2, A3};

// External button and LED
const int BUTTON_PIN = 6;
const int LED_PIN = 4;

// Automatically start SD recording at power-up
const bool START_WITH_SD = true;

// Sampling configuration
const uint32_t SAMPLE_RATE_HZ = 500;
const uint32_t SERIAL_PRINT_EVERY = 10;
const int ADC_AVERAGING = 16;

// Displacement calibration
const float MAX_TRAVEL_IN = 8.0f;

const int ADC_MIN[4] = {0, 0, 0, 0};
const int ADC_MAX[4] = {4095, 4095, 4095, 4095};

// SD card settings
const size_t SD_BUFFER_SIZE = 8192;
const uint32_t FLUSH_INTERVAL_MS = 1000;

// Button debounce
const uint32_t DEBOUNCE_MS = 30;

// LED timing
const uint32_t RECORD_LED_PERIOD = 3000;
const uint32_t IDLE_LED_PERIOD = 5000;

// ==========================================
// GLOBAL VARIABLES
// ==========================================

enum OutputMode {
  MODE_STANDBY,
  MODE_SD,
  MODE_SERIAL
};

OutputMode mode = MODE_STANDBY;

// Button state
bool lastButtonReading = HIGH;
bool stableButtonState = HIGH;
uint32_t lastDebounceMs = 0;

// LED state
uint32_t ledPatternStartMs = 0;
bool ledIsOn = false;

// SD card
File logFile;
char fileName[13];

char sdBuffer[SD_BUFFER_SIZE];
size_t bufferUsed = 0;

// Sampling
const uint32_t SAMPLE_PERIOD_US =
    1000000UL / SAMPLE_RATE_HZ;

uint32_t nextSampleUs = 0;

uint32_t previousMicros = 0;
uint64_t elapsedMicros = 0;
uint64_t sessionStartUs = 0;

uint32_t totalSamples = 0;
uint32_t missedSamples = 0;
uint32_t lastFlushMs = 0;

// CSV columns stay consistent for 1-4 sensors
const char CSV_HEADER[] =
    "time_us,missed_total,"
    "A0_raw,A1_raw,A2_raw,A3_raw,"
    "A0_in,A1_in,A2_in,A3_in\n";

// ==========================================
// NON-BLOCKING LED
// ==========================================

void setMode(OutputMode newMode) {
  mode = newMode;

  // Restart the LED pattern
  ledPatternStartMs = millis();
  ledIsOn = false;
  digitalWrite(LED_PIN, LOW);
}

void updateStatusLED() {

  uint32_t elapsed = millis() - ledPatternStartMs;
  bool shouldBeOn = false;

  if (mode == MODE_SD) {

    // Double flash every 3 seconds
    uint32_t phase = elapsed % RECORD_LED_PERIOD;

    shouldBeOn =
        (phase < 70) ||
        (phase >= 170 && phase < 240);

  } else {

    // Single flash every 5 seconds
    uint32_t phase = elapsed % IDLE_LED_PERIOD;

    shouldBeOn = (phase < 120);
  }

  if (shouldBeOn != ledIsOn) {
    digitalWrite(
        LED_PIN,
        shouldBeOn ? HIGH : LOW
    );

    ledIsOn = shouldBeOn;
  }
}

// ==========================================
// TIMESTAMP FUNCTIONS
// ==========================================

// Extend micros() to 64 bits.
// Called continuously, including in standby.
uint64_t getTimeUs() {
  uint32_t now = micros();

  elapsedMicros +=
      (uint32_t)(now - previousMicros);

  previousMicros = now;

  return elapsedMicros;
}

// Start a fresh sampling session
void resetSamplingSession() {

  sessionStartUs = getTimeUs();

  nextSampleUs =
      (uint32_t)sessionStartUs + SAMPLE_PERIOD_US;

  totalSamples = 0;
  missedSamples = 0;
}

// ==========================================
// ADC -> DISPLACEMENT
// ==========================================

float adcToInches(int raw, int sensorIndex) {

  float span =
      ADC_MAX[sensorIndex] - ADC_MIN[sensorIndex];

  if (span <= 0.0f) {
    return 0.0f;
  }

  float position =
      (raw - ADC_MIN[sensorIndex])
      * MAX_TRAVEL_IN / span;

  return constrain(
      position, 0.0f, MAX_TRAVEL_IN
  );
}

// ==========================================
// FORMAT CSV DATA
// ==========================================

int createCSVRow(
    char *line,
    size_t capacity,
    uint64_t timestamp,
    const uint16_t raw[4]
) {

  int n = snprintf(
      line, capacity,
      "%llu,%lu",
      (unsigned long long)timestamp,
      (unsigned long)missedSamples
  );

  // Raw ADC columns
  for (int i = 0; i < 4; i++) {

    if (i < NUM_SENSORS) {

      n += snprintf(
          line + n, capacity - n,
          ",%u", (unsigned int)raw[i]
      );

    } else {

      n += snprintf(
          line + n, capacity - n, ","
      );
    }
  }

  // Displacement columns
  for (int i = 0; i < 4; i++) {

    if (i < NUM_SENSORS) {

      float inches = adcToInches(raw[i], i);

      // Five decimal places
      uint32_t scaled =
          (uint32_t)(inches * 100000.0f + 0.5f);

      n += snprintf(
          line + n, capacity - n,
          ",%lu.%05lu",
          (unsigned long)(scaled / 100000),
          (unsigned long)(scaled % 100000)
      );

    } else {

      n += snprintf(
          line + n, capacity - n, ","
      );
    }
  }

  n += snprintf(
      line + n, capacity - n, "\n"
  );

  return n;
}

// ==========================================
// SD BUFFER
// ==========================================

bool writeBuffer() {

  if (bufferUsed == 0) return true;
  if (!logFile) return false;

  size_t written = logFile.write(
      (const uint8_t *)sdBuffer,
      bufferUsed
  );

  if (written != bufferUsed) {
    return false;
  }

  bufferUsed = 0;
  return true;
}

bool appendToBuffer(const char *data, size_t len) {

  if (len > SD_BUFFER_SIZE) {
    return false;
  }

  if (bufferUsed + len > SD_BUFFER_SIZE) {

    if (!writeBuffer()) {
      return false;
    }
  }

  memcpy(sdBuffer + bufferUsed, data, len);
  bufferUsed += len;

  return true;
}

// ==========================================
// START A NEW SD RECORDING
// ==========================================

bool startSDLogging() {

  // Initialize built-in microSD slot
  if (!SD.begin(BUILTIN_SDCARD)) {

    Serial.println("ERROR: SD initialization failed.");
    return false;
  }

  bool foundName = false;

  // Find the next unused filename
  for (int i = 0; i < 1000; i++) {

    snprintf(
        fileName, sizeof(fileName),
        "LDS%03d.CSV", i
    );

    if (!SD.exists(fileName)) {
      foundName = true;
      break;
    }
  }

  if (!foundName) {
    Serial.println("ERROR: No filenames available.");
    return false;
  }

  // Open a new file
  logFile = SD.open(fileName, FILE_WRITE);

  if (!logFile) {
    Serial.println("ERROR: Could not open file.");
    return false;
  }

  // Write CSV header
  size_t headerLength = strlen(CSV_HEADER);

  if (logFile.write(
          (const uint8_t *)CSV_HEADER,
          headerLength
      ) != headerLength) {

    logFile.close();
    Serial.println("ERROR: Header write failed.");
    return false;
  }

  logFile.flush();

  bufferUsed = 0;
  lastFlushMs = millis();

  // Reset timestamps for this recording
  resetSamplingSession();

  // Activate double-flash LED pattern
  setMode(MODE_SD);

  Serial.print("Recording started: ");
  Serial.println(fileName);

  return true;
}

// ==========================================
// STOP RECORDING SAFELY
// ==========================================

void stopSDLogging() {

  if (mode != MODE_SD) return;

  // Write remaining data in RAM
  bool success = writeBuffer();

  if (success) {
    logFile.flush();
  }

  // Close current recording
  logFile.close();

  // Enter standby mode
  setMode(MODE_STANDBY);

  if (success) {

    Serial.print("Recording saved: ");
    Serial.println(fileName);

    Serial.print("Samples: ");
    Serial.println(totalSamples);

    Serial.print("Missed intervals: ");
    Serial.println(missedSamples);

  } else {

    Serial.println("WARNING: SD write error.");
  }
}

// ==========================================
// SD ERROR HANDLING
// ==========================================

void handleSDError() {

  Serial.println("ERROR: SD write failed.");

  logFile.close();
  bufferUsed = 0;

  setMode(MODE_STANDBY);
}

// ==========================================
// START SERIAL OUTPUT
// ==========================================

void startSerialMode() {

  resetSamplingSession();
  setMode(MODE_SERIAL);

  Serial.print(CSV_HEADER);
}

// ==========================================
// BUTTON CONTROL (NON-BLOCKING)
// ==========================================

void updateButton() {

  bool reading = digitalRead(BUTTON_PIN);
  uint32_t now = millis();

  // Detect any change in raw button reading
  if (reading != lastButtonReading) {

    lastDebounceMs = now;
    lastButtonReading = reading;
  }

  // Wait until button state has been stable
  if ((now - lastDebounceMs) >= DEBOUNCE_MS) {

    if (reading != stableButtonState) {

      stableButtonState = reading;

      // Only act on a button PRESS
      // LOW = pressed with INPUT_PULLUP
      if (stableButtonState == LOW) {

        if (mode == MODE_SD) {

          // Press while recording:
          // save, close file, enter standby
          stopSDLogging();

        } else {

          // Press while in standby or Serial:
          // begin a NEW SD recording
          if (!startSDLogging()) {
            setMode(MODE_STANDBY);
          }
        }
      }
    }
  }
}

// ==========================================
// OPTIONAL SERIAL COMMANDS
// ==========================================

void handleCommands() {

  while (Serial.available()) {

    char command = Serial.read();

    if (command == 'l') {

      // Start a new SD recording
      if (mode != MODE_SD) {

        if (!startSDLogging()) {
          setMode(MODE_STANDBY);
        }
      }

    } else if (command == 'p') {

      // Switch to Serial output
      if (mode == MODE_SD) {
        stopSDLogging();
      }

      startSerialMode();

    } else if (command == 'q') {

      // Stop and enter standby
      if (mode == MODE_SD) {
        stopSDLogging();
      } else {
        setMode(MODE_STANDBY);
      }
    }
  }
}

// ==========================================
// SETUP
// ==========================================

void setup() {

  Serial.begin(115200);

  // External status LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Button with internal pull-up
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize debounce state
  lastButtonReading = digitalRead(BUTTON_PIN);
  stableButtonState = lastButtonReading;
  lastDebounceMs = millis();

  ledPatternStartMs = millis();

  // ADC configuration
  analogReadResolution(12);
  analogReadAveraging(ADC_AVERAGING);

  // Validate number of sensors
  if (NUM_SENSORS < 1 || NUM_SENSORS > 4) {

    Serial.println("ERROR: Invalid NUM_SENSORS.");

    setMode(MODE_STANDBY);

    while (true) {
      updateStatusLED();
    }
  }

  // Configure active sensor pins
  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(SENSOR_PINS[i], INPUT);
  }

  previousMicros = micros();

  // Default: begin recording on startup
  if (START_WITH_SD) {

    if (!startSDLogging()) {
      setMode(MODE_STANDBY);
    }

  } else {

    setMode(MODE_STANDBY);
  }
}

// ==========================================
// MAIN LOOP
// ==========================================

void loop() {

  // Check button and optional Serial commands
  updateButton();
  handleCommands();

  // Update extended timer even during standby
  uint64_t currentTime = getTimeUs();

  // Only sample when recording or printing
  if (mode != MODE_STANDBY) {

    uint32_t nowUs = (uint32_t)currentTime;

    // Check if next sample is due
    if ((int32_t)(nowUs - nextSampleUs) >= 0) {

      uint32_t lateness =
          nowUs - nextSampleUs;

      // Track intervals lost to delays
      uint32_t skipped =
          lateness / SAMPLE_PERIOD_US;

      missedSamples += skipped;

      nextSampleUs +=
          (skipped + 1) * SAMPLE_PERIOD_US;

      // Timestamp relative to this session
      uint64_t timestamp =
          currentTime - sessionStartUs;

      // Read each active sensor
      uint16_t raw[4] = {0, 0, 0, 0};

      for (int i = 0; i < NUM_SENSORS; i++) {
        raw[i] = analogRead(SENSOR_PINS[i]);
      }

      // Create CSV line
      char line[192];

      int lineLength = createCSVRow(
          line,
          sizeof(line),
          timestamp,
          raw
      );

      if (lineLength <= 0 ||
          lineLength >= (int)sizeof(line)) {

        Serial.println("ERROR: CSV line too long.");

        if (mode == MODE_SD) {
          stopSDLogging();
        } else {
          setMode(MODE_STANDBY);
        }

        return;
      }

      // SD recording
      if (mode == MODE_SD) {

        if (!appendToBuffer(line, lineLength)) {
          handleSDError();
          return;
        }

      } else if (mode == MODE_SERIAL) {

        // Print less frequently to avoid
        // overwhelming Serial output
        if (totalSamples % SERIAL_PRINT_EVERY == 0) {
          Serial.print(line);
        }
      }

      totalSamples++;
    }

    // Periodic SD buffer flush
    if (mode == MODE_SD) {

      if (millis() - lastFlushMs >= FLUSH_INTERVAL_MS) {

        if (!writeBuffer()) {
          handleSDError();
          return;
        }

        logFile.flush();
        lastFlushMs = millis();
      }
    }
  }

  // LED is updated after acquisition work
  // No delay() anywhere in the loop
  updateStatusLED();
}
