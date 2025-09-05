#include <SensorData.h>
#include <DAQInfrastructure/DAQSensors.h>
#include <BinaryBuffer.h>
#include <DAQInfrastructure/DAQPackets.h> 

int currentTime = 0;
void setup() {
  Serial.begin(9600);
}

void loop() {
  cmbtl::DAQSensorDataType data;
  cmbtl::DAQSensorDataType data2;
  // Update time
  data.setData<cmbtl::MILLI_SEC>(currentTime);
  // Encode packet
  BinaryBuffer buffer = data.encodePacket(cmbtl::packet::all_packet);
  // Pretend to transmit

  // ----------------------
  
  // Now Decode
  data2.decodePacket(cmbtl::packet::all_packet, buffer);

  // Now Print!
  Serial.print(data2.serializeDataToJSON().c_str());

  currentTime++;


}