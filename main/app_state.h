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

/* Immediately hide every scan-result row and put "Scanning..." into the
 * status line. Call from action_wifi_scan so the user sees the list
 * clear the moment they tap Rescan — without this the old results linger
 * on-screen for the whole scan window and it looks like nothing happened. */
void app_state_paint_wifi_scanning(void);

/* Two-state layout of PageWifiSetup — the page has one card for the
 * network list and a separate card + keyboard for password entry, and
 * only one state is visible at a time. Call show_list() when entering
 * the wizard or cancelling out of password entry; show_password(ssid)
 * when the user taps a locked network row. */
void app_state_wifi_show_list(void);
void app_state_wifi_show_password(const char *ssid);

/* Called by app_state.c's dynamically-created row click handler when
 * the user taps a network in the scan list. Implemented in actions.c
 * where the selection state variables live (s_selected_ssid /
 * s_selected_locked used by action_wifi_submit_password). */
void wifi_row_selected(const char *ssid, bool locked);

#ifdef __cplusplus
}
#endif
