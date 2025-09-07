#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
CAN_message_t msg;

BinaryBuffer dataBuffer;
int bufferIndex = 0;

bool decodeFromCan(BinaryBuffer * data, CAN_message_t dataMsg) {
  if (dataMsg.data[0] == 0 && dataMsg.data[1] == 0 && dataMsg.data[2] == 0 && dataMsg.data[3] == 0 && dataMsg.data[4] == 0 && dataMsg.data[5] == 0 && dataMsg.data[6] == 0 && dataMsg.data[7] == 0) {
    return true;
  }
	for (int i = 0; i < (data.getCapacity()/8); i+=8;) {
		memcpy(&data.getBuffer()+i,msg.buf,8);
	}
}

void setup(void) {
  can1.begin();
  can1.setBaudRate(250000);
  Serial.begin(9600);
}

void loop() {
  if(can1.read(msg)) {
    bool messageReady = decodeFromCan(&dataBuffer, Can);
    if(messageReady) {
      for (int i = 0; i < dataBuffer.getCapacity()/8; i+=1) {
        Serial.prinf("%c\n", dataBuffer.getBuffer[i]);
      }
    }
  }
}
