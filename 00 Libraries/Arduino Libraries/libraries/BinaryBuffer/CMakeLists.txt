add_library(BinaryBuffer INTERFACE)
target_include_directories(BinaryBuffer INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/")
add_subdirectory(tests)