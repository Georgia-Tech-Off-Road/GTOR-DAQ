# include CTest for testing 
include(CTest)

# Add all subdirectories that use CMake
add_subdirectory(BinaryBuffer)
add_subdirectory(CommboardTransmissionLibrary)

# Handle Adding Boost (submodule, not in our control)
add_library(BoostForArduino INTERFACE)
target_include_directories(BoostForArduino INTERFACE BoostForArduino/src)


# Define a library that automatically uses all of current libraries
add_library(LIBRARIES INTERFACE)
target_link_libraries(LIBRARIES INTERFACE BinaryBuffer CommboardTransmissionLibrary BoostForArduino)