__kernel void matrix_multiplication(__global int *mat1, __global int *mat2, __global int *mat3)
{
    int i = get_global_id(0);
    int j = get_global_id(1);

    mat3[i * MAT3_W + j] = 0;

    // process
    for (int k = 0; k < MAT2_H; k++)
    {
        mat3[i * MAT3_W + j] += mat1[i * MAT1_W + k] * mat2[k * MAT2_W + j];
    }
}