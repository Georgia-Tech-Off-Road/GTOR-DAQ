#include <DAQ_ADS1115.h>

//declare static variables
uint8_t ADS1115 :: _sensorNum;
uint8_t ADS1115 :: _address;
adsGain_t ADS1115 :: _gain;
uint16_t ADS1115 :: _dataRate;
uint16_t ADS1115 :: _dataReadings[4];
I2CDriverWire* ADS1115:: _deviceWire;
uint8_t ADS1115 :: _buffer[3];
volatile bool ADS1115 :: dataReady;

ADS1115 :: ADS1115(uint8_t alert, uint8_t address, adsGain_t gain, uint16_t dataRate, I2CDriverWire* deviceWire) {
    //set alert pin to pullup
    pinMode(alert, INPUT_PULLUP);
    //attach interupt to allert pin
    attachInterrupt(digitalPinToInterrupt(alert), ADS1115 :: updateAnalogValueFlag, FALLING);
    //set _gain
    _gain = gain;
    //set dataRate
    _dataRate = dataRate;
    //set device wire
    _deviceWire = deviceWire;
    //set data ready to false
    dataReady = false;
    //set sensorNum to 0
    _sensorNum = 0;
    //save i2c address
    _address = address;
    //initalize data readings
    _dataReadings[0] = 0;
    _dataReadings[1] = 0;
    _dataReadings[2] = 0;
    _dataReadings[3] = 0;
    //start readings
    ADS1115 :: startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0);
}

//function to update analogValueFlag
void ADS1115 :: updateAnalogValueFlag() {
    dataReady = true;
}

//function to get last computed result (result that's stored before interrupt is triggered)
int16_t ADS1115 :: getLastConversionResults() {
    // Read the conversion results
    return (int16_t)readRegister(ADS1X15_REG_POINTER_CONVERT);
}

//function to start ADC readings 
void ADS1115 :: startADCReading(uint16_t mux) {
    // Start with default values
    uint16_t config =
        ADS1X15_REG_CONFIG_CQUE_1CONV |   // Set CQUE to any value other than
                                          // None so we can use it in RDY mode
        ADS1X15_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
        ADS1X15_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
        ADS1X15_REG_CONFIG_CMODE_TRAD;    // Traditional comparator (default val)
    
    config |= ADS1X15_REG_CONFIG_MODE_SINGLE;
  
    // Set PGA/voltage range
    config |= _gain;
  
    // Set data rate
    config |= _dataRate;
  
    // Set channels
    config |= mux;
  
    // Set 'start single-conversion' bit
    config |= ADS1X15_REG_CONFIG_OS_SINGLE;
  
    // Write config register to the ADC
    writeRegister(ADS1X15_REG_POINTER_CONFIG, config);
  
    // Set ALERT/RDY to RDY mode.
    writeRegister(ADS1X15_REG_POINTER_HITHRESH, 0x8000);
    writeRegister(ADS1X15_REG_POINTER_LOWTHRESH, 0x0000);
  }

//function to getAnalogReadings
uint16_t* ADS1115 :: getAnalogReadings() {
    switch (_sensorNum) {
        case 0:
          _dataReadings[0] = ADS1115 :: getLastConversionResults();
          _sensorNum = 1;
          startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_1);
          break;
        case 1:
          _dataReadings[1] = ADS1115 :: getLastConversionResults();
          _sensorNum = 2;
          startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_2);
          break;
        case 2:
          _dataReadings[2] = ADS1115 :: getLastConversionResults();
          _sensorNum = 3;
          startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_3);
          break;
        case 3:
          _dataReadings[3] = ADS1115 :: getLastConversionResults();
          _sensorNum = 0;
          startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0);
          break;
    }
    return _dataReadings;
}

//use an i2c wire object

//method to write to registers
void ADS1115 :: writeRegister(uint8_t reg, uint16_t value) {
  Serial.println("Writing");
  _buffer[0] = reg;
  _buffer[1] = value >> 8;
  _buffer[2] = value & 0xFF;
  _deviceWire -> beginTransmission(_address);
  _deviceWire -> write((byte *) &value, sizeof(value));
  _deviceWire -> endTransmission(_address);
}

//method to read from registers
uint16_t ADS1115 :: readRegister(uint8_t reg) {
  _buffer[0] = reg;
  _deviceWire -> beginTransmission(_address);
  _deviceWire -> write(_buffer[0]);
  //read two bytes into the buffer
  _buffer[0] = _deviceWire -> read();
  _buffer[1] = _deviceWire -> read();
  _deviceWire -> endTransmission(_address);
  return ((_buffer[0] << 8) | _buffer[1]);
}


