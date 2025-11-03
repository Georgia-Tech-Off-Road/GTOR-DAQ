
include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

function(add_cmbtl_test test_name source_file)
  add_executable(${test_name} ${source_file})
  target_link_libraries(${test_name} PUBLIC gtest_main compiler_flags CMBTL_DAQInfrastructure)
  add_test(NAME ${test_name}Tests COMMAND ${test_name})
endfunction()

add_cmbtl_test(PacketEncodeDecodeTests PacketEncodeDecode_unittest.cpp)
add_cmbtl_test(DAQInfrastructureTests DAQInfrastructure_unittest.cpp)