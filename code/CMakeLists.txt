cmake_minimum_required(VERSION 3.10)

project(test_project)

include_directories(include)
include_directories("$ENV{CUDA_PATH}/include/")

set(SOURCES main.cpp)
add_executable(main_exe ${SOURCES})
target_link_libraries(main_exe "$ENV{CUDA_PATH}/lib/x64/OpenCL.lib")
add_custom_command(
        TARGET main_exe POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/include/kernels.cl
                ${CMAKE_CURRENT_BINARY_DIR})
                
set(TEST_SOURCES test.cpp)
add_executable(test_exe ${TEST_SOURCES})
target_link_libraries(test_exe pthread)