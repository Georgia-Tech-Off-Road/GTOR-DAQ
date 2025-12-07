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




__device__ processStructs(int structSize, int numItems, int outputSize, bool firstTime, char* structDataConfig, char* structNameConfig, char* inputStructs, char* outputData, int* sizeOfStructs) {
    int index = blockIdx.x * blockDim.x + threadIdx.x;

    //pull out the individual struct
    char * structData = inputStructs + structSize * index;

    char* outputChar = (char*)malloc(outputSize);
    //need to keep track of original pointer position
    char* returnPointer = outputChar;

    //add in the commas
    if (index != 0 || firstTime == false) {
        *outputChar = ',';
        outputChar += 1;
    }

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
    *outputChar = '\n';

    //copy output data into the 
    memcpy(&outputData[index  * outputSize], returnPointer, outputSize);
    //write to the size of structs output thing
    sizeOfStructs[index] = outputChar - returnPointer;
}

__device__ generateFinalOutput(char* intermediateOutput, int intermediateOutputSize, int* sizeOfStructs, char* finalOutput) {
    int index  = blockDim.x * blockIdx.x + threadIdx.x;
    int sizeOfThisJsonOutput = sizeOfStucts[index];
    int offsetPointer = 0;
    for (int i = 0; i < index; i += 1) {
        offsetPointer += sizeOfStructs[i];
    }
    memcpy(finalOutput + offsetPointer, intermediateOutput + intermediateOutputSize * index, sizeOfThisJsonOutput);
}


int mainProgramThing(std::string binFileName, std::string outputFilePath, char* structDataConfig, char* structNameConfig) {
    int structSize = std::atoi(structDataConfig);
    
    std::ifstream binFile;
    std::ofstream jsonFile;

    binFile.open(binFileName, std::ios::binary);
    jsonFile.open(outputFilePath);

    jsonFile << "[";

    //need to get the GPU free memory size
    size_t free_bytes;

    cudaError_t status = cudaMemGetInfo(&free_bytes);
    if (status != cudaSuccess) {
        std::cerr << "cudaMemGetInfo failed: " << cudaGetErrorString(status) << std::endl;
        return 1;
    }
    
    //calculate the necessary size information
    int structSize, numItems;
    sscanf(structDataConfig, "%d-%d", &structSize, &numItems);
    //move past numbers
    structDataConfig = strchr(structDataConfig, '-') + 1;      
    structDataConfig = strchr(structDataConfig, '-') + 1;
    int nameSize = std::atoi(structNameConfig);
    //increment pointer to move beyond
    structNameConfig += 4;
    //calcuate size needed for output json (two " per name, plus one {} each, plus one comma per each name, plus one colon per each name, plus structSize
    int outputSize =  nameSize + numItems * 40 + numItems*4 + 2;

    //calculate num items to use
    int numItems = (free_bytes) / (structSize + outputSize);
    //give a nice buffer to make space for the other stuff I need to copy in
    numItems = numItems - 100;

    char* gpuBuffer = (char*)malloc(structSize  * numItems);
    char* buffer = (char*)malloc(structSize);

    if (!binFile.is_open()) {
        std::cerr << "Failed to open file: " << binFileName << "\n";
        return -1;
    }

    char* comparisonStruct = (char*)malloc(structSize - 12);
    bool firstTime = true;

    int structsProcessed = 0;

   while(true) {
        if (structsProcessed == numItems) {
            //launch kernel here
            //create pointers for GPU data
            char* d_gpuBuffer;
            char* d_gpuOutput;
            int* d_sizeOfStructs;
            char* d_structDataConfig;
            char* d_structNameConfig;
        }
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
            //copy into buffer
            memcpy(gpuBuffer + structsProcessed * structSize, buffer, structSize);
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