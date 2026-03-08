#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_memory_utils.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "lvgl.h"
#include "screens.h"
#include "ui.h"
#include "vars.h"
#include "bsp/display.h"
#include "bsp/esp-bsp.h"
#include "mqtt_vars.h"
#include "sd_config.h"
#include "app_mqtt.h"

/* Check if WiFi is enabled (ESP-Hosted for ESP32-P4 via ESP-WIFI-REMOTE) */
#if defined(CONFIG_ESP_HOSTED_ENABLED)
#define WIFI_ENABLED 1
#include "esp_wifi.h"
#include "esp_hosted.h"
/* WiFi event handler init from actions.c */
extern void wifi_event_handler_init(void);
#else
#define WIFI_ENABLED 0
#endif

/* Nav button lookup table init from actions.c */
extern void init_nav_lookup(void);
extern void init_brightness_slider(void);

static const char *TAG = "MAIN";

/* Screen timeout / blanking state */
static volatile bool screen_blanked = false;
static lv_obj_t *wake_overlay = NULL;

/**
 * @brief Touch callback on the wake overlay.
 * Absorbs the first touch after screen blank so it doesn't trigger UI buttons,
 * then restores the persisted brightness and removes the overlay.
 * Runs in the LVGL task context (display lock already held).
 */
static void wake_touch_cb(lv_event_t *e) {
    (void)e;
    screen_blanked = false;
    /* Restore brightness to the persisted slider value */
    int32_t brightness = lv_slider_get_value(objects.slider_screen_brightness);
    if (brightness < 10) brightness = 10; /* ensure screen is actually visible */
    bsp_display_brightness_set(brightness);
    if (wake_overlay) {
        lv_obj_del(wake_overlay);
        wake_overlay = NULL;
    }
    ESP_LOGI(TAG, "Screen wake - restored brightness %d", (int)brightness);
}

#define SD_NVS_NAMESPACE "sd_config"

void app_main(void)
{
    /* Initialize NVS - required for WiFi and config storage */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Read configuration from SD card before WiFi/network init */
    bool sd_config_found = sd_config_read();
    if (sd_config_found) {
        ESP_LOGI(TAG, "SD card config loaded successfully");
    } else {
        ESP_LOGI(TAG, "No SD card present - using existing NVS settings");
    }

#if WIFI_ENABLED
    /* Initialize ESP-Hosted transport to communicate with C6 slave */
    ESP_LOGI(TAG, "ESP-Hosted: Initializing...");
    ret = esp_hosted_init();
    ESP_LOGI(TAG, "ESP-Hosted: esp_hosted_init() = %d", ret);
    if (ret != 0) {
        ESP_LOGE(TAG, "ESP-Hosted initialization failed!");
    }

    /* Initialize networking stack */
    ESP_LOGI(TAG, "WiFi: Initializing netif...");
    ret = esp_netif_init();
    ESP_LOGI(TAG, "WiFi: esp_netif_init() = %s", esp_err_to_name(ret));
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "WiFi: Creating event loop...");
    ret = esp_event_loop_create_default();
    ESP_LOGI(TAG, "WiFi: esp_event_loop_create_default() = %s", esp_err_to_name(ret));
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "WiFi: Creating default STA netif...");
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    ESP_LOGI(TAG, "WiFi: STA netif created: %p", sta_netif);

    /* Initialize WiFi with default config */
    ESP_LOGI(TAG, "WiFi: Initializing WiFi driver (ESP-Hosted for P4)...");
    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&wifi_cfg);
    ESP_LOGI(TAG, "WiFi: esp_wifi_init() = %s", esp_err_to_name(ret));
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "WiFi: Setting mode to STA...");
    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    ESP_LOGI(TAG, "WiFi: esp_wifi_set_mode(STA) = %s", esp_err_to_name(ret));
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "WiFi: Starting WiFi...");
    ret = esp_wifi_start();
    ESP_LOGI(TAG, "WiFi: esp_wifi_start() = %s", esp_err_to_name(ret));
    ESP_ERROR_CHECK(ret);

    /* Give ESP-Hosted time to initialize the C6 slave */
    ESP_LOGI(TAG, "WiFi: Waiting for ESP-Hosted slave to be ready...");
    vTaskDelay(pdMS_TO_TICKS(2000));

    /* Log WiFi status */
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    ESP_LOGI(TAG, "WiFi: Current mode = %d (1=STA, 2=AP, 3=APSTA)", mode);

    uint8_t mac[6];
    if (esp_wifi_get_mac(WIFI_IF_STA, mac) == ESP_OK) {
        ESP_LOGI(TAG, "WiFi: STA MAC = %02x:%02x:%02x:%02x:%02x:%02x",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else {
        ESP_LOGW(TAG, "WiFi: Could not get STA MAC address");
    }

    ESP_LOGI(TAG, "WiFi initialized in STA mode");

    /* Load MQTT settings from NVS (populated by SD card read) */
    mqtt_client_load_settings();
#endif

    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = {
            .task_priority = 5,
            .task_stack = 7168,
            .task_affinity = 1,       /* Pin LVGL to core 1 (core 0 runs app_main/MQTT task) */
            .task_max_sleep_ms = 500,
            .task_stack_caps = MALLOC_CAP_INTERNAL | MALLOC_CAP_DEFAULT,
            .timer_period_ms = 5,
        },
        .buffer_size = BSP_LCD_H_RES * 100, /* Larger draw buffer for fewer flushes */
        .double_buffer = 1,                  /* Enable double buffering */
        .flags = {
            .buff_dma = false,
            .buff_spiram = true,   /* Use PSRAM for draw buffers */
            .sw_rotate = true,
        }};
    lv_display_t *disp = bsp_display_start_with_config(&cfg);
    bsp_display_backlight_on();
    if (disp != NULL)
    {
        bsp_display_rotate(disp, LV_DISPLAY_ROTATION_180);
        set_var_rotation_degrees(180);
    }

    bsp_display_lock(0);
    ui_init();
    init_nav_lookup();
    init_brightness_slider();

    /* Restore persisted user settings (theme, brightness, timeout, timezone) */
    extern void restore_user_settings(void);
    restore_user_settings();

#if WIFI_ENABLED
    /* Register WiFi event handlers for UI updates (must be after ui_init) */
    wifi_event_handler_init();

    /* Auto-connect to WiFi using credentials from NVS
       (populated by SD card config.env — SD is only needed to update settings) */
    {
        nvs_handle_t nvs;
        if (nvs_open(SD_NVS_NAMESPACE, NVS_READONLY, &nvs) == ESP_OK) {
            char ssid[33] = {0};
            char password[65] = {0};
            size_t ssid_len = sizeof(ssid);
            size_t pass_len = sizeof(password);

            if (nvs_get_str(nvs, "wifiSSID", ssid, &ssid_len) == ESP_OK &&
                strlen(ssid) > 0) {
                nvs_get_str(nvs, "wifiPass", password, &pass_len);

                ESP_LOGI(TAG, "Connecting to WiFi: %s", ssid);
                wifi_config_t wifi_config = {0};
                strlcpy((char *)wifi_config.sta.ssid, ssid,
                        sizeof(wifi_config.sta.ssid));
                strlcpy((char *)wifi_config.sta.password, password,
                        sizeof(wifi_config.sta.password));
                wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
                esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
                esp_wifi_connect();
                lv_label_set_text(objects.label_wifi_connection_status,
                                  "Status: Connecting...");
                ESP_LOGI(TAG, "WiFi auto-connect initiated");
            } else {
                ESP_LOGW(TAG, "No WiFi SSID in NVS");
                lv_label_set_text(objects.label_wifi_connection_status,
                                  "Status: To update settings, insert SD card with config.env");
            }
            nvs_close(nvs);
        } else {
            lv_label_set_text(objects.label_wifi_connection_status,
                              "Status: To update settings, insert SD card with config.env");
        }
    }
#endif
    bsp_display_unlock();
    ESP_LOGI(TAG, "Setup done");
    uint32_t last_clock_tick = 0;
    while (1)
    {
        /* Process incoming MQTT messages — blocks up to 10ms on the queue,
         * wakes immediately when a message arrives. */
        mqtt_client_process_messages();

        /* Screen timeout: blank after inactivity, wake on touch.
         * Check every loop iteration (~10ms) for responsive wake. */
        if (!screen_blanked) {
            int32_t timeout_val = get_var_screen_timeout_value();
            if (timeout_val > 0) {
                uint32_t timeout_ms = (uint32_t)timeout_val * 60 * 1000;
                bsp_display_lock(0);
                uint32_t inactive_ms = lv_disp_get_inactive_time(NULL);
                if (inactive_ms >= timeout_ms) {
                    screen_blanked = true;
                    bsp_display_brightness_set(0);
                    /* Fullscreen overlay absorbs the first wake touch */
                    wake_overlay = lv_obj_create(lv_layer_top());
                    lv_obj_remove_style_all(wake_overlay);
                    lv_obj_set_size(wake_overlay, LV_PCT(100), LV_PCT(100));
                    lv_obj_add_flag(wake_overlay, LV_OBJ_FLAG_CLICKABLE);
                    lv_obj_clear_flag(wake_overlay, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_add_event_cb(wake_overlay, wake_touch_cb, LV_EVENT_CLICKED, NULL);
                    ESP_LOGI(TAG, "Screen blanked after %d min inactivity", (int)timeout_val);
                }
                bsp_display_unlock();
            }
        }

        /* Update clock display once per second.
         * lv_timer_handler() is NOT called here — the BSP LVGL port task
         * already runs it on a 5ms timer (core 1, priority 5). */
        uint32_t now_tick = xTaskGetTickCount() * portTICK_PERIOD_MS;
        if (now_tick - last_clock_tick >= 1000) {
            last_clock_tick = now_tick;
            bsp_display_lock(0);
            update_clock_display();
            bsp_display_unlock();
        }
    }
}
