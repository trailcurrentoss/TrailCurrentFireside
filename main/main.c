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
#include "esp_app_desc.h"
#include "esp_mac.h"
#include "lvgl.h"
#include "screens.h"
#include "ui.h"
#include "vars.h"
#include "bsp/display.h"
#include "bsp/esp-bsp.h"
#include "mqtt_vars.h"
#include "app_mqtt.h"
#include "button_config.h"
#include "discovery.h"
#include "ota.h"
#include "fireside_config.h"
#include "app_state.h"

/* Check if WiFi is enabled (ESP-Hosted for ESP32-P4 via ESP-WIFI-REMOTE) */
#if defined(CONFIG_ESP_HOSTED_ENABLED)
#define WIFI_ENABLED 1
#include "esp_wifi.h"
#include "esp_hosted.h"
#else
#define WIFI_ENABLED 0
#endif

/* Nav button lookup table init from actions.c */
extern void init_nav_lookup(void);
extern void init_brightness_slider(void);
extern void ui_bind_button_edit_keyboard(void);

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

void app_main(void)
{
    /* Initialize NVS — required for WiFi credentials, MQTT config, and user settings */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Load persisted Fireside config (WiFi + MQTT credentials entered through
     * the touchscreen wizard). On a freshly-flashed device this is empty and
     * the wizard handles first-time provisioning. */
    ESP_ERROR_CHECK(fireside_config_init());

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

    /* Load MQTT settings from fireside_config (touchscreen-entered NVS) */
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

    /* Load persisted button mappings before UI creation so first paint
     * already reflects user's labels/icons. */
    button_config_init();

    bsp_display_lock(0);
    ui_init();
    init_nav_lookup();
    init_brightness_slider();
    ui_bind_button_edit_keyboard();
    button_config_apply_to_ui();

    /* About screen: firmware version + base MAC */
    const esp_app_desc_t *app = esp_app_get_description();
    if (app && objects.label_version_number) {
        lv_label_set_text(objects.label_version_number, app->version);
    }
    uint8_t about_mac[6] = {0};
    /* On ESP32-P4 the WiFi MAC lives on the C6 slave over ESP-Hosted, not in
     * local efuse — esp_read_mac() can't see it. Query via esp_wifi_get_mac()
     * which goes through the ESP-Hosted RPC bridge. */
#if WIFI_ENABLED
    if (esp_wifi_get_mac(WIFI_IF_STA, about_mac) == ESP_OK) {
        char buf[18];
        snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
                 about_mac[0], about_mac[1], about_mac[2],
                 about_mac[3], about_mac[4], about_mac[5]);
        extern void set_var_mcu_mac_address(const char *value);
        set_var_mcu_mac_address(buf);
    }
#endif

    /* Restore persisted user settings (theme, brightness, timeout, timezone) */
    extern void restore_user_settings(void);
    restore_user_settings();

#if WIFI_ENABLED
    /* Touchscreen setup state machine. On first boot with no saved creds it
     * loads PageWifiSetup → scan list → password keyboard → PageWifiConnecting
     * → PageMqttSetup wizard → PageMqttConnecting → PageHome. Subsequent boots
     * with saved creds skip the wizard and go straight to PageWifiConnecting,
     * then PageHome once both WiFi + MQTT are up.
     *
     * If the EEZ wizard pages haven't been re-exported yet
     * (FIRESIDE_HAS_WIZARD_UI is 0), app_state_init logs a warning and falls
     * through — the rest of boot still works, but there's no first-boot UI for
     * provisioning until the export is run. */
    if (objects.label_wifi_connection_status) {
        if (fireside_config_has_wifi()) {
            lv_label_set_text(objects.label_wifi_connection_status,
                              "Status: Connecting...");
        } else {
            lv_label_set_text(objects.label_wifi_connection_status,
                              "Status: Not configured — open Settings → Reset Connection to provision");
        }
    }
#endif
    bsp_display_unlock();

#if WIFI_ENABLED
    /* Drive the wizard / auto-connect through app_state. Must run AFTER
     * ui_init() (objects.* are referenced) but display lock is released so
     * lv_scr_load() inside app_state can run on the LVGL thread. */
    ESP_ERROR_CHECK(app_state_init());
#endif

    discovery_init();
    ota_init();

    ESP_LOGI(TAG, "Setup done");
    uint32_t last_clock_tick = 0;
    while (1)
    {
        /* Process incoming MQTT messages — blocks up to 10ms on the queue,
         * wakes immediately when a message arrives. */
        mqtt_client_process_messages();

        /* Check per-module watchdogs and clear stale values when a module stops
         * reporting. Acquires display lock only on state transitions (rare). */
        mqtt_client_check_watchdogs();

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
