#pragma once

/*
 * Top-level Fireside state machine.
 *
 *      ┌─── BOOT ───────────────────────────────────┐
 *      │ NVS, ESP-Hosted, netif, display, LVGL, ui  │
 *      └────────────────────────────────────────────┘
 *                  │
 *                  ▼  no saved SSID
 *      ┌─── WIFI_SETUP ─────────────────────────────┐
 *      │ PageWifiSetup — scan + password panel      │
 *      └────────────────────────────────────────────┘
 *                  │  user submits credentials
 *                  ▼
 *      ┌─── WIFI_CONNECTING ────────────────────────┐
 *      │ PageWifiConnecting — spinner               │
 *      └────────────────────────────────────────────┘
 *                  │  IP_EVENT_STA_GOT_IP
 *                  ▼  saved MQTT?  yes → CONNECTING
 *      ┌─── MQTT_SETUP ─────────────────────────────┐
 *      │ PageMqttSetup — host/user/pass wizard      │
 *      └────────────────────────────────────────────┘
 *                  │
 *                  ▼
 *      ┌─── MQTT_CONNECTING ────────────────────────┐
 *      │ PageMqttConnecting — spinner               │
 *      └────────────────────────────────────────────┘
 *                  │  first MQTT result OR user-skip
 *                  ▼
 *      ┌─── READY ──────────────────────────────────┐
 *      │ PageHome — dashboard                       │
 *      └────────────────────────────────────────────┘
 */

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    APP_STATE_BOOT = 0,
    APP_STATE_WIFI_SETUP,
    APP_STATE_WIFI_CONNECTING,
    APP_STATE_MQTT_SETUP,
    APP_STATE_MQTT_CONNECTING,
    APP_STATE_READY,
} app_state_t;

esp_err_t   app_state_init(void);
void        app_state_set(app_state_t next);
app_state_t app_state_get(void);

/* Repaint the WiFi scan list rows from the latest wifi_setup scan results.
 * Safe from any task — bounces to LVGL thread internally. */
void app_state_refresh_wifi_list(void);

/* Status caption / spinner on PageWifiSetup. */
void app_state_wifi_show_scanning(void);
void app_state_wifi_show_idle_message(const char *msg);

/* Force enter the wizard from anywhere (Reset Connection in Settings). */
void app_state_reset_connection_and_reenter(void);

#ifdef __cplusplus
}
#endif
