#pragma once

#include <cmath>

#include "convolution.hpp"
#include "enums.hpp"
#include "pgm.hpp"

static const int8_t prewitt_x_kernel[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
static const int8_t prewitt_y_kernel[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
static const int16_t prewitt_div_factor = 3;

static const int8_t sobel_x_kernel[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
static const int8_t sobel_y_kernel[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
static const int16_t sobel_div_factor = 4;

void edgeX(pgm_t &src_img, pgm_t &dst_img, edge_e edge)
{
    convolve(src_img, dst_img, sobel_x_kernel, 3, sobel_div_factor, edge);
}

void edgeY(pgm_t &src_img, pgm_t &dst_img, edge_e edge)
{
    convolve(src_img, dst_img, sobel_y_kernel, 3, sobel_div_factor, edge);
}

void edgeRms(pgm_t &edgeX_img, pgm_t &edgeY_img, pgm_t &dst_img, uint8_t threshold)
{
    int width = dst_img.width();
    int height = dst_img.height();
    uint8_t *edgeX_ptr = edgeX_img.ptr();
    uint8_t *edgeY_ptr = edgeY_img.ptr();
    uint8_t *dst_ptr = dst_img.ptr();

    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            uint8_t x_val = edgeX_ptr[y * width + x];
            uint8_t y_val = edgeY_ptr[y * width + x];

            uint8_t rms = sqrt((x_val * x_val + y_val * y_val) / 2);
            dst_ptr[y * width + x] = (rms > threshold) ? ((uint8_t)rms) : (0);
        }
    }
}