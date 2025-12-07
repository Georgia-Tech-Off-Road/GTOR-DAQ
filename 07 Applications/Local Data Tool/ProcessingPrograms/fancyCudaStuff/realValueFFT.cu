struct complexData{
    float realComponent;
    float complexComponent;
};

//define the constant complex components in constant memory (need the square of the num twiddle factors)
//since theres twiddle factor purely dependent on k and then the other constant complex components
//are dependent on k*n (so n^2)
__constant__ complexData constantComplexComponents[90000];
__constant__ complexData twiddleFactors[300];


__global__ void computeRealFFT(float * inputData, size_t inputDataLength, float * outputData, size_t windowSize) {
    //don't need to worry about extern shared cause we'll be running max block size
    __shared__ float blockData[1024];

    //calculate threadID
    int threadID = threadIdx.x + blockDim.x * blockIdx.x;

    //load into shared memory (first half even last half odd)
    //this should help later with bank conflicts and reduce amount of index based calculations
    if (threadID < inputDataLength && threadIdx.x < windowSize) {
        blockData[threadIdx.x/2 + blockDim.x * threadIdx.x%2] = inputData[threadID];
    }

    //define a struct for the even and odd portions
    complexData evenPortion;
    complexData oddPortion;

    //compute even portion
    for (int i = 0; i < windowSize/2; i +=1) {

    }
    
}