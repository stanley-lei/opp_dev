#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <cassert>

#define CL_CHECK(err) do { \
    if (err != CL_SUCCESS) { \
        std::cerr << "OpenCL Error: " << err << " at " << __LINE__ << std::endl; \
        std::exit(1); \
    } \
} while (0)

std::string get_platform_info(cl_platform_id platform, cl_platform_info param) {
  size_t size = 0;
  clGetPlatformInfo(platform, param, 0, nullptr, &size);
  std::vector<char> buf(size);
  clGetPlatformInfo(platform, param, size, buf.data(), nullptr);
  return std::string(buf.begin(), buf.end() - 1);  // 去掉 '\0'
}

std::string get_device_info(cl_device_id device, cl_device_info param) {
  size_t size = 0;
  clGetDeviceInfo(device, param, 0, nullptr, &size);
  std::vector<char> buf(size);
  clGetDeviceInfo(device, param, size, buf.data(), nullptr);
  return std::string(buf.begin(), buf.end() - 1);
}

int main() {
  cl_uint num_platforms = 0;
  CL_CHECK(clGetPlatformIDs(0, nullptr, &num_platforms));
  if (num_platforms == 0) {
    std::cerr << "No OpenCL platform found." << std::endl;
    return 1;
  }

  std::vector<cl_platform_id> platforms(num_platforms);
  CL_CHECK(clGetPlatformIDs(num_platforms, platforms.data(), nullptr));

  for (cl_uint i = 0; i < num_platforms; ++i) {
    std::cout << "Platform " << i << ": " << get_platform_info(platforms[i], CL_PLATFORM_NAME) << std::endl;

    cl_uint num_devices = 0;
    cl_int err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, nullptr, &num_devices);
    if (err != CL_SUCCESS || num_devices == 0) {
      std::cout << "  No devices found on this platform." << std::endl;
      continue;
    }

    std::vector<cl_device_id> devices(num_devices);
    CL_CHECK(clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_devices, devices.data(), nullptr));

    for (cl_uint j = 0; j < num_devices; ++j) {
      std::cout << "  Device " << j << ": " << get_device_info(devices[j], CL_DEVICE_NAME) << std::endl;
    }
  }

  return 0;
}

