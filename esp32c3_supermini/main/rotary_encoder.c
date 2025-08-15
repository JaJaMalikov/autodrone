#include "rotary_encoder.h"

esp_err_t rotary_encoder_init(rotary_encoder_t *enc, gpio_num_t pin_a, gpio_num_t pin_b)
{
    pcnt_unit_config_t unit_config = {
        .low_limit = -1000,
        .high_limit = 1000,
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &enc->unit));

    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = pin_a,
        .level_gpio_num = pin_b,
    };
    pcnt_channel_handle_t chan_a;
    ESP_ERROR_CHECK(pcnt_new_channel(enc->unit, &chan_a_config, &chan_a));

    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = pin_b,
        .level_gpio_num = pin_a,
    };
    pcnt_channel_handle_t chan_b;
    ESP_ERROR_CHECK(pcnt_new_channel(enc->unit, &chan_b_config, &chan_b));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE,
                                                 PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                                  PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE,
                                                 PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                                  PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    ESP_ERROR_CHECK(pcnt_unit_enable(enc->unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(enc->unit));
    ESP_ERROR_CHECK(pcnt_unit_start(enc->unit));
    return ESP_OK;
}

int rotary_encoder_get_count(rotary_encoder_t *enc)
{
    int val = 0;
    pcnt_unit_get_count(enc->unit, &val);
    return val;
}

void rotary_encoder_reset(rotary_encoder_t *enc)
{
    pcnt_unit_clear_count(enc->unit);
}
