# ADS 1256 Notes

## Reference Links

 - [Library Documentation](https://github.com/Georgia-Tech-Off-Road/GTOR-DAQ/blob/main/00%20Libraries/Arduino%20Libraries/libraries/ADS1256-main/extras/ADS1256_ArduinoLibrary_Documentation_20250529.pdf)
 - [Datasheet](https://www.ti.com/lit/ds/symlink/ads1256.pdf)

# Notes

- Can optimize between 30k samples per second (SPS) and 23 bits noise-free
- Communicates with SPI
- Programmable gain amplifier can be programmed to provide higher resolution for smaller input signals (datasheet p.g 16). For our purposes (V_ref = 2.5 V I believe, PGA should be 2)
- DRDY should go low when new data is available (interrupt) (datasheet p.g 26)
- Verify filting with figures 25 & 26 (datasheet p.g 28-29)
- For single ended mode negative terminal should be common ground connection (assuming AINCOM?) (library doc p.g 3)
- Recommended to use PGA setting closest to maximum measured voltage (doc p.g 3)
- p.g 6-12 of library documentation detail macros that help initialize common boards (Teensy microcontroller is on page 10)
- Potential issues with interrupt based approach (doc. p.g 32)
- convertSignedTo24BitToLong() macro converts the ADC reading to a long (32 bits) (doc. pg. 35)
- Use setMux() to set which analog signal to read (doc p.g 37)
- cycleSingle() probably not going to do what we want (p.g 40)
