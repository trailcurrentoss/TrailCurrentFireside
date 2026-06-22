#pragma once

/*
 * Persistent configuration for Fireside (NVS-backed, namespace "fireside").
 *
 * Stores WiFi credentials and MQTT broker settings entered through the
 * touchscreen on first boot. Replaces the SD card config.env approach —
 * the wizard on first boot collects everything needed for a connection.
 *
 * MQTT TLS verification is skipped at the TLS layer
 * (CONFIG_ESP_TLS_INSECURE + CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY are set
 * in sdkconfig.defaults) so self-signed certificates just work; no CA cert
 * needs to live on the device.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FIRESIDE_CFG_SSID_MAX  33    /* 32 + NUL */
#define FIRESIDE_CFG_PASS_MAX  65    /* 64 + NUL */
#define FIRESIDE_CFG_HOST_MAX  65    /* hostname/IP + NUL */
#define FIRESIDE_CFG_USER_MAX  65    /* MQTT username + NUL */
#define FIRESIDE_CFG_MPASS_MAX 129   /* MQTT password + NUL */

typedef struct {
    char     wifi_ssid[FIRESIDE_CFG_SSID_MAX];
    char     wifi_pass[FIRESIDE_CFG_PASS_MAX];

    char     mqtt_host[FIRESIDE_CFG_HOST_MAX];
    char     mqtt_user[FIRESIDE_CFG_USER_MAX];
    char     mqtt_pass[FIRESIDE_CFG_MPASS_MAX];
    uint16_t mqtt_port;
} fireside_config_t;

/* Initialize NVS namespace and load cached config. Safe to call once.
 * Caller must have already called nvs_flash_init(). */
esp_err_t fireside_config_init(void);

/* Pointer to the in-memory cached config. Read-only access; mutate via setters. */
const fireside_config_t *fireside_config_get(void);

/* True if a non-empty SSID is saved. */
bool fireside_config_has_wifi(void);

/* True if mqtt_host AND mqtt_user are saved. */
bool fireside_config_has_mqtt(void);

/* Write SSID + password to NVS and update cache. password may be empty for open APs. */
esp_err_t fireside_config_set_wifi(const char *ssid, const char *pass);

/* Write MQTT host / user / password / port to NVS and update cache. port=0 means
 * use default (8883). */
esp_err_t fireside_config_set_mqtt(const char *host, const char *user,
                                   const char *pass, uint16_t port);

/* Erase WiFi creds from NVS and cache. */
esp_err_t fireside_config_clear_wifi(void);
/* Erase MQTT config. */
esp_err_t fireside_config_clear_mqtt(void);

#ifdef __cplusplus
}
#endif
