/*
 * wifi_health.c — ESP-Hosted C6 heartbeat monitor + auto-restart recovery.
 *
 * Background. ESP32-P4 has no on-die radio; WiFi runs on a separate ESP32-C6
 * that talks to us over SDIO via ESP-Hosted. Under sustained load the C6's
 * application task that answers RPC requests can hang while the SDIO
 * peripheral itself stays healthy — packets go out, nothing comes back.
 * ESP-Hosted's built-in transport-restart safety net (see
 * CONFIG_ESP_HOSTED_TRANSPORT_RESTART_ON_FAILURE) only fires on SDIO-level
 * failures, so this application-level hang leaves the P4 stranded until a
 * power cycle. Symptoms (documented at length in
 * elecrow-esp32-p4-10-in/DOCS/ElecrowLessonsLearned.md lesson 21): WiFi
 * shows 0 dBm forever, MQTT keeps failing on DNS with EAI_FAIL, buttons
 * feel sluggish because every 17 s a failed reconnect grabs the LVGL lock,
 * and the app never advances to the wizard because WIFI_EVENT_STA_DISCONNECTED
 * never fires (the C6 would have to send it).
 *
 * Recovery mechanism. Espressif's own example
 * (examples/host_hosted_events/main.c inside the esp_hosted managed
 * component) shows the pattern:
 *   1. Enable a periodic heartbeat from the C6 via
 *      esp_hosted_configure_heartbeat(true, N_SEC). The wrap layer will
 *      then post ESP_HOSTED_EVENT_CP_HEARTBEAT every N_SEC.
 *   2. Register a handler for ESP_HOSTED_EVENT. On every heartbeat, reset a
 *      one-shot esp_timer. If the timer ever fires, the C6 has gone
 *      silent — trigger recovery.
 *   3. Also react to ESP_HOSTED_EVENT_TRANSPORT_FAILURE (SDIO layer noticed
 *      the slave is unresponsive) and an unexpected ESP_HOSTED_EVENT_CP_INIT
 *      after the first-boot one (means the C6 crashed and rebooted on its
 *      own — its RPC state is now out of sync with ours).
 *
 * Recovery action here is esp_restart(). The alternative — in-place
 * esp_hosted_deinit → deinit wifi → esp_hosted_init → reinit wifi — keeps
 * the UI alive but is fragile if the SDIO peripheral state on the P4 side is
 * also compromised. A full reboot guarantees both chips come up clean
 * (CONFIG_ESP_HOSTED_SLAVE_RESET_ON_EVERY_HOST_BOOTUP=y drives the C6 reset
 * GPIO during ensure_slave_bus_ready on the next esp_hosted_init). Cost is
 * a ~3 s UI blink; benefit is guaranteed recovery. All persistent state
 * (WiFi creds, MQTT creds, alarm arm bitmaps, button assignments,
 * brightness, timezone, temp unit, screen timeout) lives in NVS and is
 * reloaded on boot, so the user only loses the last few seconds of
 * transient MQTT data.
 */

#include "wifi_health.h"

#include "esp_event.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_hosted.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "WIFI_HEALTH";

/* Heartbeat every 10 s from the C6. Timeout 45 s tolerates 4 missed
 * heartbeats before declaring a hang — long enough that a transient WiFi
 * driver stall (channel switch, background scan) won't cause a false
 * positive, short enough that recovery happens well before a user notices
 * a persistent freeze. */
#define HEARTBEAT_SEC          10
#define HEARTBEAT_TIMEOUT_US   (45LL * 1000 * 1000)

static esp_timer_handle_t s_hb_timer = NULL;

/* The first ESP_HOSTED_EVENT_CP_INIT fires as part of normal boot when the
 * C6 completes its own bring-up and reports itself to the P4. Any
 * subsequent CP_INIT means the C6 spontaneously reset (crash, watchdog),
 * which invalidates all our WiFi and RPC state — recover. */
static bool s_saw_first_init = false;

static void arm_or_restart_hb_timer(void) {
    if (!s_hb_timer) return;
    if (esp_timer_is_active(s_hb_timer)) {
        esp_timer_restart(s_hb_timer, HEARTBEAT_TIMEOUT_US);
    } else {
        esp_timer_start_once(s_hb_timer, HEARTBEAT_TIMEOUT_US);
    }
}

static void reboot_with_reason(const char *reason) {
    ESP_LOGE(TAG, "C6 wedge detected (%s) — rebooting for recovery", reason);
    /* Give UART a chance to flush the reason so it's visible in the log
     * across the reboot. 100 ms is enough for a few hundred bytes at
     * 115200 baud without noticeably delaying recovery. */
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_restart();
}

static void hb_timeout_cb(void *arg) {
    (void)arg;
    reboot_with_reason("no heartbeat from C6 in 45s");
}

static void on_hosted_event(void *arg, esp_event_base_t base,
                             int32_t id, void *data) {
    (void)arg; (void)base;
    switch (id) {
    case ESP_HOSTED_EVENT_CP_HEARTBEAT: {
        esp_hosted_event_heartbeat_t *e = (esp_hosted_event_heartbeat_t *)data;
        ESP_LOGD(TAG, "heartbeat %lu", (unsigned long)(e ? e->heartbeat : 0));
        arm_or_restart_hb_timer();
        break;
    }
    case ESP_HOSTED_EVENT_CP_INIT:
        if (!s_saw_first_init) {
            s_saw_first_init = true;
            ESP_LOGI(TAG, "C6 initial boot event — armed");
        } else {
            reboot_with_reason("unexpected C6 init event (slave crashed)");
        }
        break;
    case ESP_HOSTED_EVENT_TRANSPORT_FAILURE:
        reboot_with_reason("SDIO transport failure");
        break;
    case ESP_HOSTED_EVENT_TRANSPORT_UP:
        ESP_LOGI(TAG, "ESP-Hosted transport up");
        break;
    case ESP_HOSTED_EVENT_TRANSPORT_DOWN:
        ESP_LOGW(TAG, "ESP-Hosted transport down");
        break;
    default:
        break;
    }
}

void wifi_health_init(void) {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    esp_err_t err = esp_event_handler_register(ESP_HOSTED_EVENT, ESP_EVENT_ANY_ID,
                                                &on_hosted_event, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_event_handler_register: %s", esp_err_to_name(err));
        return;
    }

    const esp_timer_create_args_t args = {
        .callback = hb_timeout_cb,
        .name = "hb_watchdog",
        .dispatch_method = ESP_TIMER_TASK,
    };
    err = esp_timer_create(&args, &s_hb_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_timer_create: %s", esp_err_to_name(err));
        return;
    }

    /* Start the deadline immediately so we don't wait forever for the
     * first heartbeat to arrive — if the C6 is DOA the timer fires and
     * we reboot cleanly. */
    esp_timer_start_once(s_hb_timer, HEARTBEAT_TIMEOUT_US);

    err = esp_hosted_configure_heartbeat(true, HEARTBEAT_SEC);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "esp_hosted_configure_heartbeat: %s — "
                 "monitor still armed but will fire after %lld s "
                 "with no heartbeats",
                 esp_err_to_name(err), HEARTBEAT_TIMEOUT_US / 1000000);
        return;
    }

    ESP_LOGI(TAG, "armed: heartbeat=%ds timeout=%llds action=esp_restart",
             HEARTBEAT_SEC, HEARTBEAT_TIMEOUT_US / 1000000);
}
