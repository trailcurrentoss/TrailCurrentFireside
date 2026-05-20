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
#include <string.h>
#include <stdlib.h>

/* Display lock/unlock from BSP - must hold around LVGL calls from non-LVGL tasks */
extern void bsp_display_lock(int timeout_ms);
extern void bsp_display_unlock(void);

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

#define NVS_NAMESPACE "sd_config"

/* Connection settings loaded from NVS */
static char s_host[128] = {0};
static uint16_t s_port = 8883;
static char s_username[64] = {0};
static char s_password[128] = {0};
static char *s_ca_cert_pem = NULL;

static esp_mqtt_client_handle_t s_client = NULL;
static volatile bool s_connected = false;

/* Queue for passing received messages from MQTT task to main loop */
typedef struct {
    char topic[128];
    char payload[512];
    int payload_len;
} mqtt_message_t;

static QueueHandle_t s_incoming_queue = NULL;

/* Forward declarations */
static void process_message(const char *topic, const char *payload, int length);
static void process_gnss_mode(int mode);

/* --- MQTT event handler (runs in MQTT task context) --- */

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                                int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Connected to broker");
        s_connected = true;

        /* Subscribe to all data topics */
        esp_mqtt_client_subscribe(s_client, "local/lights/+/status", 0);
        esp_mqtt_client_subscribe(s_client, "local/energy/status", 0);
        esp_mqtt_client_subscribe(s_client, "local/airquality/temphumid", 0);
        esp_mqtt_client_subscribe(s_client, "local/airquality/status", 0);
        esp_mqtt_client_subscribe(s_client, "local/gps/latlon", 0);
        esp_mqtt_client_subscribe(s_client, "local/gps/alt", 0);
        esp_mqtt_client_subscribe(s_client, "local/gps/details", 0);
        esp_mqtt_client_subscribe(s_client, "local/gps/time", 0);
        esp_mqtt_client_subscribe(s_client, "local/water/status", 0);
        esp_mqtt_client_subscribe(s_client, "local/relays/+/status", 0);
        esp_mqtt_client_subscribe(s_client, "local/discovery/trigger", 0);
        esp_mqtt_client_subscribe(s_client, "local/ota/trigger", 0);
        ESP_LOGI(TAG, "Subscribed to all topics");

        bsp_display_lock(0);
        set_var_mqtt_connected(true);
        bsp_display_unlock();
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "Disconnected from broker");
        s_connected = false;
        bsp_display_lock(0);
        set_var_mqtt_connected(false);
        bsp_display_unlock();
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
            xQueueSend(s_incoming_queue, &msg, 0);
        }
        break;
    }

    case MQTT_EVENT_ERROR:
        if (event->error_handle) {
            ESP_LOGE(TAG, "Error type: %d", event->error_handle->error_type);
        }
        break;

    default:
        break;
    }
}

/* --- Public API --- */

bool mqtt_client_load_settings(void) {
    nvs_handle_t nvs;
    esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS: %s", esp_err_to_name(ret));
        return false;
    }

    size_t len;

    len = sizeof(s_host);
    if (nvs_get_str(nvs, "mqttHost", s_host, &len) == ESP_OK) {
        ESP_LOGI(TAG, "Host: %s", s_host);
    }

    if (nvs_get_u16(nvs, "mqttPort", &s_port) != ESP_OK) {
        s_port = 8883;
    }
    ESP_LOGI(TAG, "Port: %d", s_port);

    len = sizeof(s_username);
    if (nvs_get_str(nvs, "mqttUser", s_username, &len) == ESP_OK) {
        ESP_LOGI(TAG, "User: %s", s_username);
    }

    len = sizeof(s_password);
    if (nvs_get_str(nvs, "mqttPass", s_password, &len) == ESP_OK) {
        ESP_LOGI(TAG, "Password loaded");
    }

    /* CA certificate */
    len = 0;
    esp_err_t cret = nvs_get_str(nvs, "mqttCaCert", NULL, &len);
    if (cret == ESP_OK && len > 0) {
        if (s_ca_cert_pem) {
            free(s_ca_cert_pem);
        }
        s_ca_cert_pem = malloc(len);
        if (s_ca_cert_pem) {
            nvs_get_str(nvs, "mqttCaCert", s_ca_cert_pem, &len);
            ESP_LOGI(TAG, "CA cert loaded from NVS (%d bytes)", (int)len);
        } else {
            ESP_LOGE(TAG, "malloc(%d) for CA cert failed", (int)len);
        }
    } else {
        ESP_LOGW(TAG, "CA cert not in NVS: %s", esp_err_to_name(cret));
    }

    nvs_close(nvs);

    bool has_config = strlen(s_host) > 0 && strlen(s_username) > 0 &&
                      strlen(s_password) > 0;
    if (!has_config) {
        ESP_LOGW(TAG, "Missing config - host:%s user:%s pass:%s",
                 strlen(s_host) > 0 ? "ok" : "MISSING",
                 strlen(s_username) > 0 ? "ok" : "MISSING",
                 strlen(s_password) > 0 ? "ok" : "MISSING");
    }
    return has_config;
}

void mqtt_client_connect(void) {
    if (strlen(s_host) == 0 || strlen(s_username) == 0 ||
        strlen(s_password) == 0) {
        ESP_LOGW(TAG, "Cannot connect - missing MQTT configuration");
        return;
    }

    /* Create incoming message queue */
    if (!s_incoming_queue) {
        s_incoming_queue = xQueueCreate(16, sizeof(mqtt_message_t));
    }

    /* Build URI */
    char uri[192];
    snprintf(uri, sizeof(uri), "mqtts://%s:%d", s_host, s_port);

    /* Generate client ID from MAC */
    uint8_t mac[6] = {0};
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    char client_id[32];
    snprintf(client_id, sizeof(client_id), "tc-display-%02x%02x", mac[4], mac[5]);

    ESP_LOGI(TAG, "Connecting to %s as %s...", uri, s_username);

    /* Destroy previous client if reconnecting */
    if (s_client) {
        esp_mqtt_client_stop(s_client);
        esp_mqtt_client_destroy(s_client);
        s_client = NULL;
    }

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = uri,
        .broker.verification.skip_cert_common_name_check = true,
        .credentials.client_id = client_id,
        .credentials.username = s_username,
        .credentials.authentication.password = s_password,
        .network.timeout_ms = 10000,
        .session.keepalive = 30,
        .buffer.size = 1024,
    };

    if (s_ca_cert_pem) {
        mqtt_cfg.broker.verification.certificate = s_ca_cert_pem;
        ESP_LOGI(TAG, "Using self-signed CA cert");
    } else {
        ESP_LOGW(TAG, "No CA cert loaded - TLS connection will likely fail");
    }

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

void mqtt_client_process_messages(void) {
    if (!s_incoming_queue) {
        vTaskDelay(pdMS_TO_TICKS(10));
        return;
    }

    mqtt_message_t msg;
    /* Block up to 10ms waiting for the first message — wakes immediately
     * when a message arrives instead of polling on a fixed interval. */
    if (xQueueReceive(s_incoming_queue, &msg, pdMS_TO_TICKS(10)) == pdTRUE) {
        /* Acquire lock once, process all pending messages, release */
        bsp_display_lock(0);
        process_message(msg.topic, msg.payload, msg.payload_len);
        while (xQueueReceive(s_incoming_queue, &msg, 0) == pdTRUE) {
            process_message(msg.topic, msg.payload, msg.payload_len);
        }
        bsp_display_unlock();
    }
}

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
    bsp_display_lock(0);
    set_var_mqtt_connected(false);
    bsp_display_unlock();
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
        bsp_display_lock(0);
        for (int j = 0; j < n; j++) {
            switch (to_clear[j]) {
                case WD_ENERGY:     clear_var_energy();     break;
                case WD_AIRQUALITY: clear_var_airquality(); break;
                case WD_GPS:        clear_var_gps();        break;
                case WD_WATER:      clear_var_water();      break;
                default: break;
            }
        }
        bsp_display_unlock();
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
        ESP_LOGD(TAG, "Unhandled topic: %s", topic);
    }

    cJSON_Delete(doc);
}
