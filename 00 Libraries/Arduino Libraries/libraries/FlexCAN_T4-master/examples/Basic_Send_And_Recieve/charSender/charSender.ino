#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
CAN_message_t msg;

void setup(void) {
  can1.begin();
  can1.setBaudRate(250000);
  Serial.begin(9600);
}

void loop() {
  Serial.println("Type a char to send");
  char charToSend = Serial.read();
  msg.buf[0] = charToSend;
  can1.write(msg);
}
