#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
CAN_message_t msg;

func copyOverCAN(BinaryBuffer data, FlexCAN_T4 Can) {
	CAN_message_t msg;
  long long int bufferLength = data.getCapacity();
  memcpy(&msg.buff, bufferLength, 8);
  Can.write(&msg);
	for (int i = 0; i < (data.getCapacity()/8); i+=8;) {
		memcpy(&msg.buff,data.getBuffer()+i,8);
		Can.write(msg);
	}
  if (data.getCapacity() % 8 != 0) {
    memcpy(&msg.buff,data.getBuffer()+data.getCapacity()/8,data.getCapacity()%8);
    Can.write(msg);
  }
  msg.buf[0] == b11111111;
  msg.buf[1] == b11111111;
  msg.buf[2] == b11111111;
  msg.buf[3] == b11111111;
  msg.buf[4] == b11111111;
  msg.buf[5] == b11111111;
  msg.buf[6] == b11111111;
  msg.buf[7] == b11111111;
  Can.write(msg);
}

void setup(void) {
  can1.begin();
  can1.setBaudRate(250000);
  Serial.begin(9600);
}

void loop() {
  Serial.println("Type a char to send");
  while (Serial.available() == 0) {
    // Do nothing (busy wait)
  }
  char charToSend = Serial.read();
  msg.buf[0] = charToSend;
  can1.write(msg);
}
