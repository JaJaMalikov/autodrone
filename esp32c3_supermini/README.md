# ESP32-C3 Supermini Dual Display Controller

This example project targets the **ESP32-C3 Supermini** and demonstrates how to:

- Drive two GC9A01 circular displays using the [`espressif/esp_lcd_gc9a01`](https://components.espressif.com/components/espressif/esp_lcd_gc9a01) component.
- Read two rotary encoders: one to select which parameter is active and one to modify the value of the selected parameter.

The code keeps an array of parameters (altitude, speed and time) and updates the screens and serial log accordingly.

## Building

The project uses the local copy of ESP-IDF contained in this repository.  After installing the ESP-IDF tools, build with:

```bash
cd esp32c3_supermini
. ../esp-idf/export.sh
idf.py build
```

Adjust the GPIO pin assignments in `main.c` to match your hardware wiring.
