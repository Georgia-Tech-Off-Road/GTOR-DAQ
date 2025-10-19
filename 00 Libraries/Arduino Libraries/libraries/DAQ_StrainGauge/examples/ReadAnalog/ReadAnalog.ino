#define AN 18

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(AN, INPUT);
}

void loop() {
  int analog_reading;
  // put your main code here, to run repeatedly:
  analog_reading = analogRead(AN);
  float voltage_reading = (static_cast<float>(analog_reading) / 1023) * 3.3f;
  Serial.printf("%d %f\n", analog_reading, voltage_reading);
}
