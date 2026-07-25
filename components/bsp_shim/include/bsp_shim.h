/*
 * bsp_shim — thin, portable wrapper over the Waveshare
 * ESP32-P4-WIFI6-Touch-LCD-7B BSP.
 *
 * Only one symbol is exposed today: `set_lcd_blight(pct)`, which forwards
 * to `bsp_display_brightness_set(int)`. Callers in actions.c and vars.c
 * (screen-timeout logic, brightness slider) can adjust brightness without
 * depending on the Waveshare-specific BSP header.
 *
 * If you need a second symbol for a future portable file, add it here
 * rather than including a Waveshare-specific header from the business
 * logic.
 */
#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Set LCD backlight brightness, 0..100 %. Values outside the range are
 * clamped. Returns ESP_OK, or an esp_err_t from the underlying LEDC API. */
esp_err_t set_lcd_blight(uint32_t brightness_pct);

#ifdef __cplusplus
}
#endif
