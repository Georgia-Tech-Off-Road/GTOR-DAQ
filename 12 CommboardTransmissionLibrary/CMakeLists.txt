project(CMBTL)

include(CTest)

#Add compiler flags to use
add_library(compiler_flags INTERFACE)

#Specify C++ 23 standard
target_compile_features(compiler_flags INTERFACE cxx_std_11)

add_subdirectory(Testing)

add_subdirectory(misc/BinaryBuffer)

cmake_minimum_required(VERSION 3.22)