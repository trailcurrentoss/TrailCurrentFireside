#ifndef APP_MQTT_H
#define APP_MQTT_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Load MQTT settings from NVS (namespace "sd_config").
 * Must be called after sd_config_read() has populated NVS.
 * Returns true if host, username, and password are all present.
 */
bool mqtt_client_load_settings(void);

/**
 * Connect to the MQTT broker using loaded settings.
 * Builds mqtts:// URI, creates client with TLS, subscribes on connect.
 * Call this after WiFi has obtained an IP address.
 */
void mqtt_client_connect(void);

/**
 * Process queued incoming MQTT messages.
 * Dequeues messages, parses JSON, updates UI variables.
 * Call this periodically from the main loop.
 */
void mqtt_client_process_messages(void);

/**
 * Check if MQTT client is currently connected to the broker.
 */
bool mqtt_client_is_connected(void);

/**
 * Publish a message to the specified topic.
 * Returns message ID on success, -1 on failure.
 */
int mqtt_client_publish(const char *topic, const char *payload, int payload_len);

/**
 * Stop and destroy the MQTT client. Frees broker TCP/TLS sockets so the
 * device can reuse them for discovery/OTA HTTP servers. After stopping,
 * call mqtt_client_connect() to resume normal operation.
 */
void mqtt_client_stop(void);

/**
 * Write this device's hostname (esp32-XXXXXX, last 3 bytes of STA MAC in
 * uppercase hex) into out. Returns out for convenience. Caller must provide
 * a buffer of at least 14 bytes.
 */
const char *mqtt_client_hostname(char *out, size_t out_len);

#endif /* APP_MQTT_H */
