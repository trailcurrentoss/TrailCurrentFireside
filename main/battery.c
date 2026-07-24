/*
 * STC8H1KXX I2C driver for the on-board Elecrow battery on the CrowPanel
 * Advance ESP32-P4 10.1". The STC8H1KXX is an auxiliary MCU that owns the
 * TP4059-driven charging circuit and exposes battery telemetry through a
 * small I2C register bank at address 0x2F. Reg 0x00 is the start of a
 * Battery_info_t block; the vendor firmware reads it one byte at a time.
 * We mirror that access pattern for byte-order compatibility.
 */
#include "battery.h"

#include <stdatomic.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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

/* ============================================================
 * Poll pipeline — battery_read() is an I2C transaction and must not run
 * on the LVGL task (it would stall touch and rendering for the duration
 * of the transfer). Same split as the WiFi RSSI poller in vars.c:
 *
 *   - battery_poll_task: dedicated FreeRTOS task, does the I2C read
 *     every 10 s and stores the result to an atomic cache.
 *   - battery_apply_cb: LVGL timer, checks the dirty flag and calls the
 *     set_var_internal_* setters with the cached values. Runs on the
 *     LVGL task so the setters remain lock-safe.
 *
 * On a read failure the cache is populated with pct=-1 and charging=false
 * so the TopBar cluster falls back to "--%" — matches the pre-split
 * behaviour when no STC8H1KXX is present.
 * ============================================================ */

static _Atomic int32_t s_cached_pct       = -1;
static _Atomic int32_t s_cached_mv        = 0;
static _Atomic int32_t s_cached_charging  = 0;    /* 0 = not charging, 1 = charging */
static _Atomic bool    s_cached_dirty     = false;

static void battery_poll_task(void *arg) {
    (void)arg;
    while (1) {
        battery_info_t info;
        esp_err_t err = battery_read(&info);
        if (err != ESP_OK) {
            static bool warned = false;
            if (!warned) {
                ESP_LOGW(TAG, "battery_read failed: %s", esp_err_to_name(err));
                warned = true;
            }
            atomic_store(&s_cached_pct, -1);
            atomic_store(&s_cached_mv, 0);
            atomic_store(&s_cached_charging, 0);
        } else {
            /* Cap to 0..100 — the vendor firmware occasionally reports 101
             * while a cell is topping off. */
            int pct = info.bat_level_pct;
            if (pct > 100) pct = 100;
            atomic_store(&s_cached_pct, pct);
            atomic_store(&s_cached_mv, (int32_t)info.bat_voltage_mv);
            atomic_store(&s_cached_charging,
                         info.bat_state == BATTERY_STATE_CHARGING ? 1 : 0);
            ESP_LOGD(TAG, "SOC=%d%% V=%lu mV state=%u",
                     pct, (unsigned long)info.bat_voltage_mv,
                     (unsigned)info.bat_state);
        }
        atomic_store(&s_cached_dirty, true);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

static void battery_apply_cb(lv_timer_t *t) {
    (void)t;
    if (!atomic_exchange(&s_cached_dirty, false)) return;
    int32_t pct = atomic_load(&s_cached_pct);
    int32_t mv  = atomic_load(&s_cached_mv);
    bool    chg = atomic_load(&s_cached_charging) != 0;
    set_var_internal_battery_soc(pct);
    if (pct >= 0) set_var_internal_battery_voltage(mv / 1000.0f);
    set_var_internal_charging(chg);
}

void init_battery_poll(void) {
    /* Apply timer runs on the LVGL task at 500 ms so the initial cache
     * value from the poller's first read lands promptly. The callback
     * is nearly free when the dirty flag isn't set. */
    lv_timer_create(battery_apply_cb, 500, NULL);
    /* 3 KB stack: battery_read is a tight loop over i2c_read_reg; no
     * deep call stacks. Priority 1 keeps it well below the LVGL and
     * network tasks. */
    if (xTaskCreate(battery_poll_task, "battery_poll",
                    3072, NULL, 1, NULL) != pdPASS) {
        ESP_LOGE(TAG, "xTaskCreate(battery_poll) failed");
    }
}
