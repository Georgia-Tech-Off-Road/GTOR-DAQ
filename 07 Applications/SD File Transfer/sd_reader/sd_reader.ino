#include <SD.h>

// Use BUILTIN_SDCARD for Teensy 3.5, 3.6, 4.1
const int chipSelect = BUILTIN_SDCARD; 

void setup() {
  Serial.begin(115200);

  unsigned long start = millis();
  while (!Serial && (millis() - start < 3000));

  if (!SD.begin(chipSelect)) {
    Serial.println("ERROR: SD card initialization failed!");
    return;
  }

  Serial.println("SUCCESS: SD Card Ready.");
}

// Recursive function to list all files
void listFiles(File dir, String path) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;

    String fullPath = path + "/" + entry.name();

    if (entry.isDirectory()) {
      // Recurse into the directory
      listFiles(entry, fullPath);
    } else {
      Serial.print("FILE:");
      Serial.print(fullPath);
      Serial.print("|SIZE:");
      Serial.println(entry.size());
    }

    entry.close();
  }
}

// Send file byte by byte
void sendFile(String filename) {
  if (!SD.exists(filename.c_str())) {
    Serial.println("ERROR: File not found.");
    return;
  }

  File dataFile = SD.open(filename.c_str());

  if (dataFile) {
    Serial.println("START_TRANSFER");

    // Simple byte-by-byte transfer
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }

    dataFile.close();
    Serial.println("\nEND_TRANSFER");
  } else {
    Serial.println("ERROR: Could not open file.");
  }
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "LIST") {
      File root = SD.open("/");
      listFiles(root, "");
      root.close();
      Serial.println("END_LIST");
    } 
    else if (command.startsWith("GET ")) {
      String filename = command.substring(4);
      filename.trim();
      sendFile(filename);
    }
  }
}