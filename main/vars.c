/*
 * vars.c — MQTT-fed state layer for Fireside CrowPanel.
 *
 * Two responsibilities:
 *
 * 1. **Ring buffers** — every metric surfaced on the Power or Air Quality
 *    sparklines keeps its last METRIC_HISTORY_LEN samples in a ring, indexed
 *    per-minute (last-write-wins per minute bucket). This is what makes the
 *    "rolling 60-min window" feature of the redesign real: on boot the ring
 *    is empty (each slot = METRIC_MISSING), it fills as MQTT messages arrive
 *    from Ampline/Borealis/Reservoir, and the sparkline canvases pull the
 *    latest 60 samples any time they redraw.
 *
 * 2. **UI text setters** — when a value lands, the setter updates the
 *    corresponding value label on the Power/Air/Water/Trailer screen and
 *    pushes the sample into the ring. Setters are safe to call from any task
 *    — they assume the caller (mqtt_client_process_messages) already holds
 *    the display lock, which matches the Fireside contract.
 *
 * This is the source of truth for the "no local state" rule from the parent
 * CLAUDE.md: MQTT is authoritative, we mirror it into the UI, we do NOT keep
 * per-widget booleans that could drift from the MQTT stream.
 */

#include "mqtt_vars.h"
#include "alarms.h"
#include "audio.h"
#include "app_state.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#include "esp_wifi.h"
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lvgl.h"

#if __has_include("ui/screens.h")
#include "screens.h"
#include "ui.h"
#include "vars.h"
#endif

static const char *TAG = "VARS";

/* ============================================================
 * lv_label_set_text invalidates the widget unconditionally in LVGL 8,
 * even when the new string equals the old one. That's a hidden cost on
 * fan-out clusters like the six-topbar labels — a 5s poll would trigger
 * six redraws every tick on an otherwise idle screen. This helper reads
 * the label's current text and skips the set when nothing changed, so
 * the redraw only happens when the value actually moves.
 * ============================================================ */
#if __has_include("ui/screens.h")
static inline void label_set_text_if_changed(lv_obj_t *lbl, const char *text) {
    if (!lbl || !text) return;
    const char *cur = lv_label_get_text(lbl);
    if (cur && strcmp(cur, text) == 0) return;
    lv_label_set_text(lbl, text);
}
#endif

/* ============================================================
 * Ring buffers
 * ============================================================ */

typedef struct {
    int16_t  samples[METRIC_HISTORY_LEN];
    int64_t  last_bucket_us;  /* esp_timer time of the last minute bucket */
    uint8_t  count;           /* how many valid samples (capped at LEN) */
} metric_ring_t;

static metric_ring_t s_rings[METRIC_COUNT] = {0};

/* Forward decl — implemented in the "Metric charts" section below.
 * metric_push() calls this after every ring update so the on-screen
 * chart (if attached) receives the new sample. Safe no-op before
 * init_metric_charts() has been called. */
static void chart_push_sample(metric_id_t id, int16_t value);

static void metric_init(void) {
    for (int m = 0; m < METRIC_COUNT; m++) {
        for (int i = 0; i < METRIC_HISTORY_LEN; i++) {
            s_rings[m].samples[i] = METRIC_MISSING;
        }
    }
}

static void metric_push(metric_id_t id, int16_t value) {
    if (id < 0 || id >= METRIC_COUNT) return;
    metric_ring_t *r = &s_rings[id];
    int64_t now = esp_timer_get_time();

    /* First sample ever — start the bucket clock but do NOT push a bar.
     * The chart stays empty (24 LV_CHART_POINT_NONE slots) until this
     * first bucket completes 2.5 min from now. */
    if (r->last_bucket_us == 0) {
        r->samples[METRIC_HISTORY_LEN - 1] = value;
        r->count = 1;
        r->last_bucket_us = now;
        return;
    }

    int64_t elapsed = now - r->last_bucket_us;

    /* Same bucket — last-write-wins on the in-progress slot; the chart
     * does NOT shift. Bars step forward by exactly one every 2.5 min. */
    if (elapsed < METRIC_BUCKET_US) {
        r->samples[METRIC_HISTORY_LEN - 1] = value;
        if (r->count == 0) r->count = 1;
        return;
    }

    /* Bucket rolled over. The value that was in slot N-1 represents the
     * bucket that just closed — that's the bar we push. If multiple
     * buckets elapsed (WiFi drop, sensor stall), push MISSING for each
     * skipped one so gaps show as blanks in the chart. */
    int16_t completed_value = r->samples[METRIC_HISTORY_LEN - 1];
    chart_push_sample(id, completed_value);

    int rolls = (int)(elapsed / METRIC_BUCKET_US);
    if (rolls > METRIC_HISTORY_LEN) rolls = METRIC_HISTORY_LEN;
    for (int g = 1; g < rolls; g++) {
        chart_push_sample(id, METRIC_MISSING);
    }

    for (int i = 0; i < METRIC_HISTORY_LEN - rolls; i++) {
        r->samples[i] = r->samples[i + rolls];
    }
    for (int i = METRIC_HISTORY_LEN - rolls; i < METRIC_HISTORY_LEN - 1; i++) {
        r->samples[i] = METRIC_MISSING;
    }
    r->samples[METRIC_HISTORY_LEN - 1] = value;
    r->last_bucket_us += (int64_t)rolls * METRIC_BUCKET_US;
    if (r->count < METRIC_HISTORY_LEN) {
        r->count = (uint8_t)((r->count + rolls > METRIC_HISTORY_LEN)
                             ? METRIC_HISTORY_LEN
                             : r->count + rolls);
    }
}

int metric_history_get(metric_id_t id, int16_t *out) {
    if (id < 0 || id >= METRIC_COUNT || !out) return 0;
    memcpy(out, s_rings[id].samples,
           sizeof(int16_t) * METRIC_HISTORY_LEN);
    return s_rings[id].count;
}

int16_t metric_last(metric_id_t id) {
    if (id < 0 || id >= METRIC_COUNT) return METRIC_MISSING;
    return s_rings[id].samples[METRIC_HISTORY_LEN - 1];
}

/* ============================================================
 * Metric charts — one lv_chart per authored `<page>_<metric>_chart` panel.
 *
 * The chart region panels are authored in the .eez-project as transparent
 * containers (see build_air_page / _sparkline_card in gen_eez_project.py).
 * At init we create an lv_chart child inside each and configure the metric-
 * specific color, y-range, and shift-update mode.
 *
 * On every metric_push() we call chart_push_sample() which forwards the
 * new value to the matching chart via lv_chart_set_next_value. The chart
 * ring buffer inside lv_chart is separate from our s_rings ring, but they
 * receive the same samples in the same order so they stay in sync. On
 * init we prime the chart from the existing s_rings so a screen swipe
 * back to Air/Power shows the samples that arrived while another page was
 * visible.
 * ============================================================ */

#if __has_include("ui/screens.h")

typedef struct {
    metric_id_t   id;
    lv_obj_t    **parent_ref;      /* &objects.<page>_<metric>_chart */
    uint32_t      color_rgb888;    /* baked so we don't lookup palette */
    int32_t       y_min, y_max;    /* per-metric fixed Y range */
    lv_obj_t     *chart_obj;
    lv_chart_series_t *series;
} metric_chart_binding_t;

static metric_chart_binding_t s_metric_charts[] = {
    /* Air metrics — colors match spec §9. Y-ranges tightened to the
     * typical observed band so bars are visibly proportional; edge
     * values overflow at the top which is fine for a history view. */
    { METRIC_TEMP,  &objects.air_temp_chart,   0xFF5453,    40,  100, NULL, NULL },
    { METRIC_HUM,   &objects.air_hum_chart,    0x48E6FE,    20,   80, NULL, NULL },
    /* SGP41 clean-air baseline is 400 ppm; with y_min=400 every bar at
     * baseline renders at zero height and the chart looks empty even
     * though samples are landing. Pull y_min to 0 so the baseline shows
     * as a visible bar (~27% of chart height) and rises/falls read as
     * proportional deltas. */
    { METRIC_ECO2,  &objects.air_eco2_chart,   0x52A441,     0, 1500, NULL, NULL },
    { METRIC_TVOC,  &objects.air_tvoc_chart,   0xFFC107,     0,  500, NULL, NULL },
    { METRIC_CO,    &objects.air_co_chart,     0x505050,     0,   20, NULL, NULL },
    /* Power metrics. VOLTS is stored ×100 in the ring (int16 packing). */
    { METRIC_SOLAR, &objects.power_solar_chart, 0xFFC107,    0,  800, NULL, NULL },
    { METRIC_SOC,   &objects.power_soc_chart,   0x52A441,    0,  100, NULL, NULL },
    { METRIC_VOLTS, &objects.power_volts_chart, 0x505050, 1200, 1450, NULL, NULL },
    { METRIC_LOAD,  &objects.power_load_chart,  0x48E6FE,    0,  500, NULL, NULL },
};

static void chart_push_sample(metric_id_t id, int16_t value) {
    for (size_t i = 0; i < sizeof(s_metric_charts)/sizeof(*s_metric_charts); i++) {
        metric_chart_binding_t *b = &s_metric_charts[i];
        if (b->id != id || !b->chart_obj || !b->series) continue;
        lv_coord_t v = (value == METRIC_MISSING) ? LV_CHART_POINT_NONE
                                                 : (lv_coord_t)value;
        lv_chart_set_next_value(b->chart_obj, b->series, v);
        /* lv_chart_set_next_value already invalidates the chart
         * internally on LVGL 8.4 — no need for an explicit refresh
         * / invalidate here. Since we now only call this once per
         * 2.5-min bucket rollover, the redraw fires within a frame
         * of the push without a helper kick. */
    }
}

void init_metric_charts(void) {
    int16_t history[METRIC_HISTORY_LEN];
    for (size_t i = 0; i < sizeof(s_metric_charts)/sizeof(*s_metric_charts); i++) {
        metric_chart_binding_t *b = &s_metric_charts[i];
        lv_obj_t *parent = *(b->parent_ref);
        if (!parent) continue;

        /* Runtime-created chart — geometry APIs on this object are fine
         * because it's not an EEZ Studio-authored `objects.<w>` symbol.
         * Trap 5 (canvas divergence) applies only to authored widgets. */
        lv_obj_t *chart = lv_chart_create(parent);
        lv_obj_set_pos(chart, 0, 0);
        lv_obj_set_size(chart, lv_pct(100), lv_pct(100));
        lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
        lv_chart_set_point_count(chart, METRIC_HISTORY_LEN);
        lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);
        lv_chart_set_div_line_count(chart, 0, 0);
        lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y,
                           b->y_min, b->y_max);

        /* Flat visual: no chart background, no border, no ticks. */
        lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_border_width(chart, 0, LV_PART_MAIN);
        lv_obj_set_style_pad_all(chart, 0, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(chart, 0, LV_PART_MAIN);
        /* 1 px inter-bar gap so the 60 packed bars stay visually
         * distinct at ~5–6 px slot width. */
        lv_obj_set_style_pad_column(chart, 1, LV_PART_MAIN);

        lv_color_t bar_color = lv_color_hex(b->color_rgb888);
        lv_chart_series_t *ser = lv_chart_add_series(chart, bar_color,
                                                    LV_CHART_AXIS_PRIMARY_Y);

        /* Bar fill: solid metric color at 85 % opa (spec §9 — 0.85·255
         * = 217), no border, square corners. */
        lv_obj_set_style_bg_color(chart, bar_color, LV_PART_ITEMS);
        lv_obj_set_style_bg_opa(chart, 217, LV_PART_ITEMS);
        lv_obj_set_style_border_width(chart, 0, LV_PART_ITEMS);
        lv_obj_set_style_radius(chart, 0, LV_PART_ITEMS);

        b->chart_obj = chart;
        b->series    = ser;

        /* Start empty — every bar slot is LV_CHART_POINT_NONE so the
         * bar chart draws no bars. The first bar appears 2.5 min after
         * the first sample arrives (see metric_push()), and thereafter
         * one new bar appears per 2.5 min bucket.
         *
         * Note: at boot init_metric_charts() runs BEFORE MQTT connects
         * so the ring is empty anyway; the explicit NONE-fill also
         * clears any stray defaults LVGL set on `lv_chart_add_series`. */
        (void)history;
        for (int k = 0; k < METRIC_HISTORY_LEN; k++) {
            lv_chart_set_next_value(chart, ser, LV_CHART_POINT_NONE);
        }
    }
}

#else  /* ui/screens.h not yet exported — stub the forward decl */

static void chart_push_sample(metric_id_t id, int16_t value) {
    (void)id; (void)value;
}
void init_metric_charts(void) {}

#endif

/* ============================================================
 * Temperature unit + conversion
 * ============================================================ */

static int32_t s_temp_unit = 0;  /* 0=F, 1=C */
int32_t get_var_temperature_unit(void) { return s_temp_unit; }

static int32_t f_to_display(int32_t f) {
    return (s_temp_unit == 1) ? ((f - 32) * 5 / 9) : f;
}

/* Forward decls of UI painters (repaint the whole screen when the unit
 * toggles or the sample lands). */
static void paint_air_temp(void);
static void paint_notif_badge(void);

/* Toaster refresh — repopulates whichever notification toaster is currently
 * open so the row list tracks the current active-alarm set. Called from
 * paint_notif_badge on every tick that also updates the badge count. */
#if __has_include("ui/screens.h")
static void toaster_refresh_if_open(void);
#endif

void set_var_temperature_unit(int32_t value) {
    s_temp_unit = value ? 1 : 0;
    /* Repaint temp readouts everywhere the F/C toggle matters. */
    paint_air_temp();
}

/* ============================================================
 * Devices — 8 tiles on Home screen
 * ============================================================ */

static int32_t s_device_brightness[8] = {0};
int32_t get_var_device_brightness(int32_t device_id) {
    if (device_id < 1 || device_id > 8) return 0;
    return s_device_brightness[device_id - 1];
}

static void apply_device_state(int idx, int32_t value) {
    if (idx < 1 || idx > 8) return;
    s_device_brightness[idx - 1] = value;
#if __has_include("ui/screens.h")
    lv_obj_t *btns[8] = {
        objects.home_dev1, objects.home_dev2, objects.home_dev3, objects.home_dev4,
        objects.home_dev5, objects.home_dev6, objects.home_dev7, objects.home_dev8,
    };
    lv_obj_t *btn = btns[idx - 1];
    if (!btn) return;
    if (value > 0) lv_obj_add_state(btn, LV_STATE_CHECKED);
    else           lv_obj_clear_state(btn, LV_STATE_CHECKED);
#endif
}

void set_var_device01_status(int32_t v) { apply_device_state(1, v); }
void set_var_device02_status(int32_t v) { apply_device_state(2, v); }
void set_var_device03_status(int32_t v) { apply_device_state(3, v); }
void set_var_device04_status(int32_t v) { apply_device_state(4, v); }
void set_var_device05_status(int32_t v) { apply_device_state(5, v); }
void set_var_device06_status(int32_t v) { apply_device_state(6, v); }
void set_var_device07_status(int32_t v) {
    /* Device 7 is the water pump — mirror to the Water screen's pump button
     * so the "on/off" state stays in sync whether the trigger came from the
     * home tile, the water pump button, or an external MQTT publish. */
    apply_device_state(7, v);
#if __has_include("ui/screens.h")
    bool on = (v > 0);
    if (objects.water_pump_btn) {
        if (on) lv_obj_add_state(objects.water_pump_btn, LV_STATE_CHECKED);
        else    lv_obj_clear_state(objects.water_pump_btn, LV_STATE_CHECKED);
    }
    if (objects.water_pump_state) {
        lv_label_set_text(objects.water_pump_state, on ? "ON" : "OFF");
    }
#endif
}
void set_var_device08_status(int32_t v) { apply_device_state(8, v); }

static int32_t s_current_dev_bri_id = 0;
int32_t get_var_current_device_brightness_identifier(void) { return s_current_dev_bri_id; }
void    set_var_current_device_brightness_identifier(int32_t v) { s_current_dev_bri_id = v; }

/* ============================================================
 * Power — battery / solar / load
 * ============================================================ */

static int32_t s_battery_soc = 0;
static float   s_battery_v   = 0.0f;
static int32_t s_solar_w     = 0;
static int32_t s_load_w      = 0;

void set_var_battery_soc(int32_t percent) {
    s_battery_soc = percent;
    alarms_apply_battery(percent);
    metric_push(METRIC_SOC, (int16_t)percent);
#if __has_include("ui/screens.h")
    char buf[8]; snprintf(buf, sizeof(buf), "%ld", (long)percent);
    if (objects.power_soc_value) lv_label_set_text(objects.power_soc_value, buf);
    if (objects.home_pwr_batt_value) lv_label_set_text(objects.home_pwr_batt_value, buf);
    if (objects.home_pwr_batt_arc) lv_arc_set_value(objects.home_pwr_batt_arc, (int16_t)percent);
    /* Home battery color = green normally, Danger when SOC < 30 (spec §2a) */
    lv_color_t c = (percent < 30) ? lv_palette_main(LV_PALETTE_RED)
                                  : lv_color_hex(0x52A441);
    /* Use hex-literal for AccentPrimary green so dark theme still shows the
     * base green here (SOC danger is a status color, theme-invariant). */
    if (percent < 30) c = lv_color_hex(0xFF5453);
    if (objects.home_pwr_batt_icon)
        lv_obj_set_style_text_color(objects.home_pwr_batt_icon, c, LV_PART_MAIN);
    if (objects.home_pwr_batt_value)
        lv_obj_set_style_text_color(objects.home_pwr_batt_value, c, LV_PART_MAIN);
    if (objects.home_pwr_batt_arc)
        lv_obj_set_style_arc_color(objects.home_pwr_batt_arc, c, LV_PART_INDICATOR);
#endif
}

void set_var_battery_voltage(float volts) {
    s_battery_v = volts;
    metric_push(METRIC_VOLTS, (int16_t)(volts * 100.0f));
#if __has_include("ui/screens.h")
    if (objects.power_volts_value) {
        char buf[8]; snprintf(buf, sizeof(buf), "%.1f", (double)volts);
        lv_label_set_text(objects.power_volts_value, buf);
    }
#endif
}

void set_var_solar_watts(int32_t watts) {
    s_solar_w = watts;
    metric_push(METRIC_SOLAR, (int16_t)watts);
#if __has_include("ui/screens.h")
    char buf[8]; snprintf(buf, sizeof(buf), "%ld", (long)watts);
    if (objects.power_solar_value) lv_label_set_text(objects.power_solar_value, buf);
    if (objects.home_pwr_solar_value) lv_label_set_text(objects.home_pwr_solar_value, buf);
    if (objects.home_pwr_solar_arc) {
        int32_t clamped = watts < 0 ? 0 : (watts > 1500 ? 1500 : watts);
        lv_arc_set_value(objects.home_pwr_solar_arc, (int16_t)clamped);
    }
#endif
}

void set_var_solar_status(const char *status) {
#if __has_include("ui/screens.h")
    if (objects.power_charge_type && status) {
        lv_label_set_text(objects.power_charge_type, status);
    }
#else
    (void)status;
#endif
}

void set_var_consumption_watts(int32_t watts) {
    s_load_w = watts;
    metric_push(METRIC_LOAD, (int16_t)watts);
#if __has_include("ui/screens.h")
    if (objects.power_load_value) {
        char buf[8]; snprintf(buf, sizeof(buf), "%ld", (long)watts);
        lv_label_set_text(objects.power_load_value, buf);
    }
    if (objects.power_time_load) {
        char buf[24]; snprintf(buf, sizeof(buf), "at %ld W draw", (long)watts);
        lv_label_set_text(objects.power_time_load, buf);
    }
#endif
}

void set_var_time_remaining(int32_t minutes) {
#if __has_include("ui/screens.h")
    if (objects.power_time_value) {
        /* Compact 3-tier format:
         *   < 60 min      → "45m"
         *   < 24 h        → "3h 15m"
         *   >= 24 h       → "10d 2h"
         * A raw 240h (10 days) reads as "10d", not "240h" (spec §6 style
         * — treat time as the largest unit that fits without truncating). */
        char buf[16];
        if (minutes < 60) {
            snprintf(buf, sizeof(buf), "%dm", (int)minutes);
        } else if (minutes < 24 * 60) {
            snprintf(buf, sizeof(buf), "%dh %dm",
                     (int)(minutes / 60), (int)(minutes % 60));
        } else {
            int days = minutes / (24 * 60);
            int hh   = (minutes % (24 * 60)) / 60;
            if (hh) snprintf(buf, sizeof(buf), "%dd %dh", days, hh);
            else    snprintf(buf, sizeof(buf), "%dd", days);
        }
        lv_label_set_text(objects.power_time_value, buf);
    }
#else
    (void)minutes;
#endif
}

/* ============================================================
 * On-board (Elecrow STC8H1KXX) battery — TopBar cluster.
 * Percent < 0 signals "unknown" (I2C read failed / no battery).
 * ============================================================ */

static int32_t s_int_battery_pct = -1;
static float   s_int_battery_v   = 0.0f;
static bool    s_int_charging    = false;

/* FA5 battery glyphs, roughly matching the SOC range they represent. */
static const char *battery_glyph_for(int pct) {
    if (pct < 0)  return "\xEF\x89\x84"; /* U+F244 empty when unknown */
    if (pct >= 88) return "\xEF\x89\x80"; /* U+F240 full   */
    if (pct >= 63) return "\xEF\x89\x81"; /* U+F241 3/4    */
    if (pct >= 38) return "\xEF\x89\x82"; /* U+F242 half   */
    if (pct >= 13) return "\xEF\x89\x83"; /* U+F243 1/4    */
    return             "\xEF\x89\x84"; /* U+F244 empty  */
}

void set_var_internal_battery_soc(int32_t percent) {
    s_int_battery_pct = percent;
#if __has_include("ui/screens.h")
    /* Cached-last-value guard: the 10 s poller re-invokes the setter every
     * tick with the same value while the battery is quiet, and the icon
     * color is also a function of s_int_charging (see set_var_internal_charging
     * — it flips low_warn red vs charging green without SOC moving). Skip
     * only when BOTH inputs match the last paint. */
    static int32_t s_last_pct_painted      = INT32_MIN;
    static bool    s_last_charging_painted = false;
    static bool    s_first_paint           = true;
    if (!s_first_paint
        && percent == s_last_pct_painted
        && s_int_charging == s_last_charging_painted) {
        return;
    }
    s_first_paint           = false;
    s_last_pct_painted      = percent;
    s_last_charging_painted = s_int_charging;

    /* Fan out across every page that carries the TopBar. The wizard pages
     * (WiFi/MQTT setup, Alarms, Edit Buttons, Button Edit) are visited
     * during setup and user-facing configuration flows and must show the
     * same live status as the main pages. */
    lv_obj_t *pct_labels[] = {
        objects.home_topbar__topbar_battery_pct,
        objects.trailer_topbar__topbar_battery_pct,
        objects.power_topbar__topbar_battery_pct,
        objects.water_topbar__topbar_battery_pct,
        objects.air_topbar__topbar_battery_pct,
        objects.settings_topbar__topbar_battery_pct,
        objects.page_wifi_setup_topbar__topbar_battery_pct,
        objects.page_wifi_connecting_topbar__topbar_battery_pct,
        objects.page_mqtt_setup_topbar__topbar_battery_pct,
        objects.page_mqtt_connecting_topbar__topbar_battery_pct,
        objects.page_alarms_topbar__topbar_battery_pct,
        objects.page_edit_buttons_topbar__topbar_battery_pct,
        objects.page_button_edit_topbar__topbar_battery_pct,
    };
    lv_obj_t *icons[] = {
        objects.home_topbar__topbar_battery_icon,
        objects.trailer_topbar__topbar_battery_icon,
        objects.power_topbar__topbar_battery_icon,
        objects.water_topbar__topbar_battery_icon,
        objects.air_topbar__topbar_battery_icon,
        objects.settings_topbar__topbar_battery_icon,
        objects.page_wifi_setup_topbar__topbar_battery_icon,
        objects.page_wifi_connecting_topbar__topbar_battery_icon,
        objects.page_mqtt_setup_topbar__topbar_battery_icon,
        objects.page_mqtt_connecting_topbar__topbar_battery_icon,
        objects.page_alarms_topbar__topbar_battery_icon,
        objects.page_edit_buttons_topbar__topbar_battery_icon,
        objects.page_button_edit_topbar__topbar_battery_icon,
    };
    /* Clamp so the compiler's format-truncation checker sees a bounded
     * int fitting in the buffer; battery_poll_cb already caps to 0..100
     * but the setter is public so re-assert here. */
    int p = (percent < 0) ? -1 : (percent > 100 ? 100 : (int)percent);
    char buf[8];
    if (p < 0) strlcpy(buf, "--%", sizeof(buf));
    else       snprintf(buf, sizeof(buf), "%d%%", p);

    const char *glyph = battery_glyph_for((int)percent);
    /* Low-SOC red overrides the normal secondary/success color unless we're
     * actively charging (charging keeps the green cue regardless of level). */
    bool low_warn = (percent >= 0 && percent < 20 && !s_int_charging);
    for (size_t i = 0; i < sizeof(pct_labels)/sizeof(*pct_labels); i++) {
        if (pct_labels[i]) label_set_text_if_changed(pct_labels[i], buf);
        if (icons[i]) {
            label_set_text_if_changed(icons[i], glyph);
            if (low_warn) {
                lv_obj_set_style_text_color(icons[i],
                    lv_color_hex(0xFF5453), LV_PART_MAIN);
            } else if (!s_int_charging) {
                /* Restore TextSecondary (grey) so the icon reverts when SOC
                 * climbs back out of the warning band. Hex-literal matches
                 * the palette's Default-theme TextSecondary value. */
                lv_obj_set_style_text_color(icons[i],
                    lv_color_hex(0x9CA3AF), LV_PART_MAIN);
            }
        }
    }
#endif
}

void set_var_internal_battery_voltage(float volts) {
    s_int_battery_v = volts;
    /* Voltage isn't surfaced in the TopBar (percent + charging state is
     * enough at that size). Kept for future use in a Settings/About panel. */
}

void set_var_internal_charging(bool charging) {
    s_int_charging = charging;
#if __has_include("ui/screens.h")
    /* Cached-last-value guard: the poller re-invokes with the same state
     * every 10 s. Bail out when nothing changed so the six bolt icons +
     * six battery icons don't invalidate on every quiet tick. First call
     * still runs so the initial paint lands correctly. */
    static bool s_last_charging_painted = false;
    static bool s_first_paint           = true;
    if (!s_first_paint && charging == s_last_charging_painted) return;
    s_first_paint            = false;
    s_last_charging_painted  = charging;

    lv_obj_t *bolts[] = {
        objects.home_topbar__topbar_charge_icon,
        objects.trailer_topbar__topbar_charge_icon,
        objects.power_topbar__topbar_charge_icon,
        objects.water_topbar__topbar_charge_icon,
        objects.air_topbar__topbar_charge_icon,
        objects.settings_topbar__topbar_charge_icon,
        objects.page_wifi_setup_topbar__topbar_charge_icon,
        objects.page_wifi_connecting_topbar__topbar_charge_icon,
        objects.page_mqtt_setup_topbar__topbar_charge_icon,
        objects.page_mqtt_connecting_topbar__topbar_charge_icon,
        objects.page_alarms_topbar__topbar_charge_icon,
        objects.page_edit_buttons_topbar__topbar_charge_icon,
        objects.page_button_edit_topbar__topbar_charge_icon,
    };
    lv_obj_t *icons[] = {
        objects.home_topbar__topbar_battery_icon,
        objects.trailer_topbar__topbar_battery_icon,
        objects.power_topbar__topbar_battery_icon,
        objects.water_topbar__topbar_battery_icon,
        objects.air_topbar__topbar_battery_icon,
        objects.settings_topbar__topbar_battery_icon,
        objects.page_wifi_setup_topbar__topbar_battery_icon,
        objects.page_wifi_connecting_topbar__topbar_battery_icon,
        objects.page_mqtt_setup_topbar__topbar_battery_icon,
        objects.page_mqtt_connecting_topbar__topbar_battery_icon,
        objects.page_alarms_topbar__topbar_battery_icon,
        objects.page_edit_buttons_topbar__topbar_battery_icon,
        objects.page_button_edit_topbar__topbar_battery_icon,
    };
    /* Success green when charging, palette TextSecondary otherwise. Hex-lits
     * are theme-invariant on purpose — charging status has the same meaning
     * regardless of theme, and Success/Secondary here match the palette's
     * Default-theme values. */
    uint32_t hex = charging ? 0x52A441 : 0x9CA3AF;
    lv_color_t c = lv_color_hex(hex);
    for (size_t i = 0; i < sizeof(bolts)/sizeof(*bolts); i++) {
        if (bolts[i]) {
            if (charging) lv_obj_clear_flag(bolts[i], LV_OBJ_FLAG_HIDDEN);
            else          lv_obj_add_flag(bolts[i], LV_OBJ_FLAG_HIDDEN);
        }
        if (icons[i]) lv_obj_set_style_text_color(icons[i], c, LV_PART_MAIN);
    }
    /* Re-run SOC paint so a low-battery-red icon flips to charging-green
     * (or back) without waiting for the next SOC update. */
    if (s_int_battery_pct >= 0) set_var_internal_battery_soc(s_int_battery_pct);
#endif
}

/* ============================================================
 * GPS / GNSS — Trailer screen right panel
 * ============================================================ */

static float s_lat = 0.0f, s_lon = 0.0f, s_alt = 0.0f, s_spd = 0.0f, s_crs = 0.0f;
static int32_t s_sats = 0;

static void paint_gnss(void) {
#if __has_include("ui/screens.h")
    char buf[32];
    if (objects.trailer_gnss_lat) {
        /* "39.7392\xc2\xb0 N" — °N/°S from sign; UTF-8 for U+00B0. */
        snprintf(buf, sizeof(buf), "%.4f\xc2\xb0 %c",
                 (double)fabsf(s_lat), s_lat >= 0 ? 'N' : 'S');
        lv_label_set_text(objects.trailer_gnss_lat, buf);
    }
    if (objects.trailer_gnss_lon) {
        snprintf(buf, sizeof(buf), "%.4f\xc2\xb0 %c",
                 (double)fabsf(s_lon), s_lon >= 0 ? 'E' : 'W');
        lv_label_set_text(objects.trailer_gnss_lon, buf);
    }
    if (objects.trailer_gnss_alt) {
        snprintf(buf, sizeof(buf), "%.0f ft", (double)s_alt);
        lv_label_set_text(objects.trailer_gnss_alt, buf);
    }
    if (objects.trailer_gnss_sats) {
        snprintf(buf, sizeof(buf), "%ld", (long)s_sats);
        lv_label_set_text(objects.trailer_gnss_sats, buf);
    }
    if (objects.trailer_gnss_spd) {
        snprintf(buf, sizeof(buf), "%.1f mph", (double)(s_spd * 1.15078f));
        lv_label_set_text(objects.trailer_gnss_spd, buf);
    }
#endif
}

void set_var_latitude(float lat)    { s_lat = lat; paint_gnss(); }
void set_var_longitude(float lon)   { s_lon = lon; paint_gnss(); }
void set_var_altitude(float feet)   { s_alt = feet; paint_gnss(); }
void set_var_speed(float knots)     { s_spd = knots; paint_gnss(); }
void set_var_course(float degrees)  { s_crs = degrees; (void)s_crs; }
void set_var_satellite_count(int32_t v) { s_sats = v; paint_gnss(); }

void set_var_gnss_mode(const char *mode) {
#if __has_include("ui/screens.h")
    if (objects.trailer_gnss_mode && mode)
        lv_label_set_text(objects.trailer_gnss_mode, mode);
#else
    (void)mode;
#endif
}

static bool s_system_time_set = false;
bool system_time_set = false;

void set_var_gps_time(int y, int mo, int d, int h, int mi, int sec) {
    if (y < 2020) return;
    struct tm t = {.tm_year = y - 1900, .tm_mon = mo - 1, .tm_mday = d,
                   .tm_hour = h, .tm_min = mi, .tm_sec = sec};
    time_t epoch = mktime(&t);
    struct timeval tv = {.tv_sec = epoch};
    settimeofday(&tv, NULL);
    s_system_time_set = true;
    system_time_set = true;
}

/* ============================================================
 * Trailer leveling — Plateau (via Headwaters can-bridge)
 * ============================================================ */

/* Thresholds match the PWA level-indicator: > 5° = danger, > 2° = warning,
 * else level. */
#define LEVEL_DEG_LEVEL    2.0f
#define LEVEL_DEG_WARN     5.0f
/* Bubble travel calibration. The ring is 150 px across; the bubble is 16 px
 * with its default position centered on the crosshair. Cap travel so it
 * never leaves the outer ring, and scale so a ±5° tilt reaches the edge. */
#define LEVEL_PX_PER_DEG   12
#define LEVEL_PX_CLAMP     60
/* Bubble default position within each card, matching the authored coords
 * in screens.c (both cards use the same local layout). */
#define LEVEL_BUBBLE_BASE_X 162
#define LEVEL_BUBBLE_BASE_Y 209

static int level_clamp(int v, int lo, int hi) {
    return v < lo ? lo : v > hi ? hi : v;
}

static const char *level_status_text(float abs_deg) {
    if (abs_deg > LEVEL_DEG_WARN)  return "NOT LEVEL";
    if (abs_deg > LEVEL_DEG_LEVEL) return "MINOR";
    return "LEVEL";
}

/* Format inches-to-raise for one end of an axis. `is_low_end` selects
 * whether this label represents the side that needs raising given the
 * signed diff; the HIGH end always reads 0.0" so the driver can see at
 * a glance which jack to work. */
static void level_format_inches(char *buf, size_t n,
                                int32_t diff_mm, bool is_low_end) {
    if (!is_low_end || diff_mm == 0) {
        snprintf(buf, n, "0.0\"");
        return;
    }
    float in = fabsf((float)diff_mm) / 25.4f;
    snprintf(buf, n, "%.1f\"", (double)in);
}

void set_var_leveling(float pitch_deg, float roll_deg,
                      int32_t fb_diff_mm, int32_t lr_diff_mm) {
#if __has_include("ui/screens.h")
    char buf[16];

    /* --- Side card (pitch → front/back) ---
     * A physical bubble floats to the HIGH side. Positive pitch means
     * nose-up (front high), so the bubble slides UP the vline (negative
     * screen delta). fb_diff_mm > 0 means front high → BACK is the low
     * end that needs raising. */
    if (objects.trailer_side_bubble) {
        int dy = level_clamp((int)(pitch_deg * LEVEL_PX_PER_DEG),
                             -LEVEL_PX_CLAMP, LEVEL_PX_CLAMP);
        lv_obj_set_pos(objects.trailer_side_bubble,
                       LEVEL_BUBBLE_BASE_X,
                       LEVEL_BUBBLE_BASE_Y - dy);
    }
    level_format_inches(buf, sizeof(buf), fb_diff_mm, fb_diff_mm < 0);
    label_set_text_if_changed(objects.trailer_side_a_value, buf);  /* FRONT */
    level_format_inches(buf, sizeof(buf), fb_diff_mm, fb_diff_mm > 0);
    label_set_text_if_changed(objects.trailer_side_b_value, buf);  /* BACK  */
    label_set_text_if_changed(objects.trailer_side_status,
                              level_status_text(fabsf(pitch_deg)));

    /* --- Back card (roll → left/right) ---
     * Positive roll per the Plateau IMU convention means the right side is
     * high, so the bubble slides right and LEFT is the low end. */
    if (objects.trailer_back_bubble) {
        int dx = level_clamp((int)(roll_deg * LEVEL_PX_PER_DEG),
                             -LEVEL_PX_CLAMP, LEVEL_PX_CLAMP);
        lv_obj_set_pos(objects.trailer_back_bubble,
                       LEVEL_BUBBLE_BASE_X + dx,
                       LEVEL_BUBBLE_BASE_Y);
    }
    level_format_inches(buf, sizeof(buf), lr_diff_mm, lr_diff_mm > 0);
    label_set_text_if_changed(objects.trailer_back_a_value, buf);  /* LEFT  */
    level_format_inches(buf, sizeof(buf), lr_diff_mm, lr_diff_mm < 0);
    label_set_text_if_changed(objects.trailer_back_b_value, buf);  /* RIGHT */
    label_set_text_if_changed(objects.trailer_back_status,
                              level_status_text(fabsf(roll_deg)));
#else
    (void)pitch_deg; (void)roll_deg;
    (void)fb_diff_mm; (void)lr_diff_mm;
#endif
}

/* ============================================================
 * Air Quality — Borealis
 * ============================================================ */

/* -1.0f = never received (real RH is always 0..100). */
static float   s_hum  = -1.0f;
static int32_t s_eco2 = 0;
static int32_t s_tvoc = 0;

/* AQ classifier state — mirrors PWA's dataSafety/data sources. -1 = unset
 * so we can distinguish "never received" from a real 0 reading. Populated
 * by set_var_co2/tvoc/co/co_flags; consumed by paint_air_status(). */
static int32_t s_co_ppm   = -1;
static bool    s_co_warn  = false;
static bool    s_co_alarm = false;
static int32_t s_eco2_ppm = -1;
static int32_t s_tvoc_ppb = -1;

static void paint_air_status(void);   /* forward decl */
static void paint_temp_badge(void);   /* forward decl — defined below paint_badge */
static void paint_hum_badge(void);    /* forward decl — defined below paint_badge */
/* INT32_MIN = never received. 0°F is a valid winter reading, so we cannot
 * use 0 as the "unset" sentinel. */
static int32_t s_temp_f = INT32_MIN;

static void paint_air_temp(void) {
#if __has_include("ui/screens.h")
    if (objects.air_temp_value) {
        if (s_temp_f == INT32_MIN) {
            lv_label_set_text(objects.air_temp_value, "--");
        } else {
            char buf[8]; snprintf(buf, sizeof(buf), "%ld", (long)f_to_display(s_temp_f));
            lv_label_set_text(objects.air_temp_value, buf);
        }
    }
    if (objects.air_temp_unit) {
        lv_label_set_text(objects.air_temp_unit, s_temp_unit ? "°C" : "°F");
    }
    paint_temp_badge();
#endif
}

void set_var_current_interior_temperature(int32_t v) {
    s_temp_f = v;
    metric_push(METRIC_TEMP, (int16_t)v);
    paint_air_temp();
}
void set_var_current_exterior_temperature(int32_t v) { (void)v; }

void set_var_humidity(float percent) {
    s_hum = percent;
    metric_push(METRIC_HUM, (int16_t)percent);
#if __has_include("ui/screens.h")
    if (objects.air_hum_value) {
        char buf[8]; snprintf(buf, sizeof(buf), "%ld", (long)percent);
        lv_label_set_text(objects.air_hum_value, buf);
    }
    paint_hum_badge();
#endif
}

void set_var_co2(int32_t ppm) {
    s_eco2 = ppm;
    s_eco2_ppm = ppm;      /* mirror to the AQ-classifier state */
    metric_push(METRIC_ECO2, (int16_t)ppm);
#if __has_include("ui/screens.h")
    if (objects.air_eco2_value) {
        char buf[8]; snprintf(buf, sizeof(buf), "%ld", (long)ppm);
        lv_label_set_text(objects.air_eco2_value, buf);
    }
    paint_air_status();   /* recompute badges + recommendation */
#endif
}

void set_var_tvoc(int32_t ppb) {
    s_tvoc = ppb;
    s_tvoc_ppb = ppb;
    metric_push(METRIC_TVOC, (int16_t)ppb);
#if __has_include("ui/screens.h")
    if (objects.air_tvoc_value) {
        char buf[8]; snprintf(buf, sizeof(buf), "%ld", (long)ppb);
        lv_label_set_text(objects.air_tvoc_value, buf);
    }
    paint_air_status();
#endif
}

/* Classification tiers (0 unset, 1 good, 2 moderate, 3 unhealthy) —
 * matches PWA getTvocClass/getEco2Class/getCoClass. */
typedef enum { AQ_UNSET=0, AQ_GOOD=1, AQ_MOD=2, AQ_BAD=3 } aq_cls_t;

static aq_cls_t tvoc_class(void) {
    if (s_tvoc_ppb < 0) return AQ_UNSET;
    if (s_tvoc_ppb < 220)  return AQ_GOOD;
    if (s_tvoc_ppb < 660)  return AQ_MOD;
    return AQ_BAD;
}
static aq_cls_t eco2_class(void) {
    if (s_eco2_ppm < 0) return AQ_UNSET;
    if (s_eco2_ppm < 1000) return AQ_GOOD;
    if (s_eco2_ppm < 2000) return AQ_MOD;
    return AQ_BAD;
}
static aq_cls_t co_class(void) {
    if (s_co_ppm < 0) return AQ_UNSET;
    if (s_co_alarm) return AQ_BAD;
    if (s_co_warn)  return AQ_MOD;
    if (s_co_ppm >= 200) return AQ_BAD;
    if (s_co_ppm >= 70)  return AQ_MOD;
    return AQ_GOOD;
}
static aq_cls_t overall_aq_class(void) {
    aq_cls_t t = tvoc_class(), e = eco2_class(), c = co_class();
    if (t == AQ_UNSET && e == AQ_UNSET && c == AQ_UNSET) return AQ_UNSET;
    if (c == AQ_BAD) return AQ_BAD;
    if ((t != AQ_UNSET && s_tvoc_ppb >= 660) ||
        (e != AQ_UNSET && s_eco2_ppm >= 2000)) return AQ_BAD;
    if (c == AQ_MOD) return AQ_MOD;
    if ((t != AQ_UNSET && s_tvoc_ppb >= 220) ||
        (e != AQ_UNSET && s_eco2_ppm >= 1000)) return AQ_MOD;
    return AQ_GOOD;
}

/* Paint one metric badge:
 *   cls -> label text + text_color + bg_color.
 * The design spec §9 defines only Good (accent green on soft-green bg) and
 * Warning (amber on soft-amber bg) — collapse PWA's Moderate/Unhealthy to
 * the amber "Warning" visual style here. */
#if __has_include("ui/screens.h")
static void paint_badge(lv_obj_t *badge_panel, lv_obj_t *badge_label,
                        aq_cls_t cls, const char *label_text) {
    if (!badge_panel || !badge_label) return;
    lv_label_set_text(badge_label, label_text);
    lv_color_t bg, txt;
    if (cls == AQ_UNSET) {
        bg = lv_color_hex(0xEDEDED); txt = lv_color_hex(0x888888);
    } else if (cls == AQ_GOOD) {
        bg = lv_color_hex(0xCBE3C6); txt = lv_color_hex(0x52A441);
    } else if (cls == AQ_MOD) {
        bg = lv_color_hex(0xFFF4CC); txt = lv_color_hex(0xFFC107);
    } else {
        bg = lv_color_hex(0xFFDDDC); txt = lv_color_hex(0xFF5453);
    }
    lv_obj_set_style_bg_color(badge_panel, bg, LV_PART_MAIN);
    lv_obj_set_style_text_color(badge_label, txt, LV_PART_MAIN);
}

/* Temp/humidity classifiers use fixed Fahrenheit / %RH thresholds so the
 * F↔C display toggle does NOT shift category boundaries. */
static aq_cls_t temp_class(void) {
    if (s_temp_f == INT32_MIN) return AQ_UNSET;
    if (s_temp_f < 60)  return AQ_MOD;   /* Cold */
    if (s_temp_f < 80)  return AQ_GOOD;  /* Normal */
    return AQ_MOD;                       /* Hot */
}
static const char *temp_label_str(void) {
    if (s_temp_f == INT32_MIN) return "";
    if (s_temp_f < 60)  return "COLD";
    if (s_temp_f < 80)  return "NORMAL";
    return "HOT";
}
static aq_cls_t hum_class(void) {
    if (s_hum < 0.0f)   return AQ_UNSET;
    if (s_hum < 30.0f)  return AQ_MOD;   /* Dry */
    if (s_hum < 60.0f)  return AQ_GOOD;  /* Normal */
    return AQ_MOD;                       /* Wet */
}
static const char *hum_label_str(void) {
    if (s_hum < 0.0f)   return "";
    if (s_hum < 30.0f)  return "DRY";
    if (s_hum < 60.0f)  return "NORMAL";
    return "WET";
}
static void paint_temp_badge(void) {
    paint_badge(objects.air_temp_badge, objects.air_temp_badge_l,
                temp_class(), temp_label_str());
}
static void paint_hum_badge(void) {
    paint_badge(objects.air_hum_badge, objects.air_hum_badge_l,
                hum_class(), hum_label_str());
}

static const char *tvoc_label(void) {
    if (s_tvoc_ppb < 0)    return "";
    if (s_tvoc_ppb < 65)   return "EXCELLENT";
    if (s_tvoc_ppb < 220)  return "GOOD";
    if (s_tvoc_ppb < 660)  return "MODERATE";
    if (s_tvoc_ppb < 2200) return "POOR";
    return "UNHEALTHY";
}
static const char *eco2_label(void) {
    if (s_eco2_ppm < 0)    return "";
    if (s_eco2_ppm < 1000) return "NORMAL";
    if (s_eco2_ppm < 2000) return "HIGH";
    return "ALARM";
}
static const char *co_label(void) {
    if (s_co_ppm < 0) return "";
    if (s_co_alarm || s_co_ppm >= 200) return "DANGER";
    if (s_co_warn  || s_co_ppm >= 70)  return "WARNING";
    return "NORMAL";
}
__attribute__((unused))
static const char *overall_label(void) {
    switch (overall_aq_class()) {
    case AQ_GOOD: return "GOOD";
    case AQ_MOD:  return "MODERATE";
    case AQ_BAD:  return "UNHEALTHY";
    default:      return "--";
    }
}
static const char *overall_rec(void) {
    if (co_class() == AQ_BAD)
        return "Carbon monoxide detected - ventilate immediately";
    switch (overall_aq_class()) {
    case AQ_GOOD: return "Air quality is good";
    case AQ_MOD:  return "Ventilation recommended";
    case AQ_BAD:  return "Ventilation needed";
    default:      return "--";
    }
}
#endif

/* Recompute + paint all badges + recommendation card. Called from every
 * setter that changes s_tvoc/eco2/co state. */
static void paint_air_status(void) {
#if __has_include("ui/screens.h")
    paint_badge(objects.air_tvoc_badge, objects.air_tvoc_badge_l,
                tvoc_class(), tvoc_label());
    paint_badge(objects.air_eco2_badge, objects.air_eco2_badge_l,
                eco2_class(), eco2_label());
    paint_badge(objects.air_co_badge,   objects.air_co_badge_l,
                co_class(),   co_label());
    /* Temp + humidity don't have PWA classifiers, so leave those badges
     * at "--" (neutral grey) via the badge that was authored. */
    if (objects.air_rec_text)
        lv_label_set_text(objects.air_rec_text, overall_rec());
    /* Recolor the recommendation card border/label to the overall state
     * so the sidebar acts as a status summary at a glance. */
    aq_cls_t oc = overall_aq_class();
    lv_color_t accent;
    if      (oc == AQ_BAD) accent = lv_color_hex(0xFF5453);
    else if (oc == AQ_MOD) accent = lv_color_hex(0xFFC107);
    else if (oc == AQ_GOOD) accent = lv_color_hex(0x52A441);
    else                    accent = lv_color_hex(0x888888);
    if (objects.air_rec_lbl)
        lv_obj_set_style_text_color(objects.air_rec_lbl, accent, LV_PART_MAIN);
    paint_notif_badge();   /* AQ change may toggle the "unhealthy" alarm */
#endif
}

void set_var_co(int32_t ppm) {
    s_co_ppm = ppm;
    metric_push(METRIC_CO, (int16_t)ppm);
#if __has_include("ui/screens.h")
    if (objects.air_co_value) {
        char buf[8]; snprintf(buf, sizeof(buf), "%ld", (long)ppm);
        lv_label_set_text(objects.air_co_value, buf);
    }
    paint_air_status();
#endif
}

void set_var_co_flags(bool warn, bool alarm) {
    s_co_warn  = warn;
    s_co_alarm = alarm;
#if __has_include("ui/screens.h")
    paint_air_status();
#endif
}

/* ============================================================
 * Water — Reservoir
 * ============================================================ */

void set_var_water_levels(int32_t fresh, int32_t grey, int32_t black) {
#if __has_include("ui/screens.h")
    /* Two sets of tank widgets get the same values: the Water screen's
     * tall gradient bars and the Home summary's short cyan/grey/dark bars. */
    struct { lv_obj_t *bar; lv_obj_t *pct; int32_t v; } rows[6] = {
        {objects.water_fresh_bar,       objects.water_fresh_pct,       fresh},
        {objects.water_grey_bar,        objects.water_grey_pct,        grey},
        {objects.water_black_bar,       objects.water_black_pct,       black},
        {objects.home_water_fresh_bar,  objects.home_water_fresh_pct,  fresh},
        {objects.home_water_grey_bar,   objects.home_water_grey_pct,   grey},
        {objects.home_water_black_bar,  objects.home_water_black_pct,  black},
    };
    for (int i = 0; i < 6; i++) {
        if (rows[i].bar) lv_bar_set_value(rows[i].bar, rows[i].v, LV_ANIM_OFF);
        if (rows[i].pct) {
            char buf[8]; snprintf(buf, sizeof(buf), "%ld%%", (long)rows[i].v);
            lv_label_set_text(rows[i].pct, buf);
        }
    }
#else
    (void)fresh; (void)grey; (void)black;
#endif
}

/* Reset all runtime-populated widgets to their empty state. The JSON
 * carries a few author-time placeholders that would be misleading on
 * hardware — e.g. the water tank bars are authored at 50 % fill so their
 * vertical gradient is visible in EEZ Studio's canvas (spec §8 explicitly
 * says: canvas 50 %, hardware 0 %). Called from main.c under the display
 * lock, immediately after ui_init(), before any MQTT payloads land. */
void reset_placeholders(void) {
#if __has_include("ui/screens.h")
    /* Tank bars → 0 % (spec: "on hardware show 0% fill with the `--%`
     * text — never an empty white rectangle"). The bar's INDICATOR still
     * renders at 0 % because its style is applied even with zero fill;
     * the gradient just isn't visible until a positive value lands. */
    struct { lv_obj_t *bar; lv_obj_t *pct; } tanks[6] = {
        {objects.water_fresh_bar,       objects.water_fresh_pct},
        {objects.water_grey_bar,        objects.water_grey_pct},
        {objects.water_black_bar,       objects.water_black_pct},
        {objects.home_water_fresh_bar,  objects.home_water_fresh_pct},
        {objects.home_water_grey_bar,   objects.home_water_grey_pct},
        {objects.home_water_black_bar,  objects.home_water_black_pct},
    };
    for (int i = 0; i < 6; i++) {
        if (tanks[i].bar) lv_bar_set_value(tanks[i].bar, 0, LV_ANIM_OFF);
        if (tanks[i].pct) lv_label_set_text(tanks[i].pct, "--%");
    }
#endif
}

/* ============================================================
 * Connection status — topbar chips
 * ============================================================ */

/* Notification tally — the ESP-observable subset of what the Headwaters
 * PWA calls "alarms". PWA gets an authoritative active-alarms list from a
 * Headwaters WebSocket event (`alarms_update`) that includes armed
 * Switchback/Picket sensor inputs and battery-below-threshold. The arm
 * config lives in Mongo and isn't published to MQTT, so we can't mirror
 * the sensor-input side of that logic locally. What we CAN do — from the
 * same MQTT streams the backend consumes — is compute the conditions
 * this device can see for itself:
 *
 *   - MQTT broker disconnected (data has stopped flowing)
 *   - Battery critical (SOC < 20%)                     [local/energy/status]
 *   - CO warn or alarm                                 [local/airquality/safety]
 *   - Overall air-quality UNHEALTHY (eCO₂ ≥ 2000 or TVOC ≥ 660 or CO danger)
 *
 * Each contributes 1 to the tally. The topbar bell dot is visible when
 * the tally > 0, and the count number renders inside it. */

static bool s_mqtt_connected = false;

/* Case-insensitive keyword match against the user's sensor label to pick
 * the most-specific pre-recorded TTS phrase. Extend the table when you
 * regenerate audio_assets.c with new categories — the fall-through to
 * SENSOR keeps unknown labels announceable at all. */
static audio_phrase_t pick_sensor_phrase(const char *label) {
    if (!label || !label[0]) return AUDIO_PHRASE_SENSOR;
    /* Case-fold in place onto a small stack buffer for strstr. */
    char lc[ALARM_LABEL_MAX];
    size_t i = 0;
    for (; i < sizeof(lc) - 1 && label[i]; i++) {
        lc[i] = (char)tolower((unsigned char)label[i]);
    }
    lc[i] = '\0';
    /* Order matters — more specific keywords first (e.g. "refrigerator"
     * before generic "power" if that ever gets added). */
    if (strstr(lc, "cabinet") || strstr(lc, "cupboard"))
        return AUDIO_PHRASE_CABINET_ALARM;
    if (strstr(lc, "fridge") || strstr(lc, "refrigerator") ||
        strstr(lc, "freezer"))
        return AUDIO_PHRASE_REFRIGERATOR_ALARM;
    if (strstr(lc, "window"))
        return AUDIO_PHRASE_WINDOW_ALARM;
    if (strstr(lc, "compartment") || strstr(lc, "bay") ||
        strstr(lc, "storage"))
        return AUDIO_PHRASE_COMPARTMENT_ALARM;
    if (strstr(lc, "water") || strstr(lc, "tank") || strstr(lc, "leak"))
        return AUDIO_PHRASE_WATER_ALARM;
    if (strstr(lc, "motion") || strstr(lc, "movement") || strstr(lc, "pir"))
        return AUDIO_PHRASE_MOTION_ALARM;
    if (strstr(lc, "door") || strstr(lc, "hatch") || strstr(lc, "entry"))
        return AUDIO_PHRASE_DOOR_ALARM;
    return AUDIO_PHRASE_SENSOR;
}

static int notif_count(void) {
    /* The badge count must match what the toaster actually enumerates
     * (alarms_enumerate_active → sensor arms + battery-critical). MQTT-
     * down and CO / air-quality events drive their own TTS phrases and
     * dedicated on-screen surfaces (status bar, Home CO card, Air page
     * badges) but do NOT appear as toaster rows, so counting them here
     * inflated the badge past the visible toaster row count and the
     * user saw a "1" on the bell with an empty toaster panel. */
    return alarms_active_count();
}

static void paint_notif_badge(void) {
#if __has_include("ui/screens.h")
    int n = notif_count();
    /* Rising-edge detection — fire a TTS phrase when a NEW alarm class
     * appears (transition from inactive to active for that class). We
     * track prior state per-class so a persistent battery-low doesn't
     * re-announce every tick, but a CO alarm arriving on top DOES fire
     * its own phrase. First-tick suppression: skip audio until we've
     * seen at least one baseline pass. */
    static bool s_baseline_seen = false;
    static bool s_prev_mqtt_down    = false;
    static bool s_prev_co           = false;
    static bool s_prev_air_bad      = false;
    static int  s_prev_alarms_count = 0;

    bool now_mqtt_down    = !s_mqtt_connected;
    bool now_co           = (s_co_alarm || s_co_warn ||
                             (s_co_ppm > 0 && s_co_ppm >= 70));
    bool now_air_bad      = (overall_aq_class() == AQ_BAD);
    int  now_alarms_count = alarms_active_count();

    /* Battery-low is bundled into alarms_active_count() via alarms.c —
     * its edge is detected by watching for an increase in that count. */

    /* Debounce MQTT-down announcements. Under the 200+ msg/sec Headwaters
     * traffic level (measured 2026-07-23: energy/status @ 40 Hz + relays/N
     * @ 13 Hz × 8 + gps @ 13 Hz × 4), esp-mqtt occasionally loses a PINGRESP
     * to WiFi packet loss and the client aborts the session. Reconnect
     * completes in ~10 s, so a grace shorter than that guarantees the TTS
     * fires on every flap even though the outage is invisible to the user.
     *
     * 30 s covers the observed 10-15 s reconnect window with margin; anything
     * longer than 30 s down is either a real WiFi drop or broker outage and
     * SHOULD announce. The underlying PINGRESP-loss is a server-side traffic
     * issue (Ampline/rv-backend over-publishing on every poll instead of
     * on-change) — not fixable in firmware. */
    #define MQTT_DOWN_GRACE_SEC 30
    static int64_t s_mqtt_down_since_us      = 0;
    static bool    s_mqtt_announced_outage   = false;
    int64_t now_us = esp_timer_get_time();
    if (now_mqtt_down) {
        if (!s_prev_mqtt_down) {
            s_mqtt_down_since_us    = now_us;
            s_mqtt_announced_outage = false;
        }
    } else {
        s_mqtt_down_since_us    = 0;
        s_mqtt_announced_outage = false;
    }

    /* Feed the alarms rising-edge queue every tick. First call establishes
     * baseline (no events pushed) so retained-MQTT replay on boot doesn't
     * announce itself. */
    alarms_tick_edges();

    if (s_baseline_seen) {
        /* Gate audio phrases to APP_STATE_READY. During the setup wizard
         * MQTT is legitimately "not connected" because the user hasn't
         * finished entering credentials yet — the CONN_LOST phrase would
         * fire ~30 s into the wizard on every fresh device, which is
         * misleading (nothing was lost — nothing was ever established).
         * Same reasoning applies to CO / air-quality phrases: without an
         * MQTT connection no CAN sensor readings can arrive, so those
         * flags are always cleared and this branch is a no-op regardless
         * — the READY gate just makes the intent explicit. */
        bool wizard_active = (app_state_get() != APP_STATE_READY);
        if (!wizard_active &&
            now_mqtt_down && !s_mqtt_announced_outage &&
            (now_us - s_mqtt_down_since_us) >=
                (int64_t)MQTT_DOWN_GRACE_SEC * 1000000LL) {
            audio_play_phrase(AUDIO_PHRASE_CONN_LOST);
            s_mqtt_announced_outage = true;
        }
        if (now_co && !s_prev_co) {
            audio_play_phrase(s_co_alarm ? AUDIO_PHRASE_CO_DANGER
                                         : AUDIO_PHRASE_CO_WARNING);
        }
        if (now_air_bad && !s_prev_air_bad)
            audio_play_phrase(AUDIO_PHRASE_AIR_UNHEALTHY);
        /* Drain the rising-edge queue. Battery events → the specific
         * BATTERY_THRESHOLD phrase; sensor events → keyword-matched
         * category phrase from the sensor's label (falls back to generic
         * SENSOR if nothing matches). Loop caps at EDGE_Q_SZ so a stuck
         * flood can't run forever inside the paint tick. */
        for (int i = 0; i < 16; i++) {
            alarm_edge_t e;
            if (!alarms_pop_rising_edge(&e)) break;
            if (e.is_battery) {
                audio_play_phrase(AUDIO_PHRASE_BATTERY_THRESHOLD);
            } else {
                char label[ALARM_LABEL_MAX];
                alarms_get_label(e.src, e.addr, e.sensor, label, sizeof(label));
                audio_play_phrase(pick_sensor_phrase(label));
            }
        }
    }
    s_prev_mqtt_down    = now_mqtt_down;
    s_prev_co           = now_co;
    s_prev_air_bad      = now_air_bad;
    s_prev_alarms_count = now_alarms_count;
    s_baseline_seen     = true;
    (void)s_prev_alarms_count;  /* now informational only — TTS is edge-driven */

    if (n < 0) n = 0;
    if (n > 9) n = 9;
    char buf[4];
    /* Cast to unsigned so GCC's format-truncation analyzer sees a tight
     * range (0..9) instead of int's full range. */
    snprintf(buf, sizeof(buf), "%u", (unsigned)n);

    lv_obj_t *badges[] = {
        objects.home_topbar__topbar_notif_badge,
        objects.trailer_topbar__topbar_notif_badge,
        objects.power_topbar__topbar_notif_badge,
        objects.water_topbar__topbar_notif_badge,
        objects.air_topbar__topbar_notif_badge,
        objects.settings_topbar__topbar_notif_badge,
        objects.page_wifi_setup_topbar__topbar_notif_badge,
        objects.page_wifi_connecting_topbar__topbar_notif_badge,
        objects.page_mqtt_setup_topbar__topbar_notif_badge,
        objects.page_mqtt_connecting_topbar__topbar_notif_badge,
        objects.page_alarms_topbar__topbar_notif_badge,
        objects.page_edit_buttons_topbar__topbar_notif_badge,
        objects.page_button_edit_topbar__topbar_notif_badge,
    };
    lv_obj_t *counts[] = {
        objects.home_topbar__topbar_notif_badge_count,
        objects.trailer_topbar__topbar_notif_badge_count,
        objects.power_topbar__topbar_notif_badge_count,
        objects.water_topbar__topbar_notif_badge_count,
        objects.air_topbar__topbar_notif_badge_count,
        objects.settings_topbar__topbar_notif_badge_count,
        objects.page_wifi_setup_topbar__topbar_notif_badge_count,
        objects.page_wifi_connecting_topbar__topbar_notif_badge_count,
        objects.page_mqtt_setup_topbar__topbar_notif_badge_count,
        objects.page_mqtt_connecting_topbar__topbar_notif_badge_count,
        objects.page_alarms_topbar__topbar_notif_badge_count,
        objects.page_edit_buttons_topbar__topbar_notif_badge_count,
        objects.page_button_edit_topbar__topbar_notif_badge_count,
    };
    const int n_badges = (int)(sizeof(badges) / sizeof(*badges));
    for (int i = 0; i < n_badges; i++) {
        if (badges[i]) {
            if (n > 0) lv_obj_clear_flag(badges[i], LV_OBJ_FLAG_HIDDEN);
            else       lv_obj_add_flag(badges[i], LV_OBJ_FLAG_HIDDEN);
        }
        if (counts[i]) lv_label_set_text(counts[i], buf);
    }

    /* Keep the currently-open toaster in sync with the live alarm set.
     * Cheap: at most one populate pass per tick, only walks alarm bitmaps
     * (no NVS I/O), and label_set_text_if_changed suppresses redundant
     * LVGL redraws when no row's text moved. */
    toaster_refresh_if_open();
#endif
}

void set_var_mqtt_connected(bool connected) {
    /* When the C6/MQTT is in the failing-reconnect loop, esp-mqtt fires
     * MQTT_EVENT_DISCONNECTED every ~17 s and the client's event task
     * calls this setter every time — even though the value is already
     * false. paint_notif_badge() walks 13 badges + 13 counts + the
     * toaster + the alarm bitmaps under the LVGL lock (~100 ms), and
     * doing that on the LVGL lock every 17 s is what makes taps feel
     * sluggish. Bail out when nothing actually changed. */
    if (connected == s_mqtt_connected) return;
    s_mqtt_connected = connected;
#if __has_include("ui/screens.h")
    if (objects.mqtt_connecting_status) {
        lv_label_set_text(objects.mqtt_connecting_status,
                          connected ? "Connected" : "Connecting to broker...");
    }
    paint_notif_badge();
#else
    (void)connected;
#endif
}

/* label_wifi_connection_status is already painted from app_state.c's
 * static set_wifi_status_text() on every WIFI_EVENT_STA_* transition. */

void set_var_wifi_rssi(int32_t rssi_dbm) {
#if __has_include("ui/screens.h")
    /* Cached-last-value guard: the poller re-applies the same rssi every
     * 5 s while the link is quiet, and LVGL 8's lv_label_set_text always
     * invalidates. Bail out when unchanged so an idle station doesn't
     * trigger six needless redraws every tick. */
    static int32_t s_last_rssi = INT32_MIN;
    if (rssi_dbm == s_last_rssi) return;
    s_last_rssi = rssi_dbm;

    /* Fan out to every TopBar instance (one per page). */
    /* EEZ Studio's per-instance name is `<uwi_identifier>__<child_identifier>`
     * — no page-name prefix. My generator passes page_id="home" (etc.) to
     * wrap_page so the UWWs are `home_topbar`/`trailer_topbar`/... */
    lv_obj_t *labels[] = {
        objects.home_topbar__topbar_wifi_rssi,
        objects.trailer_topbar__topbar_wifi_rssi,
        objects.power_topbar__topbar_wifi_rssi,
        objects.water_topbar__topbar_wifi_rssi,
        objects.air_topbar__topbar_wifi_rssi,
        objects.settings_topbar__topbar_wifi_rssi,
        objects.page_wifi_setup_topbar__topbar_wifi_rssi,
        objects.page_wifi_connecting_topbar__topbar_wifi_rssi,
        objects.page_mqtt_setup_topbar__topbar_wifi_rssi,
        objects.page_mqtt_connecting_topbar__topbar_wifi_rssi,
        objects.page_alarms_topbar__topbar_wifi_rssi,
        objects.page_edit_buttons_topbar__topbar_wifi_rssi,
        objects.page_button_edit_topbar__topbar_wifi_rssi,
    };
    char buf[16]; snprintf(buf, sizeof(buf), "%ld dBm", (long)rssi_dbm);
    for (size_t i = 0; i < sizeof(labels)/sizeof(*labels); i++) {
        if (labels[i]) label_set_text_if_changed(labels[i], buf);
    }
#else
    (void)rssi_dbm;
#endif
}

/* ============================================================
 * Watchdogs — clear stale UI values
 * ============================================================ */

void clear_var_energy(void) {
#if __has_include("ui/screens.h")
    if (objects.power_solar_value)  lv_label_set_text(objects.power_solar_value, "--");
    if (objects.power_soc_value)    lv_label_set_text(objects.power_soc_value, "--");
    if (objects.power_volts_value)  lv_label_set_text(objects.power_volts_value, "--");
    if (objects.power_load_value)   lv_label_set_text(objects.power_load_value, "--");
    if (objects.power_charge_type)  lv_label_set_text(objects.power_charge_type, "No data");
#endif
}
void clear_var_airquality(void) {
    s_temp_f = INT32_MIN;
    s_hum    = -1.0f;
#if __has_include("ui/screens.h")
    if (objects.air_temp_value) lv_label_set_text(objects.air_temp_value, "--");
    if (objects.air_hum_value)  lv_label_set_text(objects.air_hum_value, "--");
    if (objects.air_eco2_value) lv_label_set_text(objects.air_eco2_value, "--");
    if (objects.air_tvoc_value) lv_label_set_text(objects.air_tvoc_value, "--");
    if (objects.air_co_value)   lv_label_set_text(objects.air_co_value, "--");
    paint_temp_badge();
    paint_hum_badge();
#endif
}
void clear_var_gps(void) {
    s_sats = 0; s_lat = s_lon = s_alt = s_spd = 0.0f;
    paint_gnss();
#if __has_include("ui/screens.h")
    if (objects.trailer_gnss_mode) lv_label_set_text(objects.trailer_gnss_mode, "No fix");
#endif
}
void clear_var_water(void) {
    set_var_water_levels(0, 0, 0);
}
void clear_var_leveling(void) {
#if __has_include("ui/screens.h")
    if (objects.trailer_side_bubble)
        lv_obj_set_pos(objects.trailer_side_bubble,
                       LEVEL_BUBBLE_BASE_X, LEVEL_BUBBLE_BASE_Y);
    if (objects.trailer_back_bubble)
        lv_obj_set_pos(objects.trailer_back_bubble,
                       LEVEL_BUBBLE_BASE_X, LEVEL_BUBBLE_BASE_Y);
    label_set_text_if_changed(objects.trailer_side_a_value, "--");
    label_set_text_if_changed(objects.trailer_side_b_value, "--");
    label_set_text_if_changed(objects.trailer_back_a_value, "--");
    label_set_text_if_changed(objects.trailer_back_b_value, "--");
    label_set_text_if_changed(objects.trailer_side_status, "NO DATA");
    label_set_text_if_changed(objects.trailer_back_status, "NO DATA");
#endif
}

/* ============================================================
 * Rotation / MAC / Settings
 * ============================================================ */

static int32_t s_rotation = 0;
void    set_var_rotation_degrees(int32_t v) { s_rotation = v; }
int32_t get_var_rotation_degrees(void)      { return s_rotation; }

static char s_mac[32] = "";
void set_var_mcu_mac_address(const char *v) {
    if (!v) return;
    strlcpy(s_mac, v, sizeof(s_mac));
#if __has_include("ui/screens.h")
    if (objects.mcu_mac_address_value)
        lv_label_set_text(objects.mcu_mac_address_value, v);
#endif
}
const char *get_var_mcu_mac_address(void) { return s_mac; }

static int32_t s_brightness = 80;
static int32_t s_timeout_min = 1;

int32_t get_var_screen_brightness(void)         { return s_brightness; }
void    set_var_screen_brightness(int32_t v)    { s_brightness = v; }
int32_t get_var_screen_timeout_minutes(void)    { return s_timeout_min; }
void    set_var_screen_timeout_minutes(int32_t v) { s_timeout_min = v; }

/* Paint the settings-page timeout value label from the current s_timeout_min.
 * Called by restore_user_settings() at boot AND by the +/- action handlers so
 * the label is authoritative — not the placeholder authored in the .eez-project.
 * (Historical bug: this call was missing from the restore path, so the label
 * showed the EEZ Studio placeholder text while s_timeout_min carried whatever
 * value was persisted to NVS. Confirmed on the Waveshare 7B port first and
 * back-ported here — see that project's DOCS/PORT_NOTES.md.) */
void paint_screen_timeout_label(void) {
#if __has_include("ui/screens.h")
    if (!objects.settings_timeout_value) return;
    char buf[16];
    if (s_timeout_min <= 0) snprintf(buf, sizeof(buf), "Never");
    else                    snprintf(buf, sizeof(buf), "%ld min",
                                     (long)s_timeout_min);
    lv_label_set_text(objects.settings_timeout_value, buf);
#endif
}

bool apply_timezone(const char *iana) {
    if (!iana || !*iana) return false;
    /* US-only zone set — matches the 5 options authored in the Settings
     * timezone dropdown. Rules are the post-2007 US convention: DST from
     * the 2nd Sunday of March to the 1st Sunday of November. Phoenix stays
     * on MST year-round. */
    static const struct { const char *iana; const char *posix; } zones[] = {
        { "America/New_York",    "EST5EDT,M3.2.0,M11.1.0" },
        { "America/Chicago",     "CST6CDT,M3.2.0,M11.1.0" },
        { "America/Denver",      "MST7MDT,M3.2.0,M11.1.0" },
        { "America/Los_Angeles", "PST8PDT,M3.2.0,M11.1.0" },
        { "America/Phoenix",     "MST7"                    },
    };
    for (size_t i = 0; i < sizeof(zones)/sizeof(*zones); i++) {
        if (strcmp(iana, zones[i].iana) == 0) {
            setenv("TZ", zones[i].posix, 1);
            tzset();
            return true;
        }
    }
    return false;
}

void restore_user_settings(void) {
    nvs_handle_t nvs;
    if (nvs_open("fireside", NVS_READONLY, &nvs) != ESP_OK) return;
    int32_t v;
    if (nvs_get_i32(nvs, "brightness", &v) == ESP_OK && v >= 10 && v <= 100) {
        s_brightness = v;
#if __has_include("ui/screens.h")
        if (objects.slider_screen_brightness)
            lv_slider_set_value(objects.slider_screen_brightness, v, LV_ANIM_OFF);
#endif
    }
    if (nvs_get_i32(nvs, "timeout", &v) == ESP_OK && v >= 0) s_timeout_min = v;
    /* Repaint the settings-page label to match the restored value. Without
     * this the label shows whatever placeholder the .eez-project authored
     * (typically "1 min") regardless of the true persisted value. */
    paint_screen_timeout_label();
    if (nvs_get_i32(nvs, "tempunit", &v) == ESP_OK) s_temp_unit = v ? 1 : 0;
    if (nvs_get_i32(nvs, "volume", &v) == ESP_OK && v >= 0 && v <= 100) {
        audio_set_volume((uint8_t)v);
#if __has_include("ui/screens.h")
        if (objects.slider_speaker_volume)
            lv_slider_set_value(objects.slider_speaker_volume, v, LV_ANIM_OFF);
        if (objects.settings_volume_pct) {
            char b[8]; snprintf(b, sizeof(b), "%ld%%", (long)v);
            lv_label_set_text(objects.settings_volume_pct, b);
        }
#endif
    }

    /* Timezone — string key, applied by matching against the dropdown's
     * option list. The dropdown authored 5 zones in this order:
     *   0 America/New_York, 1 America/Chicago, 2 America/Denver,
     *   3 America/Los_Angeles, 4 America/Phoenix. If the stored value
     *   isn't in the list, silently leave the dropdown at its default. */
    char tz_buf[32] = {0};
    size_t tz_len = sizeof(tz_buf);
    bool tz_have = (nvs_get_str(nvs, "tz", tz_buf, &tz_len) == ESP_OK &&
                    tz_buf[0]);
    if (tz_have) {
#if __has_include("ui/screens.h")
        if (objects.settings_timezone_dd) {
            const char *zones[5] = {
                "America/New_York", "America/Chicago", "America/Denver",
                "America/Los_Angeles", "America/Phoenix",
            };
            for (int i = 0; i < 5; i++) {
                if (strcmp(tz_buf, zones[i]) == 0) {
                    lv_dropdown_set_selected(objects.settings_timezone_dd,
                                             (uint16_t)i);
                    break;
                }
            }
        }
#endif
    }
    /* Install the POSIX TZ so localtime_r() renders local wall time. Falls
     * back to the dropdown's authored default (America/New_York) when no
     * value has been persisted yet — matches what the UI shows on a fresh
     * device. */
    apply_timezone(tz_have ? tz_buf : "America/New_York");
    nvs_close(nvs);
    ESP_LOGI(TAG, "Restored: brightness=%ld timeout=%ld tempunit=%ld tz=%s",
             (long)s_brightness, (long)s_timeout_min, (long)s_temp_unit,
             tz_buf[0] ? tz_buf : "(default)");
}

/* ============================================================
 * Clock — update the topbar clock label at 1 Hz + Home big clock.
 * Called by main loop with display lock held.
 * ============================================================ */

void update_clock_display(void) {
    /* Don't gate on s_system_time_set (the SNTP-arrived flag). The ESP32-P4
     * RTC keeps ticking across resets once ANY time source (SNTP, GPS, or
     * manual settimeofday) has landed, so a device that got the time on a
     * previous boot has valid time-of-day immediately, before WiFi comes up.
     *
     * `time(NULL)` returns seconds since epoch; ESP-IDF initialises this to
     * seconds-since-boot (unix epoch 1970-01-01) if the RTC has never been
     * set, i.e. year 1970. Only skip the paint in that pre-set state — a
     * year >= 2020 means the RTC has real wall time (from SNTP or GPS on
     * this or a previous boot). This makes the toolbar clock read the true
     * RTC time from boot, per spec §5. */
#if __has_include("ui/screens.h")
    time_t now = time(NULL);
    struct tm t;
    localtime_r(&now, &t);
    if (t.tm_year + 1900 < 2020) return;   /* RTC not yet set — keep placeholder */

    /* Called from the 1 Hz main-loop tick, but nothing in the label block
     * below moves faster than tm_min. Skip the fanout when the minute is
     * unchanged so an idle home screen doesn't re-run 20 snprintfs +
     * invalidate ~20 labels every second. paint_notif_badge still runs
     * on every tick (spec §alarms — sensor-input alarms need per-second
     * evaluation regardless of clock movement). */
    static int s_last_min  = -1;
    static int s_last_hour = -1;
    if (t.tm_min != s_last_min || t.tm_hour != s_last_hour) {
        s_last_min  = t.tm_min;
        s_last_hour = t.tm_hour;

        char short_buf[12];
        snprintf(short_buf, sizeof(short_buf), "%d:%02d %s",
                 (t.tm_hour % 12) ? (t.tm_hour % 12) : 12,
                 t.tm_min,
                 (t.tm_hour < 12) ? "AM" : "PM");

        lv_obj_t *clocks[] = {
            objects.home_topbar__topbar_clock,
            objects.trailer_topbar__topbar_clock,
            objects.power_topbar__topbar_clock,
            objects.water_topbar__topbar_clock,
            objects.air_topbar__topbar_clock,
            objects.settings_topbar__topbar_clock,
            objects.page_wifi_setup_topbar__topbar_clock,
            objects.page_wifi_connecting_topbar__topbar_clock,
            objects.page_mqtt_setup_topbar__topbar_clock,
            objects.page_mqtt_connecting_topbar__topbar_clock,
            objects.page_alarms_topbar__topbar_clock,
            objects.page_edit_buttons_topbar__topbar_clock,
            objects.page_button_edit_topbar__topbar_clock,
        };
        for (size_t i = 0; i < sizeof(clocks)/sizeof(*clocks); i++) {
            if (clocks[i]) label_set_text_if_changed(clocks[i], short_buf);
        }
        /* Home page big clock digits. label_set_text_if_changed still
         * matters even inside this minute-guarded block: hh only changes
         * hourly, weekday/date only daily, ampm twice a day, greeting a
         * handful of times a day — no reason to invalidate them on the
         * minute boundaries when they haven't moved. */
        if (objects.home_clock_hh) {
            char hh[4]; int h12 = (t.tm_hour % 12) ? (t.tm_hour % 12) : 12;
            snprintf(hh, sizeof(hh), "%d", h12);
            label_set_text_if_changed(objects.home_clock_hh, hh);
        }
        if (objects.home_clock_mm) {
            char mm[4]; snprintf(mm, sizeof(mm), "%02d", t.tm_min);
            label_set_text_if_changed(objects.home_clock_mm, mm);
        }
        if (objects.home_clock_ampm) {
            label_set_text_if_changed(objects.home_clock_ampm,
                                      (t.tm_hour < 12) ? "AM" : "PM");
        }
        /* Weekday goes ABOVE the time, in green uppercase (new design §2b).
         * The date line now shows month/day/year only — no weekday, no
         * middle dot. */
        if (objects.home_clock_weekday) {
            char wday[16];
            strftime(wday, sizeof(wday), "%A", &t);
            for (char *c = wday; *c; c++) *c = (char)toupper((unsigned char)*c);
            label_set_text_if_changed(objects.home_clock_weekday, wday);
        }
        if (objects.home_clock_date) {
            char mon[16], db[48];
            strftime(mon, sizeof(mon), "%B", &t);
            snprintf(db, sizeof(db), "%s %d, %d",
                     mon, t.tm_mday, 1900 + t.tm_year);
            label_set_text_if_changed(objects.home_clock_date, db);
        }

        /* Topbar greeting — cycles by time of day (RTC, not MQTT). Runs
         * here so it re-evaluates on the minute without needing its own
         * timer. */
        const char *greet;
        if      (t.tm_hour < 5)  greet = "Good Evening";
        else if (t.tm_hour < 12) greet = "Good Morning";
        else if (t.tm_hour < 17) greet = "Good Afternoon";
        else if (t.tm_hour < 21) greet = "Good Evening";
        else                     greet = "Good Night";
        lv_obj_t *greets[] = {
            objects.home_topbar__topbar_greeting,
            objects.trailer_topbar__topbar_greeting,
            objects.power_topbar__topbar_greeting,
            objects.water_topbar__topbar_greeting,
            objects.air_topbar__topbar_greeting,
            objects.settings_topbar__topbar_greeting,
            objects.page_wifi_setup_topbar__topbar_greeting,
            objects.page_wifi_connecting_topbar__topbar_greeting,
            objects.page_mqtt_setup_topbar__topbar_greeting,
            objects.page_mqtt_connecting_topbar__topbar_greeting,
            objects.page_alarms_topbar__topbar_greeting,
            objects.page_edit_buttons_topbar__topbar_greeting,
            objects.page_button_edit_topbar__topbar_greeting,
        };
        for (size_t i = 0; i < sizeof(greets)/sizeof(*greets); i++) {
            if (greets[i]) label_set_text_if_changed(greets[i], greet);
        }
    }

    /* Recompute the notification badge once per clock tick so
     * sensor-input alarms (evaluated in alarms.c from spoor/picket topics)
     * surface without waiting for an unrelated setter to fire. */
    paint_notif_badge();
#endif
}

/* ============================================================
 * WiFi RSSI poll — every 5 s, ask the driver for the current AP info and
 * paint the value into every topbar.
 *
 * This board remotes WiFi over ESP-Hosted SDIO to an ESP32-C6, so
 * esp_wifi_sta_get_ap_info() is a synchronous cross-chip RPC. When the
 * C6 is busy that call can stall for tens to hundreds of milliseconds,
 * so it must not run on the LVGL task (it would freeze rendering and
 * touch for the whole duration).
 *
 * Split into two halves:
 *   - wifi_rssi_poll_task: dedicated FreeRTOS task, priority 1, does the
 *     RPC and stores the result to an atomic cache. Never touches LVGL.
 *   - wifi_rssi_apply_cb: LVGL timer, checks the dirty flag and calls
 *     set_var_wifi_rssi() with the cached value. LVGL-safe by
 *     construction (runs on the LVGL task with the lock already held).
 *
 * "0 dBm" is preserved as the "not associated" placeholder: when the
 * RPC returns non-OK the task caches 0, which set_var_wifi_rssi then
 * renders as "0 dBm" in every topbar.
 * ============================================================ */

static _Atomic int32_t s_cached_rssi       = 0;
static _Atomic bool    s_cached_rssi_dirty = true;

static void wifi_rssi_poll_task(void *arg) {
    (void)arg;
    while (1) {
        wifi_ap_record_t ap;
        int32_t rssi = 0;
        if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) {
            rssi = (int32_t)ap.rssi;
        }
        atomic_store(&s_cached_rssi, rssi);
        atomic_store(&s_cached_rssi_dirty, true);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

static void wifi_rssi_apply_cb(lv_timer_t *t) {
    (void)t;
    if (!atomic_exchange(&s_cached_rssi_dirty, false)) return;
    set_var_wifi_rssi(atomic_load(&s_cached_rssi));
}

void init_wifi_rssi_poll(void) {
    /* Apply timer runs on the LVGL task at 500 ms so the initial cache
     * value lands promptly after the poller's first RPC completes. The
     * callback is nearly free when the dirty flag isn't set. */
    lv_timer_create(wifi_rssi_apply_cb, 500, NULL);
    static bool started = false;
    if (started) return;
    started = true;
    /* 4 KB stack: esp_wifi_sta_get_ap_info uses ~1 KB on this chip.
     * Priority 1 keeps it below every UI/network task; a WiFi RSSI
     * update is never latency-critical. */
    if (xTaskCreate(wifi_rssi_poll_task, "wifi_rssi_poll",
                    4096, NULL, 1, NULL) != pdPASS) {
        ESP_LOGE(TAG, "xTaskCreate(wifi_rssi_poll) failed");
    }
}

/* ============================================================
 * Screen timeout — blank the backlight after N minutes of no touch
 * activity (as reported by LVGL's indev), then restore it on the next
 * touch. Timeout value of 0 = never blank. Runs on a 1 s LVGL timer.
 * ============================================================ */

extern esp_err_t set_lcd_blight(uint32_t brightness);

static bool s_display_dimmed = false;
static lv_obj_t *s_wake_shield = NULL;

/* Wake-shield press handler: swallows the wake tap so an underlying
 * widget (a light-toggle button, an alarm ack, etc.) doesn't fire when
 * the user is only trying to see the screen again. Restores the
 * backlight immediately, then deletes the shield asynchronously so
 * subsequent taps route to the real widgets. */
static void wake_shield_press_cb(lv_event_t *e) {
    (void)e;
    if (s_display_dimmed) {
        set_lcd_blight((uint32_t)s_brightness);
        s_display_dimmed = false;
    }
    if (s_wake_shield) {
        lv_obj_del_async(s_wake_shield);
        s_wake_shield = NULL;
    }
}

static void install_wake_shield(void) {
    if (s_wake_shield) return;
    lv_obj_t *top = lv_disp_get_layer_top(NULL);
    if (!top) return;
    s_wake_shield = lv_obj_create(top);
    lv_obj_remove_style_all(s_wake_shield);
    lv_obj_set_size(s_wake_shield, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(s_wake_shield, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_clear_flag(s_wake_shield, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(s_wake_shield, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(s_wake_shield, wake_shield_press_cb,
                        LV_EVENT_PRESSED, NULL);
}

static void remove_wake_shield(void) {
    if (!s_wake_shield) return;
    lv_obj_del(s_wake_shield);
    s_wake_shield = NULL;
}

static void screen_timeout_cb(lv_timer_t *t) {
    (void)t;
    int32_t timeout_min = s_timeout_min;
    uint32_t idle_ms = lv_disp_get_inactive_time(NULL);

    if (timeout_min <= 0) {
        /* "Never" — make sure the backlight is restored if the user
         * dialed the timeout back up from a currently-blanked state. */
        if (s_display_dimmed) {
            set_lcd_blight((uint32_t)s_brightness);
            s_display_dimmed = false;
            remove_wake_shield();
        }
        return;
    }

    uint32_t timeout_ms = (uint32_t)timeout_min * 60000u;
    if (!s_display_dimmed && idle_ms >= timeout_ms) {
        install_wake_shield();
        set_lcd_blight(0);
        s_display_dimmed = true;
    } else if (s_display_dimmed && idle_ms < timeout_ms) {
        /* Belt-and-braces path: if the shield somehow didn't catch the
         * press (shouldn't happen — it sits on lv_layer_top) still
         * restore here so the user isn't left staring at a black panel. */
        set_lcd_blight((uint32_t)s_brightness);
        s_display_dimmed = false;
        remove_wake_shield();
    }
}

void init_screen_timeout(void) {
    lv_timer_create(screen_timeout_cb, 1000, NULL);
}

/* ============================================================
 * Clock dot blink — spec §4: 1000ms period, 500ms at opa 255, 500ms at
 * opa 38 (15%), hard step (no fade). Both dots blink together. LVGL timer
 * fires every 500ms and toggles bg_opa on both dot panels.
 * ============================================================ */

static void clock_dot_blink_cb(lv_timer_t *t) {
    (void)t;
#if __has_include("ui/screens.h")
    static bool on = true;
    on = !on;
    lv_opa_t opa = on ? LV_OPA_COVER : (lv_opa_t)38;
    if (objects.home_clock_dot1)
        lv_obj_set_style_bg_opa(objects.home_clock_dot1, opa, LV_PART_MAIN);
    if (objects.home_clock_dot2)
        lv_obj_set_style_bg_opa(objects.home_clock_dot2, opa, LV_PART_MAIN);
#endif
}

void init_clock_blink(void) {
    /* Caller (main.c) holds the display lock; LVGL timers are stored on
     * the display's timer list. */
    lv_timer_create(clock_dot_blink_cb, 500, NULL);
}

/* ============================================================
 * Notification bell → toaster panel with per-alarm ack.
 *
 * Tapping the topbar bell (or badge) opens the toaster panel authored
 * inside the TopBar user widget. The toaster lists every currently
 * active alarm; each row carries an "Ack" button that snoozes only that
 * one alarm — the underlying condition still contributes to the badge
 * count, but the TTS re-alert holds off for one snooze window
 * (ALARM_SNOOZE_SECS_DEFAULT = 120s) instead of firing every tick.
 *
 * The topbar is instanced on 13 pages; EEZ Studio's export gives us one
 * copy of every toaster widget per page. Only the toaster on the active
 * screen is visible at any time (LVGL doesn't render off-screen roots),
 * so opening all 13 at once is harmless — but we open only the one
 * belonging to the currently loaded screen to avoid touching state we
 * won't visibly change.
 * ============================================================ */

#define FIRESIDE_TOASTER_ROWS 8

#if __has_include("ui/screens.h")

/* Enumerate every topbar user-widget instance in the project. The prefix
 * matches the identifier column shown by `main/ui/screens.h`; each entry
 * expands to one `struct fireside_toaster` at init. If a new page hosts
 * a topbar in the future, add its prefix here and the toaster wiring
 * follows automatically — no other change needed. */
#define TOPBAR_INSTANCES(_) \
    _(home_topbar) \
    _(trailer_topbar) \
    _(power_topbar) \
    _(water_topbar) \
    _(air_topbar) \
    _(settings_topbar) \
    _(page_wifi_setup_topbar) \
    _(page_wifi_connecting_topbar) \
    _(page_mqtt_setup_topbar) \
    _(page_mqtt_connecting_topbar) \
    _(page_alarms_topbar) \
    _(page_edit_buttons_topbar) \
    _(page_button_edit_topbar)

#define TOPBAR_COUNT_ENTRY(prefix) +1
#define TOPBAR_INSTANCE_COUNT (0 TOPBAR_INSTANCES(TOPBAR_COUNT_ENTRY))

struct fireside_toaster {
    lv_obj_t *notif_icon;
    lv_obj_t *notif_badge;
    lv_obj_t *theme_btn;
    lv_obj_t *toaster;
    lv_obj_t *empty_msg;
    lv_obj_t *row_panels[FIRESIDE_TOASTER_ROWS];
    lv_obj_t *row_labels[FIRESIDE_TOASTER_ROWS];
    lv_obj_t *row_acks[FIRESIDE_TOASTER_ROWS];
    /* Populated on every open — maps row slot → alarm identity so the
     * ack button click can call the right per-alarm snooze API. */
    alarm_edge_t row_alarms[FIRESIDE_TOASTER_ROWS];
    int filled_rows;
};

/* Assign field-by-field into the module-static `s_toasters` array — one
 * `INIT_TB(prefix)` invocation per instance, indexed by a running counter
 * `idx`. The earlier version used a designated-initializer local
 * `struct fireside_toaster tb_init[TOPBAR_INSTANCE_COUNT] = { ... }` on
 * the stack, but that struct is ~184 B × 13 ≈ 2.4 KB — enough to blow
 * app_main's ~4 KB stack and trip a stack-protection panic during
 * init_notif_icon_ack_taps(). Writing to BSS directly keeps the stack
 * usage of this init helper at zero. */
#define INIT_TB(prefix) do { \
    s_toasters[idx].notif_icon      = objects.prefix##__topbar_notif_icon; \
    s_toasters[idx].notif_badge     = objects.prefix##__topbar_notif_badge; \
    s_toasters[idx].theme_btn       = objects.prefix##__topbar_theme_btn; \
    s_toasters[idx].toaster         = objects.prefix##__topbar_toaster; \
    s_toasters[idx].empty_msg       = objects.prefix##__topbar_toaster_empty; \
    s_toasters[idx].row_panels[0]   = objects.prefix##__topbar_toaster_row_0; \
    s_toasters[idx].row_panels[1]   = objects.prefix##__topbar_toaster_row_1; \
    s_toasters[idx].row_panels[2]   = objects.prefix##__topbar_toaster_row_2; \
    s_toasters[idx].row_panels[3]   = objects.prefix##__topbar_toaster_row_3; \
    s_toasters[idx].row_panels[4]   = objects.prefix##__topbar_toaster_row_4; \
    s_toasters[idx].row_panels[5]   = objects.prefix##__topbar_toaster_row_5; \
    s_toasters[idx].row_panels[6]   = objects.prefix##__topbar_toaster_row_6; \
    s_toasters[idx].row_panels[7]   = objects.prefix##__topbar_toaster_row_7; \
    s_toasters[idx].row_labels[0]   = objects.prefix##__topbar_toaster_row_0_label; \
    s_toasters[idx].row_labels[1]   = objects.prefix##__topbar_toaster_row_1_label; \
    s_toasters[idx].row_labels[2]   = objects.prefix##__topbar_toaster_row_2_label; \
    s_toasters[idx].row_labels[3]   = objects.prefix##__topbar_toaster_row_3_label; \
    s_toasters[idx].row_labels[4]   = objects.prefix##__topbar_toaster_row_4_label; \
    s_toasters[idx].row_labels[5]   = objects.prefix##__topbar_toaster_row_5_label; \
    s_toasters[idx].row_labels[6]   = objects.prefix##__topbar_toaster_row_6_label; \
    s_toasters[idx].row_labels[7]   = objects.prefix##__topbar_toaster_row_7_label; \
    s_toasters[idx].row_acks[0]     = objects.prefix##__topbar_toaster_row_0_ack; \
    s_toasters[idx].row_acks[1]     = objects.prefix##__topbar_toaster_row_1_ack; \
    s_toasters[idx].row_acks[2]     = objects.prefix##__topbar_toaster_row_2_ack; \
    s_toasters[idx].row_acks[3]     = objects.prefix##__topbar_toaster_row_3_ack; \
    s_toasters[idx].row_acks[4]     = objects.prefix##__topbar_toaster_row_4_ack; \
    s_toasters[idx].row_acks[5]     = objects.prefix##__topbar_toaster_row_5_ack; \
    s_toasters[idx].row_acks[6]     = objects.prefix##__topbar_toaster_row_6_ack; \
    s_toasters[idx].row_acks[7]     = objects.prefix##__topbar_toaster_row_7_ack; \
    idx++; \
} while (0);

static struct fireside_toaster s_toasters[TOPBAR_INSTANCE_COUNT];

/* Encode (toaster_index, row_index) into a single void* for lv_event's
 * user_data. 8 bits per field is more than enough (13 toasters, 8 rows). */
#define TB_ENCODE(ti, ri)   ((void *)(uintptr_t)(((uint32_t)(ti) << 8) | (ri)))
#define TB_DECODE_TI(p)     ((int)((uintptr_t)(p) >> 8))
#define TB_DECODE_RI(p)     ((int)((uintptr_t)(p) & 0xFFu))

struct enum_ctx {
    alarm_edge_t buf[FIRESIDE_TOASTER_ROWS];
    int count;
};

static void enum_collect(const alarm_edge_t *e, void *v) {
    struct enum_ctx *c = (struct enum_ctx *)v;
    if (c->count >= FIRESIDE_TOASTER_ROWS) return;
    c->buf[c->count++] = *e;
}

static void toaster_populate(struct fireside_toaster *tb) {
    if (!tb || !tb->toaster) return;
    struct enum_ctx ctx = { .count = 0 };
    alarms_enumerate_active(enum_collect, &ctx);
    tb->filled_rows = ctx.count;

    for (int i = 0; i < FIRESIDE_TOASTER_ROWS; i++) {
        if (i < ctx.count) {
            tb->row_alarms[i] = ctx.buf[i];
            if (tb->row_labels[i]) {
                char label[ALARM_LABEL_MAX + 8];
                if (ctx.buf[i].is_battery) {
                    snprintf(label, sizeof(label), "Battery critical");
                } else {
                    alarms_get_label(ctx.buf[i].src, ctx.buf[i].addr,
                                     ctx.buf[i].sensor, label, sizeof(label));
                }
                /* label_set_text_if_changed skips the LVGL invalidate when
                 * the text is unchanged — matters because this helper runs
                 * every 1 Hz tick while the toaster is open. */
                label_set_text_if_changed(tb->row_labels[i], label);
            }
            if (tb->row_panels[i]) {
                lv_obj_clear_flag(tb->row_panels[i], LV_OBJ_FLAG_HIDDEN);
            }
        } else {
            if (tb->row_panels[i]) {
                lv_obj_add_flag(tb->row_panels[i], LV_OBJ_FLAG_HIDDEN);
            }
        }
    }
    if (tb->empty_msg) {
        if (ctx.count == 0) {
            lv_obj_clear_flag(tb->empty_msg, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(tb->empty_msg, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

/* Refresh whichever toaster is currently open. Called from the 1 Hz clock
 * tick (paint_notif_badge) so:
 *   - rows for alarms that have since cleared self-remove
 *   - rows for newly-fired alarms appear without the user closing + reopening
 *   - the empty-message toggles as the last alarm clears
 * Only ever one toaster can be visible (only one screen is loaded at a
 * time), so we stop at the first non-hidden one. */
static void toaster_refresh_if_open(void) {
    for (int i = 0; i < TOPBAR_INSTANCE_COUNT; i++) {
        if (s_toasters[i].toaster &&
            !lv_obj_has_flag(s_toasters[i].toaster, LV_OBJ_FLAG_HIDDEN)) {
            toaster_populate(&s_toasters[i]);
            return;
        }
    }
}

static struct fireside_toaster *find_toaster_for_screen(lv_obj_t *screen) {
    if (!screen) return NULL;
    for (int i = 0; i < TOPBAR_INSTANCE_COUNT; i++) {
        if (s_toasters[i].toaster &&
            lv_obj_get_screen(s_toasters[i].toaster) == screen) {
            return &s_toasters[i];
        }
    }
    return NULL;
}

static void toaster_close_all(void) {
    for (int i = 0; i < TOPBAR_INSTANCE_COUNT; i++) {
        if (s_toasters[i].toaster) {
            lv_obj_add_flag(s_toasters[i].toaster, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void notif_icon_click_cb(lv_event_t *e) {
    (void)e;
    struct fireside_toaster *tb = find_toaster_for_screen(lv_scr_act());
    if (!tb || !tb->toaster) {
        ESP_LOGW(TAG, "notif tap: no toaster for current screen");
        return;
    }
    bool was_open = !lv_obj_has_flag(tb->toaster, LV_OBJ_FLAG_HIDDEN);
    /* Close every instance first so switching pages while open doesn't
     * leave a stale toaster hidden behind the current one. */
    toaster_close_all();
    if (!was_open) {
        toaster_populate(tb);
        lv_obj_clear_flag(tb->toaster, LV_OBJ_FLAG_HIDDEN);
        /* Z-order is authored in the JSON — the toaster is the last
         * child of the TopBar root panel so it already draws over the
         * other topbar children. Don't call lv_obj_move_foreground here
         * (it would be a Mode-A canvas-device divergence). */
        ESP_LOGI(TAG, "toaster: opened (%d active alarm(s))", tb->filled_rows);
    } else {
        ESP_LOGI(TAG, "toaster: closed");
    }
}

static void ack_button_click_cb(lv_event_t *e) {
    int ti = TB_DECODE_TI(lv_event_get_user_data(e));
    int ri = TB_DECODE_RI(lv_event_get_user_data(e));
    if (ti < 0 || ti >= TOPBAR_INSTANCE_COUNT) return;
    if (ri < 0 || ri >= FIRESIDE_TOASTER_ROWS) return;
    struct fireside_toaster *tb = &s_toasters[ti];
    if (ri >= tb->filled_rows) return;
    alarm_edge_t *ae = &tb->row_alarms[ri];
    /* Ack only snoozes the TTS re-alert. The row stays visible as long as
     * the underlying condition is still active — the 1 Hz refresh drops
     * the row when the sensor returns to normal (door closes, battery
     * recovers, etc.). If the condition happens to clear between open and
     * this tap, the ack call returns false and the refresh below removes
     * the now-stale row. */
    (void)(ae->is_battery
              ? alarms_acknowledge_battery()
              : alarms_acknowledge_sensor(ae->src, ae->addr, ae->sensor));
    toaster_populate(tb);
}
#endif

void init_notif_icon_ack_taps(void) {
#if __has_include("ui/screens.h")
    int idx = 0;
    TOPBAR_INSTANCES(INIT_TB)
    (void)idx;   /* silence unused-after-macro-expansion warning */

    for (int i = 0; i < TOPBAR_INSTANCE_COUNT; i++) {
        struct fireside_toaster *tb = &s_toasters[i];

        if (tb->notif_icon) {
            lv_obj_add_flag(tb->notif_icon, LV_OBJ_FLAG_CLICKABLE);
            /* Bell is ~16x14 px in the topbar — expand hit area to make
             * it comfortably tappable on the 10" glass without moving
             * the visual center. */
            lv_obj_set_ext_click_area(tb->notif_icon, 18);
            lv_obj_add_event_cb(tb->notif_icon, notif_icon_click_cb,
                                LV_EVENT_CLICKED, NULL);
        }
        if (tb->notif_badge) {
            lv_obj_add_flag(tb->notif_badge, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_ext_click_area(tb->notif_badge, 18);
            lv_obj_add_event_cb(tb->notif_badge, notif_icon_click_cb,
                                LV_EVENT_CLICKED, NULL);
        }
        /* Theme button is authored 28x22 — too small for a fingertip on the
         * 10" glass, so users had to aim precisely. Expand the hit zone
         * without changing the visible size. Kept smaller than the
         * notif_icon/badge extension so the two zones butt up cleanly
         * instead of overlapping. */
        if (tb->theme_btn) {
            lv_obj_set_ext_click_area(tb->theme_btn, 10);
        }
        /* Toaster clip + z-order are authored in the .eez-project:
         * OVERFLOW_VISIBLE is set on the outer <page>_topbar wrapper and
         * the wrapper is placed as the last child of its screen so LVGL
         * draws it above the body. No runtime fix-up needed here. */
        for (int r = 0; r < FIRESIDE_TOASTER_ROWS; r++) {
            if (tb->row_acks[r]) {
                lv_obj_add_event_cb(tb->row_acks[r], ack_button_click_cb,
                                    LV_EVENT_CLICKED, TB_ENCODE(i, r));
            }
            /* Every row starts hidden — populate on open uncovers the
             * first N based on active alarms. */
            if (tb->row_panels[r]) {
                lv_obj_add_flag(tb->row_panels[r], LV_OBJ_FLAG_HIDDEN);
            }
        }
    }
#endif
}

/* ============================================================
 * Touch-target hit-area widening.
 *
 * On the 10" 1024x600 glass, capacitive touch registers a single point
 * that shifts by 1-3 px between finger-down and finger-up as the user
 * lifts. LVGL only fires LV_EVENT_CLICKED when both events land on the
 * same widget, so small buttons feel "finicky" — the click gets eaten
 * whenever the release drifts off the widget's physical rectangle.
 *
 * lv_obj_set_ext_click_area extends the invisible hit rectangle without
 * changing the visible geometry (per the eezstudio skill's Gate 5, this
 * is one of the operations code IS allowed to do on EEZ-authored
 * widgets — it's a runtime property, not a canvas divergence). Applied
 * to the bottom-nav buttons on every page that carries a nav, and the
 * home page device buttons.
 * ============================================================ */

#define NAV_INSTANCES(_) \
    _(home) \
    _(trailer) \
    _(power) \
    _(water) \
    _(air) \
    _(settings) \
    _(page_wifi_setup) \
    _(page_wifi_connecting) \
    _(page_mqtt_setup) \
    _(page_mqtt_connecting) \
    _(page_alarms)

#define EXPAND_NAV_HITZONE(prefix) do { \
    if (objects.prefix##_nav__nav_home)     lv_obj_set_ext_click_area(objects.prefix##_nav__nav_home,     8); \
    if (objects.prefix##_nav__nav_trailer)  lv_obj_set_ext_click_area(objects.prefix##_nav__nav_trailer,  8); \
    if (objects.prefix##_nav__nav_power)    lv_obj_set_ext_click_area(objects.prefix##_nav__nav_power,    8); \
    if (objects.prefix##_nav__nav_water)    lv_obj_set_ext_click_area(objects.prefix##_nav__nav_water,    8); \
    if (objects.prefix##_nav__nav_air)      lv_obj_set_ext_click_area(objects.prefix##_nav__nav_air,      8); \
    if (objects.prefix##_nav__nav_settings) lv_obj_set_ext_click_area(objects.prefix##_nav__nav_settings, 8); \
} while (0);

void init_touch_target_hit_areas(void) {
#if __has_include("ui/screens.h")
    NAV_INSTANCES(EXPAND_NAV_HITZONE)

    /* Tiered extensions: tiny controls get the widest boost because the
     * physical target is smallest; medium buttons get a middle bump;
     * large cards get just a small pad so adjacent tiles don't overlap. */

    /* +10 px — very small controls where a fingertip barely fits (arrows,
     * switch, wizard rescan). */
    lv_obj_t *tiny[] = {
        objects.settings_timeout_up,
        objects.settings_timeout_down,
        objects.alarms_bat_switch,
        objects.wifi_setup_scan_btn,
    };
    for (size_t i = 0; i < sizeof(tiny)/sizeof(*tiny); i++) {
        if (tiny[i]) lv_obj_set_ext_click_area(tiny[i], 10);
    }

    /* +8 px — standard buttons (~200-250 wide, ~30-46 tall). */
    lv_obj_t *standard[] = {
        objects.settings_light_btn,   objects.settings_dark_btn,
        objects.settings_c_btn,       objects.settings_f_btn,
        objects.settings_alarms_btn,  objects.settings_reset_btn,
        objects.alarms_back_btn,
        objects.wifi_pw_cancel,       objects.wifi_pw_submit,
        objects.mqtt_submit_btn,      objects.mqtt_skip_btn,
        objects.btn_edit_buttons_back, objects.btn_edit_save,
        objects.modal_factory_reset_confirm,
        objects.modal_factory_reset_cancel,
    };
    for (size_t i = 0; i < sizeof(standard)/sizeof(*standard); i++) {
        if (standard[i]) lv_obj_set_ext_click_area(standard[i], 8);
    }

    /* +6 px — larger cards / tiles. Kept small so a fingertip near the
     * seam between two adjacent tiles doesn't land on both. */
    lv_obj_t *large[] = {
        objects.home_dev1, objects.home_dev2, objects.home_dev3, objects.home_dev4,
        objects.home_dev5, objects.home_dev6, objects.home_dev7, objects.home_dev8,
        objects.btn_settings_edit_buttons,
        objects.water_pump_btn,
        objects.btn_edit_tile01, objects.btn_edit_tile02, objects.btn_edit_tile03,
        objects.btn_edit_tile04, objects.btn_edit_tile05, objects.btn_edit_tile06,
        objects.btn_edit_tile07, objects.btn_edit_tile08,
    };
    for (size_t i = 0; i < sizeof(large)/sizeof(*large); i++) {
        if (large[i]) lv_obj_set_ext_click_area(large[i], 6);
    }
#endif
}

/* One-shot ring-buffer init on library load — before app_main runs. */
__attribute__((constructor))
static void vars_ctor(void) { metric_init(); }
