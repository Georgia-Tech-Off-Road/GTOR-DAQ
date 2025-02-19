#include "DataTranslator.h"
//Namespace qualifiers;
using std::string;
using std::ifstream;
using std::ofstream;
using std::cout;
using cvf::Time;


// Personal coding conventions for C++
// I use snake_casing for basic types, char, int, double, etc.
// I use camelCasing for classes and booleans

// If sensor layout changes, you can make a new struct and modfiy this alias. Orrr (more dangerously) modify the current struct
using Sensor = DAQSensor;

// MISC
bool verbose_flag = false;
std::vector<Sensor> sensors;

int main(int argc, char *argv[]) {
    //  ---Function Variables---

    // File IO
    string inputFileName;
    string outputFileName;
    string configFileName;

    ifstream in_file;
    ofstream out_file;
    ifstream config_file;

    LOG_INFO("You have entered %d cmd line arguments!\n", argc);

    processCLIArgs(argc, argv, &inputFileName, &outputFileName, &configFileName);
    openFiles(&in_file, inputFileName, &out_file, outputFileName, &config_file, configFileName);

    readConfigFile(&config_file);

    convertInputFile(&in_file, &out_file);



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
    if (sensors.size() == 0) {
        char msg[200];
        snprintf(msg, 200, "No sensors could be loaded from the config file!");
        throw std::invalid_argument(msg);
    }
}

// Self explanatory function name
void processCLIArgs(int argc, char *argv[], string *inputFileName, string *outputFileName, string *configFileName) {
    // Flags
    bool assignedInput = false;
    bool assignedOutput = false;
    bool assignedConfig = false;

    // Get input and output file names
    for (int i = 1; i < argc; i++) {
        if (i < argc - 1) {
         if (strcmp("-i", argv[i]) == 0) {
              inputFileName->assign(argv[i + 1]);
              assignedInput = true;
           } else if (strcmp("-o", argv[i]) == 0) {
               outputFileName->assign(argv[i + 1]);
             assignedOutput = true;
          }
            else if (strcmp("-c", argv[i]) == 0) {
                configFileName->assign(argv[i + 1]);
                assignedConfig = true;
            }
        }
        else if (strcmp("-v", argv[i]) == 0) {
            verbose_flag = true;
        }
    }
    if (!assignedInput || !assignedOutput || !assignedConfig) {
        throw std::invalid_argument("You must provide an input and output file in this format: ./dataprocess -i input.txt -o output.csv -c config.txt\n");
    }
    LOG_INFO("Verbose: %s\n", verbose_flag ? "true" : "false");
    LOG_INFO("Running dataprocess with following parameters:\n");
    LOG_INFO("Input file: %s\n", inputFileName->c_str());
    LOG_INFO("Output file: %s\n", outputFileName->c_str());
    LOG_INFO("Config file name: %s\n", configFileName->c_str());
}

// Opens input, output, and config files
void openFiles(ifstream *in_file, std::string inputFileName, ofstream *out_file, std::string outputFileName, ifstream *config_file, std::string configFileName) {
    // Open input file in read mode
    in_file->open(inputFileName.c_str(), std::ios_base::in);
    // Open output file in write mode
    out_file->open(outputFileName.c_str(), std::ios_base::out);
    // Open config file in read mode
    config_file->open(configFileName.c_str(), std::ios_base::in);

    // Check to see if input file is open
    if (!in_file->is_open()) {
        char msg[100];
        snprintf(msg, 100, "Input file %s could not be opened! %s", inputFileName.c_str(), std::strerror(errno));
        throw std::invalid_argument(msg);
    }

    // Check to see if output file is open
    if (!out_file->is_open()) {
        char msg[100];
        snprintf(msg, 100, "Output file %s could not be opened! %s", outputFileName.c_str(), std::strerror(errno));
        throw std::invalid_argument(msg);
    }

    // Check to see if config file is open
    if (!config_file->is_open()) {
        char msg[100];
        snprintf(msg, 100, "Config file %s could not be opened! %s", configFileName.c_str(), std::strerror(errno));
        throw std::invalid_argument(msg);
    }
}

void convertInputFile(std::ifstream *inf, std::ofstream *of) {
    auto startTime = std::chrono::system_clock::now();
    char line_buffer[LINE_BUFFER_SIZE];
    // While getline() does not result in an error (i.e end of file), keep adding result of line to line_buffer
    while (inf->getline(line_buffer, LINE_BUFFER_SIZE)) {
        processInputLine(line_buffer, of);
    }
    auto endTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endTime - startTime;
    LOG_INFO("Finished converting file in %f sec!", elapsed_seconds.count());
}

void processInputLine(char* line_buffer, std::ofstream *of) {
    //A line segment is the token of chars that is bordered by commas, should correspond to a sensor
    //Set the length that each line_segment can occupy
    int constexpr LINE_SEGMENT_SIZE = 25;
    Time time = getTimeFromLine(line_buffer);
    char* line_segment;
    for (int i = 0; i < static_cast<int>(sensors.size()); i++) {
        line_segment = strtok(i > 0 ? NULL : line_buffer, ",");
        //Check to see if token is null
        if (line_segment == nullptr) {
            char msg[200];
            snprintf(msg, 200, "There are more sensors specified in the config than their are columns in your data file!");
            throw std::invalid_argument(msg);
        }
        if(line_segment[0] != '\0') {
            char* res = sensors[i].convertLineSegment(line_segment, &time);
            //Write the result of converting the line segment to the output file
            of->write(res, strlen(res));
            //Add a comma if not last data elem
            if(i != sensors.size() - 1) {
                of->put(',');
            }
        } else {    
            LOG_ERROR("Line segment is null on line: %s", line_buffer);
            throw std::invalid_argument("Strtok on line segment is null");
        }
    }
    of->put('\n');
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
            if (secSensorIndex != -1 && milliSecSensorIndex != -1) {
                break;
            }
        }
        if (secSensorIndex == -1 || milliSecSensorIndex == -1) {
            throw std::invalid_argument("A sec and millisec \"sensor\" could not be found in the config file. Please may sure their titles includes these tokens.");
        }
    }
    char line_buffer[LINE_BUFFER_SIZE];
    //Copy line into line_buffer so that strtok does not override original line
    snprintf(line_buffer, LINE_BUFFER_SIZE, "%s", line);

    char line_segment[25];
    for (int i = 0; i < std::max(secSensorIndex, milliSecSensorIndex) + 1; i++) {
        int result = snprintf(line_segment, 25, "%s", strtok(i > 0 ? NULL : line_buffer, ","));
        //Check for error!
        if (result < 0) {
            char msg[200];
            snprintf(msg, 100, "Line number %d: The line segment size is not enough for snprintf! %s", __LINE__, std::strerror(errno));
            throw std::logic_error(msg);
        }
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
