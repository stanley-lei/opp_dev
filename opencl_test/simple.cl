__kernel void add(__global const float* A,
                  __global const float* B,
                  __global float* C,
                  int N)
{
    int id = get_global_id(0);
    if (id < N) C[id] = A[id] + B[id];
}

