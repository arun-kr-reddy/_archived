#pragma once

#include "enums.hpp"

void resize(pgm_t &src_img, pgm_t &dst_img, resize_e method)
{
    int src_width = src_img.width();
    int src_height = src_img.height();
    int dst_width = dst_img.width();
    int dst_height = dst_img.height();
    uint8_t *src_ptr = src_img.ptr();
    uint8_t *dst_ptr = dst_img.ptr();

    float scale_x = src_width / (float)dst_width;
    float scale_y = src_height / (float)dst_height;
    for (uint32_t y = 0; y < dst_height; y++)
    {
        for (uint32_t x = 0; x < dst_width; x++)
        {
            if (method == nearest_neighbor)
            {
                uint16_t x_nearest = (uint16_t)(x * scale_x);
                uint16_t y_nearest = (uint16_t)(y * scale_y);
                dst_ptr[y * dst_width + x] = src_ptr[y_nearest * src_width + x_nearest];
            }
            else if (method == bilinear)
            {
                //  Q11      P1       Q12
                //
                //           P
                //
                //  Q21      P2       Q22

                float x_pos = x * scale_x;
                float y_pos = y * scale_y;

                uint16_t x1 = (uint16_t)(x_pos);
                uint16_t y1 = (uint16_t)(y_pos);
                uint16_t x2 = x1 + 1;
                uint16_t y2 = y1 + 1;

                uint8_t Q11 = src_ptr[y1 * src_width + x1];
                uint8_t Q12 = src_ptr[y1 * src_width + x2];
                uint8_t Q21 = src_ptr[y2 * src_width + x1];
                uint8_t Q22 = src_ptr[y2 * src_width + x2];

                uint8_t P1 = (x2 - x_pos) * Q11 + (x_pos - x1) * Q12;
                uint8_t P2 = (x2 - x_pos) * Q21 + (x_pos - x1) * Q22;
                uint8_t P = (y2 - y_pos) * P1 + (y_pos - y1) * P2;

                dst_ptr[y * dst_width + x] = Q11;
            }
        }
    }
}