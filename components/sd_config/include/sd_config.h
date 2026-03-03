#ifndef SD_CONFIG_H
#define SD_CONFIG_H

#include <stdbool.h>

/**
 * Read config.env and optional ca.crt from SD card, store values in NVS.
 *
 * config.env format (same as TrailCurrentWaveshareEsp32s3Remote):
 *   WIFI_SSID=YourNetworkName
 *   WIFI_PWD=YourWiFiPassword
 *   MQTT_HOST=192.168.1.100
 *   MQTT_PORT=8883
 *   MQTT_USER=mqttuser
 *   MQTT_PASS=mqttpassword
 *
 * NVS namespace: "sd_config"
 * Returns true if at least WIFI_SSID was found.
 */
bool sd_config_read(void);

#endif /* SD_CONFIG_H */
