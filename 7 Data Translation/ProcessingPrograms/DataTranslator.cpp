#include "DataTranslator.h"

using namespace std;
//Personal coding conventions for C++
//I use snake_casing for basic types, char, int, double, etc.
//I use camelCasing for classes and booleans

//Enum for SensorType
enum DataType {digital = 0, analogue = 1};

//Define a struct that contains the basic data for each sensor
struct Sensor {
    int index;
    DataType dataType;
    std::string sensorName;
    int pollingRate;
    int numTeeth;
};

bool verbose_flag = false;
int main(int argc, char *argv[]) {
    LOG_INFO("You have entered %d cmd line arguments!\n", argc);
    //Get name of input and output files
    string inputFileName;
    string outputFileName;
    string configFileName;

    //Flags
    bool assignedInput = false;
    bool assignedOutput = false;
    bool assignedConfig = false;



    //Get input and output file names
    for (int i = 1; i < argc; i++) {
        if (i < argc - 1) {
         if(((string)"-i").compare(argv[i]) == 0) {
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
    if(!assignedInput || !assignedOutput || !assignedConfig) {
        fprintf(stderr, "You must provide an input and output file in this format: ./dataprocess -i input.txt -o output.csv -c config.txt\n");
        return -1;
    }
    LOG_INFO("Verbose: %s\n", verbose_flag ? "true" : "false");
    LOG_INFO("Running dataprocess with following parameters:\n");
    LOG_INFO("Input file: %s\n", inputFileName.c_str());
    LOG_INFO("Output file: %s\n", outputFileName.c_str());
    LOG_INFO("Config file name: %s\n", configFileName.c_str());

    //Open input file in read mode
    ifstream in_file(inputFileName, std::ios_base::in);

    //Open output file in read mode
    ofstream out_file(outputFileName, std::ios_base::out);

    //Open config file in read mode
    ifstream config_file(configFileName, std::ios_base::in);

    //Check to see if input file is open
    if(!in_file.is_open()) {
        LOG_ERROR("Input file %s could not be opened!", inputFileName.c_str());
    }

    //Check to see if output file is open
    if(!out_file.is_open()) {
        LOG_ERROR("Output file %s could not be opened!", outputFileName.c_str());
    }

    //Check to see if config file is open
    if(!config_file.is_open()) {
        LOG_ERROR("Config file %s could not be opened!", configFileName.c_str());
    }

    readConfigFile(&config_file);

    //Before function termination
    in_file.close();
    out_file.close();
    config_file.close();
    return 0;
}

void readConfigFile(ifstream *cf) {
    const int CONFIG_BUFFER_SIZE = 70;
    char line_buffer[CONFIG_BUFFER_SIZE];
    //Get rid of header
    (*cf).getline(line_buffer, CONFIG_BUFFER_SIZE);
    (*cf).getline(line_buffer, CONFIG_BUFFER_SIZE);
    LOG_INFO("Line from config file: %s\n", line_buffer);
    
}

