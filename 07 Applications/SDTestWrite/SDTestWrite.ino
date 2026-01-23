/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

#include <SPI.h>
#include <SD.h>

File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  int SDCARDINIT = SD.begin(BUILTIN_SDCARD);
  Serial.printf("SD Card init status: %d\n",SDCARDINIT);
  if(!SDCARDINIT) {
    Serial.printf("SD FAILED TO INIT\n");
  } else {
    Serial.printf("SD INITIALIZED\n");
  }

  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void loop() {
  // nothing happens after setup
}

/*#include <SD.h>
#include <TimeLib.h>

File test_file;

void setup() {
  // put your setup code here, to run once:
  initSD();

  // Write to file
  String time =  String(year()) + "-" + String(month()) + "-" + String(day()) + " " + String(hour()) + "_" + String(minute()) + "_" + String(second());
  test_file = SD.open(String("/test_" + time + ".txt").c_str());
  if (test_file) {
    test_file.printf("Wrote test file at %s", time);
    test_file.close();
    Serial.printf("Test file created!");
  } else {
    Serial.printf("Error creating test file.");
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}


// Initialize SD card
inline void initSD() {
  int SDCARDINIT = SD.begin(BUILTIN_SDCARD);
  Serial.printf("SD Card init status: %d\n",SDCARDINIT);
  if(!SDCARDINIT) {
    Serial.printf("SD FAILED TO INIT\n");
  } else {
    Serial.printf("SD INITIALIZED\n");
  }
}
  
/*  SD.mkdir(time.c_str());
  Serial.println(time.c_str());
  File structConfigFile = SD.open(String("/"+time+"/"+time+"Config.txt").c_str(), FILE_WRITE);
  writeStructData(structConfigFile);
  structConfigFile.close();
  if (binary) {
    outputFile = SD.open(String("/"+time+"/"+time+".bin").c_str(),  FILE_WRITE);
  } else {
    outputFile = SD.open(String("/"+time+"/"+time+".txt").c_str(),  FILE_WRITE);
    outputFile.printf("[\n");
  } */
