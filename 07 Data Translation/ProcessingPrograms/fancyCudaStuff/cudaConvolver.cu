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

//note this code will break if the convolution is too massive 
__constant__ float convolutionKernel[9001];

__global__ void cudaConvolver(float* data, size_t length, int kernelLength) {
    extern __shared__ float sharedMemoryBuffer[];
    int threadId = blockDim.x *  blockIdx.x + threadIdx.x;

    bool doNothing  = false;
    
    if(threadId >= length) {
        doNothing = true;
    }
    //professor conversation notes
    //load into local registers
    //perform warp shuffle
    //push to shared memory outside boundaries

    //kernel size 7
    //halo halo halo warp(not in shared mem) halo halo halo 

    //__shfl_up(0xFFFFFFFF, localValue, stride);

    //warp size of 4
    //kernel size 3
    //OD ID ID ID ID OD

    //shuffle to sum in warp
    //atomic warp to sum warps in block
    //atomic add for blocks in grid

    if(!doNothing) {
        //load shared memory
        for (int i = 0; i < blockDim.x + kernelLength - 1 - threadIdx.x; i += blockDim.x) {
            int globalIndex = threadId - kernelLength/2 + i;
            if (globalIndex >= 0 && globalIndex < length) {
                sharedMemoryBuffer[i + threadIdx.x] = data[globalIndex];
            } else {
                sharedMemoryBuffer[i + threadIdx.x] = 0;
            }
        }
    }

    __syncthreads();

    if(!doNothing) {
        float localVar = 0;
        //actually compute the jawn
        for (int i = 0; i < kernelLength; i += 1) {
            localVar += convolutionKernel[i] * sharedMemoryBuffer[threadIdx.x + i];
        }
        data[threadId] = localVar;
    }
}

//slow version that uses global memory for massive kernels
__global__ void ridiculousConvolutionKernel(float* data, size_t length, float* kernel, int kernelLength, float* output) {
    //intialize register for local value
    float localValue = 0;
    //calculate thread index
    int threadID = blockDim.x * blockIdx.x + threadIdx.x;
    //check for if out of bounds
    if (threadID >= length) {
        return;
    }
    //compute the local value
    for (int i = 0; i < kernelLength; i += 1) {
        if ((threadID - kernelLength/2 + i) >= 0 && (threadID - kernelLength/2 + i) < length) {
            localValue += __ldg(&kernel[i]) * __ldg(&data[threadID - kernelLength/2 + i]);
        }
    }
    //write output
    output[threadID] = localValue;
}

namespace cudaConvolverFunctions {
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

void convolveData(float * data, size_t length, float* kernel, int kernelLength) {
    //this is what is used in 99.99999% of cases
    if (kernelLength <= 9001) {
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

        //calculate num threads per block
        auto sharedMemoryCalculatorTupleReturn = cudaConvolverFunctions::calculateSharedMemoryNeeded(prop, kernelLength);
        int blockSize;
        int sharedMemAmount;
        std::tie(blockSize, sharedMemAmount) = sharedMemoryCalculatorTupleReturn;
        
        //may cause a problem if a GPU can't launch this many threads
        cudaConvolver<<<(length + blockSize - 1)/blockSize, blockSize, sharedMemAmount>>>(d_data, length, kernelLength);
        // Check for errors
        cudaError_t err = cudaGetLastError();
        if (err != cudaSuccess) printf("Launch error: %s\n", cudaGetErrorString(err));

        err = cudaDeviceSynchronize();
        if (err != cudaSuccess) printf("Execution error: %s\n", cudaGetErrorString(err));
        CHECK_CUDA(cudaMemcpy(data, d_data, length * sizeof(float), cudaMemcpyDeviceToHost));

        CHECK_CUDA(cudaFree(d_data));
    } else {
        //this handles cases for massive kernels

        //allocate a device pointer for the data
        float * d_data;
        //malloc space for the data
        CHECK_CUDA(cudaMalloc((void**)&d_data, length * sizeof(float)));
        //copy the data over
        CHECK_CUDA(cudaMemcpy(d_data, data, length * sizeof(float), cudaMemcpyHostToDevice));

        //allocate a device pointer for the kernel
        float * d_kernel;
        //malloc space for the data
        CHECK_CUDA(cudaMalloc((void**)&d_kernel, kernelLength * sizeof(float)));
        //copy the data over
        CHECK_CUDA(cudaMemcpy(d_kernel, kernel, kernelLength * sizeof(float), cudaMemcpyHostToDevice));

        //allocate a device pointer for output
        float * d_out;
        //malloc space for the output
        CHECK_CUDA(cudaMalloc((void**)&d_out, length * sizeof(float)));

        //run 512 block size cause arbitrary number
        int blockSize = 512;
        int gridSize = (length + 511)/ blockSize;

        ridiculousConvolutionKernel<<<gridSize, blockSize>>>(d_data, length, d_kernel, kernelLength, d_out);
        cudaError_t err = cudaGetLastError();
        if (err != cudaSuccess) printf("Launch error: %s\n", cudaGetErrorString(err));

        err = cudaDeviceSynchronize();
        if (err != cudaSuccess) printf("Execution error: %s\n", cudaGetErrorString(err));
        //copy data back over
        CHECK_CUDA(cudaMemcpy(data, d_out, length * sizeof(float), cudaMemcpyDeviceToHost));

        //free all the things
        CHECK_CUDA(cudaFree(d_data));
        CHECK_CUDA(cudaFree(d_out));
        CHECK_CUDA(cudaFree(d_kernel));
    }
} 

//translates the numpy stuff
void pythonTranslator(pybind11::array_t<float> pythonData, pybind11::array_t<float> kernel) {
    auto dataBuff = pythonData.request();
    auto kernelBuff = kernel.request();
    convolveData(static_cast<float*>(dataBuff.ptr), dataBuff.size, static_cast<float*>(kernelBuff.ptr), kernelBuff.size);
}

//special pybind11 macro for this to define the thing
PYBIND11_MODULE(cudaConvolver, m) {
    m.def("run", &pythonTranslator);
}