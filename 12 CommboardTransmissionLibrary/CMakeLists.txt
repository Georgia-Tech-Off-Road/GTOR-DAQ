project(CMBTL)

<<<<<<< HEAD
include(CTest)

=======
>>>>>>> 038d0c9c22c866c18d3ad23a59a4946df5a71809
#Add compiler flags to use
add_library(compiler_flags INTERFACE)

#Specify C++ 23 standard
<<<<<<< HEAD
target_compile_features(compiler_flags INTERFACE cxx_std_11)
=======
target_compile_features(compiler_flags INTERFACE cxx_std_23)
>>>>>>> 038d0c9c22c866c18d3ad23a59a4946df5a71809

add_subdirectory(Testing)

add_subdirectory(misc/BinaryBuffer)

cmake_minimum_required(VERSION 3.22)