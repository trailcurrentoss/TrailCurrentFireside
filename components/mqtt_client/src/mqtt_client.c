#include "app_mqtt.h"
#include "mqtt_client.h" /* ESP-IDF esp_mqtt library */
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <string.h>
#include <stdlib.h>

/* Display lock/unlock from BSP - must hold around LVGL calls from non-LVGL tasks */
extern void bsp_display_lock(int timeout_ms);
extern void bsp_display_unlock(void);

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
extern void set_var_latitude(float lat);
extern void set_var_longitude(float lon);
extern void set_var_altitude(float feet);
extern void set_var_speed(float knots);
extern void set_var_course(float degrees);
extern void set_var_gnss_mode(const char *mode);
extern void set_var_humidity(float percent);
extern void set_var_mqtt_connected(bool connected);
extern void set_var_satellite_count(int32_t value);
extern void set_var_current_interior_temperature(int32_t value);
extern void set_var_gps_time(int year, int month, int day, int hour, int minute, int second);

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
        esp_mqtt_client_subscribe(s_client, "local/gps/latlon", 0);
        esp_mqtt_client_subscribe(s_client, "local/gps/alt", 0);
        esp_mqtt_client_subscribe(s_client, "local/gps/details", 0);
        esp_mqtt_client_subscribe(s_client, "local/gps/time", 0);
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
    if (nvs_get_str(nvs, "mqttCaCert", NULL, &len) == ESP_OK && len > 0) {
        if (s_ca_cert_pem) {
            free(s_ca_cert_pem);
        }
        s_ca_cert_pem = malloc(len);
        if (s_ca_cert_pem) {
            nvs_get_str(nvs, "mqttCaCert", s_ca_cert_pem, &len);
            ESP_LOGI(TAG, "CA cert loaded (%d bytes)", (int)len);
        }
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

int mqtt_client_publish(const char *topic, const char *payload, int payload_len) {
    if (!s_connected || !s_client) {
        ESP_LOGW(TAG, "Not connected, cannot publish to %s", topic);
        return -1;
    }
    int msg_id = esp_mqtt_client_publish(s_client, topic, payload, payload_len, 0, 0);
    ESP_LOGI(TAG, "Published to %s (msg_id=%d)", topic, msg_id);
    return msg_id;
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

    /* local/lights/{id}/status */
    if (strncmp(topic, "local/lights/", 13) == 0) {
        const char *id_str = topic + 13;
        int id = atoi(id_str);

        cJSON *state_j = cJSON_GetObjectItem(doc, "state");
        cJSON *brightness_j = cJSON_GetObjectItem(doc, "brightness");
        int state = state_j ? state_j->valueint : 0;
        int brightness = brightness_j ? brightness_j->valueint : 0;
        int value = (state > 0) ? ((brightness > 0) ? brightness : 1) : 0;

        switch (id) {
        case 1: set_var_device01_status(value); break;
        case 2: set_var_device02_status(value); break;
        case 3: set_var_device03_status(value); break;
        case 4: set_var_device04_status(value); break;
        case 5: set_var_device05_status(value); break;
        case 6: set_var_device06_status(value); break;
        case 7: set_var_device07_status(value); break;
        case 8: set_var_device08_status(value); break;
        default: ESP_LOGW(TAG, "Unknown light id: %d", id); break;
        }
    }
    /* local/energy/status */
    else if (strcmp(topic, "local/energy/status") == 0) {
        cJSON *bp = cJSON_GetObjectItem(doc, "battery_percent");
        cJSON *bv = cJSON_GetObjectItem(doc, "battery_voltage");
        cJSON *sw = cJSON_GetObjectItem(doc, "solar_watts");
        cJSON *ct = cJSON_GetObjectItem(doc, "charge_type");

        if (bp) set_var_battery_soc((int32_t)bp->valuedouble);
        if (bv) set_var_battery_voltage((float)bv->valuedouble);
        if (sw) set_var_solar_watts((int32_t)sw->valuedouble);
        if (ct && ct->valuestring) set_var_solar_status(ct->valuestring);
    }
    /* local/airquality/temphumid */
    else if (strcmp(topic, "local/airquality/temphumid") == 0) {
        cJSON *temp_f = cJSON_GetObjectItem(doc, "tempInF");
        cJSON *humid = cJSON_GetObjectItem(doc, "humidity");

        if (temp_f) {
            set_var_current_interior_temperature((int32_t)temp_f->valuedouble);
        }
        if (humid) {
            set_var_humidity((float)humid->valuedouble);
        }
    }
    /* local/gps/latlon */
    else if (strcmp(topic, "local/gps/latlon") == 0) {
        cJSON *lat = cJSON_GetObjectItem(doc, "latitude");
        cJSON *lon = cJSON_GetObjectItem(doc, "longitude");
        if (lat) set_var_latitude((float)lat->valuedouble);
        if (lon) set_var_longitude((float)lon->valuedouble);
    }
    /* local/gps/alt */
    else if (strcmp(topic, "local/gps/alt") == 0) {
        cJSON *alt = cJSON_GetObjectItem(doc, "altitudeFeet");
        if (alt) set_var_altitude((float)alt->valuedouble);
    }
    /* local/gps/details */
    else if (strcmp(topic, "local/gps/details") == 0) {
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
    else {
        ESP_LOGD(TAG, "Unhandled topic: %s", topic);
    }

    cJSON_Delete(doc);
}
