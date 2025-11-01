#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <filesystem>
#include <cmath>

//may not handle padding well, if so modify to be able to do that
char*  printStruct(char* structData, char* structDataConfig, char* structNameConfig) {
    int structSize, numItems;
    sscanf(structDataConfig, "%d-%d", &structSize, &numItems);
    //move past numbers
    structDataConfig = strchr(structDataConfig, '-') + 1;      
    structDataConfig = strchr(structDataConfig, '-') + 1;
    //increment pointer to move beyond
    int nameSize = std::atoi(structNameConfig);
    //increment pointer to move beyond
    structNameConfig += 4;
    //calcuate size needed for output json (two " per name, plus one {} each, plus one comma per each name, plus one colon per each name, plus structSize
    
    int outputSize =  nameSize + numItems * 40 + numItems*4 + 2;

    char* outputChar = (char*)malloc(outputSize);
    //need to keep track of original pointer position
    char* returnPointer = outputChar;

    *outputChar = '{';
    outputChar += 1;

    auto writeName =  [&] {
        *outputChar = '\"';
        outputChar += 1;
        while(*structNameConfig != '-' && *structNameConfig != '\0') {
            //printf("%c\n", *structNameConfig);
            *outputChar = *structNameConfig;
            outputChar += 1;
            structNameConfig += 1;
        }
        //get past the - so the next one works
        structNameConfig += 1;
        *outputChar = '\"';
        outputChar += 1;
        *outputChar = ':';
        outputChar += 1;
    };
    while(*structDataConfig != '\0') {
        writeName();
        //breakdown of what integers mean what
        //0: char
        //1: int
        //2: float
        //3: double
        //4: long int
        //5: long long int
        //6: unsigned long int
        //7: unsigned long long int
        switch (*structDataConfig - '0') {
            case 0:
                outputChar += sprintf(outputChar, "%c", *(char*)structData);
                structData += 1;
                break;
            case 1:
                outputChar += sprintf(outputChar, "%d", *(int*)structData);
                structData += 4;
                break;
            case 2: {
                    float zeFloat = *(float*)structData;
                    if (!std::isfinite(zeFloat)) {
                        outputChar += sprintf(outputChar, "null");
                    } else {
                        outputChar += sprintf(outputChar, "%g", zeFloat);
                    }
                    structData += 4;
                    break;
                }
            case 3: {
                    double zeDouble = *(double*)structData;
                    if (!std::isfinite(zeDouble)) {
                        outputChar += sprintf(outputChar, "null");
                    } else {
                        outputChar += sprintf(outputChar, "%g", zeDouble);
                    }
                    structData += 8;
                    break;
                }
            case 4:
                outputChar += sprintf(outputChar, "%ld", *(long int*)structData);
                structData += 4;
                break;
            case 5:
                outputChar += sprintf(outputChar, "%lld", *(long long int*)structData);
                structData += 8;
                break;
            case 6:
                outputChar += sprintf(outputChar, "%lu", *(unsigned long int*)structData);
                structData += 4;
                break;
            case 7:
                outputChar += sprintf(outputChar, "%llu", *(unsigned long long int*)structData);
                structData += 8;
                break;
        }
        *outputChar = ',';
        outputChar += 1;
        structDataConfig += 1;

    }
    outputChar -= 1;
    *outputChar = '}';
    outputChar += 1;
    *outputChar = '\0';
    return returnPointer;
}


int mainProgramThing(std::string binFileName, std::string outputFilePath, char* structDataConfig, char* structNameConfig) {
    int structSize = std::atoi(structDataConfig);
    
    std::ifstream binFile;
    std::ofstream jsonFile;

    binFile.open(binFileName, std::ios::binary);
    jsonFile.open(outputFilePath);

    jsonFile << "[";

    char* buffer = (char*)malloc(structSize);

    if (!binFile.is_open()) {
        std::cerr << "Failed to open file: " << binFileName << "\n";
        return -1;
    }

    char* comparisonStruct = (char*)malloc(structSize - 12);
    bool firstTime = true;

    int structsProcessed = 0;

    while(true) {
        binFile.read(buffer, structSize);

        std::streamsize bytesRead = binFile.gcount();

        if (bytesRead != structSize) {
            if(structsProcessed * structSize == std::filesystem::file_size(binFileName)) {
                break;
            }
            std::cerr << "Warning: Expected " << structSize << " bytes, but read " << bytesRead << " bytes\n";
            break;
        } 

        //create pointer to allow for comparison (walk past the time stamps)
        char* comparisonPointer = buffer + 12;
        //if this ends up beings too slow switch to SIMD instructions (faster for large data (bout to 2-3x potential perf improvement I think))
        if (std::memcmp(comparisonPointer, comparisonStruct, structSize - 12) != 0) {
            char* jsonOutput = printStruct(buffer, structDataConfig, structNameConfig);
            if (!firstTime) {
                jsonFile << ",\n";
            }
            jsonFile << jsonOutput;
            free(jsonOutput);
            memcpy(comparisonStruct, comparisonPointer, structSize - 12);
            firstTime = false;
        }
        structsProcessed += 1;
    }
    jsonFile << "]";
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <binFileName> <outputFilePath> <structDataConfig> <structNameConfig>\n";
        return -1;
    }
    std::string binFileName = argv[1];
    std::string outputFilePath = argv[2];
    char* structDataConfig = argv[3];
    char* structNameConfig = argv[4];

    return mainProgramThing(binFileName, outputFilePath, structDataConfig, structNameConfig);
}