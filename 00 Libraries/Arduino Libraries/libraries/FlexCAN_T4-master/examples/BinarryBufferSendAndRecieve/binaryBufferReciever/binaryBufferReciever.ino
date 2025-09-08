#include <FlexCAN_T4.h>
#include <BinaryBuffer.h>
#include <boost_endian_arithmetic.hpp>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
CAN_message_t msg;

//move BinaryBuffer into current namespace
using cmbtl::BinaryBuffer;


BinaryBuffer dataBuffer(24);
int bufferIndex = 0;
bool messageTransmitInProgress = false;
long long int sizeOfMessage = 0;
long long int bitsRecieved = 0;

//pass by reference cause gamur
bool decodeFromCan(BinaryBuffer& data, CAN_message_t& dataMsg) {
  if (!messageTransmitInProgress) {
    memcpy(&sizeOfMessage, &dataMsg.buf, 8);
  }
  if (dataMsg.buf[0] == 0b11111111 && dataMsg.buf[1] == 0b11111111 && dataMsg.buf[2] == 0b11111111 && dataMsg.buf[3] == 0b11111111 && dataMsg.buf[4] == 0b11111111 && dataMsg.buf[5] == 0b11111111 && dataMsg.buf[6] == 0b11111111 && dataMsg.buf[7] == 0b11111111) {
    return true;
  }
  for (int i = 0; i < 8; i += 1) {
    if (bitsRecieved >= sizeOfMessage) {
      break;
    }
    data.write(dataMsg.buf[i]);
    bitsRecieved += 8;
  }
  return false;
}

void setup(void) {
  can1.begin();
  can1.setBaudRate(250000);
  Serial.begin(9600);
}

void loop() {
  if(can1.read(msg)) {
    bool messageReady = decodeFromCan(dataBuffer, msg);
    if(messageReady) {
      for (unsigned long int i = 0; i < dataBuffer.getCapacity()/8; i+=1) {
        uint8_t placeholder = 0;
        for (int y = 0; y < 8; y += 1) {
          placeholder += dataBuffer.readBit() << y;
        }
        Serial.printf("%c\n", placeholder);
      }
    }
  }
}
