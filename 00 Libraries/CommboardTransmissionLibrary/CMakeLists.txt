cmake_minimum_required(VERSION 3.22)

include(CTest)

#Add compiler flags to use
add_library(compiler_flags INTERFACE)

#Specify C++ 11 standard
target_compile_features(compiler_flags INTERFACE cxx_std_11)

# Create the CommboardTransmissionLibrary as an interface library
add_library(CommboardTransmissionLibrary INTERFACE)

# Link to BinaryBuffer library
target_link_libraries(CommboardTransmissionLibrary INTERFACE BinaryBuffer BoostForArduino)

# Set include directories for this library
target_include_directories(CommboardTransmissionLibrary INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(Testing)