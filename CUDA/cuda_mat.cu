#define BLOCK_SIZE 32

template <typename T>
__global__ void gpu_matrix_mult(T *a, T *b, T *c, uint16_t ax_sz, uint16_t ay_sz, uint16_t bx_sz, uint16_t by_sz)
{
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    if (row < ay_sz && col < bx_sz)
    {
        for (uint16_t i = 0; i < ax_sz; ++i)
        {
            c[col + (row * bx_sz)] += a[i + (row * ax_sz)] * b[col + (i * bx_sz)];
        }
    }
}

template <typename T>
void mat_matrix_mult(T *a, T *b, T *c, uint16_t ax_sz, uint16_t ay_sz, uint16_t bx_sz, uint16_t by_sz, uint16_t cx_sz, uint16_t cy_sz)
{
    T *reg_a;
    T *reg_b;
    T *reg_c;

    cudaMalloc((void **)&reg_a, ax_sz * ay_sz * sizeof(T));
    cudaMalloc((void **)&reg_b, bx_sz * by_sz * sizeof(T));
    cudaMalloc((void **)&reg_c, cx_sz * cy_sz * sizeof(T));

    cudaMemcpy(reg_a, a, ax_sz * ay_sz * sizeof(T), cudaMemcpyHostToDevice);
    cudaMemcpy(reg_b, b, bx_sz * by_sz * sizeof(T), cudaMemcpyHostToDevice);

    unsigned int grid_rows = (ax_sz + BLOCK_SIZE - 1) / BLOCK_SIZE;
    unsigned int grid_cols = (by_sz + BLOCK_SIZE - 1) / BLOCK_SIZE;

    dim3 dimGrid(grid_cols, grid_rows);
    dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);

    gpu_matrix_mult<<<dimGrid, dimBlock>>>(reg_a, reg_b, reg_c, ax_sz, ay_sz, bx_sz, by_sz);
    cudaMemcpy(c, reg_c, sizeof(T) * cx_sz * cy_sz, cudaMemcpyDeviceToHost);
    cudaThreadSynchronize();

    cudaFree(reg_a);
    cudaFree(reg_b);
    cudaFree(reg_c);
}
