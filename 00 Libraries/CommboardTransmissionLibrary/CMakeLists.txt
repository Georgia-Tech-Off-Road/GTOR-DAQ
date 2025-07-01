cmake_minimum_required(VERSION 3.22)

project(CMBTL VERSION 1.0)

include(CTest)

#Add compiler flags to use
add_library(compiler_flags INTERFACE)

#Specify C++ 11 standard
target_compile_features(compiler_flags INTERFACE cxx_std_11)

add_subdirectory(Testing)

add_library(BinaryBuffer INTERFACE)
target_include_directories(BinaryBuffer INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../BinaryBuffer")
