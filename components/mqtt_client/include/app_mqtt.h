#ifndef APP_MQTT_H
#define APP_MQTT_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Load MQTT settings from fireside_config (NVS namespace "fireside").
 * fireside_config_init() must have been called first.
 * Returns true if host + username are present (enough to attempt a connection).
 */
bool mqtt_client_load_settings(void);

/**
 * Register a callback invoked when MQTT goes up or down. The callback runs
 * on the MQTT event task — keep it short and bounce LVGL work to the LVGL
 * thread (lv_async_call). Pass NULL to clear.
 */
typedef void (*mqtt_client_state_cb_t)(bool connected);
void mqtt_client_set_state_callback(mqtt_client_state_cb_t cb);

/**
 * Connect to the MQTT broker using loaded settings.
 * Builds mqtts:// URI, creates client with TLS, subscribes on connect.
 * Call this after WiFi has obtained an IP address.
 *
 * Thread-safe, and idempotent only where that is actually correct: a
 * duplicate request is ignored when the existing client is connected or was
 * started moments ago (the two boot callers fire ~36 ms apart), but a client
 * that has been disconnected for longer is rebuilt. That distinction matters
 * — after a WiFi reassociation the old client is bound to a dead socket, and
 * treating its mere existence as "nothing to do" strands the display
 * offline. Use mqtt_client_reconnect() when the broker settings themselves
 * have changed.
 */
void mqtt_client_connect(void);

/**
 * Force a full teardown and reconnect, ignoring the idempotency check.
 * Call after changing broker host, port, or credentials.
 */
void mqtt_client_reconnect(void);

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

/**
 * Check all module watchdogs and clear stale UI values.
 * Call from the main loop on every iteration (after process_messages).
 * Zero overhead unless a module transitions state.
 */
void mqtt_client_check_watchdogs(void);

#endif /* APP_MQTT_H */
