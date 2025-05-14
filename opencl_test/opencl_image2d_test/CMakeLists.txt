cmake_minimum_required(VERSION 3.10)
project(OpenCLImage2DTest)

find_package(OpenCL REQUIRED)

add_executable(image2d_test main.cpp)
target_include_directories(image2d_test PRIVATE ${OpenCL_INCLUDE_DIRS})
target_link_libraries(image2d_test ${OpenCL_LIBRARIES})
