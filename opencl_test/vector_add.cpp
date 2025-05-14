#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <cassert>

const char* kernelSource = R"CLC(
__kernel void vector_add(__global const float *A,
                         __global const float *B,
                         __global float *C)
{
    int i = get_global_id(0);
    C[i] = A[i] + B[i];
}
)CLC";

int main() {
    const int N = 1024;
    std::vector<float> A(N, 1.0f), B(N, 2.0f), C(N, 0.0f);

    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    // Get first platform and device
    err = clGetPlatformIDs(1, &platform, nullptr);
    assert(err == CL_SUCCESS);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, nullptr);
    assert(err == CL_SUCCESS);

    // Create context and queue
    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    assert(err == CL_SUCCESS);
    queue = clCreateCommandQueue(context, device, 0, &err);
    assert(err == CL_SUCCESS);

    // Create buffers
    cl_mem dA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float)*N, A.data(), &err);
    cl_mem dB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float)*N, B.data(), &err);
    cl_mem dC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float)*N, nullptr, &err);
    assert(err == CL_SUCCESS);

    // Create program from source
    program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, &err);
    assert(err == CL_SUCCESS);
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        // 打印构建错误日志
        size_t len;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &len);
        std::vector<char> log(len);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, len, log.data(), nullptr);
        std::cerr << "Build log:\n" << log.data() << "\n";
        return 1;
    }

    // 创建 kernel 并设置参数
    kernel = clCreateKernel(program, "vector_add", &err);
    assert(err == CL_SUCCESS);
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &dA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &dB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &dC);

    // 启动 kernel
    size_t globalSize = N;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalSize, nullptr, 0, nullptr, nullptr);
    assert(err == CL_SUCCESS);

    // 读取结果
    err = clEnqueueReadBuffer(queue, dC, CL_TRUE, 0, sizeof(float)*N, C.data(), 0, nullptr, nullptr);
    assert(err == CL_SUCCESS);

    // 验证结果
    bool pass = true;
    for (int i = 0; i < N; ++i) {
        if (C[i] != 3.0f) {
            std::cerr << "Error at index " << i << ": " << C[i] << "\n";
            pass = false;
            break;
        }
    }

    std::cout << (pass ? "Result = PASS\n" : "Result = FAIL\n");

    // 清理资源
    clReleaseMemObject(dA);
    clReleaseMemObject(dB);
    clReleaseMemObject(dC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return 0;
}

