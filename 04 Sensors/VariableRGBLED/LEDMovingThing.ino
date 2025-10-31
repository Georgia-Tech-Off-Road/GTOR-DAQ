// Potentiometric Linear Position Sensor (Analog Input)
// Connected to Arduino Uno A0

const int sensorPin = A0;  
int sensorValue = 0;       
float positionPercent = 0.0;

const int redPin = 5;
const int greenPin = 6;

void setup() {
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);              
  pinMode(greenPin, OUTPUT);
}

void loop() {
  // 1️⃣ Read the sensor value (0–1023)
  sensorValue = analogRead(sensorPin);

  // 2️⃣ Convert to percentage (0–100%)
  positionPercent = ((sensorValue / 1023.0) * 100.0);

  // 3️⃣ Print readings
  Serial.print("Raw value: ");
  Serial.print(sensorValue);
  Serial.print("   Position: ");
  Serial.print(positionPercent, 1);
  Serial.println("%");

  // 4️⃣ Map 0–100% → 0–255 brightness
  int redBrightness = map(positionPercent, 0, 100, 0, 255);
  int greenBrightness = 255 - redBrightness;

  analogWrite(redPin, redBrightness);
  analogWrite(greenPin, greenBrightness);

  delay(100);
}
