#pragma once

/*
 * Fireside CrowPanel top-level state machine.
 *
 *   BOOT
 *     │
 *     ▼
 *   WIFI_SETUP       → PageWifiSetup    (scan + password panel)
 *     │
 *     ▼
 *   WIFI_CONNECTING  → PageWifiConnecting
 *     │
 *     ▼
 *   MQTT_SETUP       → PageMqttSetup    (host / user / pass wizard)
 *     │
 *     ▼
 *   MQTT_CONNECTING  → PageMqttConnecting
 *     │
 *     ▼
 *   READY            → PageHome (or last-seen page)
 *
 * A boot with saved WiFi + MQTT creds skips straight from BOOT to READY.
 * Reset Connection (Settings page) wipes NVS and re-enters WIFI_SETUP.
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

/* Called from the WiFi bring-up task once esp_wifi_init/start has run.
 * Registers wifi_setup event handlers and kicks off scan-or-connect. */
void        app_state_start_wifi(void);

/* Force enter the wizard from anywhere (Reset Connection in Settings). */
void app_state_reset_connection_and_reenter(void);

#ifdef __cplusplus
}
#endif
