#include <FlexCAN_T4.h>
#include <BinaryBuffer.h>
#include <boost_endian_arithmetic.hpp>


FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
CAN_message_t msg;

using cmbtl::BinaryBuffer;

template <typename CAN_t> void copyOverCAN(const BinaryBuffer& data, CAN_t& Can) {
	CAN_message_t msg;
  long long int bufferLength = data.getCapacity();
  memcpy(&msg.buf, &bufferLength, 8);
  Can.write(msg);
	for (unsigned long int i = 0; i < (data.getCapacity()/8); i += 8) {
    //copy each byte
    for (int j = 0; j < 8; j += 1) {
      //copy each bit
      uint8_t placeholder = 0;
      for (int y = 0; y < 8; y += 1) {
        placeholder += data.readBit() << y;
      }
      msg.buf[j] = placeholder;
    }
		Can.write(msg);
	}
  //write last few bytes (can likely combine this with top for-loop but no need to bother with that as this is just a demo)
  for (unsigned long int i = 0; i < data.getCapacity()%8; i+= 1) {
    //copy each bit
      uint8_t placeholder = 0;
      for (int y = 0; y < 8; y += 1) {
        placeholder += data.readBit() << y;
      }
      msg.buf[i] = placeholder;
  }
  Can.write(msg);
  msg.buf[0] = 0b11111111;
  msg.buf[1] = 0b11111111;
  msg.buf[2] = 0b11111111;
  msg.buf[3] = 0b11111111;
  msg.buf[4] = 0b11111111;
  msg.buf[5] = 0b11111111;
  msg.buf[6] = 0b11111111;
  msg.buf[7] = 0b11111111;
  Can.write(msg);
}

void setup(void) {
  can1.begin();
  can1.setBaudRate(250000);
  Serial.begin(9600);
}

void loop() {
  BinaryBuffer dataToBeSent(24);
  Serial.println("Type a char to send");
  while (Serial.available() == 0) {
    // Do nothing (busy wait)
  }
  char charToSend = Serial.read();
  for (int i = 0; i < 24; i +=1) {
    dataToBeSent.write(charToSend);
  }
  copyOverCAN(dataToBeSent, can1);
}
