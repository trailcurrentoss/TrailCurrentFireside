/*
 * perf.c — LVGL task stall detector + keyboard mode-switch profiler.
 *
 * Two lightweight probes, both intended for on-device diagnosis of the
 * "keyboard mode switch takes forever" complaint on the wizard pages.
 *
 * 1. Stall watchdog — an lv_timer that fires every 100 ms. Because
 *    lv_timers run on the LVGL task, a stalled task can't fire them on
 *    time. When the delta between fires exceeds 200 ms we log the actual
 *    duration; that number is a direct measurement of how long the LVGL
 *    task was blocked on something (a slow render, a long callback, a
 *    lock contention, etc.).
 *
 * 2. Keyboard VALUE_CHANGED probe — hooks LV_EVENT_VALUE_CHANGED on each
 *    of the three keyboards and logs each event with a monotonic
 *    timestamp. Correlate with the very-next "LVGL task stalled" line to
 *    see how long the mode-switch render took: a stall right after a kb
 *    event = the mode switch was what blocked the task.
 *
 * Compile-guarded on ui/screens.h so the file collapses when the export
 * hasn't run yet.
 */

#include "esp_log.h"
#include "esp_timer.h"
#include "lvgl.h"

#if __has_include("ui/screens.h")
#include "screens.h"
#endif

static const char *TAG = "PERF";

/* -------- Stall watchdog -------------------------------------------- */

#define STALL_LOG_THRESHOLD_US 200000LL   /* 200 ms — one keyboard mode
                                           * switch under load is around
                                           * this order on the P4. */

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

static void kb_value_changed_cb(lv_event_t *e) {
    lv_obj_t *kb = lv_event_get_current_target(e);
    /* Print a leading '>' so it's easy to spot in the log stream that a
     * kb event landed. The very next "LVGL task stalled" line (if any)
     * measures the mode-switch render cost. */
    ESP_LOGI(TAG, "> kb event  %p  t=%lld", (void *)kb,
             (long long)(esp_timer_get_time() / 1000));
}

/* -------- Raw touch state sampler ----------------------------------- */

/* Polls lv_indev_get_state() every 20 ms and logs on transitions. This
 * captures what LVGL SAW from the touch driver — independent of whether
 * a widget's own callback fired. If a user tap doesn't produce a
 * DOWN/UP pair here, the touch driver (esp_lcd_touch_gt911 via
 * esp_lvgl_port) failed to convert the physical press into an
 * LV_INDEV_STATE_PRESSED. That's the smoking gun for "keys don't
 * register even though the LVGL task never stalled". */
static void touch_sampler_cb(lv_timer_t *t) {
    (void)t;
    static lv_indev_state_t last = LV_INDEV_STATE_RELEASED;

    /* Find the pointer indev (there's exactly one on this build). */
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

    lv_point_t pt = indev->proc.types.pointer.act_point;
    ESP_LOGI(TAG, "* touch %s @ (%d,%d) t=%lld",
             st == LV_INDEV_STATE_PRESSED ? "DOWN" : "UP  ",
             (int)pt.x, (int)pt.y,
             (long long)(esp_timer_get_time() / 1000));
    last = st;
}

/* -------- Touch coordinate glitch filter ----------------------------- */

/* The 10" glass's GT911 emits two kinds of coordinate error:
 *   1. Gradual drift under a stationary finger (handled by scroll_limit).
 *   2. Single-sample jumps, worst at lift-off — the release-adjacent
 *      sample can teleport 100+ px (observed: DOWN (936,415) → UP
 *      (947,277), a 138 px jump). btnmatrix tracks the point while
 *      pressed, so a lift-off jump changes WHICH KEY fires.
 * This wrapper around esp_lvgl_port's read_cb rejects any in-press jump
 * larger than JUMP_REJECT_PX unless it persists for two consecutive
 * samples. A real fast drag keeps reporting the new position, so it's
 * accepted one sample (~10 ms) late; a lift-off glitch is followed by
 * RELEASE, never gets confirmed, and is discarded. */

#define JUMP_REJECT_PX 60

static void (*s_orig_read_cb)(lv_indev_drv_t *drv, lv_indev_data_t *data);

static void filtered_touch_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    static bool have_last = false;    /* inside a press, last_pt valid   */
    static bool have_pending = false; /* unconfirmed jump candidate      */
    static lv_point_t last_pt;
    static lv_point_t pending_pt;

    s_orig_read_cb(drv, data);

    if (data->state != LV_INDEV_STATE_PRESSED) {
        have_last = false;
        have_pending = false;
        return;
    }

    if (!have_last) {                 /* first sample of the press */
        have_last = true;
        last_pt = data->point;
        return;
    }

    int32_t dx = data->point.x - last_pt.x;
    int32_t dy = data->point.y - last_pt.y;
    if (dx * dx + dy * dy <= JUMP_REJECT_PX * JUMP_REJECT_PX) {
        last_pt = data->point;        /* normal movement */
        have_pending = false;
        return;
    }

    if (have_pending) {
        int32_t px = data->point.x - pending_pt.x;
        int32_t py = data->point.y - pending_pt.y;
        if (px * px + py * py <= JUMP_REJECT_PX * JUMP_REJECT_PX) {
            last_pt = data->point;    /* jump confirmed — real fast drag */
            have_pending = false;
            return;
        }
    }

    pending_pt = data->point;         /* hold position until confirmed */
    have_pending = true;
    data->point = last_pt;
}

/* -------- Public init ---------------------------------------------- */

void perf_init(void) {
    /* Fires every 100 ms on the LVGL task. Runtime cost is a single
     * esp_timer_get_time() + a compare. */
    lv_timer_create(stall_watchdog_cb, 100, NULL);
    /* Raw touch state — 20 ms sample interval catches every press+release
     * pair (typical fingertip tap > 40 ms). Logs only on transitions. */
    lv_timer_create(touch_sampler_cb, 20, NULL);

    /* Bump the pointer-drift tolerances on the touch indev.
     *
     * Root cause observed on this 10" glass: fingertip presses commonly
     * drift 20-140 px between the DOWN and UP samples. LVGL's default
     * scroll_limit (10 px) is well below that, so any press that moves
     * more than 10 px is classified as a scroll gesture instead of a
     * click, and no CLICKED event fires on the keyboard button under
     * the finger. User sees "I tapped the key, nothing happened,
     * tapped again." The DOWN/UP pair is present in the touch log —
     * LVGL just refused to convert it into a click.
     *
     * scroll_limit=150 covers the full observed drift range (up to
     * ~140 px measured on this glass) with margin — presses that drift
     * up to two fingertip widths still register as clicks.
     * LVGL's PRESS_LOCK guarantees the CLICKED event still fires on the
     * ORIGINAL widget under the press (not whatever the finger drifted
     * onto) so this doesn't cause wrong-key registrations. Genuine
     * scroll gestures typically travel 200+ px in a single direction so
     * they still work.
     *
     * long_press_time bumped to 800 ms (from LVGL default 400 ms) so a
     * user who holds a fraction of a second longer than average still
     * gets a normal CLICKED instead of LONG_PRESSED (which the keyboard
     * treats differently — it repeats the character). */
    lv_indev_t *ptr = lv_indev_get_next(NULL);
    while (ptr && lv_indev_get_type(ptr) != LV_INDEV_TYPE_POINTER) {
        ptr = lv_indev_get_next(ptr);
    }
    if (ptr && ptr->driver) {
        ptr->driver->scroll_limit    = 150;
        ptr->driver->long_press_time = 800;
        /* Interpose the glitch filter between esp_lvgl_port's GT911 read
         * and LVGL's indev processing. */
        s_orig_read_cb = ptr->driver->read_cb;
        ptr->driver->read_cb = filtered_touch_read_cb;
        ESP_LOGI(TAG,
            "touch: scroll_limit=%u long_press_time=%u ms jump_filter=%d px",
            ptr->driver->scroll_limit, ptr->driver->long_press_time,
            JUMP_REJECT_PX);
    }

#if __has_include("ui/screens.h")
    lv_obj_t *kbs[] = {
        objects.wifi_setup_kb,
        objects.mqtt_setup_kb,
        objects.kb_button_edit,
    };
    for (size_t i = 0; i < sizeof(kbs)/sizeof(*kbs); i++) {
        if (kbs[i]) {
            lv_obj_add_event_cb(kbs[i], kb_value_changed_cb,
                                LV_EVENT_VALUE_CHANGED, NULL);
        }
    }
#endif

    ESP_LOGI(TAG, "perf probes installed (stall watchdog + kb event log)");
}
