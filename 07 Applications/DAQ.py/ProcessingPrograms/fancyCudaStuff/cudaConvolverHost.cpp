#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <filesystem>
#include "rapidjson-master/include/rapidjson/reader.h"
#include "rapidjson-master/include/rapidjson/filereadstream.h"
#include "rapidjson-master/include/rapidjson/filewritestream.h"
#include "rapidjson-master/include/rapidjson/writer.h"
#include "rapidjson-master/include/rapidjson/document.h"

//declare the colveData function from the cuda obj thing
extern "C" void convolveData(float* data, size_t length, float* kernel, int kernelLength);
//declare the spikeFiltererHandler
extern "C" void spikeFiltererHandler(float * data, size_t length, float* kernel, int kernelLength, float percentDiff, int timesToRun);


//it just works, thank you Sam Altman
extern "C" {

// Structure to hold intermediate state for parsing
struct JSONParserContext {
    const char** keys;
    size_t numKeys;
    float** outputs;       // float* array, one per key
    size_t* counts;        // number of elements collected per key
    size_t* capacity;      // current capacity per key
    const char* currentKey;
    bool inObject;
};

// RapidJSON SAX handler
struct SAXHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, SAXHandler> {
    JSONParserContext* ctx;

    SAXHandler(JSONParserContext* c) : ctx(c) {}

    bool StartObject() { ctx->inObject = true; return true; }
    bool EndObject(rapidjson::SizeType) { ctx->inObject = false; return true; }

    bool Key(const char* str, rapidjson::SizeType length, bool) {
        ctx->currentKey = str;
        return true;
    }

    void appendValue(float val) {
        for (size_t i = 0; i < ctx->numKeys; ++i) {
            if (strcmp(ctx->keys[i], ctx->currentKey) == 0) {
                // allocate/reallocate if needed
                if (ctx->counts[i] >= ctx->capacity[i]) {
                    size_t newCap = ctx->capacity[i] ? ctx->capacity[i] * 2 : 128;
                    ctx->outputs[i] = (float*)realloc(ctx->outputs[i], newCap * sizeof(float));
                    ctx->capacity[i] = newCap;
                }
                ctx->outputs[i][ctx->counts[i]++] = val;
                break;
            }
        }
    }

    bool Double(double d) { appendValue(static_cast<float>(d)); return true; }
    bool Int(int i) { appendValue(static_cast<float>(i)); return true; }
    bool Uint(unsigned u) { appendValue(static_cast<float>(u)); return true; }
    bool Null() { appendValue(0.0f); return true; }  // treat null as 0
};

// C-style function
void parse_json_buffer(char* buffer, size_t bufferSize,
                       const char** keys, size_t numKeys,
                       float** outputs, size_t* lengths)
{
    JSONParserContext ctx = {};
    ctx.keys = keys;
    ctx.numKeys = numKeys;
    ctx.outputs = outputs;
    ctx.counts = (size_t*)calloc(numKeys, sizeof(size_t));
    ctx.capacity = (size_t*)calloc(numKeys, sizeof(size_t));
    ctx.currentKey = nullptr;
    ctx.inObject = false;

    // Initialize outputs to nullptr
    for (size_t i = 0; i < numKeys; ++i) {
        outputs[i] = nullptr;
        lengths[i] = 0;
    }

    rapidjson::MemoryStream ms(buffer, bufferSize);
    rapidjson::Reader reader;
    SAXHandler handler(&ctx);

    reader.Parse(ms, handler);

    // Shrink allocations to fit
    for (size_t i = 0; i < numKeys; ++i) {
        if (outputs[i]) {
            outputs[i] = (float*)realloc(outputs[i], ctx.counts[i] * sizeof(float));
        }
        lengths[i] = ctx.counts[i];
    }

    free(ctx.counts);
    free(ctx.capacity);
    free(buffer);  // free the input buffer
}

} // extern "C"

//more chatgpt special
void write_json_with_convolved(
    const char* inputFile,
    const char* outputFile,
    const char** keys,
    float** outputs,
    size_t* lengths, int numKeys)
{
    // Open input file
    FILE* fp = std::fopen(inputFile, "r");
    if (!fp) { perror("fopen input"); return; }

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    printf("Randomdebug1\n");
    // Parse JSON as a DOM
    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    if (!doc.IsArray()) {
        fprintf(stderr, "Input JSON is not an array!\n");
        return;
    }
    printf("Randomdebug2\n");
    // Track the index in outputs for each key
    std::vector<size_t> indices(numKeys, 0);
    printf("Randomdebug3\n");
    for (rapidjson::SizeType i = 0; i < doc.Size(); ++i) {
        auto& obj = doc[i];
        if (!obj.IsObject()) continue;
        for (size_t k = 0; k < numKeys; ++k) {
            const char* key = keys[k];
            if (!obj.HasMember(key)) {
                printf("Missing key: %s\n", key);
            }
            if (obj.HasMember(key) && indices[k] < lengths[k]) {
                obj[key].SetFloat(outputs[k][indices[k]++]);
            }
        }
    }

    // Write back to output file
    printf("outputFile: %s\n", outputFile);
    FILE* ofp = std::fopen(outputFile, "w");
    if (!ofp) { perror("fopen output"); return; }

    char writeBuffer[65536];
    rapidjson::FileWriteStream os(ofp, writeBuffer, sizeof(writeBuffer));
    rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
    doc.Accept(writer);

    fclose(ofp);
}

int main(int argc, char* argv[]) {
    if (argc < 6 || (argc - 4)%2 != 0) {
        std::cerr << "Usage : jsonfileName jsonOutputName jsonDataName, convolution kernel ALSO YOU MUST HAVE A KERNEL STRING FOR EVERY SENSOR NAME";
        return 1;
    }
    char ** sensorNameStrings = (char**)malloc(sizeof(char*) * (argc-4)/2);
    int numJsonSensors = 0;

    for (int i = 4; i < argc; i +=2) {
        sensorNameStrings[(i-4)/2] = argv[i];
        numJsonSensors += 1;
    }
    //open the file
    std::ifstream inputJsonFile;
    inputJsonFile.open(argv[1]);

    //get the file size
    size_t fileSize = std::filesystem::file_size(argv[1]);

    //allocate a buffer to hold the entire file
    char* buffer = (char*)malloc(fileSize);

    //read all the items into the file
    inputJsonFile.read(buffer, fileSize);

    //create float** pointers to hold the data output
    //lists will be output from the json thing in the order that they were fed in
    float ** floatLists = (float**)malloc(sizeof(float*) * numJsonSensors);

    //lists of how long the float lists are (should be all the same but I guess you never know?)
    size_t * lengths = (size_t *)malloc(numJsonSensors * sizeof(size_t));
    
    //we treat everything as floats since that's generally what we save stuff as, if this becomes a problem add a flag to let people use doubles
    //activate ze chatgpt
    parse_json_buffer(buffer, fileSize, (const char**)sensorNameStrings, numJsonSensors, floatLists, lengths);

    printf("json parsed!\n");
    //pull out kernels and calculate their lengths

    int* kernelLengths = (int*)malloc(numJsonSensors * sizeof(int));
    float **kernels = (float**)malloc(numJsonSensors * sizeof(float*));

    for (int i = 5; i < argc; i +=2) {
        //allocate an array of massive size in case someone wants to do a stupidly large kernel
        float* kernel = (float*)malloc(9999999 * sizeof(float));
        //create a char list for current number
        char* currentNumber = (char*)malloc(500 * sizeof(char));
        //create a counter for the while loop
        int counter = 0;
        int currentNumberCounter = 0;
        //create a kernelCounter
        int kernelCounter = 0;
        while(true) {
            char charOfArgv = argv[i][counter];
            if(charOfArgv == '[') {
                counter += 1;
                continue;
            } else if (charOfArgv ==']') {
                currentNumber[currentNumberCounter] = '\0';
                kernel[kernelCounter] = std::atof(currentNumber);
                kernelCounter += 1;
                break;
            } else if (charOfArgv == ',') {
                currentNumber[currentNumberCounter] = '\0';
                kernel[kernelCounter] = std::atof(currentNumber);
                kernelCounter += 1;
                currentNumberCounter = 0;
            } else {
                currentNumber[currentNumberCounter] = charOfArgv;
                currentNumberCounter += 1;
            }
            counter += 1;
        }
        //allocate a proper size kernel
        float* finalKernel = (float*)malloc(kernelCounter * sizeof(float));
        memcpy(finalKernel, kernel, kernelCounter * sizeof(float));
        kernels[(i-4)/2] = finalKernel;
        kernelLengths[(i-4)/2] = kernelCounter;
        //free all the things
        free(kernel);
        free(currentNumber);
    }
    if (argv[3] == "convolve" ) {
        //perform convolutions on the data
        printf("NUMSENSORS %d\n", numJsonSensors);
        for (int i = 0; i < numJsonSensors; i += 1) {
            printf("Calling GPU function\n");
            convolveData(floatLists[i], lengths[i], kernels[i], kernelLengths[i]);
            printf("%s\n", sensorNameStrings[i]);
        }
    } else {
        float percentageJump = 0;
        //create a char list for current number
        char* currentNumber = (char*)malloc(500 * sizeof(char));
        int currentNumberCounter = 0;
        while(true) {
            char charOfArgv = argv[3][currentNumberCounter];
            if (charOfArgv == '\0') {
                currentNumber[currentNumberCounter] = '\0';
                percentageJump = std::atof(currentNumber);
                break;
            } else {
                currentNumber[currentNumberCounter] = charOfArgv;
                currentNumberCounter += 1;
            }
        }
        free(currentNumber);
        for (int i = 0; i < numJsonSensors; i += 1) {
            printf("Calling GPU function\n");
            spikeFiltererHandler(floatLists[i], lengths[i], kernels[i], kernelLengths[i], percentageJump, 100);
            printf("%s\n", sensorNameStrings[i]);
        }
    }
    //write the output
    write_json_with_convolved(argv[1], argv[2], (const char**)sensorNameStrings, floatLists, lengths, numJsonSensors);

    //free everything
    for (int i = 0; i < numJsonSensors; i +=1) {
        free(floatLists[i]);
        free(kernels[i]);
        free(sensorNameStrings[i]);
    }
    free(kernelLengths);
    free(kernels);
    free(lengths);
    free(floatLists);
    free(sensorNameStrings);
    return 0;
}