cmake_minimum_required(VERSION 3.22)

project(GTOR-DAQ VERSION 1.0)

# Include CTest for testing framework
include (CTest)

# Add directories that use CMake to compile
add_subdirectory(00\ Libraries)

#Give ability for everyone to access libraries inside '00 Libaries/'
add_library(LIBRARIES INTERFACE)
target_include_directories(LIBRARIES INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/00 Libraries")