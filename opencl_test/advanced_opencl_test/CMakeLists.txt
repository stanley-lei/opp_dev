cmake_minimum_required(VERSION 3.10)
project(AdvancedOpenCLTest)

find_package(OpenCL REQUIRED)

add_executable(ocl_advanced main.cpp)
target_include_directories(ocl_advanced PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(ocl_advanced ${OpenCL_LIBRARIES})
