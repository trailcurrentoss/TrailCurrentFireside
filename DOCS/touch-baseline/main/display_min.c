/*
 * Minimal EK79007 MIPI-DSI bring-up — vendor Lesson07 display_port_init()
 * verbatim (bus, timing, porches, clocks), with LVGL left out. Purpose is
 * only to (a) replicate the display power/init sequence that makes the
 * backlight controllable on this board, and (b) paint solid stage colors
 * so the operator can see stage changes during the touch test.
 */

#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_ldo_regulator.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_ek79007.h"

#define H_SIZE 1024
#define V_SIZE 600

static esp_lcd_dsi_bus_handle_t   s_dsi_bus;
static esp_lcd_panel_io_handle_t  s_dbi_io;
static esp_lcd_panel_handle_t     s_panel;
static uint16_t                  *s_strip;

esp_err_t display_min_init(void)
{
    esp_err_t err;

    /* On-chip LDOs power the display subsystem (same as Fireside main.c):
     * LDO3 2.5 V = MIPI DSI PHY (without it esp_lcd_new_dsi_bus spins
     * forever waiting for the PHY PLL), LDO4 3.3 V = LCD VCC. */
    static esp_ldo_channel_handle_t s_ldo3, s_ldo4;
    esp_ldo_channel_config_t ldo3_cfg = { .chan_id = 3, .voltage_mv = 2500 };
    err = esp_ldo_acquire_channel(&ldo3_cfg, &s_ldo3);
    if (err != ESP_OK) return err;
    esp_ldo_channel_config_t ldo4_cfg = { .chan_id = 4, .voltage_mv = 3300 };
    err = esp_ldo_acquire_channel(&ldo4_cfg, &s_ldo4);
    if (err != ESP_OK) return err;

    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = 2,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = 900,
    };
    err = esp_lcd_new_dsi_bus(&bus_config, &s_dsi_bus);
    if (err != ESP_OK) return err;

    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    err = esp_lcd_new_panel_io_dbi(s_dsi_bus, &dbi_config, &s_dbi_io);
    if (err != ESP_OK) return err;

    const esp_lcd_dpi_panel_config_t dpi_config = {
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = 51,
        .virtual_channel = 0,
        .pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB565,
        .num_fbs = 1,
        .video_timing = {
            .h_size = H_SIZE,
            .v_size = V_SIZE,
            .hsync_back_porch = 160,
            .hsync_pulse_width = 70,
            .hsync_front_porch = 160,
            .vsync_back_porch = 23,
            .vsync_pulse_width = 10,
            .vsync_front_porch = 12,
        },
        .flags.use_dma2d = true,
    };

    ek79007_vendor_config_t vendor_config = {
        .mipi_config = {
            .dsi_bus = s_dsi_bus,
            .dpi_config = &dpi_config,
        },
    };

    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .vendor_config = &vendor_config,
    };

    err = esp_lcd_new_panel_ek79007(s_dbi_io, &panel_config, &s_panel);
    if (err != ESP_OK) return err;
    err = esp_lcd_panel_reset(s_panel);
    if (err != ESP_OK) return err;
    err = esp_lcd_panel_init(s_panel);
    if (err != ESP_OK) return err;

    /* One full frame in PSRAM. draw_bitmap with use_dma2d is async, so a
     * single whole-screen draw from a persistent buffer avoids the
     * buffer-reuse race a strip loop would have. */
    s_strip = heap_caps_malloc(H_SIZE * V_SIZE * sizeof(uint16_t),
                               MALLOC_CAP_SPIRAM);
    return s_strip ? ESP_OK : ESP_ERR_NO_MEM;
}

void display_min_fill(uint16_t rgb565)
{
    if (!s_panel || !s_strip) return;
    for (int i = 0; i < H_SIZE * V_SIZE; i++) s_strip[i] = rgb565;
    esp_lcd_panel_draw_bitmap(s_panel, 0, 0, H_SIZE, V_SIZE, s_strip);
}
