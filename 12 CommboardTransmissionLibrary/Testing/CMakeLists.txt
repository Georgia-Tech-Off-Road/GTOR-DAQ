
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
target_link_libraries(SensorsTest gtest_main)
target_include_directories(SensorsTest PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.. ")
target_link_libraries(SensorsTest PUBLIC compiler_flags)
add_test(UNITTEST sensors_unit_test COMMAND SensorsTest)