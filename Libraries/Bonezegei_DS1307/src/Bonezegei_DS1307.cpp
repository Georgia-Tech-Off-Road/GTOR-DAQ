/*
  This Library is written for DS1307 RTC
  Author: Bonezegei (Jofel Batutay)
  Date: Jan 28 2024
  Updated: March 2024
*/

#include "Bonezegei_DS1307.h"

Bonezegei_DS1307::Bonezegei_DS1307() {}
Bonezegei_DS1307::Bonezegei_DS1307(uint8_t addr) {
  _addr = addr;
 
}

char Bonezegei_DS1307::begin() {
	Wire.begin();
	Wire.beginTransmission(_addr);
    Wire.write(0x00);
    Wire.endTransmission();

    int a = 0;
    int r = (uint8_t)Wire.requestFrom((int)_addr, 8);
    while (Wire.available()) {
      _data[a] = Wire.read();
      a++;
    }

    if (_data[0] & 0x80) {
      Wire.beginTransmission(_addr);
      Wire.write(0x00);
      Wire.write(0x7F & _data[0]);
      Wire.endTransmission();
    }

  if (r) {

    return r;
  } else {
    return 0;
  }
}
uint8_t Bonezegei_DS1307::convert(uint8_t data) {
  uint8_t result = data & 0x0f;
  result += ((data & 0xf0) >> 4) * 10;
  return result;
}
uint8_t Bonezegei_DS1307::convertBCD(int data) {
  int tmp1;
  if (data > 10) {
    tmp1 = (int)(data / 10);
  } else {
    tmp1 = 0;
  }
  int tmp2 = data - (tmp1 * 10);

  uint8_t result = (tmp1 << 4) | tmp2;

  return result;
}
char Bonezegei_DS1307::getTime() {
  Wire.beginTransmission(_addr);
  Wire.write(0x00);
  Wire.endTransmission();

  int a = 0;
  int r = (uint8_t)Wire.requestFrom((int)_addr, 8);
  while (Wire.available()) {
    _data[a] = Wire.read();
    a++;
  }
  //Serial.printf("0x%02x\n", _data[0]);
  //time
  _seconds = convert(_data[0]);  //0 - 60 seconds
  _minutes = convert(_data[1]);  //0 - 60 minutes

  if (_data[2] & 0x40) {
    _hour = convert(_data[2] & 0x1F);
    _ampm = (_data[2] & 0x20) >> 5;
    _ampm = (~_ampm)&1;
    _hour12 = 12;
  } else {
    _hour = convert(_data[2] & 0x3F);
    _hour12 = 24;
  }

  //date
  _day = _data[3];                    // 1-7  day of week
  _date = convert(_data[4]);          // 1-31 date
  _month = convert(_data[5] & 0x3f);  // 1- 12 month
  _year = convert(_data[6]);          // 0- 99 year

  return r;
}

uint8_t Bonezegei_DS1307::getSeconds() {
  return _seconds;
}

uint8_t Bonezegei_DS1307::getMinute() {
  return _minutes;
}

uint8_t Bonezegei_DS1307::getHour() {
  return _hour;
}

uint8_t Bonezegei_DS1307::getDay() {
  return _day;
}

uint8_t Bonezegei_DS1307::getDate() {
  return _date;
}

uint8_t Bonezegei_DS1307::getMonth() {
  return _month;
}

uint8_t Bonezegei_DS1307::getYear() {
  return _year;
}

uint8_t Bonezegei_DS1307::getAMPM() {
  return _ampm;
}

uint8_t Bonezegei_DS1307::getFormat() {
  return _hour12;
}

void Bonezegei_DS1307::setFormat(uint8_t fmt) {
    getTime();

  if (fmt == 12) {
    _data[2] |= 0x40;
    Wire.beginTransmission(_addr);
    Wire.write(0x02);
    Wire.write(_data[2]);
    Wire.endTransmission();
    _hour12 = 12;
  } else {
    _data[2] &= 0x3f;
    Wire.beginTransmission(_addr);
    Wire.write(0x02);
    Wire.write(_data[2]);
    Wire.endTransmission();
    _hour12 = 24;
  }
}

void Bonezegei_DS1307::setAMPM(uint8_t ampm) {
    getTime();

  if (ampm == 1) {
    _data[2] |= 0x20;
    Wire.beginTransmission(_addr);
    Wire.write(0x02);
    Wire.write(_data[2]);
    Wire.endTransmission();
    _ampm = 1;
  } else {
    _data[2] &= 0x5f;
    Wire.beginTransmission(_addr);
    Wire.write(0x02);
    Wire.write(_data[2]);
    Wire.endTransmission();
    _ampm = 0;
  }
}
void Bonezegei_DS1307::setTime(const char *t) {
  int sec;
  int min;
  int hour;
  sscanf(t, "%d:%d:%d", &hour, &min, &sec);

  _data[0] = convertBCD(sec);
  _data[1] = convertBCD(min);
  _data[2] &= 0x60;
  _data[2] |= convertBCD(hour);

  Wire.beginTransmission(_addr);
  Wire.write(0x00);
  Wire.write(_data[0]);
  Wire.write(_data[1]);
  Wire.write(_data[2]);
  Wire.endTransmission();
}
void Bonezegei_DS1307::setDate(const char *d) {
  int date;
  int mon;
  int year;
  sscanf(d, "%d/%d/%d", &mon, &date, &year);

  _data[4] = convertBCD(date);
  _data[5] &= 0x80;
  _data[5] = convertBCD(mon);
  _data[6] |= convertBCD(year);

  Wire.beginTransmission(_addr);
  Wire.write(0x04);
  Wire.write(_data[4]);
  Wire.write(_data[5]);
  Wire.write(_data[6]);
  Wire.endTransmission();
}
void Bonezegei_DS1307::setDay(uint8_t d) {
  _data[2] = convertBCD(d);
}
