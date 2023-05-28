#pragma once

#include "convolution.hpp"
#include "enums.hpp"
#include "pgm.hpp"

static const int8_t box_kernel[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
static const int16_t box_div_factor = 9;

static const int8_t gaussian_kernel[9] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
static const int16_t gaussian_div_factor = 16;

void blur(pgm_t &src_img, pgm_t &dst_img, edge_e edge)
{
    convolve(src_img, dst_img, gaussian_kernel, 3, gaussian_div_factor, edge);
}