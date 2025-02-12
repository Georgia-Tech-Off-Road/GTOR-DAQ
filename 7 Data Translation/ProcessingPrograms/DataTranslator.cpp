#include "DataTranslator.h"

using namespace std;
// Personal coding conventions for C++
// I use snake_casing for basic types, char, int, double, etc.
// I use camelCasing for classes and booleans

// If sensor layout changes, you can make a new struct and modfiy this alias. Orrr (more dangerously) modify the current struct
using Sensor = DAQSensor;

//  ---GLOBALS---

// File IO
string inputFileName;
string outputFileName;
string configFileName;

ifstream in_file;
ofstream out_file;
ifstream config_file;

// MISC
bool verbose_flag = false;
std::vector<Sensor> sensors;

int main(int argc, char *argv[]) {
    LOG_INFO("You have entered %d cmd line arguments!\n", argc);

    processCLIArgs(argc, argv);
    openFiles();

    readConfigFile(&config_file);

    readInputFile(&in_file);



    // Before function termination
    in_file.close();
    out_file.close();
    config_file.close();
    return 0;
}

void readConfigFile(ifstream *cf) {
    const int CONFIG_BUFFER_SIZE = 70;
    // Stores the text of ONE line from the file
    char line_buffer[CONFIG_BUFFER_SIZE];

    // Get rid of header
    cf->getline(line_buffer, CONFIG_BUFFER_SIZE);

    while (cf->getline(line_buffer, CONFIG_BUFFER_SIZE)) {
        // Declare an empty Sensor
        Sensor currSensor;

        // Initialize the sensor given the options specified in the line
        currSensor.initializeSensor(line_buffer);

        // Push to back of vector
        sensors.push_back(currSensor);
    }

    for (Sensor sensor : sensors) {
        cout << sensor.toStr().c_str();
    }
}

// Self explanatory function name
void processCLIArgs(int argc, char *argv[]) {
    // Flags
    bool assignedInput = false;
    bool assignedOutput = false;
    bool assignedConfig = false;

    // Get input and output file names
    for (int i = 1; i < argc; i++) {
        if (i < argc - 1) {
         if (((string)"-i").compare(argv[i]) == 0) {
             inputFileName.assign(argv[i + 1]);
              assignedInput = true;
           } else if (((string)"-o").compare(argv[i]) == 0) {
               outputFileName.assign(argv[i + 1]);
             assignedOutput = true;
          }
            else if (((string)"-c").compare(argv[i]) == 0) {
                configFileName.assign(argv[i + 1]);
                assignedConfig = true;
            }
        }
        else if (((string)"-v").compare(argv[i]) == 0) {
            verbose_flag = true;
        }
    }
    if (!assignedInput || !assignedOutput || !assignedConfig) {
        throw std::invalid_argument("You must provide an input and output file in this format: ./dataprocess -i input.txt -o output.csv -c config.txt\n");
    }
    LOG_INFO("Verbose: %s\n", verbose_flag ? "true" : "false");
    LOG_INFO("Running dataprocess with following parameters:\n");
    LOG_INFO("Input file: %s\n", inputFileName.c_str());
    LOG_INFO("Output file: %s\n", outputFileName.c_str());
    LOG_INFO("Config file name: %s\n", configFileName.c_str());
}

// Opens input, output, and config files
void openFiles() {
    // Open input file in read mode
    in_file.open(inputFileName, std::ios_base::in);
    // Open output file in write mode
    out_file.open(outputFileName, std::ios_base::out);
    // Open config file in read mode
    config_file.open(configFileName, std::ios_base::in);

    // Check to see if input file is open
    if (!in_file.is_open()) {
        LOG_ERROR("Input file %s could not be opened!", inputFileName.c_str());
    }

    // Check to see if output file is open
    if (!out_file.is_open()) {
        LOG_ERROR("Output file %s could not be opened!", outputFileName.c_str());
    }

    // Check to see if config file is open
    if (!config_file.is_open()) {
        LOG_ERROR("Config file %s could not be opened!", configFileName.c_str());
    }
}

void readInputFile(std::ifstream *inf) {
    char line_buffer[LINE_BUFFER_SIZE];
    // While getline() does not result in an error (i.e end of file), keep adding result of line to line_buffer
    while (inf->getline(line_buffer, LINE_BUFFER_SIZE)) {
        processInputLine(line_buffer);
    }
    LOG_INFO("Finished reading file!");
}

void processInputLine(char* line_buffer) {
    constexpr size_t INPUT_BUFFER_SIZE = 25;
    char line_segment[INPUT_BUFFER_SIZE];
    Time time = getTimeFromLine(line_buffer);
    for (int i = 0; i < (int) sensors.size(); i++) {
        snprintf(line_segment, INPUT_BUFFER_SIZE, strtok(i == 0 ? line_buffer : NULL, ", " + '\n'));;
        if(line_segment != nullptr) {
            sensors[i].convertLine(line_segment, &time);
        } else {
            LOG_INFO("Line segment is null on line: %s", line_buffer);
            throw std::invalid_argument("Strtok on line segment is null");
        }
    }
}

cvf::Time getTimeFromLine(char* line) {
    cvf::Time time;
    static int secSensorIndex = -1;
    static int milliSecSensorIndex = -1;
    //If we have not already found the index of the time sensors, do so
    if (secSensorIndex == -1 || milliSecSensorIndex == -1) {
        for(int i = 0; i < sensors.size(); i++) {
            if (sensors[i].getSensorName().find("millisec") != std::string::npos) {
                milliSecSensorIndex = i;
            } else if (sensors[i].getSensorName().find("sec") != std::string::npos) {
                secSensorIndex = i;
            }
        }
        if (secSensorIndex == -1 || milliSecSensorIndex == -1) {
            throw std::invalid_argument("A second and millisec \"sensor\" could not be found in the config file. Please may sure their titles includes these tokens.");
        }
    }
    char line_segment[25];
    for (int i = 0; i < max(secSensorIndex, milliSecSensorIndex); i++) {
        snprintf(line_segment, 25, strtok(i > 0 ? NULL : line, ","));
        if (i == secSensorIndex) {
            //Converts line_segment to unsigned long, then to uint32_t
            time.sec = static_cast<uint32_t>(std::stoul(line_segment));
        }
        else if (i == milliSecSensorIndex) {
            //Converts line_segment to unsigned long, then to uint32_t
            time.milli = static_cast<uint32_t>(std::stoul(line_segment));
        }
    }
    return time;
}