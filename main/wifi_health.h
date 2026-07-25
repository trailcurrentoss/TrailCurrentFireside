/*
 * wifi_health.h — public entry point for the ESP-Hosted C6 heartbeat
 * monitor. See wifi_health.c for the full rationale.
 */
#pragma once

/* Register the ESP_HOSTED_EVENT handler, create the heartbeat-timeout
 * timer, and ask the C6 to start sending heartbeats. Call once from
 * main.c after esp_hosted_init() + esp_wifi_start() have completed.
 * Idempotent — subsequent calls are no-ops. */
void wifi_health_init(void);
