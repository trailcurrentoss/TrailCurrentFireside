/*
 * STC8H1KXX I2C driver for the on-board Elecrow battery on the CrowPanel
 * Advance ESP32-P4 10.1". The STC8H1KXX is an auxiliary MCU that owns the
 * TP4059-driven charging circuit and exposes battery telemetry through a
 * small I2C register bank at address 0x2F. Reg 0x00 is the start of a
 * Battery_info_t block; the vendor firmware reads it one byte at a time.
 * We mirror that access pattern for byte-order compatibility.
 */
#include "battery.h"

#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "lvgl.h"

#include "bsp_i2c.h"

static const char *TAG = "BATTERY";

#define STC8H1KXX_I2C_ADDR    0x2F
#define STC8_REG_BATTERY      0x00

static i2c_master_dev_handle_t s_dev = NULL;

/* set_var_internal_* live in vars.c; declared extern here to avoid pulling
 * in EEZ's generated vars.h from a plain driver TU. */
extern void set_var_internal_battery_soc(int32_t percent);
extern void set_var_internal_battery_voltage(float volts);
extern void set_var_internal_charging(bool charging);

esp_err_t battery_init(void) {
    if (s_dev) return ESP_OK;
    s_dev = i2c_dev_register(STC8H1KXX_I2C_ADDR);
    if (!s_dev) {
        ESP_LOGE(TAG, "i2c_dev_register(0x%02X) failed", STC8H1KXX_I2C_ADDR);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "STC8H1KXX registered @ 0x%02X", STC8H1KXX_I2C_ADDR);
    return ESP_OK;
}

esp_err_t battery_read(battery_info_t *out) {
    if (!s_dev || !out) return ESP_ERR_INVALID_STATE;
    memset(out, 0, sizeof(*out));
    uint8_t *p = (uint8_t *)out;
    for (size_t i = 0; i < sizeof(*out); i++) {
        esp_err_t err = i2c_read_reg(s_dev, STC8_REG_BATTERY + i, p + i, 1);
        if (err != ESP_OK) return err;
    }
    return ESP_OK;
}

static void battery_poll_cb(lv_timer_t *t) {
    (void)t;
    battery_info_t info;
    esp_err_t err = battery_read(&info);
    if (err != ESP_OK) {
        /* No STC8H1KXX response — no battery board or comms error. Show the
         * cluster as unknown so the user can tell the reading is not live. */
        static bool warned = false;
        if (!warned) {
            ESP_LOGW(TAG, "battery_read failed: %s", esp_err_to_name(err));
            warned = true;
        }
        set_var_internal_battery_soc(-1);
        set_var_internal_charging(false);
        return;
    }

    /* Cap to 0..100 — the vendor firmware occasionally reports 101 while a
     * cell is topping off. */
    int pct = info.bat_level_pct;
    if (pct > 100) pct = 100;

    bool charging = (info.bat_state == BATTERY_STATE_CHARGING);
    set_var_internal_battery_soc(pct);
    set_var_internal_battery_voltage(info.bat_voltage_mv / 1000.0f);
    set_var_internal_charging(charging);

    ESP_LOGD(TAG, "SOC=%d%% V=%lu mV state=%u",
             pct, (unsigned long)info.bat_voltage_mv,
             (unsigned)info.bat_state);
}

void init_battery_poll(void) {
    /* One immediate paint so the boot placeholder ("--%") swaps to a real
     * reading as soon as I2C responds, without waiting the full poll period. */
    battery_poll_cb(NULL);
    lv_timer_create(battery_poll_cb, 10000, NULL);
}
