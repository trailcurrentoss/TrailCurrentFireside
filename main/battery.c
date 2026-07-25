/*
 * battery.c — ADC-based battery monitor for the Waveshare
 * ESP32-P4-WIFI6-Touch-LCD-7B.
 *
 * The Waveshare board has no fuel-gauge IC. V_bat is read via a 200k/100k
 * voltage divider into GPIO 20. See battery.h for the wiring and math.
 *
 * Downstream data pipeline:
 *
 *     1. Sample: 5 averaged reads 40 ms apart -> median.
 *     2. OCV lookup: median mV -> percent via LI_OCV_CURVE.
 *     3. EMA (alpha=0.35): softens curve-knee jitter.
 *     4. Freeze window: on a bat_state transition (IDLE<->CHARGING<->
 *        FULLY_CHARGED), suppress display updates for FREEZE_WINDOW_US so
 *        the user doesn't see terminal-voltage snap while the pack settles.
 *
 * bat_state is inferred from a rolling trend + near-full threshold
 * (there's no dedicated STAT pin):
 *
 *     V_bat >= V_BAT_MV_CHARGING (4.15 V) -> CHARGING (USB present /
 *                                            top-off / charge-complete
 *                                            with charger attached — the
 *                                            three cases the user reads
 *                                            as "bolt icon on")
 *     trend up   >= +10 mV over 30 s      -> CHARGING (latch)
 *     trend down <=  -5 mV over 30 s      -> IDLE     (latch)
 *     else                                -> keep previous state (hysteresis)
 *
 * With V_bat >= V_BAT_MV_FULL (4.20 V) we further label it as
 * FULLY_CHARGED so downstream code can distinguish "charging" from
 * "topped off but still plugged in" if it wants. The vars.c / TopBar
 * cluster today treats them identically (both -> bolt icon).
 */
#include "battery.h"

#include <stdatomic.h>
#include <string.h>

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lvgl.h"

static const char *TAG = "BATTERY";

/* --- Wiring / divider constants (see battery.h). ---------------------- */
#define BAT_ADC_GPIO         20
#define DIVIDER_NUM          3      /* (200 kΩ + 100 kΩ) / 100 kΩ = 3     */
#define DIVIDER_DEN          1

/* --- Sample & smoothing pipeline. ------------------------------------ */
#define VOLTAGE_SAMPLES      5
#define VOLTAGE_SAMPLE_MS    40     /* 5 * 40 ms = 200 ms sampling window */
#define SOC_EMA_ALPHA_NUM    35
#define SOC_EMA_ALPHA_DEN   100
#define POLL_PERIOD_MS       10000  /* full pipeline runs every 10 s      */
#define FREEZE_WINDOW_US     ((int64_t)60 * 1000 * 1000)   /* 60 s hold   */

/* --- Trend detector (no dedicated STAT/CHG pin on this board). -------- */
#define TREND_WINDOW         30     /* 30 samples at ~1 s each = 30 s     */
#define TREND_UP_MV          10
#define TREND_DOWN_MV       (-5)

/* --- Voltage thresholds (millivolts at the battery). ------------------ */
#define V_BAT_MV_FULL        4200   /* charge complete                     */
#define V_BAT_MV_CHARGING    4150   /* USB present / near-full latch       */
#define V_BAT_MV_EMPTY       3000   /* clamp SOC to 0 below this           */
#define V_BAT_MV_VISIBLE_MIN 2500   /* below this: no battery detected     */

/* --- setter externs (defined in vars.c) — declared here to avoid pulling
 *     in EEZ Studio's generated vars.h from a plain driver TU. */
extern void set_var_internal_battery_soc(int32_t percent);
extern void set_var_internal_battery_voltage(float volts);
extern void set_var_internal_charging(bool charging);

/* --- Li-Ion 1S OCV curve at ~25 °C, C/5 discharge. Voltages in mV,
 *     SoC in %. Descending so lookup can early-exit at the endpoints. */
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

/* Linear-interpolated OCV -> SoC. Clamped to curve endpoints. */
static int voltage_to_soc(int mv) {
    if (mv >= LI_OCV_CURVE[0].mv) return LI_OCV_CURVE[0].pct;
    for (size_t i = 0; i + 1 < LI_OCV_ROWS; i++) {
        int hi_mv = LI_OCV_CURVE[i].mv,  lo_mv = LI_OCV_CURVE[i + 1].mv;
        int hi_pc = LI_OCV_CURVE[i].pct, lo_pc = LI_OCV_CURVE[i + 1].pct;
        if (mv <= hi_mv && mv >= lo_mv) {
            return lo_pc + (mv - lo_mv) * (hi_pc - lo_pc) / (hi_mv - lo_mv);
        }
    }
    return LI_OCV_CURVE[LI_OCV_ROWS - 1].pct;
}

/* In-place ascending sort of a fixed 5-element int array. */
static void sort5(int v[VOLTAGE_SAMPLES]) {
    for (int i = 0; i < VOLTAGE_SAMPLES - 1; i++) {
        for (int j = 0; j < VOLTAGE_SAMPLES - 1 - i; j++) {
            if (v[j] > v[j + 1]) {
                int t = v[j]; v[j] = v[j + 1]; v[j + 1] = t;
            }
        }
    }
}

/* --- ADC state (touched only from init + battery_task). -------------- */
static adc_oneshot_unit_handle_t s_adc      = NULL;
static adc_channel_t             s_chan     = 0;
static adc_cali_handle_t         s_cali     = NULL;
static bool                      s_adc_ok   = false;

/* --- Trend detector state (touched only from battery_task). ---------- */
static int  s_trend[TREND_WINDOW];
static int  s_trend_head    = 0;
static int  s_trend_count   = 0;
static bool s_charging      = false;

/* --- Smoothing / cache state (touched from battery_task; cached values
 *     read by the LVGL timer via atomics). --------------------------- */
static _Atomic int32_t s_cached_pct       = -1;
static _Atomic int32_t s_cached_mv        = 0;
static _Atomic int32_t s_cached_charging  = 0;
static _Atomic bool    s_cached_dirty     = false;

static float   s_soc_smoothed    = -1.0f;
static uint8_t s_last_bat_state  = 0xFF;
static int64_t s_state_change_us = 0;

static void push_trend_sample(int mv_bat) {
    s_trend[s_trend_head] = mv_bat;
    s_trend_head = (s_trend_head + 1) % TREND_WINDOW;
    if (s_trend_count < TREND_WINDOW) s_trend_count++;
}

/* Waveshare charge-detection: near-full latch OR long-window trend.
 * Returns true when we think a charger is present (bolt icon on). */
static bool detect_charging(int mv_bat) {
    /* Absolute threshold: at or above ~4.15 V, the battery is either
     * mid-CC-CV, at CV top-off, or hovering at charge-complete-with-USB.
     * In all three cases the user reads it as "plugged in" — latch on. */
    if (mv_bat >= V_BAT_MV_CHARGING) {
        s_charging = true;
        return true;
    }

    /* Trend compares the current reading to the oldest in the 30 s ring.
     * Only fires once the ring is full so we don't false-trigger during
     * boot. Between UP/DOWN thresholds we latch to previous state
     * (hysteresis) so a plateaued reading doesn't flap. */
    if (s_trend_count >= TREND_WINDOW) {
        int oldest = s_trend[s_trend_head % TREND_WINDOW];
        int delta  = mv_bat - oldest;
        if      (delta >= TREND_UP_MV)   s_charging = true;
        else if (delta <= TREND_DOWN_MV) s_charging = false;
    }
    return s_charging;
}

/* Map (bat_voltage_mv, charging_flag) to the BATTERY_STATE_* enum used by
 * downstream freeze/hold logic. */
static uint8_t derive_bat_state(int mv_bat, bool charging) {
    if (mv_bat < V_BAT_MV_VISIBLE_MIN)  return BATTERY_STATE_NO_CHARGE;
    if (charging && mv_bat >= V_BAT_MV_FULL) return BATTERY_STATE_FULLY_CHARGED;
    if (charging)                       return BATTERY_STATE_CHARGING;
    return BATTERY_STATE_IDLE;
}

esp_err_t battery_init(void) {
    if (s_adc_ok) return ESP_OK;

    adc_unit_t unit;
    esp_err_t err = adc_oneshot_io_to_channel(BAT_ADC_GPIO, &unit, &s_chan);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "GPIO%d has no ADC channel on this chip: %s",
                 BAT_ADC_GPIO, esp_err_to_name(err));
        return err;
    }

    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id  = unit,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    err = adc_oneshot_new_unit(&init_cfg, &s_adc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "adc_oneshot_new_unit(unit=%d) failed: %s",
                 unit, esp_err_to_name(err));
        return err;
    }

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten    = ADC_ATTEN_DB_12,   /* full 0–~3.1 V range at the ADC pin */
    };
    err = adc_oneshot_config_channel(s_adc, s_chan, &chan_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "adc_oneshot_config_channel(GPIO%d): %s",
                 BAT_ADC_GPIO, esp_err_to_name(err));
        return err;
    }

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_cfg = {
        .unit_id  = unit,
        .chan     = s_chan,
        .atten    = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    if (adc_cali_create_scheme_curve_fitting(&cali_cfg, &s_cali) != ESP_OK) {
        s_cali = NULL;
    }
#endif

    s_adc_ok = true;
    ESP_LOGI(TAG, "battery ADC ready on GPIO%d (unit=%d ch=%d, %s)",
             BAT_ADC_GPIO, unit, s_chan,
             s_cali ? "calibrated" : "UNCALIBRATED");
    return ESP_OK;
}

/* Read one averaged voltage sample. Returns mV at the battery
 * (after divider correction), or -1 on ADC error. */
static int read_bat_mv_once(void) {
    if (!s_adc_ok) return -1;
    const int AVG_SAMPLES = 16;
    int sum_raw = 0, n = 0;
    for (int i = 0; i < AVG_SAMPLES; i++) {
        int raw = 0;
        if (adc_oneshot_read(s_adc, s_chan, &raw) == ESP_OK) {
            sum_raw += raw;
            n++;
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
    if (n == 0) return -1;
    int raw_avg = sum_raw / n;

    int mv_adc = 0;
    if (s_cali) {
        if (adc_cali_raw_to_voltage(s_cali, raw_avg, &mv_adc) != ESP_OK) {
            mv_adc = raw_avg * 3100 / 4095;
        }
    } else {
        mv_adc = raw_avg * 3100 / 4095;
    }
    return mv_adc * DIVIDER_NUM / DIVIDER_DEN;
}

esp_err_t battery_read(battery_info_t *out) {
    if (!out) return ESP_ERR_INVALID_ARG;
    if (!s_adc_ok) return ESP_ERR_INVALID_STATE;

    int mv_bat = read_bat_mv_once();
    if (mv_bat < 0) return ESP_FAIL;

    /* Do NOT push into the class-level trend detector from here — this
     * function is a one-shot public API and shouldn't disturb the
     * poll-task's rolling window. Use a local snapshot instead. */
    bool  chg   = (mv_bat >= V_BAT_MV_CHARGING);
    int   pct   = voltage_to_soc(mv_bat);
    if (mv_bat >= V_BAT_MV_FULL) pct = 100;
    if (pct > 100) pct = 100;
    if (pct < 0)   pct = 0;

    memset(out, 0, sizeof(*out));
    out->adc_voltage_mv = (uint32_t)(mv_bat / DIVIDER_NUM);
    out->bat_voltage_mv = (uint32_t)mv_bat;
    out->bat_level_pct  = (uint8_t)pct;
    out->bat_state      = derive_bat_state(mv_bat, chg);
    out->led_state      = 0;   /* not exposed on Waveshare board */
    return ESP_OK;
}

static void battery_poll_task(void *arg) {
    (void)arg;
    /* If battery_init failed (no ADC channel), exit quietly. UI will
     * show "--%" and the rest of the firmware boots normally. */
    if (!s_adc_ok) {
        ESP_LOGW(TAG, "poll task exiting — battery_init did not succeed");
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        /* Collect VOLTAGE_SAMPLES rapid samples for the median filter. */
        int samples_mv[VOLTAGE_SAMPLES] = {0};
        int good = 0;
        for (int i = 0; i < VOLTAGE_SAMPLES; i++) {
            int mv = read_bat_mv_once();
            if (mv < 0) break;
            samples_mv[good++] = mv;
            if (i < VOLTAGE_SAMPLES - 1) {
                vTaskDelay(pdMS_TO_TICKS(VOLTAGE_SAMPLE_MS));
            }
        }

        if (good < 3) {
            static bool warned = false;
            if (!warned) {
                ESP_LOGW(TAG, "ADC reads failing — reporting no battery");
                warned = true;
            }
            atomic_store(&s_cached_pct, -1);
            atomic_store(&s_cached_mv, 0);
            atomic_store(&s_cached_charging, 0);
            atomic_store(&s_cached_dirty, true);
            s_soc_smoothed = -1.0f;
            s_last_bat_state = 0xFF;
            vTaskDelay(pdMS_TO_TICKS(POLL_PERIOD_MS));
            continue;
        }

        sort5(samples_mv);
        int median_mv = samples_mv[good / 2];   /* good==5 -> index 2 */

        push_trend_sample(median_mv);
        bool    charging  = detect_charging(median_mv);
        uint8_t bat_state = derive_bat_state(median_mv, charging);

        /* OCV lookup with EMA smoothing. */
        int pct_ocv = voltage_to_soc(median_mv);
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
        if (median_mv >= V_BAT_MV_FULL) pct_smoothed = 100;
        if (pct_smoothed < 0)   pct_smoothed = 0;
        if (pct_smoothed > 100) pct_smoothed = 100;

        int64_t now_us = esp_timer_get_time();
        bool transition = (s_last_bat_state != 0xFF &&
                           bat_state != s_last_bat_state);
        if (transition) {
            s_state_change_us = now_us;
            ESP_LOGI(TAG,
                "bat_state %u -> %u, holding SoC for %llds",
                (unsigned)s_last_bat_state, (unsigned)bat_state,
                (long long)(FREEZE_WINDOW_US / 1000000));
        }
        s_last_bat_state = bat_state;
        bool in_freeze = s_state_change_us != 0 &&
                         (now_us - s_state_change_us) < FREEZE_WINDOW_US;

        /* Voltage + charging state publish immediately; SoC holds during
         * the freeze window. */
        atomic_store(&s_cached_mv,       (int32_t)median_mv);
        atomic_store(&s_cached_charging, charging ? 1 : 0);
        if (!in_freeze) {
            atomic_store(&s_cached_pct, pct_smoothed);
        }
        atomic_store(&s_cached_dirty, true);

        ESP_LOGD(TAG,
            "V_median=%d mV pct_ocv=%d pct_smoothed=%d state=%u%s chg=%d",
            median_mv, pct_ocv, pct_smoothed,
            (unsigned)bat_state, in_freeze ? " [FROZEN]" : "",
            (int)charging);

        vTaskDelay(pdMS_TO_TICKS(POLL_PERIOD_MS));
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
    lv_timer_create(battery_apply_cb, 500, NULL);
    if (xTaskCreate(battery_poll_task, "battery_poll",
                    3072, NULL, 1, NULL) != pdPASS) {
        ESP_LOGE(TAG, "xTaskCreate(battery_poll) failed");
    }
}
