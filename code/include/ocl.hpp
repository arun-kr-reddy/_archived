#define CL_TARGET_OPENCL_VERSION 210
#define PROGRAM_FILE "kernels.cl"
#define KERNEL_FUNC "matrix_multiplication"

// https://en.wikipedia.org/wiki/File:Matrix_multiplication_qtl1.svg
#define MAT1_H (1024U)  // l
#define MAT1_W (1024U)  // m
#define MAT2_H (MAT1_W)
#define MAT2_W (1024U)  // n
#define MAT3_H (MAT1_H)
#define MAT3_W (MAT2_W)

#include <CL/cl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <chrono>

typedef struct
{
    uint32_t width;
    uint32_t height;
    int32_t *ptr;
} matrix_t;

void print_matrix(matrix_t mat)
{
    for (int y = 0; y < mat.height; y++)
    {
        for (int x = 0; x < mat.width; x++)
        {
            printf("%d\t", mat.ptr[y * mat.height + x]);
        }
        printf("\n");
    }
    printf("\n\n");
}

int verify_matrix_multiplication(matrix_t mat1, matrix_t mat2, matrix_t mat3_ocl)
{
    // init
    bool ret = true;

    matrix_t mat3_cpp;
    mat3_cpp.height = mat3_ocl.height;
    mat3_cpp.width = mat3_ocl.width;
    mat3_cpp.ptr = (int32_t *)malloc(mat3_cpp.height * mat3_cpp.width * sizeof(int32_t));

    // process
    for (int i = 0; i < mat1.height; i++)
    {
        for (int j = 0; j < mat2.width; j++)
        {
            mat3_cpp.ptr[j * mat3_cpp.width + i] = 0;

            for (int k = 0; k < mat2.height; k++)
            {
                mat3_cpp.ptr[j * mat3_cpp.width + i] += mat1.ptr[k * mat1.width + i] * mat2.ptr[j * mat2.width + k];
            }

            if (mat3_cpp.ptr[j * mat3_cpp.width + i] != mat3_ocl.ptr[j * mat3_ocl.width + i])
            {
                printf("output mismatch @ %d %d : %d : %d\n", i, j, mat3_cpp.ptr[j * mat3_cpp.width + i],
                       mat3_ocl.ptr[j * mat3_ocl.width + i]);
                ret = false;
            }
        }
    }

    // print_matrix(mat1);
    // print_matrix(mat2);
    // print_matrix(mat3_ocl);
    // print_matrix(mat3_cpp);

    // deinit
    free(mat3_cpp.ptr);

    return (ret);
}

int matrix_multiplication(void)
{
    // init
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_event event;
    cl_program program;
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;
    cl_kernel kernel;
    cl_int err;
    cl_mem mat1_buff;
    cl_mem mat2_buff;
    cl_mem mat3_buff;

    // allocate matrix
    matrix_t mat1;
    mat1.height = MAT1_H;
    mat1.width = MAT1_W;
    mat1.ptr = (int32_t *)malloc(mat1.height * mat1.width * sizeof(int32_t));

    matrix_t mat2;
    mat2.height = MAT2_H;
    mat2.width = MAT2_W;
    mat2.ptr = (int32_t *)malloc(mat2.height * mat2.width * sizeof(int32_t));

    matrix_t mat3;
    mat3.height = MAT3_H;
    mat3.width = MAT3_W;
    mat3.ptr = (int32_t *)malloc(mat3.height * mat3.width * sizeof(int32_t));

    // populate matrix
    for (int i = 0; i < MAT1_H * MAT1_W; i++)
    {
        mat1.ptr[i] = i;
    }

    for (int i = 0; i < MAT2_H * MAT2_W; i++)
    {
        mat2.ptr[i] = i;
    }

    // get device
    err = clGetPlatformIDs(1, &platform, NULL);
    assert(err == CL_SUCCESS);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    assert(err == CL_SUCCESS);

    // create context
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    assert(err == CL_SUCCESS);

    // read program file
    program_handle = fopen(PROGRAM_FILE, "r");
    assert(program_handle != NULL);
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char *)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);

    // create program
    program = clCreateProgramWithSource(context, 1, (const char **)&program_buffer, &program_size, &err);
    assert(err == CL_SUCCESS);
    free(program_buffer);

    // build program
    char build_flags[100];
    sprintf(build_flags, "-DMAT1_H=%u -DMAT1_W=%u -DMAT2_H=%u -DMAT2_W=%u -DMAT3_H=%u -DMAT3_W=%u", mat1.height,
            mat1.width, mat2.height, mat2.width, mat3.height, mat3.width);
    err = clBuildProgram(program, 0, NULL, build_flags, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        program_log = (char *)malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        assert(0);
    }

    // create kernel
    kernel = clCreateKernel(program, KERNEL_FUNC, &err);
    assert(err == CL_SUCCESS);

    // create buffers
    mat1_buff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                               mat1.width * mat1.height * sizeof(int32_t), mat1.ptr, &err);
    assert(err == CL_SUCCESS);
    mat2_buff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                               mat2.width * mat2.height * sizeof(int32_t), mat2.ptr, &err);
    assert(err == CL_SUCCESS);
    mat3_buff = clCreateBuffer(context, CL_MEM_WRITE_ONLY, mat3.width * mat3.height * sizeof(int32_t), NULL, &err);
    assert(err == CL_SUCCESS);

    // set kernel args
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mat1_buff);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &mat2_buff);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &mat3_buff);
    assert(err == CL_SUCCESS);

    // create command queue
    queue = clCreateCommandQueueWithProperties(context, device, NULL, &err);
    assert(err == CL_SUCCESS);

    // NDRange
    size_t global_dim[2] = {mat3.width, mat3.height};
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, (const size_t *)&global_dim, NULL, 0, NULL, NULL);
    assert(err == CL_SUCCESS);

    err = clFinish(queue);
    assert(err == CL_SUCCESS);

    // read output
    err = clEnqueueReadBuffer(queue, mat3_buff, 1, 0, mat3.width * mat3.height * sizeof(int32_t), mat3.ptr, 0, NULL,
                              NULL);

    // verify output
    if (true == verify_matrix_multiplication(mat1, mat2, mat3))
    {
        printf("=======output OK=======\n");
    }
    else
    {
        printf("=======output NOT OK=======\n");
    }

    // deinit
    clReleaseMemObject(mat1_buff);
    clReleaseMemObject(mat2_buff);
    clReleaseMemObject(mat3_buff);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    free(mat1.ptr);
    free(mat2.ptr);
    free(mat3.ptr);

    return err;
}