#include <SD.h>

// --- Configuration Pin & Constants ---
const uint8_t ENGINE_PIN = 11;
const uint8_t REARPM_PIN = 10;
const uint8_t BUTTON_PIN = 9;
const float TEETH_PER_REVOLUTION = 69.0; //Rear
const float SPARK_PER_REVOLUTION = 1.0;  // Engine

// --- Volatile Variables (Shared with Interrupt) ---
volatile uint32_t lastPulseTimeMicros = 0;
volatile uint32_t pulseIntervalMicros = 0;
volatile uint32_t lastSparkTimeMicros = 0;
volatile uint32_t sparkIntervalMicros = 0;
volatile uint32_t debugPulseCount = 0;
volatile uint32_t debugSparkCount = 0;

// --- Timing Variables for Serial Output ---
uint32_t lastPrintTimeMs = 0;
const uint32_t PRINT_INTERVAL_MS = 250;

// --- Button / SD Logging State ---
bool loggingActive = false;
bool lastButtonReading = HIGH;// idle state with INPUT_PULLUP
bool debouncedButtonState = HIGH;
uint32_t lastDebounceTimeMs = 0;
const uint32_t DEBOUNCE_DELAY_MS = 50;

File logFile;
uint16_t logFileIndex = 0;

// --- Interrupt Service Routine (ISR) ---
void FASTRUN rearpmISR() {
  uint32_t currentTimeMicros = micros();
  uint32_t delta = currentTimeMicros - lastPulseTimeMicros;

// Signal debounce: Ignore noise faster than 10,000 RPM (87 us)
  if (delta > 500) {
    pulseIntervalMicros = delta;
    lastPulseTimeMicros = currentTimeMicros;
    debugPulseCount++;
  }
}

void FASTRUN engineISR() {
  uint32_t currentTimeMicros = micros();
  uint32_t delta = currentTimeMicros - lastSparkTimeMicros;

// Signal debounce: Ignore noise faster than 10,000 RPM (6000 us)
  if (delta > 6000) {
    sparkIntervalMicros = delta;
    lastSparkTimeMicros = currentTimeMicros;
    debugSparkCount++;
  }
}

// --- Start a new log file (auto-incrementing name so old runs aren't overwritten) ---
void startLogging() {
  char filename[16];
  do {
    snprintf(filename, sizeof(filename), "rpmlog%d.csv", logFileIndex);
    logFileIndex++;
    } while (SD.exists(filename) && logFileIndex < 1000);

  logFile = SD.open(filename, FILE_WRITE);
  if (logFile) {
    logFile.println("millis,rear rpm,engine rpm");
    loggingActive = true;
    Serial.print("Logging STARTED -> ");
    Serial.println(filename);
  } else {
    Serial.println("ERROR: could not open log file on SD card");
    loggingActive = false;
  }
}

void stopLogging() {
  if (logFile) {
    logFile.close();
  }
  loggingActive = false;
  Serial.println("Logging STOPPED (file saved)");
}

// --- Debounced button check, called every loop() ---
void checkButton() {
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTimeMs = millis();
  }

  if ((millis() - lastDebounceTimeMs) > DEBOUNCE_DELAY_MS) {
    if (reading != debouncedButtonState) {
      debouncedButtonState = reading;

// Button wired to GND -> a press pulls the pin LOW
      if (debouncedButtonState == LOW) {
        loggingActive ? stopLogging() : startLogging();
      }
    }
  }

  lastButtonReading = reading;
}

void setup() {
  Serial.begin(115200);

// Try INPUT_PULLUP first. If the sensor pulls to ground, this works perfectly.
  pinMode(REARPM_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(REARPM_PIN), rearpmISR, FALLING);

  pinMode(ENGINE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENGINE_PIN), engineISR, FALLING);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  delay(2000); // Brief pause to open Serial Monitor
  Serial.println("--- MINIMAL RPM DEBUGGER STARTED ---");

  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("SD card init FAILED - check card is inserted/formatted (FAT32)");
  } else {
    Serial.println("SD card ready. Press button on pin 9 to start/stop logging.");
  }

  //isHigh = digitalRead(SENSOR_PIN);
}

void loop() {
  checkButton();

  uint32_t currentMillis = millis();

  float rearRPM = 0.0;
  float engineRPM = 0.0;

  // Atomic copy of volatile timing values
    noInterrupts();
    uint32_t r_interval = pulseIntervalMicros;
    uint32_t e_interval = sparkIntervalMicros;

    uint32_t r_lastTime = lastPulseTimeMicros;
    uint32_t e_lastTime = lastSparkTimeMicros;

    uint32_t pulseCount = debugPulseCount;
    uint32_t sparkCount = debugSparkCount;
    interrupts();

// Timeout check: Reset to 0 if no pulse in 0.5s
  if (micros() - r_lastTime > 500000) {
    rearRPM = 0.0;
} else if (r_interval > 0) {
  rearRPM = (60000000.0 / (float)r_interval) / TEETH_PER_REVOLUTION;
}

  if (micros() - e_lastTime > 500000) {
    engineRPM = 0.0;
} else if (e_interval > 0) {
  engineRPM = (60000000.0 / (float)e_interval) / SPARK_PER_REVOLUTION;
}

// Print/log loop executing 4 times a second
  if (currentMillis - lastPrintTimeMs >= PRINT_INTERVAL_MS) {
    lastPrintTimeMs = currentMillis;

// Read the actual physical voltage state of the pin
    bool rHigh = digitalRead(REARPM_PIN);
    bool eHigh = digitalRead(ENGINE_PIN);

// Print everything to the Serial Monitor
    Serial.print("Pin 10 State: ");
    Serial.print(rHigh ? "HIGH (3.3)" : "LOW (GND) ");
    Serial.print(" | Pulses: ");
    Serial.print(pulseCount);
    Serial.print(" | RPM: ");
    Serial.print(rearRPM, 1);
    Serial.println("|");

    Serial.print("Pin 11 State: ");
    Serial.print(eHigh ? "HIGH (3.3)" : "LOW (GND) ");
    Serial.print(" | Pulses: ");
    Serial.print(sparkCount);
    Serial.print(" | RPM: ");
    Serial.print(engineRPM, 1);
    Serial.println(loggingActive ? " | LOGGING" : " | (idle)");

// Write the same data to the SD card if a logging session is active
    if (loggingActive && logFile) {
      if (digitalRead(LED_BUILTIN)) {
        digitalWrite(LED_BUILTIN, LOW);
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
      }

      logFile.print(currentMillis);
      logFile.print(",");
      logFile.print(rearRPM, 2);
      logFile.print(",");
      logFile.println(engineRPM, 2);
      logFile.flush(); // commit to card now so a power loss mid-run doesn't lose data
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
}