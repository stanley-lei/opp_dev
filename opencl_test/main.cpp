#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <cassert>

#define CHECK(err) if (err != CL_SUCCESS) { std::cerr << "OpenCL Error " << err << " at line " << __LINE__ << std::endl; exit(1); }

void printDeviceInfo(cl_device_id device) {
    char name[128];
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(name), name, nullptr);
    std::cout << "Device Name: " << name << std::endl;

    cl_uint compute_units;
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, nullptr);
    std::cout << "Compute Units: " << compute_units << std::endl;

    size_t wg_size;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(wg_size), &wg_size, nullptr);
    std::cout << "Max Workgroup Size: " << wg_size << std::endl;
}

int main() {
    cl_int err;
    cl_uint numPlatforms = 0;
    CHECK(clGetPlatformIDs(0, nullptr, &numPlatforms));
    std::vector<cl_platform_id> platforms(numPlatforms);
    CHECK(clGetPlatformIDs(numPlatforms, platforms.data(), nullptr));

    std::cout << "Detected " << numPlatforms << " OpenCL platform(s)" << std::endl;

    for (size_t i = 0; i < platforms.size(); ++i) {
        char pname[128];
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(pname), pname, nullptr);
        std::cout << "Platform[" << i << "]: " << pname << std::endl;
    }

    cl_device_id device;
    CHECK(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 1, &device, nullptr));
    printDeviceInfo(device);

    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    CHECK(err);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
    CHECK(err);

    const int N = 1024;
    std::vector<float> A(N, 1.0f), B(N, 2.0f), C(N, 0.0f);

    cl_mem dA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, A.data(), &err);
    CHECK(err);
    cl_mem dB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, B.data(), &err);
    CHECK(err);
    cl_mem dC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * N, nullptr, &err);
    CHECK(err);

    FILE* f = fopen("simple.cl", "r");
    fseek(f, 0, SEEK_END);
    size_t src_size = ftell(f);
    rewind(f);
    char* src = new char[src_size + 1];
    fread(src, 1, src_size, f);
    src[src_size] = '\0';
    fclose(f);

    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&src, nullptr, &err);
    CHECK(err);
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
        std::vector<char> log(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr);
        std::cerr << "Build failed:\n" << log.data() << std::endl;
        exit(1);
    }

    cl_kernel kernel = clCreateKernel(program, "add", &err);
    CHECK(err);
    CHECK(clSetKernelArg(kernel, 0, sizeof(cl_mem), &dA));
    CHECK(clSetKernelArg(kernel, 1, sizeof(cl_mem), &dB));
    CHECK(clSetKernelArg(kernel, 2, sizeof(cl_mem), &dC));
    CHECK(clSetKernelArg(kernel, 3, sizeof(int), &N));

    size_t globalSize = N;
    cl_event kernel_event;
    CHECK(clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalSize, nullptr, 0, nullptr, &kernel_event));
    clWaitForEvents(1, &kernel_event);

    CHECK(clEnqueueReadBuffer(queue, dC, CL_TRUE, 0, sizeof(float) * N, C.data(), 0, nullptr, nullptr));

    // Validate
    bool pass = true;
    for (int i = 0; i < N; ++i) {
        if (C[i] != 3.0f) {
            std::cerr << "Error at " << i << ": " << C[i] << std::endl;
            pass = false;
            break;
        }
    }

    std::cout << (pass ? "✅ PASS" : "❌ FAIL") << std::endl;

    // Cleanup
    clReleaseMemObject(dA);
    clReleaseMemObject(dB);
    clReleaseMemObject(dC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    delete[] src;

    return 0;
}

