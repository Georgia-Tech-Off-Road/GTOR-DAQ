# include CTest for testing 
include(CTest)

# Handle Adding Boost (submodule, not in our control)
add_library(BoostForArduino INTERFACE)
target_include_directories(BoostForArduino INTERFACE Arduino\ Libraries/libraries/BoostForArduino/src)

# Add all subdirectories that use CMake
add_subdirectory(Arduino\ Libraries/libraries/BinaryBuffer)
add_subdirectory(Arduino\ Libraries/libraries/CommboardTransmissionLibrary)
add_subdirectory(Arduino\ Libraries/libraries/CMBTL_DAQInfrastructure)


# Define a library that automatically uses all of current libraries
add_library(LIBRARIES INTERFACE)
target_link_libraries(LIBRARIES INTERFACE BinaryBuffer CommboardTransmissionLibrary BoostForArduino)