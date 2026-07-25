/*
 * bsp_shim.c — see bsp_shim.h for the rationale. Waveshare's
 * bsp_display_brightness_set() already clamps to 0..100, so this is a
 * near-trivial forwarder. Kept as its own component so we can grow the
 * shim without leaking bsp/esp32_p4_wifi6_touch_lcd_7b.h into the shared
 * business-logic files.
 */
#include "bsp_shim.h"
#include "bsp/esp32_p4_wifi6_touch_lcd_7b.h"

esp_err_t set_lcd_blight(uint32_t brightness_pct)
{
    if (brightness_pct > 100) brightness_pct = 100;
    return bsp_display_brightness_set((int)brightness_pct);
}
