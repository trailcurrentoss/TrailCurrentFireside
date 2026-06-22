#include "fireside_config.h"

#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "fireside_cfg";
#define NVS_NS "fireside"

static fireside_config_t s_cfg;
static bool s_loaded = false;

static esp_err_t load_str(nvs_handle_t h, const char *key, char *out, size_t out_sz)
{
    size_t n = out_sz;
    esp_err_t r = nvs_get_str(h, key, out, &n);
    if (r == ESP_ERR_NVS_NOT_FOUND) {
        out[0] = '\0';
        return ESP_OK;
    }
    return r;
}

esp_err_t fireside_config_init(void)
{
    if (s_loaded) return ESP_OK;
    memset(&s_cfg, 0, sizeof(s_cfg));

    nvs_handle_t h;
    esp_err_t r = nvs_open(NVS_NS, NVS_READONLY, &h);
    if (r == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "no saved config — using empty defaults");
        s_cfg.mqtt_port = 8883;
        s_loaded = true;
        return ESP_OK;
    }
    if (r != ESP_OK) return r;

    load_str(h, "wifi_ssid", s_cfg.wifi_ssid, sizeof(s_cfg.wifi_ssid));
    load_str(h, "wifi_pass", s_cfg.wifi_pass, sizeof(s_cfg.wifi_pass));
    load_str(h, "mqtt_host", s_cfg.mqtt_host, sizeof(s_cfg.mqtt_host));
    load_str(h, "mqtt_user", s_cfg.mqtt_user, sizeof(s_cfg.mqtt_user));
    load_str(h, "mqtt_pass", s_cfg.mqtt_pass, sizeof(s_cfg.mqtt_pass));
    uint16_t port = 0;
    if (nvs_get_u16(h, "mqtt_port", &port) != ESP_OK) port = 0;
    s_cfg.mqtt_port = port == 0 ? 8883 : port;
    nvs_close(h);

    s_loaded = true;
    ESP_LOGI(TAG, "loaded: wifi_ssid=%s (pass %s), mqtt=%s:%u user=%s",
             s_cfg.wifi_ssid[0] ? s_cfg.wifi_ssid : "(empty)",
             s_cfg.wifi_pass[0] ? "set" : "unset",
             s_cfg.mqtt_host[0] ? s_cfg.mqtt_host : "(empty)",
             (unsigned)s_cfg.mqtt_port,
             s_cfg.mqtt_user[0] ? s_cfg.mqtt_user : "(empty)");
    return ESP_OK;
}

const fireside_config_t *fireside_config_get(void) { return &s_cfg; }

bool fireside_config_has_wifi(void)
{
    return s_cfg.wifi_ssid[0] != '\0';
}

bool fireside_config_has_mqtt(void)
{
    return s_cfg.mqtt_host[0] != '\0' && s_cfg.mqtt_user[0] != '\0';
}

esp_err_t fireside_config_set_wifi(const char *ssid, const char *pass)
{
    if (!ssid) return ESP_ERR_INVALID_ARG;

    strlcpy(s_cfg.wifi_ssid, ssid, sizeof(s_cfg.wifi_ssid));
    strlcpy(s_cfg.wifi_pass, pass ? pass : "", sizeof(s_cfg.wifi_pass));

    nvs_handle_t h;
    esp_err_t r = nvs_open(NVS_NS, NVS_READWRITE, &h);
    if (r != ESP_OK) return r;
    nvs_set_str(h, "wifi_ssid", s_cfg.wifi_ssid);
    nvs_set_str(h, "wifi_pass", s_cfg.wifi_pass);
    r = nvs_commit(h);
    nvs_close(h);
    ESP_LOGI(TAG, "saved wifi: %s", s_cfg.wifi_ssid);
    return r;
}

esp_err_t fireside_config_clear_wifi(void)
{
    s_cfg.wifi_ssid[0] = '\0';
    s_cfg.wifi_pass[0] = '\0';

    nvs_handle_t h;
    esp_err_t r = nvs_open(NVS_NS, NVS_READWRITE, &h);
    if (r != ESP_OK) return r;
    nvs_erase_key(h, "wifi_ssid");
    nvs_erase_key(h, "wifi_pass");
    r = nvs_commit(h);
    nvs_close(h);
    return r;
}

esp_err_t fireside_config_set_mqtt(const char *host, const char *user,
                                   const char *pass, uint16_t port)
{
    strlcpy(s_cfg.mqtt_host, host ? host : "", sizeof(s_cfg.mqtt_host));
    strlcpy(s_cfg.mqtt_user, user ? user : "", sizeof(s_cfg.mqtt_user));
    strlcpy(s_cfg.mqtt_pass, pass ? pass : "", sizeof(s_cfg.mqtt_pass));
    s_cfg.mqtt_port = port == 0 ? 8883 : port;

    nvs_handle_t h;
    esp_err_t r = nvs_open(NVS_NS, NVS_READWRITE, &h);
    if (r != ESP_OK) return r;
    nvs_set_str(h, "mqtt_host", s_cfg.mqtt_host);
    nvs_set_str(h, "mqtt_user", s_cfg.mqtt_user);
    nvs_set_str(h, "mqtt_pass", s_cfg.mqtt_pass);
    nvs_set_u16(h, "mqtt_port", s_cfg.mqtt_port);
    r = nvs_commit(h);
    nvs_close(h);
    ESP_LOGI(TAG, "saved mqtt: %s:%u user=%s",
             s_cfg.mqtt_host, (unsigned)s_cfg.mqtt_port, s_cfg.mqtt_user);
    return r;
}

esp_err_t fireside_config_clear_mqtt(void)
{
    s_cfg.mqtt_host[0] = '\0';
    s_cfg.mqtt_user[0] = '\0';
    s_cfg.mqtt_pass[0] = '\0';
    s_cfg.mqtt_port = 8883;

    nvs_handle_t h;
    esp_err_t r = nvs_open(NVS_NS, NVS_READWRITE, &h);
    if (r != ESP_OK) return r;
    nvs_erase_key(h, "mqtt_host");
    nvs_erase_key(h, "mqtt_user");
    nvs_erase_key(h, "mqtt_pass");
    nvs_erase_key(h, "mqtt_port");
    r = nvs_commit(h);
    nvs_close(h);
    return r;
}
