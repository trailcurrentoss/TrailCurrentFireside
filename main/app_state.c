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

/* Populate the scan-list rows on PageWifiSetup from the latest scan results.
 * Rows we don't have data for get HIDDEN. Called from the wifi_setup state
 * callback when scanning finishes. */
void app_state_paint_wifi_rows(void) {
#if __has_include("ui/screens.h")
    wifi_setup_network_t nets[8];
    size_t n = wifi_setup_get_scan_results(nets, 8);

    if (!lvgl_port_lock(0)) return;
    struct { lv_obj_t *row, *ssid, *rssi, *lock; } rows[8] = {
        {objects.wifi_row1, objects.wifi_row1_ssid, objects.wifi_row1_rssi, objects.wifi_row1_lock},
        {objects.wifi_row2, objects.wifi_row2_ssid, objects.wifi_row2_rssi, objects.wifi_row2_lock},
        {objects.wifi_row3, objects.wifi_row3_ssid, objects.wifi_row3_rssi, objects.wifi_row3_lock},
        {objects.wifi_row4, objects.wifi_row4_ssid, objects.wifi_row4_rssi, objects.wifi_row4_lock},
        {objects.wifi_row5, objects.wifi_row5_ssid, objects.wifi_row5_rssi, objects.wifi_row5_lock},
        {objects.wifi_row6, objects.wifi_row6_ssid, objects.wifi_row6_rssi, objects.wifi_row6_lock},
        {objects.wifi_row7, objects.wifi_row7_ssid, objects.wifi_row7_rssi, objects.wifi_row7_lock},
        {objects.wifi_row8, objects.wifi_row8_ssid, objects.wifi_row8_rssi, objects.wifi_row8_lock},
    };
    char buf[16];
    for (size_t i = 0; i < 8; i++) {
        if (!rows[i].row) continue;
        if (i < n) {
            lv_obj_clear_flag(rows[i].row, LV_OBJ_FLAG_HIDDEN);
            if (rows[i].ssid) lv_label_set_text(rows[i].ssid, nets[i].ssid);
            if (rows[i].rssi) {
                snprintf(buf, sizeof(buf), "%d dBm", (int)nets[i].rssi);
                lv_label_set_text(rows[i].rssi, buf);
            }
            /* Lock glyph: 0xF023 (FA lock). If not locked, hide the label. */
            if (rows[i].lock) {
                if (nets[i].locked) lv_label_set_text(rows[i].lock, "\xEF\x80\xA3");
                else                lv_label_set_text(rows[i].lock, "");
            }
        } else {
            lv_obj_add_flag(rows[i].row, LV_OBJ_FLAG_HIDDEN);
        }
    }
    if (n == 0) {
        set_wifi_status_text("No networks found — tap Rescan");
    } else {
        char status[64];
        snprintf(status, sizeof(status), "%d network%s found",
                 (int)n, n == 1 ? "" : "s");
        set_wifi_status_text(status);
    }
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
    case APP_STATE_WIFI_SETUP:      load(objects.page_wifi_setup); break;
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
