#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

#define CHECK(err) if ((err) != CL_SUCCESS) { std::cerr << "OpenCL error: " << err << " @ line " << __LINE__ << std::endl; exit(EXIT_FAILURE); }

void printDeviceInfo(cl_device_id device) {
    char name[128], version[128];
    cl_uint cu;
    size_t wg;

    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(name), name, nullptr);
    clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(version), version, nullptr);
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cu), &cu, nullptr);
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(wg), &wg, nullptr);

    std::cout << "Device: " << name << " (" << version << ")"
              << "Compute Units: " << cu << ", Max WG Size: " << wg << "\n";
}

std::string loadKernelSource(const std::string& filename) {
    FILE* f = fopen(filename.c_str(), "r");
    if (!f) { perror("fopen"); exit(1); }
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    rewind(f);
    std::string source(len, '\0');
    fread(&source[0], 1, len, f);
    fclose(f);
    return source;
}

int main() {
    cl_int err;
    const int N = 1024;
    std::vector<float> A(N, 1.0f), B(N, 2.0f), C(N), D(N);

    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;

    CHECK(clGetPlatformIDs(1, &platform, nullptr));
    CHECK(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, nullptr));
    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err); CHECK(err);
    queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err); CHECK(err);

    printDeviceInfo(device);

    cl_mem dA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, N * sizeof(float), A.data(), &err); CHECK(err);
    cl_mem dB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, N * sizeof(float), B.data(), &err); CHECK(err);
    cl_mem dC = clCreateBuffer(context, CL_MEM_READ_WRITE, N * sizeof(float), nullptr, &err); CHECK(err);
    cl_mem dD = clCreateBuffer(context, CL_MEM_WRITE_ONLY, N * sizeof(float), nullptr, &err); CHECK(err);

    std::string source = loadKernelSource("kernel.cl");
    const char* src = source.c_str();
    cl_program program = clCreateProgramWithSource(context, 1, &src, nullptr, &err); CHECK(err);
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
        std::vector<char> log(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr);
        std::cerr << "Build log:\n" << log.data() << std::endl;
        return 1;
    }

    cl_kernel kernel_add = clCreateKernel(program, "add", &err); CHECK(err);
    cl_kernel kernel_root = clCreateKernel(program, "root", &err); CHECK(err);

    size_t local_size = 64;
    size_t global_size = ((N + local_size - 1) / local_size) * local_size;

    clSetKernelArg(kernel_add, 0, sizeof(cl_mem), &dA);
    clSetKernelArg(kernel_add, 1, sizeof(cl_mem), &dB);
    clSetKernelArg(kernel_add, 2, sizeof(cl_mem), &dC);
    clSetKernelArg(kernel_add, 3, local_size * sizeof(float), nullptr);
    clSetKernelArg(kernel_add, 4, sizeof(int), &N);

    cl_event evt_add, evt_root;

    CHECK(clEnqueueNDRangeKernel(queue, kernel_add, 1, nullptr, &global_size, &local_size, 0, nullptr, &evt_add));
    clWaitForEvents(1, &evt_add);

    clSetKernelArg(kernel_root, 0, sizeof(cl_mem), &dC);
    clSetKernelArg(kernel_root, 1, sizeof(cl_mem), &dD);
    clSetKernelArg(kernel_root, 2, sizeof(int), &N);

    CHECK(clEnqueueNDRangeKernel(queue, kernel_root, 1, nullptr, &global_size, &local_size, 0, nullptr, &evt_root));
    clWaitForEvents(1, &evt_root);

    clEnqueueReadBuffer(queue, dD, CL_TRUE, 0, sizeof(float) * N, D.data(), 0, nullptr, nullptr);

    // Profiling
    cl_ulong t0, t1;
    clGetEventProfilingInfo(evt_add, CL_PROFILING_COMMAND_START, sizeof(t0), &t0, nullptr);
    clGetEventProfilingInfo(evt_add, CL_PROFILING_COMMAND_END, sizeof(t1), &t1, nullptr);
    std::cout << "Add Kernel Time: " << (t1 - t0)/1e6 << " ms\n";

    clGetEventProfilingInfo(evt_root, CL_PROFILING_COMMAND_START, sizeof(t0), &t0, nullptr);
    clGetEventProfilingInfo(evt_root, CL_PROFILING_COMMAND_END, sizeof(t1), &t1, nullptr);
    std::cout << "Root Kernel Time: " << (t1 - t0)/1e6 << " ms\n";

    // Validate
    bool valid = true;
    for (int i = 0; i < N; ++i) {
        if (fabs(D[i] - std::sqrt(3.0f)) > 1e-4) {
            std::cerr << "Mismatch at " << i << ": " << D[i] << std::endl;
            valid = false;
            break;
        }
    }
    std::cout << (valid ? "✅ PASS" : "❌ FAIL") << std::endl;

    clReleaseMemObject(dA); clReleaseMemObject(dB); clReleaseMemObject(dC); clReleaseMemObject(dD);
    clReleaseKernel(kernel_add); clReleaseKernel(kernel_root);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return 0;
}
