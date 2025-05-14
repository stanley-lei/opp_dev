 cmake_minimum_required(VERSION 3.10)
project(OpenCLTest)

find_package(OpenCL REQUIRED)

add_executable(opencl_test main.cpp)
target_include_directories(opencl_test PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(opencl_test ${OpenCL_LIBRARIES})

