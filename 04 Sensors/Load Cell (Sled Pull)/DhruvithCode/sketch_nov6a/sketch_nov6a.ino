Adafruit_ADS1115 ads1;
volatile bool analogValueFlag1 = false;

int currentAnalogSensor1 = 0;


//initialize save flag 
// volatile bool saveFlag = false;


void setup() {
  // put your setup code here, to run once:
  //set up ADC interrupts
  pinMode(40, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(40), updateAnalogValueFlag1, FALLING);

  //configure ADCs
  ads1.begin(0x48, &Wire);

  while(ads1.getDataRate() != RATE_ADS1115_860SPS) {
    ads1.setDataRate(RATE_ADS1115_860SPS);
  }
  ads1.setGain(GAIN_TWOTHIRDS);

  ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);

}

void loop() {
  if (analogValueFlag1) {
      readAnalogValues1();
      analogValueFlag1 = false;
    }
}


void updateAnalogValueFlag1() {
  analogValueFlag1 = true;
}


void readAnalogValues1() {
  switch (currentAnalogSensor1) {
      case 0:
        DAQData.setData<cmbtl::RearBrakePressure>(rearBrakePressure.computeSensorReading(ads1.getLastConversionResults()));
        currentAnalogSensor1 = 1;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_1, false);
        break;
      case 1:
        DAQData.setData<cmbtl::FrontBrakePressure>(frontBrakePressure.computeSensorReading(ads1.getLastConversionResults()));
        currentAnalogSensor1 = 2;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_2, false);
        break;
      case 2:
        DAQData.setData<cmbtl::LDSFrontRight>(LDSFrontRight.computeSensorReading(ads1.getLastConversionResults()));
        currentAnalogSensor1 = 3;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_3, false);
        break;
      case 3:
        DAQData.setData<cmbtl::LDSFrontLeft>(LDSFrontLeft.computeSensorReading(ads1.getLastConversionResults()));
        currentAnalogSensor1 = 0;
        ads1.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, false);
        break;
  }
}


