#include "app_state.h"

#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "lvgl.h"
#include "wifi_setup.h"
#include "fireside_config.h"
#include "app_mqtt.h"

/* The wizard screens/objects only exist after the EEZ patch is applied AND
 * the user re-runs EEZ Studio → Build. Until that's done, this TU compiles
 * to no-op stubs so the firmware keeps building.
 *
 * After the EEZ export contains the new pages, define FIRESIDE_HAS_WIZARD_UI=1
 * (either in main/CMakeLists.txt as a compile flag or in a local header) to
 * activate the wizard logic. */
#include "ui/screens.h"

#ifndef FIRESIDE_HAS_WIZARD_UI
#define FIRESIDE_HAS_WIZARD_UI 0
#endif

#if FIRESIDE_HAS_WIZARD_UI
#  define APP_STATE_UI_READY 1
#else
#  define APP_STATE_UI_READY 0
#endif

extern void bsp_display_lock(int timeout_ms);
extern void bsp_display_unlock(void);

static const char *TAG = "app_state";
static app_state_t s_state = APP_STATE_BOOT;

#if APP_STATE_UI_READY

/* ============================================================================
 * Helper resolvers — every reference to an `objects.*` member that the new
 * wizard requires goes through one of these. If the EEZ export hasn't
 * materialized the symbol yet (because the .eez-project patch hasn't been
 * applied / built), the guard collapses each to NULL and the state machine
 * stays compilable.
 *
 * The expected widget identifiers (added by the EEZ patch script):
 *   page_wifi_setup, page_wifi_connecting, page_mqtt_setup, page_mqtt_connecting
 *   wifi_scan_status, wifi_scan_spinner
 *   wifi_net_0..7  + per-row ssid/lock/bar1/bar2/bar3 labels
 *   wifi_password_panel, wifi_pwd_ssid, wifi_pwd_input, wifi_pwd_keyboard,
 *     wifi_pwd_reveal_icon
 *   wifi_connecting_ssid
 *   mqtt_caption, mqtt_step, mqtt_input, mqtt_keyboard,
 *     mqtt_next_lbl, mqtt_back_lbl, mqtt_connecting_host
 * ============================================================================ */

#define OBJ(name) (objects.name)

/* Per-row WiFi scan list paint helpers. */
static lv_obj_t *wifi_net_row(int idx)
{
    switch (idx) {
    case 0: return OBJ(wifi_net_0);
    case 1: return OBJ(wifi_net_1);
    case 2: return OBJ(wifi_net_2);
    case 3: return OBJ(wifi_net_3);
    case 4: return OBJ(wifi_net_4);
    case 5: return OBJ(wifi_net_5);
    case 6: return OBJ(wifi_net_6);
    case 7: return OBJ(wifi_net_7);
    default: return NULL;
    }
}
static lv_obj_t *wifi_net_ssid(int idx)
{
    switch (idx) {
    case 0: return OBJ(wifi_net_ssid_0);
    case 1: return OBJ(wifi_net_ssid_1);
    case 2: return OBJ(wifi_net_ssid_2);
    case 3: return OBJ(wifi_net_ssid_3);
    case 4: return OBJ(wifi_net_ssid_4);
    case 5: return OBJ(wifi_net_ssid_5);
    case 6: return OBJ(wifi_net_ssid_6);
    case 7: return OBJ(wifi_net_ssid_7);
    default: return NULL;
    }
}
static lv_obj_t *wifi_net_lock(int idx)
{
    switch (idx) {
    case 0: return OBJ(wifi_net_lock_0);
    case 1: return OBJ(wifi_net_lock_1);
    case 2: return OBJ(wifi_net_lock_2);
    case 3: return OBJ(wifi_net_lock_3);
    case 4: return OBJ(wifi_net_lock_4);
    case 5: return OBJ(wifi_net_lock_5);
    case 6: return OBJ(wifi_net_lock_6);
    case 7: return OBJ(wifi_net_lock_7);
    default: return NULL;
    }
}
static lv_obj_t *wifi_net_bar(int idx, int bar)
{
    lv_obj_t *bars[8][3] = {
        { OBJ(wifi_net_bar1_0), OBJ(wifi_net_bar2_0), OBJ(wifi_net_bar3_0) },
        { OBJ(wifi_net_bar1_1), OBJ(wifi_net_bar2_1), OBJ(wifi_net_bar3_1) },
        { OBJ(wifi_net_bar1_2), OBJ(wifi_net_bar2_2), OBJ(wifi_net_bar3_2) },
        { OBJ(wifi_net_bar1_3), OBJ(wifi_net_bar2_3), OBJ(wifi_net_bar3_3) },
        { OBJ(wifi_net_bar1_4), OBJ(wifi_net_bar2_4), OBJ(wifi_net_bar3_4) },
        { OBJ(wifi_net_bar1_5), OBJ(wifi_net_bar2_5), OBJ(wifi_net_bar3_5) },
        { OBJ(wifi_net_bar1_6), OBJ(wifi_net_bar2_6), OBJ(wifi_net_bar3_6) },
        { OBJ(wifi_net_bar1_7), OBJ(wifi_net_bar2_7), OBJ(wifi_net_bar3_7) },
    };
    if (idx < 0 || idx >= 8 || bar < 0 || bar >= 3) return NULL;
    return bars[idx][bar];
}

static void row_set_visible(int idx, bool visible)
{
    lv_obj_t *r = wifi_net_row(idx);
    if (!r) return;
    if (visible) lv_obj_clear_flag(r, LV_OBJ_FLAG_HIDDEN);
    else         lv_obj_add_flag(r, LV_OBJ_FLAG_HIDDEN);
}
static void row_set_ssid(int idx, const char *ssid)
{
    lv_obj_t *l = wifi_net_ssid(idx);
    if (l) lv_label_set_text(l, ssid);
}
static void row_set_locked(int idx, bool locked)
{
    lv_obj_t *l = wifi_net_lock(idx);
    if (!l) return;
    if (locked) lv_obj_clear_flag(l, LV_OBJ_FLAG_HIDDEN);
    else        lv_obj_add_flag(l, LV_OBJ_FLAG_HIDDEN);
}
static void row_set_bars(int idx, uint8_t bars)
{
    for (int i = 0; i < 3; i++) {
        lv_obj_t *b = wifi_net_bar(idx, i);
        if (!b) continue;
        if (i < bars) lv_obj_add_state(b, LV_STATE_CHECKED);
        else          lv_obj_clear_state(b, LV_STATE_CHECKED);
    }
}

static void load_screen(lv_obj_t *target) { if (target) lv_scr_load(target); }

static void make_unscrollable(lv_obj_t *screen)
{
    if (!screen) return;
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);
}

/* Externs implemented in actions.c — they own the per-page setup wiring. */
extern void fireside_mqtt_setup_enter(void);
extern void fireside_wifi_password_panel_show(bool show);

#endif /* APP_STATE_UI_READY */

/* Hooks into the existing UI side-effects in actions.c (label, RSSI polling,
 * mDNS, MQTT kick). Declared unconditionally so app_state.c always sees them;
 * the actions.c definitions are themselves WIFI_ENABLED-gated. */
#if defined(CONFIG_ESP_HOSTED_ENABLED)
extern void fireside_wifi_connected(uint32_t ip);
extern void fireside_wifi_disconnected(void);
#else
static inline void fireside_wifi_connected(uint32_t ip) { (void)ip; }
static inline void fireside_wifi_disconnected(void) {}
#endif

void app_state_refresh_wifi_list(void)
{
#if APP_STATE_UI_READY
    bsp_display_lock(0);
    wifi_setup_network_t nets[WIFI_SETUP_MAX_SCAN_RESULTS];
    size_t n = wifi_setup_get_scan_results(nets, WIFI_SETUP_MAX_SCAN_RESULTS);
    for (int i = 0; i < 8; i++) {
        if (i < (int)n) {
            row_set_visible(i, true);
            row_set_ssid(i, nets[i].ssid);
            row_set_locked(i, nets[i].locked);
            row_set_bars(i, nets[i].bars);
        } else {
            row_set_visible(i, false);
        }
    }
    if (OBJ(wifi_scan_spinner)) lv_obj_add_flag(OBJ(wifi_scan_spinner), LV_OBJ_FLAG_HIDDEN);
    if (OBJ(wifi_scan_status)) {
        char buf[48];
        snprintf(buf, sizeof(buf),
                 n == 0 ? "No networks found" : "%u network%s found",
                 (unsigned)n, n == 1 ? "" : "s");
        lv_label_set_text(OBJ(wifi_scan_status), buf);
    }
    bsp_display_unlock();
#endif
}

void app_state_wifi_show_scanning(void)
{
#if APP_STATE_UI_READY
    bsp_display_lock(0);
    if (OBJ(wifi_scan_spinner)) lv_obj_clear_flag(OBJ(wifi_scan_spinner), LV_OBJ_FLAG_HIDDEN);
    if (OBJ(wifi_scan_status))  lv_label_set_text(OBJ(wifi_scan_status), "Scanning...");
    bsp_display_unlock();
#endif
}

void app_state_wifi_show_idle_message(const char *msg)
{
#if APP_STATE_UI_READY
    bsp_display_lock(0);
    if (OBJ(wifi_scan_spinner)) lv_obj_add_flag(OBJ(wifi_scan_spinner), LV_OBJ_FLAG_HIDDEN);
    if (OBJ(wifi_scan_status))  lv_label_set_text(OBJ(wifi_scan_status), msg);
    bsp_display_unlock();
#else
    (void)msg;
#endif
}

/* ============================================================================
 * wifi_setup state callback. Runs on the WiFi event task — bounce LVGL work
 * via lv_async_call so we don't touch the UI from a non-LVGL context.
 * ============================================================================ */
#if APP_STATE_UI_READY
static void async_refresh_list(void *arg)   { (void)arg; app_state_refresh_wifi_list(); }
static void async_show_scanning(void *arg)  { (void)arg; app_state_wifi_show_scanning(); }
static void async_state_setup(void *arg)    { (void)arg; app_state_set(APP_STATE_WIFI_SETUP); }

static void async_show_fail(void *arg)
{
    (void)arg;
    const char *msg = "Connection failed. Pick a network and try again.";
    switch (wifi_setup_get_last_failure_reason()) {
    case WIFI_SETUP_FAIL_BAD_PASSWORD: msg = "Incorrect password. Try again."; break;
    case WIFI_SETUP_FAIL_AP_NOT_FOUND: msg = "Network not in range. Rescan."; break;
    case WIFI_SETUP_FAIL_TIMEOUT:      msg = "Timed out joining. Check signal."; break;
    default: break;
    }
    app_state_wifi_show_idle_message(msg);
}

static void async_wifi_up_next(void *arg)
{
    (void)arg;
    /* Notify the existing actions.c UI hooks (label, RSSI polling). It also
     * handles mDNS + the initial MQTT connect — so for the "saved MQTT" path
     * we don't double-call mqtt_client_connect(). */
    fireside_wifi_connected(wifi_setup_get_ip());

    if (fireside_config_has_mqtt()) {
        if (mqtt_client_load_settings()) {
            app_state_set(APP_STATE_MQTT_CONNECTING);
            /* fireside_wifi_connected already calls mqtt_client_connect() —
             * no need to repeat it here. */
        } else {
            app_state_set(APP_STATE_MQTT_SETUP);
        }
    } else {
        app_state_set(APP_STATE_MQTT_SETUP);
    }
}

static void async_wifi_down(void *arg)
{
    (void)arg;
    fireside_wifi_disconnected();
}

static void on_wifi_state(wifi_setup_state_t st, void *user_ctx)
{
    (void)user_ctx;
    ESP_LOGI(TAG, "wifi_setup state = %d", (int)st);
    switch (st) {
    case WIFI_SETUP_STATE_IDLE:
        if (s_state == APP_STATE_WIFI_SETUP) {
            lv_async_call(async_refresh_list, NULL);
        }
        break;
    case WIFI_SETUP_STATE_SCANNING:
        if (s_state == APP_STATE_WIFI_SETUP) {
            lv_async_call(async_show_scanning, NULL);
        }
        break;
    case WIFI_SETUP_STATE_CONNECTED:
        if (s_state == APP_STATE_WIFI_CONNECTING) {
            lv_async_call(async_wifi_up_next, NULL);
        }
        break;
    case WIFI_SETUP_STATE_FAILED:
        lv_async_call(async_wifi_down, NULL);
        if (s_state == APP_STATE_WIFI_CONNECTING) {
            lv_async_call(async_show_fail, NULL);
            lv_async_call(async_state_setup, NULL);
        }
        break;
    default: break;
    }
}

/* MQTT state callback — advance to READY on the first MQTT result either way.
 * Even if the broker is unreachable we still want the dashboard usable; the
 * MQTT client keeps retrying in the background. */
static void async_state_ready_from_mqtt(void *arg)
{
    (void)arg;
    if (s_state == APP_STATE_MQTT_CONNECTING) app_state_set(APP_STATE_READY);
}

static void on_mqtt_state(bool connected)
{
    ESP_LOGI(TAG, "mqtt connected = %d", (int)connected);
    lv_async_call(async_state_ready_from_mqtt, NULL);
    (void)connected;
}

/* Tap-anywhere-to-skip on the MQTT connecting spinner — without this an
 * unreachable broker traps the device with no escape. */
static void mqtt_connecting_skip_cb(lv_event_t *e)
{
    (void)e;
    if (s_state == APP_STATE_MQTT_CONNECTING) {
        ESP_LOGI(TAG, "MQTT-connecting skipped by user — advancing to READY");
        app_state_set(APP_STATE_READY);
    }
}
static void mqtt_connecting_wire_skip(void)
{
    static bool wired = false;
    if (wired || !OBJ(page_mqtt_connecting)) return;
    lv_obj_add_flag(OBJ(page_mqtt_connecting), LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(OBJ(page_mqtt_connecting), mqtt_connecting_skip_cb,
                        LV_EVENT_CLICKED, NULL);
    wired = true;
}
#endif /* APP_STATE_UI_READY */

void app_state_set(app_state_t next)
{
    if (s_state == next) return;
    ESP_LOGI(TAG, "state %d -> %d", (int)s_state, (int)next);
    s_state = next;

#if APP_STATE_UI_READY
    bsp_display_lock(0);
    switch (next) {
    case APP_STATE_WIFI_SETUP:
        make_unscrollable(OBJ(page_wifi_setup));
        if (OBJ(wifi_password_panel)) make_unscrollable(OBJ(wifi_password_panel));
        load_screen(OBJ(page_wifi_setup));
        fireside_wifi_password_panel_show(false);
        /* Hide all 8 placeholder rows ("Network 1".."Network 8" from the EEZ
         * authoring placeholders). app_state_refresh_wifi_list() unhides as
         * many as the scan returns once results arrive. */
        for (int i = 0; i < 8; i++) row_set_visible(i, false);
        app_state_wifi_show_scanning();
        wifi_setup_scan_start();
        break;
    case APP_STATE_WIFI_CONNECTING:
        load_screen(OBJ(page_wifi_connecting));
        break;
    case APP_STATE_MQTT_SETUP:
        if (OBJ(page_mqtt_setup)) {
            make_unscrollable(OBJ(page_mqtt_setup));
            load_screen(OBJ(page_mqtt_setup));
            fireside_mqtt_setup_enter();
        } else {
            ESP_LOGW(TAG, "page_mqtt_setup not exported — skipping to READY");
            app_state_set(APP_STATE_READY);
        }
        break;
    case APP_STATE_MQTT_CONNECTING:
        if (OBJ(page_mqtt_connecting)) {
            load_screen(OBJ(page_mqtt_connecting));
            mqtt_connecting_wire_skip();
        }
        break;
    case APP_STATE_READY:
        load_screen(OBJ(page_home));
        break;
    default: break;
    }
    bsp_display_unlock();
#endif
}

app_state_t app_state_get(void) { return s_state; }

#if APP_STATE_UI_READY
/* THE ONLY remaining C-side visual override.
 *
 * LVGL's lv_keyboard widget overrides its left/top/width/height with its
 * own internal alignment at runtime - the EEZ-authored coordinates are
 * ignored. Spotter hit the same bug and fixes it the same way (see
 * TrailCurrentSpotter/main/main.c `fix_keyboard_alignment()`).
 *
 * Force LV_ALIGN_TOP_LEFT then re-pin position + size to EXACTLY the
 * coordinates the .eez-project authors so the EEZ Studio canvas and the
 * device agree on where the keyboard lives. EVERY OTHER visual fix
 * (font sizes, button positions, label centering, signal-bar baselines)
 * lives in the .eez-project per the CLAUDE.md rule:
 * "Code Controls STATE, EEZ Studio Controls APPEARANCE". */
static void fix_keyboard(lv_obj_t *kb, int x, int y, int w, int h)
{
    if (!kb) return;
    lv_obj_set_align(kb, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(kb, x, y);
    lv_obj_set_size(kb, w, h);
}

static void fixup_wizard_keyboards(void)
{
    /* Coordinates must match the .eez-project values for these two
     * keyboard widgets. If you change either, change both. */
    fix_keyboard(OBJ(mqtt_keyboard),    8, 180, 1008, 340);
    fix_keyboard(OBJ(wifi_pwd_keyboard), 8, 180, 1008, 340);
}
#endif

esp_err_t app_state_init(void)
{
#if APP_STATE_UI_READY
    ESP_ERROR_CHECK(wifi_setup_init(on_wifi_state, NULL));
    mqtt_client_set_state_callback(on_mqtt_state);
    /* Only LVGL-widget-quirk fixup that has to stay in C: the lv_keyboard
     * widget overrides its EEZ-authored geometry at runtime. */
    bsp_display_lock(0);
    fixup_wizard_keyboards();
    bsp_display_unlock();

    if (fireside_config_has_wifi()) {
        const fireside_config_t *cfg = fireside_config_get();
        ESP_LOGI(TAG, "auto-connecting to saved SSID: %s", cfg->wifi_ssid);
        app_state_set(APP_STATE_WIFI_CONNECTING);
        bsp_display_lock(0);
        if (OBJ(wifi_connecting_ssid)) {
            lv_label_set_text(OBJ(wifi_connecting_ssid), cfg->wifi_ssid);
        }
        bsp_display_unlock();
        wifi_setup_connect(cfg->wifi_ssid, cfg->wifi_pass);
    } else {
        ESP_LOGI(TAG, "no saved WiFi — entering setup");
        app_state_set(APP_STATE_WIFI_SETUP);
    }
#else
    ESP_LOGW(TAG, "EEZ wizard screens not exported yet — app_state_init is a no-op");
#endif
    return ESP_OK;
}

void app_state_reset_connection_and_reenter(void)
{
    ESP_LOGW(TAG, "Reset confirmed — clearing fireside_config and rebooting");
    fireside_config_clear_wifi();
    fireside_config_clear_mqtt();
    vTaskDelay(pdMS_TO_TICKS(250));
    esp_restart();
}
