#include "ota.h"
#include "discovery.h"
#include "app_mqtt.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "mdns.h"

static const char *TAG = "ota";

static volatile bool s_ota_running = false;
static volatile bool s_ota_complete = false;

static esp_ota_handle_t s_ota_handle;
static const esp_partition_t *s_update_partition;

static esp_err_t ota_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "OTA upload started (%d bytes)", req->content_len);

    s_update_partition = esp_ota_get_next_update_partition(NULL);
    if (s_update_partition == NULL) {
        ESP_LOGE(TAG, "No OTA partition available");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No OTA partition");
        return ESP_FAIL;
    }

    esp_err_t err = esp_ota_begin(s_update_partition, OTA_WITH_SEQUENTIAL_WRITES, &s_ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed: %s", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OTA begin failed");
        return ESP_FAIL;
    }

    char buf[1024];
    int received = 0;
    int total = 0;

    while (total < req->content_len) {
        received = httpd_req_recv(req, buf, sizeof(buf));
        if (received <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) continue;
            ESP_LOGE(TAG, "OTA receive error");
            esp_ota_abort(s_ota_handle);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Receive failed");
            return ESP_FAIL;
        }

        err = esp_ota_write(s_ota_handle, buf, received);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_ota_write failed: %s", esp_err_to_name(err));
            esp_ota_abort(s_ota_handle);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Write failed");
            return ESP_FAIL;
        }

        total += received;
        if ((total % (64 * 1024)) == 0 || total == req->content_len) {
            ESP_LOGI(TAG, "OTA progress: %d / %d bytes", total, req->content_len);
        }
    }

    err = esp_ota_end(s_ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_end failed: %s", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OTA end failed");
        return ESP_FAIL;
    }

    err = esp_ota_set_boot_partition(s_update_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed: %s", esp_err_to_name(err));
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Set boot partition failed");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "OTA upload complete, rebooting...");
    httpd_resp_sendstr(req, "OTA OK, rebooting...\n");

    s_ota_complete = true;
    return ESP_OK;
}

static httpd_handle_t start_ota_server(const char *hostname)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return NULL;
    }

    httpd_uri_t ota_uri = {
        .uri     = "/ota",
        .method  = HTTP_POST,
        .handler = ota_post_handler,
    };
    httpd_register_uri_handler(server, &ota_uri);

    ESP_LOGI(TAG, "OTA HTTP server started on port %d", config.server_port);
    ESP_LOGI(TAG, "Upload: curl -X POST http://%s.local/ota --data-binary @build/fireside.bin",
             hostname);

    return server;
}

static void ota_task_fn(void *arg)
{
    ESP_LOGI(TAG, "=== Entering OTA mode ===");

    char hostname[16];
    mqtt_client_hostname(hostname, sizeof(hostname));

    mqtt_client_stop();

    mdns_init();
    mdns_hostname_set(hostname);
    mdns_instance_name_set("TrailCurrent Fireside OTA");
    ESP_LOGI(TAG, "mDNS hostname: %s.local", hostname);

    httpd_handle_t server = start_ota_server(hostname);

    s_ota_complete = false;
    int64_t start = esp_timer_get_time();

    while (!s_ota_complete) {
        vTaskDelay(pdMS_TO_TICKS(100));
        int64_t elapsed_ms = (esp_timer_get_time() - start) / 1000;
        if (elapsed_ms >= OTA_TIMEOUT_MS) {
            ESP_LOGW(TAG, "OTA timeout — no upload received");
            break;
        }
    }

    if (server) httpd_stop(server);
    mdns_free();

    if (s_ota_complete) {
        ESP_LOGI(TAG, "Restarting with new firmware...");
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_restart();
    }

    /* Timed out without an upload — resume normal operation. */
    mqtt_client_connect();
    ESP_LOGI(TAG, "=== OTA mode exited, resumed normal operation ===");

    s_ota_running = false;
    vTaskDelete(NULL);
}

void ota_init(void)
{
    ESP_LOGI(TAG, "OTA ready — will respond to local/ota/trigger");
}

bool ota_is_running(void)
{
    return s_ota_running;
}

void ota_handle_trigger(void)
{
    if (s_ota_running) {
        ESP_LOGW(TAG, "OTA already in progress — ignoring trigger");
        return;
    }
    if (discovery_is_running()) {
        ESP_LOGW(TAG, "Discovery in progress — cannot start OTA");
        return;
    }
    s_ota_running = true;
    xTaskCreate(ota_task_fn, "ota", 8192, NULL, 3, NULL);
}
