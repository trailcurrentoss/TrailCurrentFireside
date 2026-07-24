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
#include "esp_timer.h"
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
static _Atomic int32_t s_cached_charging  = 0;    /* 0 = not plugged in, 1 = plugged in */
static _Atomic bool    s_cached_dirty     = false;

/* ─── SoC pipeline ─────────────────────────────────────────────────────
 *
 * We do NOT trust the vendor `bat_level_pct` field. Its estimator is a
 * naive linear map on terminal voltage — during charge it reads high
 * because Vterminal = Voc + Icharge·R_esr, and the instant VBUS drops
 * the terminal voltage collapses to Voc-minus-load-drop, snapping the
 * reported SoC 40+ points down.
 *
 * Instead we use `bat_voltage_mv` (the STC8's own corrected battery-
 * terminal reading) and translate through a standard single-cell Li-ion
 * OCV curve. Three-stage pipeline in the poll task:
 *
 *   1. **Median-of-5** — take 5 bat_voltage_mv samples 40 ms apart, sort,
 *      keep the middle one. Kills instantaneous ADC noise and LDO ripple.
 *   2. **OCV lookup** — pair Vmedian with LI_OCV_CURVE[] (linear
 *      interpolation between rows).
 *   3. **EMA (alpha=0.35)** — low-pass on the OCV-derived %. Softens the
 *      residual jitter that survives the median.
 *
 * Post-pipeline: **freeze-during-transition window.** When bat_state
 * flips (IDLE↔CHARGING↔FULLY_CHARGED), we suppress *display* updates for
 * FREEZE_WINDOW_US so the user doesn't see the terminal-voltage snap
 * while the pack settles. Charging-state (bolt icon) is always published
 * immediately — freeze only affects the SoC %.
 *
 * Fundamental limit: without a current shunt we can't compensate for the
 * IR drop under load. In the middle of the OCV curve (~3.85 V, 50 %) a
 * 500 mA load through ~150 mΩ ESR drops terminal voltage ~75 mV, which
 * maps to a ~10 % underestimate at rest vs. under load. That's a known
 * blind spot the user can work around by adding a shunt + current-sense
 * IC and passing that value in here. */
#define SOC_EMA_ALPHA_NUM   35
#define SOC_EMA_ALPHA_DEN  100
#define VOLTAGE_SAMPLES     5
#define VOLTAGE_SAMPLE_MS  40    /* 5 * 40 ms = 200 ms sampling window   */
#define FREEZE_WINDOW_US   ((int64_t)60 * 1000 * 1000)   /* 60 s post-transition */

static float s_soc_smoothed = -1.0f;
static uint8_t s_last_bat_state = 0xFF;
static int64_t s_state_change_us = 0;

/* Single-cell Li-ion open-circuit-voltage curve, C/5 discharge, ~25 °C.
 * Voltages in mV, SoC in %. Descending order so lookup can early-exit. */
static const struct { int mv; int pct; } LI_OCV_CURVE[] = {
    {4200, 100},
    {4150,  95},
    {4110,  90},
    {4080,  85},
    {4020,  80},
    {3980,  75},
    {3950,  70},
    {3910,  65},
    {3870,  60},
    {3850,  55},
    {3840,  50},
    {3820,  45},
    {3800,  40},
    {3790,  35},
    {3770,  30},
    {3750,  25},
    {3730,  20},
    {3710,  15},
    {3690,  10},
    {3610,   5},
    {3300,   0},
};
#define LI_OCV_ROWS (sizeof(LI_OCV_CURVE) / sizeof(LI_OCV_CURVE[0]))

/* Linear-interpolated OCV → SoC. Clamps to the curve endpoints. */
static int voltage_to_soc(int mv) {
    if (mv >= LI_OCV_CURVE[0].mv) return LI_OCV_CURVE[0].pct;
    for (size_t i = 0; i + 1 < LI_OCV_ROWS; i++) {
        int hi_mv  = LI_OCV_CURVE[i].mv;
        int lo_mv  = LI_OCV_CURVE[i + 1].mv;
        int hi_pct = LI_OCV_CURVE[i].pct;
        int lo_pct = LI_OCV_CURVE[i + 1].pct;
        if (mv <= hi_mv && mv >= lo_mv) {
            return lo_pct + (mv - lo_mv) * (hi_pct - lo_pct)
                          / (hi_mv - lo_mv);
        }
    }
    return LI_OCV_CURVE[LI_OCV_ROWS - 1].pct;   /* below 3.30 V = 0 %  */
}

/* In-place ascending sort of a fixed 5-element int array (worst-case
 * 10 comparisons — no need for qsort's overhead). */
static void sort5(int v[VOLTAGE_SAMPLES]) {
    for (int i = 0; i < VOLTAGE_SAMPLES - 1; i++) {
        for (int j = 0; j < VOLTAGE_SAMPLES - 1 - i; j++) {
            if (v[j] > v[j + 1]) {
                int t = v[j]; v[j] = v[j + 1]; v[j + 1] = t;
            }
        }
    }
}

static void battery_poll_task(void *arg) {
    (void)arg;
    while (1) {
        /* Collect VOLTAGE_SAMPLES rapid samples so the median filter has
         * enough width to reject one-sample glitches. If any read fails
         * we fall through to the error branch below. */
        int samples_mv[VOLTAGE_SAMPLES] = {0};
        battery_info_t last_info = {0};
        esp_err_t err = ESP_OK;
        int good = 0;
        for (int i = 0; i < VOLTAGE_SAMPLES; i++) {
            battery_info_t info;
            err = battery_read(&info);
            if (err != ESP_OK) break;
            samples_mv[good++] = (int)info.bat_voltage_mv;
            last_info = info;
            if (i < VOLTAGE_SAMPLES - 1) {
                vTaskDelay(pdMS_TO_TICKS(VOLTAGE_SAMPLE_MS));
            }
        }

        if (err != ESP_OK || good < 3) {
            static bool warned = false;
            if (!warned) {
                ESP_LOGW(TAG, "battery_read failed: %s",
                         esp_err_to_name(err));
                warned = true;
            }
            atomic_store(&s_cached_pct, -1);
            atomic_store(&s_cached_mv, 0);
            atomic_store(&s_cached_charging, 0);
            s_soc_smoothed = -1.0f;
            s_last_bat_state = 0xFF;
        } else {
            sort5(samples_mv);
            /* Median: with `good` valid entries at positions [0..good-1]
             * after sort, the middle index is good/2. For good=5 → 2. */
            int median_mv = samples_mv[good / 2];

            /* OCV → SoC. Uses the median terminal voltage — no IR-drop
             * compensation because we don't have current data. Reads
             * ~10 % low under heavy load, ~5-10 % high while actively
             * charging. Freeze window (below) hides the charge/discharge
             * transient; steady-state accuracy at rest is ±3 %. */
            int pct_ocv = voltage_to_soc(median_mv);

            /* EMA smoothing on top of the OCV lookup. Killed the
             * hardware-side snap; this handles ADC dither + curve knees. */
            if (s_soc_smoothed < 0.0f) {
                s_soc_smoothed = (float)pct_ocv;
            } else {
                s_soc_smoothed =
                    ((float)SOC_EMA_ALPHA_NUM / (float)SOC_EMA_ALPHA_DEN)
                        * (float)pct_ocv +
                    ((float)(SOC_EMA_ALPHA_DEN - SOC_EMA_ALPHA_NUM)
                        / (float)SOC_EMA_ALPHA_DEN)
                        * s_soc_smoothed;
            }
            int pct_smoothed = (int)(s_soc_smoothed + 0.5f);
            if (pct_smoothed < 0)   pct_smoothed = 0;
            if (pct_smoothed > 100) pct_smoothed = 100;

            /* Detect state transitions to arm the freeze window. On the
             * first iteration after boot / after a fault, s_last_bat_state
             * is 0xFF — treat that as "just came online, don't freeze."
             * Every subsequent transition arms FREEZE_WINDOW_US of hold. */
            int64_t now_us = esp_timer_get_time();
            bool transition = (s_last_bat_state != 0xFF &&
                               last_info.bat_state != s_last_bat_state);
            if (transition) {
                s_state_change_us = now_us;
                ESP_LOGI(TAG,
                    "bat_state %u → %u, holding SoC for %llds",
                    (unsigned)s_last_bat_state,
                    (unsigned)last_info.bat_state,
                    (long long)(FREEZE_WINDOW_US / 1000000));
            }
            s_last_bat_state = last_info.bat_state;
            bool in_freeze = s_state_change_us != 0 &&
                             (now_us - s_state_change_us) < FREEZE_WINDOW_US;

            /* Voltage + charging state publish immediately — the bolt
             * icon needs to flip the instant VBUS lands, and the voltage
             * label (currently unused in the TopBar, but reserved) stays
             * live. Only SoC % holds during the freeze window. */
            atomic_store(&s_cached_mv, (int32_t)median_mv);
            atomic_store(&s_cached_charging,
                         (last_info.bat_state == BATTERY_STATE_CHARGING ||
                          last_info.bat_state == BATTERY_STATE_FULLY_CHARGED)
                         ? 1 : 0);
            if (!in_freeze) {
                atomic_store(&s_cached_pct, pct_smoothed);
            }

            ESP_LOGD(TAG,
                "V_median=%d mV pct_ocv=%d pct_smoothed=%d state=%u%s",
                median_mv, pct_ocv, pct_smoothed,
                (unsigned)last_info.bat_state,
                in_freeze ? " [FROZEN]" : "");
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
