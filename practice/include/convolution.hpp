#pragma once

#include "enums.hpp"
#include "pgm.hpp"

void convolve(pgm_t &src_img, pgm_t &dst_img, const int8_t *kernel, uint8_t k_size,
              const int16_t div_factor, edge_e edge)
{
    int width = src_img.width();
    int height = src_img.height();
    uint8_t *src_ptr = src_img.ptr();
    uint8_t *dst_ptr = dst_img.ptr();

    uint8_t k_half_size = (k_size - 1) / 2;

    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            int16_t sum = 0;
            for (int k_y = -k_half_size; k_y <= k_half_size; k_y++)
            {
                for (int k_x = -k_half_size; k_x <= k_half_size; k_x++)
                {
                    int pos_y = y + k_y;
                    int pos_x = x + k_x;
                    if ((pos_x >= 0) && (pos_x < width) && (pos_y >= 0) && (pos_y < height))
                    {
                        sum += src_ptr[pos_y * width + pos_x] *
                               kernel[(k_y + k_half_size) * k_size + (k_x + k_half_size)];
                    }
                    else if (edge == clamp)
                    {
                        if (pos_x < 0)
                        {
                            pos_x = 0;
                        }
                        else if (pos_x >= width)
                        {
                            pos_x = width - 1;
                        }

                        if (pos_y < 0)
                        {
                            pos_y = 0;
                        }
                        else if (pos_y >= height)
                        {
                            pos_y = height - 1;
                        }

                        sum += src_ptr[pos_y * width + pos_x] *
                               kernel[(k_y + k_half_size) * k_size + (k_x + k_half_size)];
                    }
                    else if (edge == mirror)
                    {
                        if (pos_x < 0)
                        {
                            pos_x = -(pos_x);
                        }
                        else if (pos_x >= width)
                        {
                            pos_x = width - (pos_x - width) - 1;
                        }

                        if (pos_y < 0)
                        {
                            pos_y = -(pos_y);
                        }
                        else if (pos_y >= height)
                        {
                            pos_y = height - (pos_y - height) - 1;
                        }

                        sum += src_ptr[pos_y * width + pos_x] *
                               kernel[(k_y + k_half_size) * k_size + (k_x + k_half_size)];
                    }
                }
            }
            dst_ptr[y * width + x] = (uint8_t)(abs(sum / div_factor));
        }
    }
}