/*
 * app_state.c — Fireside CrowPanel top-level state machine.
 *
 * Boot flow:
 *   BOOT
 *     │
 *     ├── saved WiFi + saved MQTT ── WIFI_CONNECTING → MQTT_CONNECTING → READY (Home)
 *     ├── saved WiFi, no MQTT ────── WIFI_CONNECTING → MQTT_SETUP → …
 *     └── no WiFi ─────────────────── WIFI_SETUP (scan list) → WIFI_CONNECTING → …
 *
 * The wizard pages (`page_wifi_setup`, `page_wifi_connecting`, etc.) are
 * authored in the EEZ project. wifi_setup + mqtt_client are called by the
 * action handlers in actions.c; we just react to their state callbacks and
 * flip to the appropriate screen.
 */
#include "app_state.h"

#include "esp_log.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "fireside_config.h"
#include "wifi_setup.h"
#include "app_mqtt.h"

#if __has_include("ui/screens.h")
#include "screens.h"
#include "ui.h"
#include "vars.h"
#endif

static const char *TAG = "APP_STATE";
static app_state_t s_state = APP_STATE_BOOT;

static void load(lv_obj_t *scr) {
    if (!scr) { ESP_LOGW(TAG, "screen NULL, skipping"); return; }
    if (lvgl_port_lock(0)) {
        lv_scr_load(scr);
        lvgl_port_unlock();
    }
}

static void set_wifi_status_text(const char *text) {
#if __has_include("ui/screens.h")
    if (!lvgl_port_lock(0)) return;
    if (objects.label_wifi_connection_status)
        lv_label_set_text(objects.label_wifi_connection_status, text);
    if (objects.wifi_connecting_status)
        lv_label_set_text(objects.wifi_connecting_status, text);
    lvgl_port_unlock();
#else
    (void)text;
#endif
}

/* MQTT state callback — flip to Home once we're connected to the broker. */
static void on_mqtt_state(bool connected) {
    ESP_LOGI(TAG, "MQTT state: %s", connected ? "connected" : "disconnected");
    if (connected && s_state == APP_STATE_MQTT_CONNECTING) {
        app_state_set(APP_STATE_READY);
    }
}

/* WiFi state callback — advance the state machine as scan/connect progresses. */
static void on_wifi_state(wifi_setup_state_t st, void *ctx) {
    (void)ctx;
    ESP_LOGI(TAG, "WiFi state: %d", (int)st);
    switch (st) {
    case WIFI_SETUP_STATE_SCANNING:
        set_wifi_status_text("Scanning...");
        break;
    case WIFI_SETUP_STATE_IDLE:
        /* Scan finished — repaint the row list on the WiFi setup page. */
        extern void app_state_paint_wifi_rows(void);
        app_state_paint_wifi_rows();
        break;
    case WIFI_SETUP_STATE_CONNECTING:
        set_wifi_status_text("Connecting...");
        break;
    case WIFI_SETUP_STATE_CONNECTED:
        set_wifi_status_text("Connected");
        /* Save creds if we don't have them yet (means user just went
         * through the wizard). Then advance to MQTT setup or connect. */
        if (fireside_config_has_mqtt()) {
            app_state_set(APP_STATE_MQTT_CONNECTING);
            mqtt_client_load_settings();
            mqtt_client_connect();
        } else {
            app_state_set(APP_STATE_MQTT_SETUP);
        }
        break;
    case WIFI_SETUP_STATE_FAILED: {
        wifi_setup_fail_reason_t r = wifi_setup_get_last_failure_reason();
        const char *msg =
            (r == WIFI_SETUP_FAIL_BAD_PASSWORD) ? "Bad password" :
            (r == WIFI_SETUP_FAIL_AP_NOT_FOUND) ? "Network not found" :
            (r == WIFI_SETUP_FAIL_TIMEOUT)      ? "Connection timeout"
                                                : "Connection failed";
        set_wifi_status_text(msg);
        /* Bounce back to setup screen so the user can retry. */
        app_state_set(APP_STATE_WIFI_SETUP);
        break;
    }
    default: break;
    }
}

/* Dynamic WiFi row list.
 *
 * The scan results panel is now a scrollable container (wifi_row_container
 * on the .eez-project) that we fill row-by-row at scan-complete time.
 * Each row is a code-created lv_btn with SSID + RSSI + lock-icon labels
 * and a click handler that captures the row's SSID into a cache and
 * bounces to actions.c's wifi_row_selected() to do the open-connect or
 * password-screen transition.
 *
 * The SSID cache exists so the click handler can look up the tapped
 * SSID without walking the LVGL child list. Cache index = row_index =
 * user_data on the button.
 */

static char s_row_ssid[WIFI_SETUP_MAX_SCAN_RESULTS][WIFI_SETUP_SSID_MAX];
static bool s_row_locked[WIFI_SETUP_MAX_SCAN_RESULTS];
static size_t s_row_count = 0;

#if __has_include("ui/screens.h")
#include "styles.h"
#include "fonts.h"

static void wifi_row_click_cb(lv_event_t *e) {
    intptr_t idx = (intptr_t)lv_event_get_user_data(e);
    if (idx < 0 || (size_t)idx >= s_row_count) return;
    wifi_row_selected(s_row_ssid[idx], s_row_locked[idx]);
}

/* Create one row inside the scrollable container. Absolute positioned so
 * the container's inherent scroll math just works (content height = N*42
 * pixels; anything past the container's 410 px viewport scrolls). */
static void wifi_build_row(lv_obj_t *container, size_t idx,
                           const wifi_setup_network_t *net) {
    lv_obj_t *row = lv_btn_create(container);
    add_style_button_default(row);
    lv_obj_set_pos(row, 0, (lv_coord_t)(idx * 42));
    lv_obj_set_size(row, 960, 36);   /* 968-8 leaves a bit for the scrollbar */
    lv_obj_add_event_cb(row, wifi_row_click_cb, LV_EVENT_CLICKED,
                        (void *)(intptr_t)idx);

    /* SSID text — left, most of the row. */
    lv_obj_t *ssid = lv_label_create(row);
    add_style_label_default(ssid);
    lv_obj_set_style_text_font(ssid, &ui_font_rr14,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ssid,
        lv_color_hex(theme_colors[active_theme_index][6]),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ssid, LV_TEXT_ALIGN_LEFT,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_long_mode(ssid, LV_LABEL_LONG_DOT);
    lv_obj_set_pos(ssid, 12, 9);
    lv_obj_set_size(ssid, 742, 20);
    lv_label_set_text(ssid, net->ssid);

    /* RSSI dBm — right-aligned in the middle-right column. */
    lv_obj_t *rssi = lv_label_create(row);
    add_style_label_default(rssi);
    lv_obj_set_style_text_font(rssi, &ui_font_rr13,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(rssi,
        lv_color_hex(theme_colors[active_theme_index][8]),
        LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(rssi, LV_TEXT_ALIGN_RIGHT,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_pos(rssi, 766, 9);
    lv_obj_set_size(rssi, 140, 20);
    char buf[16];
    snprintf(buf, sizeof(buf), "%d dBm", (int)net->rssi);
    lv_label_set_text(rssi, buf);

    /* Lock glyph on locked networks (fa14 U+F023 padlock). */
    if (net->locked) {
        lv_obj_t *lock = lv_label_create(row);
        add_style_label_default(lock);
        lv_obj_set_style_text_font(lock, &ui_font_fa14,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(lock,
            lv_color_hex(theme_colors[active_theme_index][8]),
            LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(lock, LV_TEXT_ALIGN_CENTER,
                                    LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_pos(lock, 916, 9);
        lv_obj_set_size(lock, 20, 20);
        lv_label_set_text(lock, "\xEF\x80\xA3");
    }
}
#endif

/* Populate the scan-list rows on PageWifiSetup from the latest scan results.
 * Called from the wifi_setup state callback when scanning finishes. */
void app_state_paint_wifi_rows(void) {
#if __has_include("ui/screens.h")
    wifi_setup_network_t nets[WIFI_SETUP_MAX_SCAN_RESULTS];
    size_t n = wifi_setup_get_scan_results(nets, WIFI_SETUP_MAX_SCAN_RESULTS);

    if (!lvgl_port_lock(0)) return;
    if (!objects.wifi_row_container) {
        lvgl_port_unlock();
        return;
    }

    /* Destroy previous rows + rebuild from the latest scan. lv_obj_clean
     * removes every child recursively — cheap since rows are simple
     * buttons with 2-3 label children. */
    lv_obj_clean(objects.wifi_row_container);
    /* Reset scroll so the user sees the top of the new list. */
    lv_obj_scroll_to(objects.wifi_row_container, 0, 0, LV_ANIM_OFF);

    if (n > WIFI_SETUP_MAX_SCAN_RESULTS) n = WIFI_SETUP_MAX_SCAN_RESULTS;
    s_row_count = n;
    for (size_t i = 0; i < n; i++) {
        strlcpy(s_row_ssid[i], nets[i].ssid, sizeof(s_row_ssid[i]));
        s_row_locked[i] = nets[i].locked;
        wifi_build_row(objects.wifi_row_container, i, &nets[i]);
    }

    if (n == 0) {
        set_wifi_status_text("No networks found — tap Rescan");
    } else {
        char status[64];
        snprintf(status, sizeof(status), "%zu network%s found",
                 n, n == 1 ? "" : "s");
        set_wifi_status_text(status);
    }
    lvgl_port_unlock();
#endif
}

/* --- Two-state layout of PageWifiSetup ---------------------------------
 * List picker (state 1) and password entry (state 2) share one page and
 * are toggled by hiding/showing the two cards + keyboard. Both helpers
 * take the display lock; the transition happens under a single lock so
 * the user never sees a half-drawn intermediate. */
void app_state_wifi_show_list(void) {
#if __has_include("ui/screens.h")
    if (!lvgl_port_lock(0)) return;
    if (objects.wifi_setup_list_card)
        lv_obj_clear_flag(objects.wifi_setup_list_card, LV_OBJ_FLAG_HIDDEN);
    if (objects.wifi_setup_right_card)
        lv_obj_add_flag(objects.wifi_setup_right_card, LV_OBJ_FLAG_HIDDEN);
    if (objects.wifi_setup_kb)
        lv_obj_add_flag(objects.wifi_setup_kb, LV_OBJ_FLAG_HIDDEN);
    lvgl_port_unlock();
#endif
}

void app_state_wifi_show_password(const char *ssid) {
#if __has_include("ui/screens.h")
    if (!lvgl_port_lock(0)) return;
    if (objects.wifi_setup_list_card)
        lv_obj_add_flag(objects.wifi_setup_list_card, LV_OBJ_FLAG_HIDDEN);
    if (objects.wifi_setup_right_card)
        lv_obj_clear_flag(objects.wifi_setup_right_card, LV_OBJ_FLAG_HIDDEN);
    if (objects.wifi_setup_kb)
        lv_obj_clear_flag(objects.wifi_setup_kb, LV_OBJ_FLAG_HIDDEN);
    /* Repurpose the pw_title label as the "Enter password for <SSID>"
     * prompt. Authored text is the generic version so the EEZ canvas
     * preview reads correctly; runtime substitutes the real SSID. */
    if (objects.wifi_setup_pw_title && ssid && *ssid) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Enter password for \"%s\"", ssid);
        lv_label_set_text(objects.wifi_setup_pw_title, buf);
    }
    if (objects.wifi_pw_input) {
        lv_textarea_set_text(objects.wifi_pw_input, "");
        if (objects.wifi_setup_kb) {
            lv_keyboard_set_textarea(objects.wifi_setup_kb,
                                     objects.wifi_pw_input);
        }
    }
    lvgl_port_unlock();
#endif
}

void app_state_paint_wifi_scanning(void) {
#if __has_include("ui/screens.h")
    if (!lvgl_port_lock(0)) return;
    if (objects.wifi_row_container) {
        lv_obj_clean(objects.wifi_row_container);
        s_row_count = 0;
    }
    set_wifi_status_text("Scanning...");
    lvgl_port_unlock();
#endif
}

esp_err_t app_state_init(void) {
    /* Register MQTT state callback so we can flip to READY on connect.
     * MQTT client doesn't need to be initialized yet — this just stores the
     * function pointer. */
    mqtt_client_set_state_callback(on_mqtt_state);

    /* NOTE: wifi_setup_init() is NOT called here — it registers WIFI_EVENT
     * handlers which require esp_wifi_init to have run first. That happens
     * on the background WiFi task in main.c; app_state_start_wifi() below
     * gets called from that task once WiFi is ready.
     *
     * All we do here is load the correct starting screen based on saved
     * creds — either the wizard (no creds) or Connecting... (creds saved).
     * That way the UI shows immediately even if the C6 slave is dead. */
#if __has_include("ui/screens.h")
    if (fireside_config_has_wifi()) {
        const fireside_config_t *cfg = fireside_config_get();
        char msg[96];
        snprintf(msg, sizeof(msg), "Connecting to %s...", cfg->wifi_ssid);
        set_wifi_status_text(msg);
        app_state_set(APP_STATE_WIFI_CONNECTING);
    } else {
        set_wifi_status_text("Scanning...");
        app_state_set(APP_STATE_WIFI_SETUP);
    }
#else
    ESP_LOGW(TAG, "ui/screens.h not present — cannot drive wizard");
#endif
    return ESP_OK;
}

/* Called by the WiFi bring-up task in main.c once esp_wifi_init +
 * esp_wifi_start have completed. Registers the wifi_setup event handlers
 * and kicks off scan-or-auto-connect based on saved config. */
void app_state_start_wifi(void) {
    esp_err_t err = wifi_setup_init(on_wifi_state, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "wifi_setup_init: %s", esp_err_to_name(err));
        return;
    }
    if (fireside_config_has_wifi()) {
        const fireside_config_t *cfg = fireside_config_get();
        ESP_LOGI(TAG, "auto-connecting to saved SSID %s", cfg->wifi_ssid);
        wifi_setup_connect(cfg->wifi_ssid, cfg->wifi_pass);
    } else {
        ESP_LOGI(TAG, "no saved wifi — starting scan");
        wifi_setup_scan_start();
    }
}

void app_state_set(app_state_t next) {
    s_state = next;
#if __has_include("ui/screens.h")
    switch (next) {
    case APP_STATE_WIFI_SETUP:
        load(objects.page_wifi_setup);
        /* Always enter the wizard on the list-picker sub-state, so if the
         * user was mid-password-entry when we bounced back (e.g. bad
         * password → FAILED → WIFI_SETUP), they land on the network list
         * instead of an orphaned password screen for the previous SSID. */
        app_state_wifi_show_list();
        break;
    case APP_STATE_WIFI_CONNECTING: load(objects.page_wifi_connecting); break;
    case APP_STATE_MQTT_SETUP:      load(objects.page_mqtt_setup); break;
    case APP_STATE_MQTT_CONNECTING: load(objects.page_mqtt_connecting); break;
    case APP_STATE_READY:           load(objects.page_home); break;
    default: break;
    }
#endif
}

app_state_t app_state_get(void) { return s_state; }

void app_state_reset_connection_and_reenter(void) {
    ESP_LOGI(TAG, "Reset Connection — clearing NVS + re-entering wizard");
    fireside_config_clear_wifi();
    fireside_config_clear_mqtt();
    mqtt_client_stop();
    wifi_setup_disconnect();
    app_state_set(APP_STATE_WIFI_SETUP);
    wifi_setup_scan_start();
}
