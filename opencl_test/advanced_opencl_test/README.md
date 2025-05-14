# Advanced OpenCL Test (POCL Compatible)

This project is designed to test a wide range of OpenCL APIs for compatibility, stability, and correctness, especially on POCL (Portable Computing Language) platforms.

## Features

- Vector addition with local memory usage
- Two-stage kernel execution (`A+B -> C`, then `sqrt(C) -> D`)
- Asynchronous kernel execution with `cl_event`
- Host mapping with `clEnqueueMapBuffer`
- Full device/platform info dump
- Optional profiling with `clGetEventProfilingInfo`

## Build & Run

```bash
mkdir build && cd build
cmake ..
make
./ocl_advanced
```
