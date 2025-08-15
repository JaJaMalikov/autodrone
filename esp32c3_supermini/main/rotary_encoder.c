#include <stdbool.h>
#include "rotary_encoder.h"

static void rotary_isr_handler(void *arg)
{
    rotary_encoder_t *enc = (rotary_encoder_t *)arg;
    int a = gpio_get_level(enc->pin_a);
    int b = gpio_get_level(enc->pin_b);
    if (a == b) {
        enc->count++;
    } else {
        enc->count--;
    }
}

esp_err_t rotary_encoder_init(rotary_encoder_t *enc, gpio_num_t pin_a, gpio_num_t pin_b)
{
    enc->pin_a = pin_a;
    enc->pin_b = pin_b;
    enc->count = 0;

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin_a) | (1ULL << pin_b),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    static bool isr_service_installed = false;
    if (!isr_service_installed) {
        ESP_ERROR_CHECK(gpio_install_isr_service(0));
        isr_service_installed = true;
    }
    ESP_ERROR_CHECK(gpio_isr_handler_add(pin_a, rotary_isr_handler, enc));
    ESP_ERROR_CHECK(gpio_isr_handler_add(pin_b, rotary_isr_handler, enc));
    return ESP_OK;
}

int rotary_encoder_get_count(rotary_encoder_t *enc)
{
    return enc->count;
}

void rotary_encoder_reset(rotary_encoder_t *enc)
{
    enc->count = 0;
}
