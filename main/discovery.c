#include "discovery.h"
#include "ota.h"
#include "app_mqtt.h"

#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_app_desc.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "mdns.h"

static const char *TAG = "discovery";

#ifndef MODULE_TYPE
#define MODULE_TYPE "fireside"
#endif

static volatile bool s_confirmed = false;
static volatile bool s_discovery_running = false;

static void discovery_mdns_start(const char *hostname)
{
    const esp_app_desc_t *app = esp_app_get_description();

    mdns_init();
    mdns_hostname_set(hostname);
    mdns_instance_name_set("TrailCurrent Module");

    mdns_txt_item_t txt[] = {
        { "type", MODULE_TYPE },
        { "fw",   app->version },
    };

    mdns_service_add("TrailCurrent Discovery", "_trailcurrent", "_tcp",
                     80, txt, sizeof(txt) / sizeof(txt[0]));

    ESP_LOGI(TAG, "mDNS discovery: %s.local type=%s fw=%s",
             hostname, MODULE_TYPE, app->version);
}

static esp_err_t confirm_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Discovery confirmed by Headwaters");
    httpd_resp_sendstr(req, "confirmed\n");
    s_confirmed = true;
    return ESP_OK;
}

static httpd_handle_t discovery_start_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return NULL;
    }

    httpd_uri_t confirm_uri = {
        .uri     = "/discovery/confirm",
        .method  = HTTP_GET,
        .handler = confirm_handler,
    };
    httpd_register_uri_handler(server, &confirm_uri);

    return server;
}

static void discovery_task_fn(void *arg)
{
    ESP_LOGI(TAG, "=== Entering discovery mode ===");

    char hostname[16];
    mqtt_client_hostname(hostname, sizeof(hostname));

    /* Free broker sockets so the HTTP server can bind port 80 cleanly. */
    mqtt_client_stop();

    discovery_mdns_start(hostname);
    httpd_handle_t server = discovery_start_server();

    s_confirmed = false;
    int64_t start = esp_timer_get_time();

    while (!s_confirmed) {
        vTaskDelay(pdMS_TO_TICKS(100));
        int64_t elapsed_ms = (esp_timer_get_time() - start) / 1000;
        if (elapsed_ms >= DISCOVERY_TIMEOUT_MS) {
            ESP_LOGW(TAG, "Discovery timeout — no confirmation received");
            break;
        }
    }

    if (server) httpd_stop(server);
    mdns_free();

    /* Return to normal operation regardless of confirm / timeout. */
    mqtt_client_connect();

    if (s_confirmed) {
        ESP_LOGI(TAG, "=== Discovery complete — module registered ===");
    } else {
        ESP_LOGI(TAG, "=== Discovery timed out — resumed normal operation ===");
    }

    s_discovery_running = false;
    vTaskDelete(NULL);
}

void discovery_init(void)
{
    ESP_LOGI(TAG, "Discovery ready — will respond to local/discovery/trigger");
}

bool discovery_is_running(void)
{
    return s_discovery_running;
}

void discovery_handle_trigger(void)
{
    if (s_discovery_running) {
        ESP_LOGW(TAG, "Discovery already in progress — ignoring trigger");
        return;
    }
    if (ota_is_running()) {
        ESP_LOGW(TAG, "OTA in progress — ignoring discovery trigger");
        return;
    }
    s_discovery_running = true;
    xTaskCreate(discovery_task_fn, "discovery", 8192, NULL, 3, NULL);
}
