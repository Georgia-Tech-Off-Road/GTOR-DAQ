
include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(SensorsTest Sensors_unittest.cpp)
target_link_libraries(SensorsTest PUBLIC gtest_main)
target_include_directories(SensorsTest PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(SensorsTest PUBLIC compiler_flags)
add_test(NAME SensorsUnitTest COMMAND SensorsTest)

add_executable(SensorInfoTests SensorInfo_unittest.cpp)
target_link_libraries(SensorInfoTests PUBLIC gtest_main)
target_include_directories(SensorInfoTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(SensorInfoTests PUBLIC compiler_flags)
add_test(NAME SensorInfoUnitTest COMMAND SensorInfoTests)

add_executable(SensorDataTests SensorData_unittest.cpp)
target_link_libraries(SensorDataTests PUBLIC gtest_main)
target_include_directories(SensorDataTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(SensorDataTests PUBLIC compiler_flags)
add_test(NAME SensorDataUnitTest COMMAND SensorDataTests)

add_executable(PacketEncodeDecodeTests PacketEncodeDecode_unittest.cpp)
target_link_libraries(PacketEncodeDecodeTests PUBLIC gtest_main)
target_include_directories(PacketEncodeDecodeTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(PacketEncodeDecodeTests PUBLIC compiler_flags)
add_test(NAME PacketEncodeDecodeUnitTest COMMAND PacketEncodeDecodeTests)

add_executable(DAQInfrastructureTests DAQInfrastructure_unittest.cpp)
target_link_libraries(DAQInfrastructureTests PUBLIC gtest_main)
target_include_directories(DAQInfrastructureTests PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(DAQInfrastructureTests PUBLIC compiler_flags)
add_test(NAME DAQInfrastructureTests COMMAND DAQInfrastructureTests)