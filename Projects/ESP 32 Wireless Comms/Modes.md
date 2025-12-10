## Data Transfer Mode
Master: Teensy 4.1 / Collection Microcontroller
Subservient: ESP 32 / Communication Microcontroller

### Overview
This mode is the standard mode during vehicle operation. The ESP32 signals to the main microcontroller when it is available to receive data, however, the main microcontroller is in charge of when in receives. The main microcontroller writes to the SD card and runs conversion functions for its data.

## Configuration Mode
Master: ESP 32 / Communication Microcontroller
Subservient: Teensy 4.1 / Collection Microcontroller

### Overview
This mode is used to initialize teensy values (e.g sensor indices, conversion functions, etc.) and to modify values while running. 
  
**Example Use Case:**
While the vehicle is in the middle of testing, DAQ would like to see what sensors are mapped to which ports. The ESP32 signals to the Teensy 4.1 to enter command mode and issues the "list sensors" command.