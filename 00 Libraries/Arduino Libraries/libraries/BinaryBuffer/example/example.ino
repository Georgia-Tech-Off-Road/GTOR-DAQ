#include <BinaryBuffer.h>

// To hammer home the point that big endian values must ABSOLUTELY be used, we are using boost's endian library
// However, boost's library is not built for Arduino, so we are using a port: https://github.com/Georgia-Tech-Off-Road/BoostForArduino/tree/master
// Thank you so much kekyo for developing the ProxyHeaderBuilder tool, this is a life saver!
// Please install this library according to instructions
#include <boost_endian_arithmetic.hpp>

using cmbtl::BinaryBuffer;
using RPM_type = boost::endian::big_int16_t;
// We create a struct to hold our RPM values using big-endian integers
struct RPM {
  RPM_type front;
  RPM_type rear;

  // For arguments sake let us say that we know RPM will not exceed 2^12 - 1 = 4095 = max value held by uint16_t
  // We now define encode and decode functions for the struct

    // Default constructor
    RPM() : front(0), rear(0) {}

    RPM (RPM_type front, RPM_type rear) {
      this->front = front;
      this->rear = rear;
    }

    // Write values to buffer
    void encodeRPM(BinaryBuffer &buffer) {
      //Write only 12 bits for each axis
      buffer.writeValue(front, 12);
      buffer.writeValue(rear, 12);
    }

    // Update values from buffer
    void decodeRPM(BinaryBuffer const &buffer) {
      front = buffer.readValue<RPM_type>(12);
      rear = buffer.readValue<RPM_type>(12);
    }

    // Helper function to print RPM values
    void printRPM() {
      Serial.print("Front RPM: ");
      Serial.print(front);
      Serial.print("\nRear RPM: ");
      Serial.println(rear);
    }
};

void setup() {
  Serial.begin(9600);
}

void loop() {

  // We create a binary buffer that can hold 24 bits
  BinaryBuffer transmissionBuffer(24);

  // Set a new RPM for each time
  RPM carRPM(random(0, 4096), random(0, 4096));

  Serial.println("RPM Before Transmission:");
  carRPM.printRPM();
  carRPM.encodeRPM(transmissionBuffer);

  // Pretend that we sent this somehow

  RPM newCarRPM;

  newCarRPM.decodeRPM(transmissionBuffer);
  Serial.println("RPM After Transmission:");
  newCarRPM.printRPM();

}
