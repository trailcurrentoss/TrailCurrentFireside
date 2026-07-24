#include "app_mqtt.h"
#include <stddef.h>
#include "mqtt_client.h" /* ESP-IDF esp_mqtt library */
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_timer.h"
#include "nvs.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <string.h>
#include <stdlib.h>
#include "fireside_config.h"

/* Display lock/unlock from BSP - must hold around LVGL calls from non-LVGL tasks */
#include "esp_lvgl_port.h"

/* --- Per-module watchdog --- */

typedef enum {
    WD_ENERGY = 0,
    WD_AIRQUALITY,
    WD_GPS,
    WD_WATER,
    WD_COUNT
} watchdog_id_t;

static const int64_t WATCHDOG_TIMEOUT_US[WD_COUNT] = {
    [WD_ENERGY]     = 10LL * 1000000LL,  /* 10 s — Ampline publishes at 1 s */
    [WD_AIRQUALITY] = 20LL * 1000000LL,  /* 20 s — Borealis reads at 2 s */
    [WD_GPS]        = 15LL * 1000000LL,  /* 15 s — Milepost at ~1 Hz */
    [WD_WATER]      = 10LL * 1000000LL,  /* 10 s — Reservoir publishes at 1 s */
};

/* 0 = never seen (watchdog not yet armed for this module) */
static int64_t s_watchdog_last_seen[WD_COUNT] = {0};
static bool    s_watchdog_timed_out[WD_COUNT] = {false, false, false, false};

extern void clear_var_energy(void);
extern void clear_var_airquality(void);
extern void clear_var_gps(void);
extern void clear_var_water(void);

/* MQTT variable setters (mqtt_vars.h / vars.c) */
extern void set_var_device01_status(int32_t value);
extern void set_var_device02_status(int32_t value);
extern void set_var_device03_status(int32_t value);
extern void set_var_device04_status(int32_t value);
extern void set_var_device05_status(int32_t value);
extern void set_var_device06_status(int32_t value);
extern void set_var_device07_status(int32_t value);
extern void set_var_device08_status(int32_t value);
extern void set_var_battery_soc(int32_t percent);
extern void set_var_battery_voltage(float volts);
extern void set_var_solar_watts(int32_t watts);
extern void set_var_solar_status(const char *status);
extern void set_var_consumption_watts(int32_t watts);
extern void set_var_time_remaining(int32_t minutes);
extern void set_var_latitude(float lat);
extern void set_var_longitude(float lon);
extern void set_var_altitude(float feet);
extern void set_var_speed(float knots);
extern void set_var_course(float degrees);
extern void set_var_gnss_mode(const char *mode);
extern void set_var_humidity(float percent);
extern void set_var_co2(int32_t ppm);
extern void set_var_tvoc(int32_t ppb);
extern void set_var_co(int32_t ppm);
extern void set_var_co_flags(bool warn, bool alarm);

/* Alarms — MQTT input handlers dispatch here. Implemented in main/alarms.c. */
typedef enum { ALARM_SRC_SWITCHBACK = 0, ALARM_SRC_PICKET = 1 } alarm_src_t;
extern void alarms_apply_inputs(alarm_src_t src, uint8_t addr, uint16_t bits);
extern void set_var_mqtt_connected(bool connected);
extern void set_var_satellite_count(int32_t value);
extern void set_var_current_interior_temperature(int32_t value);
extern void set_var_gps_time(int year, int month, int day, int hour, int minute, int second);
extern int32_t get_var_current_device_brightness_identifier(void);
extern void set_var_water_levels(int32_t fresh, int32_t grey, int32_t black);

#include "button_config.h"

/* Route a module/instance/channel status update to the configured button. */
static void apply_module_status(module_type_t mod, uint8_t inst, uint8_t ch, int value) {
    uint8_t btn = button_config_find(mod, inst, ch);
    if (btn == 0) return;
    if (btn >= 1 && btn <= NUM_BUTTONS) g_button_state[btn - 1] = (uint8_t)(value > 0);
    switch (btn) {
    case 1: set_var_device01_status(value); break;
    case 2: set_var_device02_status(value); break;
    case 3: set_var_device03_status(value); break;
    case 4: set_var_device04_status(value); break;
    case 5: set_var_device05_status(value); break;
    case 6: set_var_device06_status(value); break;
    case 7: set_var_device07_status(value); break;
    case 8: set_var_device08_status(value); break;
    }
}

/* Discovery / OTA trigger dispatch (implemented in main/) */
extern void discovery_handle_trigger(void);
extern void ota_handle_trigger(void);

static const char *TAG = "MQTT";

/* Connection settings — copied out of fireside_config so the broker URI buffer
 * stays stable across reconfiguration. */
static char s_host[128] = {0};
static uint16_t s_port = 8883;
static char s_username[64] = {0};
static char s_password[128] = {0};

static esp_mqtt_client_handle_t s_client = NULL;
static volatile bool s_connected = false;
static mqtt_client_state_cb_t s_state_cb = NULL;

/* Populated in mqtt_client_connect(); read in the event handler on
 * MQTT_EVENT_CONNECTED to publish the retained "online" status that
 * overrides the retained LWT the broker will publish on the next drop. */
static char s_lwt_topic[64] = {0};

void mqtt_client_set_state_callback(mqtt_client_state_cb_t cb)
{
    s_state_cb = cb;
}

/* Queue for passing received messages from MQTT task to main loop */
typedef struct {
    char topic[128];
    char payload[512];
    int payload_len;
    /* PROBE: timestamp when MQTT_EVENT_DATA delivered the message. Used to
     * measure end-to-end (broker-to-UI) latency via the LAT: log lines. */
    int64_t rx_time_us;
} mqtt_message_t;

static QueueHandle_t s_incoming_queue = NULL;

/* PROBE: track queue high-water mark so we can see if messages are backing
 * up under Headwaters' 200+ msg/sec load. Reset every ~5s. */
static uint32_t s_queue_hwm = 0;
static int64_t  s_queue_hwm_reset_us = 0;

/* Forward declarations */
static void process_message(const char *topic, const char *payload, int length);
static void mqtt_dispatch_task_start(void);
static void process_gnss_mode(int mode);

/* --- MQTT event handler (runs in MQTT task context) --- */

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                                int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Connected to broker");
        s_connected = true;
        if (s_state_cb) s_state_cb(true);

        /* Overwrite the retained LWT with an "online" marker so anyone
         * subscribed to local/fireside/<mac>/status sees the current
         * liveness state. QoS 1 + retain matches the LWT settings. */
        if (s_lwt_topic[0]) {
            esp_mqtt_client_publish(s_client, s_lwt_topic, "online", 6, 1, 1);
        }

        /* Subscribe to all data topics */
        esp_mqtt_client_subscribe(s_client, "local/lights/+/status", 0);
        esp_mqtt_client_subscribe(s_client, "local/energy/status", 0);
        esp_mqtt_client_subscribe(s_client, "local/airquality/temphumid", 0);
        esp_mqtt_client_subscribe(s_client, "local/airquality/status", 0);
        esp_mqtt_client_subscribe(s_client, "local/airquality/safety", 0);
        esp_mqtt_client_subscribe(s_client, "local/gps/latlon", 0);
        esp_mqtt_client_subscribe(s_client, "local/gps/alt", 0);
        esp_mqtt_client_subscribe(s_client, "local/gps/details", 0);
        esp_mqtt_client_subscribe(s_client, "local/gps/time", 0);
        esp_mqtt_client_subscribe(s_client, "local/water/status", 0);
        esp_mqtt_client_subscribe(s_client, "local/relays/+/status", 0);
        /* Alarm-source topics — Switchback digital inputs + Picket reed
         * switches. ESP evaluates the active-alarm list locally against
         * its own per-mode arm config (Phase 1: config is empty so no
         * alarms fire; Phase 2 wires the Settings UI to edit the config). */
        esp_mqtt_client_subscribe(s_client, "local/spoor/+/inputs", 0);
        esp_mqtt_client_subscribe(s_client, "local/picket/+/inputs", 0);
        esp_mqtt_client_subscribe(s_client, "local/discovery/trigger", 0);
        esp_mqtt_client_subscribe(s_client, "local/ota/trigger", 0);
        ESP_LOGI(TAG, "Subscribed to all topics");

        lvgl_port_lock(0);
        set_var_mqtt_connected(true);
        lvgl_port_unlock();
        break;

    case MQTT_EVENT_DISCONNECTED:
        /* Log everything we can pry out of the event so the user has a
         * shot at diagnosing WHY it dropped. Common causes:
         *   1. Same client_id as another connected session (broker kicks
         *      the older one — check for another Fireside or the Headwaters
         *      PWA using tc-display-XXXX).
         *   2. Keepalive timeout — PINGRESP didn't return in ~30s.
         *   3. WiFi drop — check for a preceding WIFI_EVENT_STA_DISCONNECTED
         *      in the log.
         *   4. Broker session eviction / ACL revocation. */
        if (event && event->error_handle) {
            ESP_LOGW(TAG,
                "Disconnected. err_type=%d transport_sock_errno=%d "
                "tls_last_esp_err=0x%x tls_stack_err=0x%x conn_return=%d",
                event->error_handle->error_type,
                event->error_handle->esp_transport_sock_errno,
                event->error_handle->esp_tls_last_esp_err,
                event->error_handle->esp_tls_stack_err,
                event->error_handle->connect_return_code);
        } else {
            ESP_LOGW(TAG, "Disconnected from broker (no error handle)");
        }
        s_connected = false;
        if (s_state_cb) s_state_cb(false);
        lvgl_port_lock(0);
        set_var_mqtt_connected(false);
        lvgl_port_unlock();
        break;

    case MQTT_EVENT_DATA: {
        /* Skip fragmented messages */
        if (event->current_data_offset != 0 ||
            event->data_len != event->total_data_len) {
            ESP_LOGW(TAG, "Skipping fragmented message");
            break;
        }

        mqtt_message_t msg = {0};
        int topic_len = (event->topic_len < (int)sizeof(msg.topic) - 1)
                            ? event->topic_len
                            : (int)sizeof(msg.topic) - 1;
        int data_len = (event->data_len < (int)sizeof(msg.payload) - 1)
                           ? event->data_len
                           : (int)sizeof(msg.payload) - 1;

        memcpy(msg.topic, event->topic, topic_len);
        msg.topic[topic_len] = '\0';
        memcpy(msg.payload, event->data, data_len);
        msg.payload[data_len] = '\0';
        msg.payload_len = data_len;

        /* Demoted to DEBUG — was fatal at INFO under Headwaters' ~200
         * msg/sec load. ESP_LOGI writes synchronously to UART at 115200
         * baud (~11.5 KB/s throughput). Each RX line is ~80 bytes, so
         * 200 msg/sec × 80 B = 16 KB/s of log traffic, well above the
         * UART's sustained write capacity. When the log ring fills, the
         * MQTT event task blocks in vfs_write() → new inbound messages
         * pile up in the TCP receive buffer → status updates arrive
         * ~10 s late even though the broker delivered them promptly.
         * PWA and other MQTT clients (which have no console log path)
         * see the same messages instantly, confirming Fireside was the
         * bottleneck. Re-raise to INFO for one debug session by adding
         * `esp_log_level_set("MQTT", ESP_LOG_DEBUG)` at boot. */
        ESP_LOGD(TAG, "RX: %s (%d bytes)", msg.topic, msg.payload_len);

        /* Discovery / OTA triggers are dispatched directly from the MQTT task
         * (they spawn their own worker tasks) rather than going through the
         * LVGL-synchronous main-loop queue. Payload is a hostname string
         * (esp32-XXXXXX); discovery also accepts "*" broadcast. */
        if (strcmp(msg.topic, "local/discovery/trigger") == 0 ||
            strcmp(msg.topic, "local/ota/trigger") == 0) {
            char my_host[16];
            mqtt_client_hostname(my_host, sizeof(my_host));
            bool is_discovery = (strcmp(msg.topic, "local/discovery/trigger") == 0);
            bool broadcast = is_discovery && msg.payload_len == 1 && msg.payload[0] == '*';
            if (broadcast || strncmp(msg.payload, my_host, msg.payload_len) == 0) {
                if (is_discovery) discovery_handle_trigger();
                else              ota_handle_trigger();
            } else {
                ESP_LOGD(TAG, "Trigger for %.*s — not us (%s)",
                         msg.payload_len, msg.payload, my_host);
            }
            break;
        }

        if (s_incoming_queue) {
            /* PROBE: stamp receive time so the main-loop consumer can
             * compute queue-latency + total-latency. */
            msg.rx_time_us = esp_timer_get_time();
            xQueueSend(s_incoming_queue, &msg, 0);
            /* PROBE: track queue high-water mark for backlog visibility. */
            uint32_t depth = (uint32_t)uxQueueMessagesWaiting(s_incoming_queue);
            if (depth > s_queue_hwm) s_queue_hwm = depth;
        }
        break;
    }

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "SUBACK msg_id=%d — broker accepted subscription", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGW(TAG, "UNSUBACK msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_ERROR:
        if (event->error_handle) {
            ESP_LOGE(TAG, "MQTT error type=%d transport=0x%x tls_last=0x%x",
                     event->error_handle->error_type,
                     event->error_handle->esp_transport_sock_errno,
                     event->error_handle->esp_tls_last_esp_err);
        }
        break;

    default:
        ESP_LOGD(TAG, "MQTT event id=%d", (int)event_id);
        break;
    }
}

/* --- Public API --- */

bool mqtt_client_load_settings(void) {
    const fireside_config_t *pc = fireside_config_get();
    if (!pc || !pc->mqtt_host[0]) {
        ESP_LOGW(TAG, "No MQTT config in fireside_config");
        return false;
    }

    strlcpy(s_host,     pc->mqtt_host, sizeof(s_host));
    strlcpy(s_username, pc->mqtt_user, sizeof(s_username));
    strlcpy(s_password, pc->mqtt_pass, sizeof(s_password));
    s_port = pc->mqtt_port ? pc->mqtt_port : 8883;

    bool has_config = strlen(s_host) > 0 && strlen(s_username) > 0;
    if (has_config) {
        ESP_LOGI(TAG, "Loaded from fireside_config: %s:%u user=%s",
                 s_host, (unsigned)s_port, s_username);
    } else {
        ESP_LOGW(TAG, "Incomplete config - host:%s user:%s",
                 strlen(s_host) > 0 ? "ok" : "MISSING",
                 strlen(s_username) > 0 ? "ok" : "MISSING");
    }
    return has_config;
}

void mqtt_client_connect(void) {
    if (strlen(s_host) == 0 || strlen(s_username) == 0) {
        ESP_LOGW(TAG, "Cannot connect - missing MQTT configuration");
        return;
    }

    /* Create incoming message queue.
     * Capacity chosen for Headwaters' ~200 msg/sec burst rate: at 64
     * entries × 640 bytes = ~40 KB internal RAM, and the queue can now
     * absorb a ~300 ms hiccup on the drain side without dropping. Was 16
     * previously — insufficient under sustained load, dropped alarm and
     * light-status messages silently when the main loop paused. */
    if (!s_incoming_queue) {
        s_incoming_queue = xQueueCreate(64, sizeof(mqtt_message_t));
    }

    /* Start the dispatch task (if not already running). See
     * mqtt_dispatch_task above for the rationale — this decouples MQTT
     * message processing from the main app loop so status updates arrive
     * within a few ms of the network delivery rather than waiting for the
     * next main-loop iteration + LVGL-lock window. */
    mqtt_dispatch_task_start();

    /* Build URI */
    char uri[192];
    snprintf(uri, sizeof(uri), "mqtts://%s:%d", s_host, s_port);

    /* Generate client ID from MAC. Use the full 6-byte MAC so no two
     * Fireside displays (or any other tc-display-* client someone spins
     * up later) can ever collide on client_id — MQTT brokers eject the
     * older session when a duplicate connects, which manifests as
     * flapping "Connection Lost" if two boards share the same 4-hex
     * suffix. Old form: tc-display-%02x%02x (16-bit space, ~65K). */
    static char client_id[40];  /* static so the pointer stays valid for
                                 * the client's lifetime — esp-mqtt keeps
                                 * a reference into this buffer. */
    uint8_t mac[6] = {0};
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    snprintf(client_id, sizeof(client_id), "tc-display-%02x%02x%02x%02x%02x%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    /* LWT topic + payload — broker publishes "offline" (retained) to this
     * topic when it notices we've dropped, and we overwrite with "online"
     * on connect below. Makes flapping visible broker-side and lets the
     * PWA render a Fireside health indicator without polling. */
    snprintf(s_lwt_topic, sizeof(s_lwt_topic),
             "local/fireside/%02x%02x%02x%02x%02x%02x/status",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    ESP_LOGI(TAG, "Connecting to %s as %s (client_id=%s)...",
             uri, s_username, client_id);
    /* Diagnostic heap snapshot before the TLS handshake. If the log line
     * that follows shows `largest` shrinking below ~20 KB across successive
     * reconnects, mbedtls's dynamic-buffer allocation is going to start
     * failing and the session flap will migrate from PINGRESP-timeout to
     * handshake-failure. Watch this over a long run to catch a slow leak
     * before it kills TLS entirely. */
    ESP_LOGI(TAG, "  heap before connect: free=%u largest_internal=%u",
             (unsigned)esp_get_free_heap_size(),
             (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));

    /* Destroy previous client if reconnecting */
    if (s_client) {
        esp_mqtt_client_stop(s_client);
        esp_mqtt_client_destroy(s_client);
        s_client = NULL;
    }

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        /* Cert verification is bypassed by CONFIG_ESP_TLS_INSECURE +
         * CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY in sdkconfig.defaults; this
         * flag covers the CN-match check inside esp-mqtt itself. No CA cert
         * lives on the device — the TrailCurrent broker's self-signed cert is
         * accepted automatically. */
        .broker.verification.skip_cert_common_name_check = true,
        .credentials.client_id = client_id,
        .credentials.username = s_username,
        .credentials.authentication.password = s_password,
        /* Post-diagnosis 2026-07-23 — matches Spotter's proven config from
         * their 2026-06-26 fix. Broker log evidence: display received PINGRESP
         * (mosquitto logged "Sending PINGRESP to tc-display-...") yet still
         * closed the socket 15 s later. That's the esp-mqtt keepalive path
         * at mqtt_client.c:694 ("No PING_RESP, disconnected") firing because
         * `wait_for_ping_resp` never got cleared — i.e. a PINGRESP packet
         * was dropped on the WiFi link (or held in a TLS record long enough
         * that the 10 s network.timeout_ms tore the socket down first).
         *
         *   - keepalive 60 s: PINGREQ every ~30 s instead of every ~15 s.
         *     Halves the per-second chance a lost ping kills the session.
         *   - network.timeout_ms 20 s: covers one full TCP RTO doubling
         *     cycle (1s → 2s → 4s → 8s) before esp-mqtt gives up on the
         *     socket read, so a single lost segment gets retransmitted
         *     instead of taking the whole connection with it. */
        .network.timeout_ms = 20000,
        .session.keepalive = 60,
        /* Retained LWT — broker publishes this the moment it decides
         * we're gone. On connect we overwrite with an "online" retained
         * message (see MQTT_EVENT_CONNECTED below). */
        .session.last_will.topic  = s_lwt_topic,
        .session.last_will.msg    = "offline",
        .session.last_will.msg_len = 7,
        .session.last_will.qos    = 1,
        .session.last_will.retain = 1,
        .buffer.size = 1024,
    };

    s_client = esp_mqtt_client_init(&mqtt_cfg);
    if (!s_client) {
        ESP_LOGE(TAG, "Failed to init MQTT client");
        return;
    }

    esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID,
                                   mqtt_event_handler, NULL);

    esp_err_t err = esp_mqtt_client_start(s_client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start MQTT client: %s", esp_err_to_name(err));
    }
}

/* ============================================================
 * Dedicated MQTT dispatch task
 *
 * Previously, MQTT messages sat in s_incoming_queue until the main app
 * loop got around to calling mqtt_client_process_messages(). The main
 * loop also drives the clock tick, watchdogs, and (transitively) the
 * LVGL redraw cadence, so any of those slowing down delayed status
 * processing. Under Headwaters' ~200 msg/sec load this manifested as
 * ~10 s lag between the broker delivering a light-status update and
 * the tile flipping on the display — even though PWA and other MQTT
 * subscribers on the same broker saw the same message instantly.
 *
 * This task blocks on the queue with portMAX_DELAY, wakes the moment a
 * message arrives, drains everything that's queued, and hands each one
 * to process_message() with the LVGL lock held only for the individual
 * setter call. Because it's an independent FreeRTOS task, the main loop
 * (clock updates, watchdogs) and the LVGL task (touch, painting) each
 * get their own scheduling slot instead of all serializing behind one
 * consumer.
 * ============================================================ */

static TaskHandle_t s_dispatch_task = NULL;

static void mqtt_dispatch_task(void *arg) {
    (void)arg;
    ESP_LOGI(TAG, "MQTT dispatch task started");
    mqtt_message_t msg;
    while (1) {
        /* portMAX_DELAY == "wake me only when there's actually a message". */
        if (xQueueReceive(s_incoming_queue, &msg, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        int64_t deq_us = esp_timer_get_time();
        int64_t age_ms = (deq_us - msg.rx_time_us) / 1000;
        if (strncmp(msg.topic, "local/spoor/", 12) == 0 ||
            strncmp(msg.topic, "local/picket/", 13) == 0 ||
            strcmp(msg.topic, "local/airquality/safety") == 0 ||
            strcmp(msg.topic, "local/energy/status") == 0) {
            uint32_t depth = (uint32_t)uxQueueMessagesWaiting(s_incoming_queue);
            ESP_LOGI(TAG, "LAT: dequeue topic=%s queue_age=%lldms depth=%u+1",
                     msg.topic, age_ms, (unsigned)depth);
        }
        lvgl_port_lock(0);
        process_message(msg.topic, msg.payload, msg.payload_len);
        lvgl_port_unlock();
        /* Opportunistically drain anything else that arrived while we
         * were processing this one — one LVGL lock per message, so the
         * LVGL task can still slip in between. */
        while (xQueueReceive(s_incoming_queue, &msg, 0) == pdTRUE) {
            lvgl_port_lock(0);
            process_message(msg.topic, msg.payload, msg.payload_len);
            lvgl_port_unlock();
        }
        /* 5s window report — carried over from the old process_messages
         * summary so we still see queue backlog trends in the log. */
        int64_t now_us = esp_timer_get_time();
        if (now_us - s_queue_hwm_reset_us >= 5000000LL) {
            ESP_LOGI(TAG, "LAT: 5s window — queue_hwm=%u",
                     (unsigned)s_queue_hwm);
            s_queue_hwm = 0;
            s_queue_hwm_reset_us = now_us;
        }
    }
}

static void mqtt_dispatch_task_start(void) {
    if (s_dispatch_task) return;
    /* Priority 4: below the esp-mqtt event task (default 5) so we don't
     * starve the wire-level receive path, and below the LVGL task
     * (usually 5-6) so touch + painting stay responsive. Stack 6 KB:
     * cJSON parses on the stack and topic dispatch is deep. */
    BaseType_t r = xTaskCreate(mqtt_dispatch_task, "mqtt_dispatch",
                               6144, NULL, 4, &s_dispatch_task);
    if (r != pdPASS) {
        ESP_LOGE(TAG, "xTaskCreate(mqtt_dispatch) failed");
        s_dispatch_task = NULL;
    }
}

void mqtt_client_process_messages(void) {
    /* Retained as a no-op so callers (main.c) that still invoke it don't
     * fail to link. Actual processing moved to mqtt_dispatch_task above. */
}

/* Legacy path — no longer called from the main loop. Left here (dead
 * code, unreachable) as documentation of what the pre-task processing
 * used to look like. Once the new task path is proven, delete. */
#if 0
static void mqtt_client_process_messages_legacy(void) {
    if (!s_incoming_queue) {
        vTaskDelay(pdMS_TO_TICKS(10));
        return;
    }

    mqtt_message_t msg;
    /* Block up to 10ms waiting for the first message — wakes immediately
     * when a message arrives instead of polling on a fixed interval.
     * Take the display lock per message (not around the whole drain) so
     * the LVGL task can get in between messages. Wrapping the whole drain
     * in one lock, as this file used to, starves the LVGL task under any
     * sustained MQTT throughput and makes touch feel dead — Headwaters
     * replaying retained messages was enough to lock out touch entirely. */
    int drained = 0;
    if (xQueueReceive(s_incoming_queue, &msg, pdMS_TO_TICKS(10)) == pdTRUE) {
        int64_t deq_us  = esp_timer_get_time();
        int64_t age_ms  = (deq_us - msg.rx_time_us) / 1000;
        /* PROBE: only log the age on alarm-source topics + the badge-
         * driving CO/AQ topics. Otherwise we'd log 200x/sec and drown
         * the console. */
        if (strncmp(msg.topic, "local/spoor/", 12) == 0 ||
            strncmp(msg.topic, "local/picket/", 13) == 0 ||
            strcmp(msg.topic, "local/airquality/safety") == 0 ||
            strcmp(msg.topic, "local/energy/status") == 0) {
            uint32_t depth = (uint32_t)uxQueueMessagesWaiting(s_incoming_queue);
            ESP_LOGI(TAG, "LAT: dequeue topic=%s queue_age=%lldms depth=%u+1",
                     msg.topic, age_ms, (unsigned)depth);
        }
        lvgl_port_lock(0);
        process_message(msg.topic, msg.payload, msg.payload_len);
        lvgl_port_unlock();
        drained++;
        while (xQueueReceive(s_incoming_queue, &msg, 0) == pdTRUE) {
            lvgl_port_lock(0);
            process_message(msg.topic, msg.payload, msg.payload_len);
            lvgl_port_unlock();
            drained++;
        }
    }
    /* PROBE: every ~5s, publish a summary of queue backlog + drain rate. */
    int64_t now_us = esp_timer_get_time();
    if (now_us - s_queue_hwm_reset_us >= 5000000LL) {
        ESP_LOGI(TAG, "LAT: 5s window — queue_hwm=%u drained_this_tick=%d",
                 (unsigned)s_queue_hwm, drained);
        s_queue_hwm = 0;
        s_queue_hwm_reset_us = now_us;
    }
}
#endif  /* legacy mqtt_client_process_messages */

bool mqtt_client_is_connected(void) {
    return s_connected;
}

void mqtt_client_stop(void) {
    if (s_client) {
        esp_mqtt_client_stop(s_client);
        esp_mqtt_client_destroy(s_client);
        s_client = NULL;
    }
    s_connected = false;
    lvgl_port_lock(0);
    set_var_mqtt_connected(false);
    lvgl_port_unlock();
    ESP_LOGI(TAG, "MQTT client stopped");
}

const char *mqtt_client_hostname(char *out, size_t out_len) {
    uint8_t mac[6] = {0};
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    snprintf(out, out_len, "esp32-%02X%02X%02X", mac[3], mac[4], mac[5]);
    return out;
}

int mqtt_client_publish(const char *topic, const char *payload, int payload_len) {
    if (!s_connected || !s_client) {
        ESP_LOGW(TAG, "Not connected, cannot publish to %s", topic);
        return -1;
    }
    int msg_id = esp_mqtt_client_publish(s_client, topic, payload, payload_len, 0, 0);
    ESP_LOGI(TAG, "Published to %s (msg_id=%d)", topic, msg_id);
    return msg_id;
}

/* --- Module watchdog check --- */

void mqtt_client_check_watchdogs(void) {
    int64_t now = esp_timer_get_time();
    int to_clear[WD_COUNT];
    int n = 0;

    for (int i = 0; i < WD_COUNT; i++) {
        if (s_watchdog_last_seen[i] == 0) continue; /* never seen — not yet armed */
        bool timed_out = (now - s_watchdog_last_seen[i]) > WATCHDOG_TIMEOUT_US[i];
        if (timed_out && !s_watchdog_timed_out[i]) {
            s_watchdog_timed_out[i] = true;
            to_clear[n++] = i;
            ESP_LOGW(TAG, "Module watchdog timeout: id=%d", i);
        } else if (!timed_out && s_watchdog_timed_out[i]) {
            s_watchdog_timed_out[i] = false;
            ESP_LOGI(TAG, "Module watchdog recovered: id=%d", i);
        }
    }

    if (n > 0) {
        lvgl_port_lock(0);
        for (int j = 0; j < n; j++) {
            switch (to_clear[j]) {
                case WD_ENERGY:     clear_var_energy();     break;
                case WD_AIRQUALITY: clear_var_airquality(); break;
                case WD_GPS:        clear_var_gps();        break;
                case WD_WATER:      clear_var_water();      break;
                default: break;
            }
        }
        lvgl_port_unlock();
    }
}

/* --- GNSS mode helper --- */

static void process_gnss_mode(int mode) {
    switch (mode) {
    case 1: set_var_gnss_mode("GPS"); break;
    case 2: set_var_gnss_mode("Beidou"); break;
    case 3: set_var_gnss_mode("GPS + Beidou"); break;
    case 4: set_var_gnss_mode("GLONASS"); break;
    case 5: set_var_gnss_mode("GPS + GLONASS"); break;
    case 6: set_var_gnss_mode("Beidou + GLONASS"); break;
    case 7: set_var_gnss_mode("GPS + Beidou + GLONASS"); break;
    default: set_var_gnss_mode("Unknown"); break;
    }
}

/* --- Process incoming MQTT message --- */

static void process_message(const char *topic, const char *payload, int length) {
    cJSON *doc = cJSON_ParseWithLength(payload, length);
    if (!doc) {
        ESP_LOGW(TAG, "JSON parse error for topic: %s", topic);
        return;
    }

    /* local/lights/{id}/status — Torrent instance 0, channel id-1 */
    if (strncmp(topic, "local/lights/", 13) == 0) {
        int id = atoi(topic + 13);
        if (id < 1 || id > 8) { cJSON_Delete(doc); return; }

        /* Skip updates for the device whose brightness is being adjusted
         * by the user — avoids fighting with another controller. */
        uint8_t btn_editing = (uint8_t)button_config_find(MOD_TORRENT, 0, (uint8_t)(id - 1));
        if (btn_editing && btn_editing == (uint8_t)get_var_current_device_brightness_identifier()) {
            cJSON_Delete(doc);
            return;
        }

        cJSON *state_j = cJSON_GetObjectItem(doc, "state");
        cJSON *brightness_j = cJSON_GetObjectItem(doc, "brightness");
        int state = state_j ? state_j->valueint : 0;
        int brightness = brightness_j ? brightness_j->valueint : 0;
        int value = (state > 0) ? ((brightness > 0) ? brightness : 1) : 0;
        apply_module_status(MOD_TORRENT, 0, (uint8_t)(id - 1), value);
    }
    /* local/relays/{id}/status — Switchback instance 0, channel id-1 */
    else if (strncmp(topic, "local/relays/", 13) == 0) {
        int id = atoi(topic + 13);
        if (id < 1 || id > 8) { cJSON_Delete(doc); return; }
        cJSON *state_j = cJSON_GetObjectItem(doc, "state");
        int state = state_j ? state_j->valueint : 0;
        apply_module_status(MOD_SWITCHBACK, 0, (uint8_t)(id - 1), state);
    }
    /* local/energy/status */
    else if (strcmp(topic, "local/energy/status") == 0) {
        s_watchdog_last_seen[WD_ENERGY] = esp_timer_get_time();
        cJSON *bp = cJSON_GetObjectItem(doc, "battery_percent");
        cJSON *bv = cJSON_GetObjectItem(doc, "battery_voltage");
        cJSON *sw = cJSON_GetObjectItem(doc, "solar_watts");
        cJSON *ct = cJSON_GetObjectItem(doc, "charge_type");
        cJSON *cw = cJSON_GetObjectItem(doc, "consumption_watts");
        cJSON *tr = cJSON_GetObjectItem(doc, "time_remaining_minutes");

        if (bp) set_var_battery_soc((int32_t)bp->valuedouble);
        if (bv) set_var_battery_voltage((float)bv->valuedouble);
        if (sw) set_var_solar_watts((int32_t)sw->valuedouble);
        if (ct && ct->valuestring) set_var_solar_status(ct->valuestring);
        if (cw) set_var_consumption_watts((int32_t)cw->valuedouble);
        if (tr) set_var_time_remaining((int32_t)tr->valuedouble);
    }
    /* local/airquality/temphumid */
    else if (strcmp(topic, "local/airquality/temphumid") == 0) {
        s_watchdog_last_seen[WD_AIRQUALITY] = esp_timer_get_time();
        cJSON *temp_f = cJSON_GetObjectItem(doc, "tempInF");
        cJSON *humid = cJSON_GetObjectItem(doc, "humidity");

        if (temp_f) {
            set_var_current_interior_temperature((int32_t)temp_f->valuedouble);
        }
        if (humid) {
            set_var_humidity((float)humid->valuedouble);
        }
    }
    /* local/airquality/status */
    else if (strcmp(topic, "local/airquality/status") == 0) {
        s_watchdog_last_seen[WD_AIRQUALITY] = esp_timer_get_time();
        cJSON *eco2 = cJSON_GetObjectItem(doc, "eco2_ppm");
        cJSON *tvoc = cJSON_GetObjectItem(doc, "tvoc_ppb");

        if (eco2 && cJSON_IsNumber(eco2))
            set_var_co2((int32_t)eco2->valuedouble);
        if (tvoc && cJSON_IsNumber(tvoc))
            set_var_tvoc((int32_t)tvoc->valuedouble);
    }
    /* local/airquality/safety — Borealis CO safety data (via can-bridge).
     * Payload: { co_ppm, co_warn (bool), co_alarm (bool), alarm_flags }.
     * PWA treats the boolean flags as authoritative and falls back to
     * ppm thresholds only if flags are absent — mirror that here. */
    else if (strcmp(topic, "local/airquality/safety") == 0) {
        s_watchdog_last_seen[WD_AIRQUALITY] = esp_timer_get_time();
        cJSON *cop  = cJSON_GetObjectItem(doc, "co_ppm");
        cJSON *warn = cJSON_GetObjectItem(doc, "co_warn");
        cJSON *alrm = cJSON_GetObjectItem(doc, "co_alarm");
        if (cop && cJSON_IsNumber(cop))
            set_var_co((int32_t)cop->valuedouble);
        bool w = warn && cJSON_IsBool(warn) ? cJSON_IsTrue(warn) : false;
        bool a = alrm && cJSON_IsBool(alrm) ? cJSON_IsTrue(alrm) : false;
        set_var_co_flags(w, a);
    }
    /* local/gps/latlon */
    else if (strcmp(topic, "local/gps/latlon") == 0) {
        s_watchdog_last_seen[WD_GPS] = esp_timer_get_time();
        cJSON *lat = cJSON_GetObjectItem(doc, "latitude");
        cJSON *lon = cJSON_GetObjectItem(doc, "longitude");
        if (lat) set_var_latitude((float)lat->valuedouble);
        if (lon) set_var_longitude((float)lon->valuedouble);
    }
    /* local/gps/alt */
    else if (strcmp(topic, "local/gps/alt") == 0) {
        s_watchdog_last_seen[WD_GPS] = esp_timer_get_time();
        cJSON *alt = cJSON_GetObjectItem(doc, "altitudeFeet");
        if (alt) set_var_altitude((float)alt->valuedouble);
    }
    /* local/gps/details */
    else if (strcmp(topic, "local/gps/details") == 0) {
        s_watchdog_last_seen[WD_GPS] = esp_timer_get_time();
        cJSON *sats = cJSON_GetObjectItem(doc, "numberOfSatellites");
        cJSON *spd = cJSON_GetObjectItem(doc, "speedOverGround");
        cJSON *crs = cJSON_GetObjectItem(doc, "courseOverGround");
        cJSON *gnss = cJSON_GetObjectItem(doc, "gnssMode");

        if (sats) set_var_satellite_count(sats->valueint);
        if (spd) set_var_speed((float)spd->valuedouble);
        if (crs) set_var_course((float)crs->valuedouble);
        if (gnss) process_gnss_mode(gnss->valueint);
    }
    /* local/gps/time */
    else if (strcmp(topic, "local/gps/time") == 0) {
        s_watchdog_last_seen[WD_GPS] = esp_timer_get_time();
        cJSON *yr = cJSON_GetObjectItem(doc, "year");
        cJSON *mo = cJSON_GetObjectItem(doc, "month");
        cJSON *dy = cJSON_GetObjectItem(doc, "day");
        cJSON *hr = cJSON_GetObjectItem(doc, "hour");
        cJSON *mn = cJSON_GetObjectItem(doc, "minute");
        cJSON *sc = cJSON_GetObjectItem(doc, "second");

        if (yr && mo && dy && hr && mn && sc) {
            set_var_gps_time(yr->valueint, mo->valueint, dy->valueint,
                             hr->valueint, mn->valueint, sc->valueint);
        }
    }
    /* local/spoor/<addr>/inputs — Switchback DIs (8 bits per board).
     * Payload matches Headwaters alarms-service.js: { addr, inputs }. */
    else if (strncmp(topic, "local/spoor/", 12) == 0 &&
             strstr(topic, "/inputs") != NULL) {
        cJSON *addr_j = cJSON_GetObjectItem(doc, "addr");
        cJSON *bits_j = cJSON_GetObjectItem(doc, "inputs");
        if (addr_j && cJSON_IsNumber(addr_j) &&
            bits_j && cJSON_IsNumber(bits_j)) {
            alarms_apply_inputs(ALARM_SRC_SWITCHBACK,
                                (uint8_t)addr_j->valueint,
                                (uint16_t)bits_j->valueint);
        }
    }
    /* local/picket/<addr>/inputs — Picket reed switches (12 bits per board). */
    else if (strncmp(topic, "local/picket/", 13) == 0 &&
             strstr(topic, "/inputs") != NULL) {
        cJSON *addr_j = cJSON_GetObjectItem(doc, "addr");
        cJSON *bits_j = cJSON_GetObjectItem(doc, "inputs");
        if (addr_j && cJSON_IsNumber(addr_j) &&
            bits_j && cJSON_IsNumber(bits_j)) {
            alarms_apply_inputs(ALARM_SRC_PICKET,
                                (uint8_t)addr_j->valueint,
                                (uint16_t)bits_j->valueint);
        }
    }
    /* local/water/status — tank levels (0-100%) from Reservoir via can-bridge */
    else if (strcmp(topic, "local/water/status") == 0) {
        s_watchdog_last_seen[WD_WATER] = esp_timer_get_time();
        cJSON *fresh = cJSON_GetObjectItem(doc, "fresh");
        cJSON *grey  = cJSON_GetObjectItem(doc, "grey");
        cJSON *black = cJSON_GetObjectItem(doc, "black");
        set_var_water_levels(
            fresh ? (int32_t)fresh->valuedouble : 0,
            grey  ? (int32_t)grey->valuedouble  : 0,
            black ? (int32_t)black->valuedouble : 0);
    }
    else {
        ESP_LOGW(TAG, "Unhandled topic (subscribed but no case): %s", topic);
    }

    cJSON_Delete(doc);
}
