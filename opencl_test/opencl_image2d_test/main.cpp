#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>
#include <iostream>
#include <vector>
#include <cassert>

#define CHECK(status, msg) \
  if (status != CL_SUCCESS) { \
    std::cerr << "❌ " << msg << " failed: " << status << std::endl; exit(1); \
  } else { \
    std::cout << "✅ " << msg << " succeeded." << std::endl; \
  }

int main() {
    cl_int err;

    // Get platform and device
    cl_platform_id platform;
    cl_device_id device;
    err = clGetPlatformIDs(1, &platform, nullptr);
    CHECK(err, "clGetPlatformIDs");
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);
    CHECK(err, "clGetDeviceIDs");

    // Check image support
    cl_bool imageSupport = CL_FALSE;
    err = clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imageSupport, nullptr);
    CHECK(err, "clGetDeviceInfo (image support)");
    if (!imageSupport) {
        std::cerr << "❌ Device does not support images." << std::endl;
        return 1;
    }

    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    CHECK(err, "clCreateContext");

    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
    CHECK(err, "clCreateCommandQueue");

    // Define image size and format
    const int width = 4;
    const int height = 4;
    cl_image_format img_format;
    img_format.image_channel_order = CL_RGBA;
    img_format.image_channel_data_type = CL_UNORM_INT8;

    cl_image_desc img_desc = {};
    img_desc.image_type = CL_MEM_OBJECT_IMAGE2D;
    img_desc.image_width = width;
    img_desc.image_height = height;

    std::vector<unsigned char> input_image(width * height * 4, 255);  // White image

    cl_mem image = clCreateImage(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 &img_format, &img_desc, input_image.data(), &err);
    CHECK(err, "clCreateImage");

    size_t origin[3] = {0, 0, 0};
    size_t region[3] = {static_cast<size_t>(width), static_cast<size_t>(height), 1};
    std::vector<unsigned char> read_back(width * height * 4, 0);

    err = clEnqueueReadImage(queue, image, CL_TRUE, origin, region, 0, 0, read_back.data(), 0, nullptr, nullptr);
    CHECK(err, "clEnqueueReadImage");

    bool pass = true;
    for (int i = 0; i < width * height * 4; ++i) {
        if (read_back[i] != 255) {
            std::cerr << "❌ Image mismatch at index " << i << ": " << (int)read_back[i] << std::endl;
            pass = false;
            break;
        }
    }

    std::cout << (pass ? "✅ Image test PASS" : "❌ Image test FAIL") << std::endl;

    clReleaseMemObject(image);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return 0;
}
