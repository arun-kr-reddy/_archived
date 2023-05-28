#pragma once

#include "pgm.hpp"

static uint32_t s_freq[255];
static uint32_t s_cum_freq[255];
static double s_cum_prob[255];
static uint32_t s_num_samples;

void calc_freq(pgm_t &img)
{
    s_num_samples = img.width() * img.height();
    uint8_t *img_ptr = img.ptr();
    for (uint32_t i = 0; i < s_num_samples; i++)
    {
        uint8_t val = *(img_ptr + i);
        s_freq[val]++;
    }
}

void calc_cum_freq()
{
    uint32_t cum_sum = 0;
    for (uint32_t i = 0; i < 255; i++)
    {
        cum_sum += s_freq[i];
        s_cum_freq[i] = cum_sum;
        s_cum_prob[i] = cum_sum / (double)(s_num_samples);
    }
}

void apply_hist_eq(pgm_t &img)
{
    uint8_t *img_ptr = img.ptr();
    for (uint32_t i = 0; i < s_num_samples; i++)
    {
        uint8_t val = *(img_ptr + i);
        uint8_t eq_val = val * s_cum_prob[val];
        *(img_ptr + i) = eq_val;
    }
}

void histogram(pgm_t &img)
{
    memset(s_freq, 0, sizeof(s_freq));
    memset(s_cum_freq, 0, sizeof(s_cum_freq));
    calc_freq(img);
    calc_cum_freq();
    apply_hist_eq(img);
}