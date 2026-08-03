/*
 * perf.c — LVGL diagnostic probes, compiled in only when
 * CONFIG_FIRESIDE_TOUCH_DIAG is enabled (menuconfig → Fireside).
 *
 * 1. Stall watchdog — an lv_timer that fires every 100 ms. Because
 *    lv_timers run on the LVGL task, a stalled task can't fire them on
 *    time. When the delta between fires exceeds 200 ms we log the actual
 *    duration; that number is a direct measurement of how long the LVGL
 *    task was blocked on something (a slow render, a long callback, a
 *    lock contention, etc.).
 *
 * 2. Raw touch sampler — polls the pointer indev every 20 ms and logs
 *    DOWN/UP transitions with coordinates, independent of widget
 *    callbacks. If a physical tap produces no DOWN/UP pair here, the
 *    touch driver never delivered it to LVGL.
 *
 * 3. Keyboard VALUE_CHANGED probe — logs each keyboard event with a
 *    monotonic timestamp; correlate with the next stall line to measure
 *    keyboard mode-switch render cost.
 *
 * History note: this file once also raised scroll_limit to 150 px,
 * long_press_time to 800 ms, and interposed a 60 px "jump filter" on the
 * touch read callback, to mask 20–140 px coordinate drift observed on
 * this glass. The 2026-08-03 baseline test (DOCS/touch-baseline/) proved
 * the drift never existed at the hardware level — it was manufactured by
 * flash writes (nvs_commit) blocking the LVGL task from inside UI event
 * callbacks, since fixed by main/settings_store.c. The band-aids are
 * gone; LVGL defaults (scroll_limit 10 px, long_press 400 ms) are
 * correct on this hardware.
 */

#include "sdkconfig.h"

#if CONFIG_FIRESIDE_TOUCH_DIAG

#include "esp_log.h"
#include "esp_timer.h"
#include "lvgl.h"

#if __has_include("ui/screens.h")
#include "screens.h"
#endif

static const char *TAG = "PERF";

/* -------- Stall watchdog -------------------------------------------- */

#define STALL_LOG_THRESHOLD_US 200000LL

static void stall_watchdog_cb(lv_timer_t *t) {
    (void)t;
    static int64_t last_us = 0;
    int64_t now = esp_timer_get_time();
    if (last_us) {
        int64_t delta = now - last_us;
        if (delta >= STALL_LOG_THRESHOLD_US) {
            ESP_LOGW(TAG, "LVGL task stalled: %lld ms since last 100ms tick",
                     (long long)(delta / 1000));
        }
    }
    last_us = now;
}

/* -------- Keyboard event probe -------------------------------------- */

/* Logs the btnmatrix state machine per keyboard press: which button the
 * matrix selected at PRESSED, selection changes while PRESSING, and
 * whether RELEASED produced a VALUE_CHANGED (the actual keystroke). A
 * missed key shows exactly which link broke: no PRESSED = indev never
 * delivered it; PRESSED with sel=NONE = hit a gap; PRESSED with a valid
 * sel but no VALUE_CHANGED = the matrix dropped it in between. */
static void kb_diag_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *kb = lv_event_get_current_target(e);
    static uint16_t last_sel = 0xFFFF;
    uint16_t sel = lv_btnmatrix_get_selected_btn(kb);
    int64_t t = esp_timer_get_time() / 1000;

    switch (code) {
    case LV_EVENT_PRESSED:
        ESP_LOGI(TAG, "> kb PRESSED  sel=%u t=%lld", sel, (long long)t);
        break;
    case LV_EVENT_PRESSING:
        if (sel != last_sel) {
            ESP_LOGI(TAG, "> kb SELCHG   sel=%u->%u t=%lld",
                     last_sel, sel, (long long)t);
        }
        break;
    case LV_EVENT_RELEASED:
        ESP_LOGI(TAG, "> kb RELEASED sel=%u t=%lld", sel, (long long)t);
        break;
    case LV_EVENT_VALUE_CHANGED: {
        const char *txt = (sel == 0xFFFF) ? NULL
                          : lv_btnmatrix_get_btn_text(kb, sel);
        ESP_LOGI(TAG, "> kb KEY      sel=%u '%s' t=%lld", sel,
                 txt ? txt : "?", (long long)t);
        break;
    }
    default:
        break;
    }
    last_sel = sel;
}

/* -------- Raw touch state sampler ----------------------------------- */

static void touch_sampler_cb(lv_timer_t *t) {
    (void)t;
    static lv_indev_state_t last = LV_INDEV_STATE_RELEASED;

    lv_indev_t *indev = lv_indev_get_next(NULL);
    while (indev && lv_indev_get_type(indev) != LV_INDEV_TYPE_POINTER) {
        indev = lv_indev_get_next(indev);
    }
    if (!indev) return;

    /* LVGL 8 doesn't expose lv_indev_get_state as a public function; the
     * state lives on the indev struct at indev->proc.state and the point
     * at indev->proc.types.pointer.act_point. */
    lv_indev_state_t st = indev->proc.state;
    if (st == last) return;

    /* act_obj = the object LVGL attributed this press to (press-lock
     * target — the widget that will get CLICKED on release). scroll_obj
     * set = LVGL reclassified the press as a scroll and the widget will
     * NOT be clicked. */
    lv_point_t pt = indev->proc.types.pointer.act_point;
    lv_obj_t *act = indev->proc.types.pointer.act_obj;
    lv_obj_t *scr = indev->proc.types.pointer.scroll_obj;
    ESP_LOGI(TAG, "* touch %s @ (%d,%d) act=%p scroll=%p t=%lld",
             st == LV_INDEV_STATE_PRESSED ? "DOWN" : "UP  ",
             (int)pt.x, (int)pt.y, (void *)act, (void *)scr,
             (long long)(esp_timer_get_time() / 1000));
    last = st;
}

/* -------- Public init ---------------------------------------------- */

void perf_init(void) {
    lv_timer_create(stall_watchdog_cb, 100, NULL);
    lv_timer_create(touch_sampler_cb, 20, NULL);

#if __has_include("ui/screens.h")
    lv_obj_t *kbs[] = {
        objects.wifi_setup_kb,
        objects.mqtt_setup_kb,
        objects.kb_button_edit,
    };
    for (size_t i = 0; i < sizeof(kbs)/sizeof(*kbs); i++) {
        if (kbs[i]) {
            lv_obj_add_event_cb(kbs[i], kb_diag_cb, LV_EVENT_ALL, NULL);
        }
    }
#endif

    ESP_LOGI(TAG, "diag probes installed (stall watchdog + touch sampler + kb log)");
}

#else /* !CONFIG_FIRESIDE_TOUCH_DIAG */

void perf_init(void) {}

#endif
