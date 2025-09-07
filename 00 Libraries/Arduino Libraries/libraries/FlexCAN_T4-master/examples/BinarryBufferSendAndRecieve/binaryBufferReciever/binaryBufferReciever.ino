#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
CAN_message_t msg;

BinaryBuffer dataBuffer;
int bufferIndex = 0;
bool messageTransmitInProgress = false;
long long int sizeOfMessage = 0;
long long int bitsRecieved = 0;

bool decodeFromCan(BinaryBuffer * data, CAN_message_t dataMsg) {
  if (!messageTransmitInProgress) {
    memcpy(&sizeOfMessage, &dataMsg.buf, 8);
  }
  if (dataMsg.data[0] == b11111111 && dataMsg.data[1] == b11111111 && dataMsg.data[2] == b11111111 && dataMsg.data[3] == b11111111 && dataMsg.data[4] == b11111111 && dataMsg.data[5] == b11111111 && dataMsg.data[6] == b11111111 && dataMsg.data[7] == b11111111) {
    return true;
  }
  if (bitsRecieved + 64 > sizeOfMessage) {
    memcpy(&data.getBuffer()+bitsRecieved/8, msg.buf, sizeOfMessage - bitsRecieved/8);
    bitsRecieved = sizeOfMessage;
  }
	memcpy(&data.getBuffer()+bitsRecieved/8,msg.buf,8);
  bitsRecieved += 64;
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
