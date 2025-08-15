#pragma once
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"
#include "esp_err.h"

typedef struct {
    pcnt_unit_handle_t unit;
} rotary_encoder_t;

esp_err_t rotary_encoder_init(rotary_encoder_t *enc, gpio_num_t pin_a, gpio_num_t pin_b);
int rotary_encoder_get_count(rotary_encoder_t *enc);
void rotary_encoder_reset(rotary_encoder_t *enc);
