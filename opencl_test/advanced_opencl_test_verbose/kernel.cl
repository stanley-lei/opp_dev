__kernel void add(__global const float* A,
                   __global const float* B,
                   __global float* C,
                   __local float* temp,
                   int N)
{
    int id = get_global_id(0);
    if (id < N) {
        temp[id % get_local_size(0)] = A[id] + B[id];
        C[id] = temp[id % get_local_size(0)];
    }
}

__kernel void root(__global const float* C,
                   __global float* D,
                   int N)
{
    int id = get_global_id(0);
    if (id < N)
        D[id] = sqrt(C[id]);
}
