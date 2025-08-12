# include CTest for testing 
include(CTest)

# Add all subdirectories that use CMake
add_subdirectory(CommboardTransmissionLibrary)
add_subdirectory(BinaryBuffer)

# Define a library that automatically uses all of current libraries + Boost
add_library(LIBRARIES INTERFACE)
target_include_directories(LIBRARIES INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(LIBRARIES INTERFACE BinaryBuffer CommboardTransmissionLibrary)