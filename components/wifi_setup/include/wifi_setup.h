#pragma once

/*
 * WiFi setup — STA mode scan + connect for Fireside.
 *
 * The ESP32-P4 has no native WiFi radio. ESP-Hosted bridges to an ESP32-C6
 * slave over SDIO (Waveshare ESP32-P4 WiFi6 Touch LCD 7B board layout); the
 * standard esp_wifi_* APIs work as soon as ESP-Hosted is up.
 *
 * Credentials live in NVS via fireside_config — this module never touches NVS
 * directly. The wizard UI calls:
 *
 *   1. wifi_setup_init()              once, after netif + event loop + STA
 *                                      driver are initialized
 *   2. wifi_setup_scan_start()        to populate the network list
 *   3. wifi_setup_connect(ssid, pwd)  when the user submits the password
 *   4. wifi_setup_disconnect()        to drop the link (used when changing WiFi)
 *
 * NOTE: Unlike Spotter's wifi_setup, this module does NOT create the netif or
 * start the WiFi driver — main.c does that during ESP-Hosted bring-up. We only
 * register event handlers, drive scans/connects, and report state.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_SETUP_MAX_SCAN_RESULTS 24
#define WIFI_SETUP_SSID_MAX         33
#define WIFI_SETUP_BSSID_LEN        6

typedef enum {
    WIFI_SETUP_STATE_IDLE = 0,
    WIFI_SETUP_STATE_SCANNING,
    WIFI_SETUP_STATE_CONNECTING,
    WIFI_SETUP_STATE_CONNECTED,
    WIFI_SETUP_STATE_FAILED,
} wifi_setup_state_t;

typedef enum {
    WIFI_SETUP_FAIL_NONE = 0,
    WIFI_SETUP_FAIL_BAD_PASSWORD,
    WIFI_SETUP_FAIL_AP_NOT_FOUND,
    WIFI_SETUP_FAIL_TIMEOUT,
    WIFI_SETUP_FAIL_UNKNOWN,
} wifi_setup_fail_reason_t;

typedef struct {
    char     ssid[WIFI_SETUP_SSID_MAX];
    uint8_t  bssid[WIFI_SETUP_BSSID_LEN];
    int8_t   rssi;
    uint8_t  bars;
    bool     locked;
} wifi_setup_network_t;

/* Callback when the state changes. Runs on the WiFi event task — keep it short
 * and bounce LVGL updates onto the LVGL thread with bsp_display_lock(). */
typedef void (*wifi_setup_state_cb_t)(wifi_setup_state_t state, void *user_ctx);

/* Register event handlers and prepare for scan/connect. The caller is responsible
 * for esp_netif_init + esp_event_loop_create_default + esp_netif_create_default_wifi_sta
 * + esp_wifi_init + esp_wifi_set_mode(STA) + esp_wifi_start before calling this. */
esp_err_t wifi_setup_init(wifi_setup_state_cb_t state_cb, void *user_ctx);

/* Start an async scan. Callback fires with STATE_IDLE once results are ready. */
esp_err_t wifi_setup_scan_start(void);

/* Fill an array of recently-scanned networks. Returns the number written. */
size_t wifi_setup_get_scan_results(wifi_setup_network_t *out, size_t out_cap);

/* Connect using the given SSID + password (password may be empty for open networks). */
esp_err_t wifi_setup_connect(const char *ssid, const char *password);

/* Disconnect and stop reconnect attempts. */
esp_err_t wifi_setup_disconnect(void);

wifi_setup_state_t wifi_setup_get_state(void);
wifi_setup_fail_reason_t wifi_setup_get_last_failure_reason(void);

/* Last-known IPv4 address, or 0 if not connected. */
uint32_t wifi_setup_get_ip(void);
void     wifi_setup_format_ip(char *buf, size_t buf_sz);

#ifdef __cplusplus
}
#endif
