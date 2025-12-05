#include <cuda_runtime.h>
#include <tuple>
#include <cstdio>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#define CHECK_CUDA(call) do { \
    cudaError_t err = call; \
    if(err != cudaSuccess) { \
        printf("CUDA error at %s:%d: %s\n", __FILE__, __LINE__, cudaGetErrorString(err)); \
        exit(err); \
    } \
} while(0)


__constant__ float convolutionKernel[99];


__global__ void spikeFilterer(float* data, float * globalData, size_t length, int kernelLength, float percentDiff) {
    //allocate ze shared memory
    extern __shared__ float sharedMemBuffer[];

    //calculate ze thread index
    int threadID = blockDim.x * blockIdx.x + threadIdx.x;

    //load ze data into ze shared memory
    sharedMemBuffer[threadIdx.x] = data[threadID];
    //load one extra value for right side of block
    if(threadIdx.x == blockDim.x - 1 && threadID + 1 < length) {
        sharedMemBuffer[threadIdx.x + 1] = data[threadID + 1];
    }

    //sync ze threads
    __syncthreads();

    //calculate ze slopes (if last item then set slope to 1)
    float mySlope = 0;
    if (threadID != length - 1) {
        mySlope = (sharedMemBuffer[threadIdx.x + 1] -  sharedMemBuffer[threadIdx.x])/2;
    } else {
        mySlope = 1;
    }

    //save each threads data to a register for later
    float myData = sharedMemBuffer[threadIdx.x];

    //write ze slopes to ze global memory
    globalData[threadID] = mySlope;

    //sync ze threads
    __syncthreads();
    
    //write ze slopes to ze shared memory
    for(int i = 0; i < blockDim.x + kernelLength - 1 - threadIdx.x; i += blockDim.x) {
        //do it this way to prevent underflow
        if (threadID >= kernelLength/2 + i) {
            sharedMemBuffer[threadIdx.x + i] = globalData[threadID - kernelLength/2 + i];
        } else {
            sharedMemBuffer[threadIdx.x + i] = 0;
        }
    }

    //sync ze threads
    __syncthreads();

    //compute ze convolution
    float localVar = 0;
    for(int i = 0; i < kernelLength; i +=1) {
        localVar += sharedMemBuffer[threadIdx.x + i] * convolutionKernel[i];
    }

    //write ze data to ze global memory
    globalData[threadID] = localVar;

    //sync ze threads
    __syncthreads();
    
    //load ze pertinent data into ze shared memory (need num block elements + one for the slope at the end)
    for (int i = 0; i <= blockDim.x + 1 - threadIdx.x; i += blockDim.x) {
        //shift the global memory accesses down one so we have surrounding slopes available
        if(threadID >= 1 && threadID < length + 1) {
            sharedMemBuffer[i + threadIdx.x] = globalData[threadID + i - 1];
        } else {
            sharedMemBuffer[i + threadIdx.x] = 0;
        }
    }
    //sync ze threads
    __syncthreads();
    //compare ze slopes and set ze new data
    //logic for this is as follows, each data point has the slope of (data[threadId + 1] - data[threadId])/2 saved to sharedMemBuffer[threadIdx.x]
    //therefore if the slopes are outside the proper region, you need to set the data point after the thread to the threads data
    //offset of +1 since that's how we saved to shared mem
    if (threadID != 0 && threadID != length - 1) {
        if(sharedMemBuffer[threadIdx.x + 1] > sharedMemBuffer[threadIdx.x] * percentDiff) {
            data[threadID+1] = myData;
        }
    }
}
namespace RPMSpikeFilterFunctions {
    std::tuple<int, int> calculateSharedMemoryNeeded(cudaDeviceProp prop, int kernelLength) {
        //save maxSharedMem to a variable
        int maxSharedMem = prop.sharedMemPerBlock;
        //walk through all the possibilites starting from max
        for (int blockSize = prop.maxThreadsPerBlock; blockSize >= 0; blockSize -=1) {
            int sharedMemoryAmount = (blockSize + kernelLength - 1) * sizeof(float);

            if (maxSharedMem >= sharedMemoryAmount) {
                
                return std::make_tuple(blockSize, sharedMemoryAmount);
            }
        }
        //if this return ever fires thhere's problems
        return std::make_tuple(0, 0);
    }
}

void spikeFiltererHandler(float * data, size_t length, float* kernel, int kernelLength, float percentDiff, int timesToRun) {
    //copy kernel into the constant data
    CHECK_CUDA(cudaMemcpyToSymbol(convolutionKernel, kernel, kernelLength * sizeof(float)));

    //get GPU stats
    int device;
    CHECK_CUDA(cudaGetDevice(&device));

    cudaDeviceProp prop;
    CHECK_CUDA(cudaGetDeviceProperties(&prop, device));
    
    //allocate a device pointer for the data
    float * d_data;
    //malloc space for the data
    CHECK_CUDA(cudaMalloc((void**)&d_data, length * sizeof(float)));
    //copy the data over
    CHECK_CUDA(cudaMemcpy(d_data, data, length * sizeof(float), cudaMemcpyHostToDevice));
    //allocate a device pointer for its intermediary calculations
    float * d_intermediary;
    //malloc space for the data
    CHECK_CUDA(cudaMalloc((void**)&d_intermediary, length * sizeof(float)));
    //calculate num threads per block
    auto sharedMemoryCalculatorTupleReturn = RPMSpikeFilterFunctions::calculateSharedMemoryNeeded(prop, kernelLength);
    int blockSize;
    int sharedMemAmount;
    std::tie(blockSize, sharedMemAmount) = sharedMemoryCalculatorTupleReturn;
    for (int i = 0; i < timesToRun; i += 1) {
        //may cause a problem if a GPU can't launch this many threads
        spikeFilterer<<<(length + blockSize - 1)/blockSize, blockSize, sharedMemAmount>>>(d_data, d_intermediary, length, kernelLength, percentDiff);
        // Check for errors
        cudaError_t err = cudaGetLastError();
        if (err != cudaSuccess) printf("Launch error: %s\n", cudaGetErrorString(err));

        err = cudaDeviceSynchronize();
        if (err != cudaSuccess) printf("Execution error: %s\n", cudaGetErrorString(err));
    }
    CHECK_CUDA(cudaFree(d_intermediary));
    CHECK_CUDA(cudaMemcpy(data, d_data, length * sizeof(float), cudaMemcpyDeviceToHost));
    CHECK_CUDA(cudaFree(d_data));
}

//translates numpy stuff
void pythonTranslator(pybind11::array_t<float> pythonData, pybind11::array_t<float> kernel, float percentDiff, int timesToRun) {
    auto dataBuff = pythonData.request();
    auto kernelBuff = kernel.request();
    spikeFiltererHandler(static_cast<float*>(dataBuff.ptr), dataBuff.size, static_cast<float*>(kernelBuff.ptr), kernelBuff.size, percentDiff, timesToRun);
}

//special pybind11 macro for this to define the thing
PYBIND11_MODULE(RPMSpikeFilter, m) {
    m.def("run", &pythonTranslator);
}