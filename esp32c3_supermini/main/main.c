#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_gc9a01.h"
#include "rotary_encoder.h"

#define TAG "SUPERMINI"

// SPI pins (adjust to match your hardware)
#define PIN_NUM_MOSI 6
#define PIN_NUM_CLK 4
#define PIN_NUM_CS_NAV 5
#define PIN_NUM_CS_VAL 7
#define PIN_NUM_DC 2
#define PIN_NUM_RST 1

// Rotary encoder pins
#define ENC_NAV_A GPIO_NUM_8
#define ENC_NAV_B GPIO_NUM_9
#define ENC_VAL_A GPIO_NUM_10
#define ENC_VAL_B GPIO_NUM_18

static esp_lcd_panel_handle_t nav_panel;
static esp_lcd_panel_handle_t value_panel;
static rotary_encoder_t nav_encoder;
static rotary_encoder_t value_encoder;

static void init_display(esp_lcd_panel_handle_t *panel, int cs)
{
    static bool bus_initialized = false;
    spi_bus_config_t buscfg = {
        .sclk_io_num = PIN_NUM_CLK,
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 240 * 240 * 2,
    };
    if (!bus_initialized) {
        ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
        bus_initialized = true;
    }
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_DC,
        .cs_gpio_num = cs,
        .pclk_hz = 40 * 1000 * 1000,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    esp_lcd_panel_io_handle_t io_handle;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((spi_bus_handle_t)SPI2_HOST, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_gc9a01(io_handle, &panel_config, panel));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(*panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(*panel));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_off(*panel, false));
}

static void fill_color(esp_lcd_panel_handle_t panel, uint16_t color)
{
    uint16_t line[240];
    for (int x = 0; x < 240; ++x) {
        line[x] = color;
    }
    for (int y = 0; y < 240; ++y) {
        esp_lcd_panel_draw_bitmap(panel, 0, y, 240, y + 1, line);
    }
}

void app_main(void)
{
    init_display(&nav_panel, PIN_NUM_CS_NAV);
    init_display(&value_panel, PIN_NUM_CS_VAL);
    rotary_encoder_init(&nav_encoder, ENC_NAV_A, ENC_NAV_B);
    rotary_encoder_init(&value_encoder, ENC_VAL_A, ENC_VAL_B);

    const char *params[] = {"ALTITUDE", "SPEED", "TIME"};
    int values[3] = {0};
    int current = 0;

    while (1) {
        int nav_pos = rotary_encoder_get_count(&nav_encoder);
        current = (nav_pos % 3 + 3) % 3; // ensure positive index

        int val_pos = rotary_encoder_get_count(&value_encoder);
        values[current] = val_pos;

        ESP_LOGI(TAG, "Param %s = %d", params[current], values[current]);

        fill_color(nav_panel, 0xFFFF - current * 0x1000);
        fill_color(value_panel, 0x001F + values[current]);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
