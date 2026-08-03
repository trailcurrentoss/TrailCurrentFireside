/*
 * CrowPanel Advance ESP32-P4 10.1" — Fireside UI + WiFi.
 *
 * Bring-up order (Lesson09-first, WiFi last):
 *   1. NVS + fireside_config
 *   2. LDO3 + LDO4
 *   3. I2C
 *   4. touch_init  — GT911
 *   5. display_init — MIPI-DSI + lvgl_port + touch indev
 *   6. set_lcd_blight(80)
 *   7. ui_init      — EEZ screens
 *   8. esp_netif + esp_event + esp_hosted + esp_wifi_start (WiFi LAST)
 *   9. app_state_init + app_state_start_wifi (kicks scan/auto-connect)
 *  10. Main loop: MQTT pump + clock
 *
 * The failure mode this ordering avoids: bringing up ESP-Hosted / esp_wifi
 * *before* touch_init + lvgl_port_add_touch left LVGL's indev polling dead
 * (verified — GT911 driver reported presses, LVGL saw none). Doing display
 * + touch first, WiFi last, matches Lesson09 for touch and Lesson16 for
 * WiFi and works with both alive.
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "settings_store.h"
#include "sd_config.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_ldo_regulator.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_app_desc.h"
#include "esp_wifi.h"
#include "esp_hosted.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"

#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "bsp_i2c.h"
#include "bsp_display.h"
#include "bsp_illuminate.h"

#include "battery.h"
#include "fireside_config.h"
#include "app_mqtt.h"
#include "mqtt_vars.h"
#include "button_config.h"
#include "wifi_health.h"

#if __has_include("ui/screens.h")
#include "screens.h"
#include "ui.h"
#include "vars.h"
#include "app_state.h"
#define UI_EXPORT_PRESENT 1
#else
#define UI_EXPORT_PRESENT 0
#endif

static const char *TAG = "MAIN";

/* mqtt_client component pulls these symbols in. Stub them here since we
 * don't wire discovery/OTA on this port yet. */
void discovery_handle_trigger(void) {}
void ota_handle_trigger(void)       {}

extern bool system_time_set;
extern void restore_user_settings(void);

static esp_ldo_channel_handle_t s_ldo3 = NULL;
static esp_ldo_channel_handle_t s_ldo4 = NULL;

static void init_fail(const char *module, esp_err_t err) {
    while (1) {
        ESP_LOGE(TAG, "[%s] init failed: %s", module, esp_err_to_name(err));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void on_sntp_sync(struct timeval *tv) {
    (void)tv;
    system_time_set = true;
    ESP_LOGI(TAG, "SNTP synced");
}

static void ip_got_ip(void *arg, esp_event_base_t base, int32_t id, void *data) {
    (void)arg; (void)base; (void)id; (void)data;
    static bool sntp_started = false;
    if (!sntp_started) {
        esp_sntp_config_t cfg = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
        cfg.sync_cb = on_sntp_sync;
        esp_netif_sntp_init(&cfg);
        sntp_started = true;
    }
    if (fireside_config_has_mqtt()) {
        mqtt_client_load_settings();
        mqtt_client_connect();
    }
}

void app_main(void) {
    esp_err_t err;
    ESP_LOGI(TAG, "CrowPanel P4 boot (UI %s, WiFi LAST)",
             UI_EXPORT_PRESENT ? "present" : "MISSING");

    /* 1. NVS + fireside_config + alarms config. */
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    /* Deferred NVS writer — UI callbacks persist settings through this
     * instead of committing flash on the LVGL task. */
    settings_store_init();
    ESP_ERROR_CHECK(fireside_config_init());
    /* Alarms module reads per-mode arm config from NVS ns "fireside_alarm".
     * Empty on first boot — no alarms fire until user configures via
     * Settings > Alarms (Phase 2). */
    extern void alarms_init(void);
    alarms_init();

    /* 2. LDO3 (DSI PHY) + LDO4 (LCD VCC). */
    esp_ldo_channel_config_t ldo3_cfg = { .chan_id = 3, .voltage_mv = 2500 };
    err = esp_ldo_acquire_channel(&ldo3_cfg, &s_ldo3);
    if (err != ESP_OK) init_fail("ldo3", err);

    esp_ldo_channel_config_t ldo4_cfg = { .chan_id = 4, .voltage_mv = 3300 };
    err = esp_ldo_acquire_channel(&ldo4_cfg, &s_ldo4);
    if (err != ESP_OK) init_fail("ldo4", err);

    /* 3-6. I2C → touch → LCD+LVGL+touch-indev → backlight. */
    err = i2c_init();     if (err != ESP_OK) init_fail("i2c", err);
    /* STC8H1KXX battery + charge status shares the same I2C bus (0x2F). If
     * no battery board is populated the reg reads will fail later and the
     * TopBar cluster falls back to "--%" without blocking boot. */
    err = battery_init();
    if (err != ESP_OK) ESP_LOGW(TAG, "battery_init: %s", esp_err_to_name(err));
    err = touch_init();   if (err != ESP_OK) init_fail("touch", err);
    err = display_init(); if (err != ESP_OK) init_fail("display", err);
    err = set_lcd_blight(80);
    if (err != ESP_OK) ESP_LOGW(TAG, "set_lcd_blight: %s", esp_err_to_name(err));

#if UI_EXPORT_PRESENT
    /* 7. ui_init under LVGL lock, land on page_home so we have widgets. */
    if (lvgl_port_lock(0)) {
        ui_init();
        set_var_rotation_degrees(0);
        const esp_app_desc_t *app = esp_app_get_description();
        if (app && objects.label_version_number) {
            lv_label_set_text(objects.label_version_number, app->version);
        }
        restore_user_settings();
        /* Load persisted button assignments and paint them onto both the
         * home tiles and the (optional) Edit Buttons grid. Must run under
         * the LVGL lock and after ui_init(). */
        button_config_init();
        button_config_apply_to_ui();
        if (objects.page_home) lv_scr_load(objects.page_home);
        init_clock_blink();     /* start 500ms dot-blink timer */
        init_metric_charts();   /* create lv_chart in every _chart panel */
        init_wifi_rssi_poll();  /* 5s WiFi RSSI poll → topbar dBm labels */
        init_battery_poll();    /* 10s battery poll → topbar %/icon/bolt */
        init_notif_icon_ack_taps();  /* topbar bell tap → alarms ack */
        init_touch_target_hit_areas(); /* widen ext_click_area on nav + home dev */
        {
            /* Perf probes: LVGL task stall watchdog + keyboard mode-switch
             * timing. Logs to ESP_LOGW/I under TAG "PERF". */
            extern void perf_init(void);
            perf_init();
        }
        init_screen_timeout();  /* 1s idle poll → blank backlight after
                                 * get_var_screen_timeout_minutes() min */
        {
            /* Bring up the I2S DAC + TTS phrase player. audio_init is
             * defined in main/audio.c; declared here as an extern rather
             * than a header include to keep this section self-contained. */
            extern void audio_init(void);
            audio_init();
        }
        {
            /* Peregrine push-to-talk voice terminal. URL/token come from
             * /sdcard/environment.conf (same card + file as the Waveshare
             * build — sd_config.c). The TALK button is shown only when the
             * card supplied both PEREGRINE_URL and PEREGRINE_VOICE_TOKEN. */
            extern void peregrine_voice_init(void);
            extern void init_ptt_glow(void);
            extern void apply_ptt_availability(bool);

            peregrine_voice_init();
            sd_config_load();
            init_ptt_glow();
            apply_ptt_availability(sd_config_peregrine_present());
        }
        reset_placeholders();   /* clear canvas-only authored placeholders
                                 * (tank bars 50 % → 0 %, etc.) so the
                                 * device doesn't show fake data before
                                 * MQTT arrives */
        {
            /* MQTT setup wizard — wire textareas to the keyboard so tapping
             * a field re-binds the keyboard target. Declared in actions.c. */
            extern void init_mqtt_setup_bindings(void);
            init_mqtt_setup_bindings();
            /* Wire the PageButtonEdit textarea → keyboard show/hide
             * (keyboard is authored HIDDEN so it doesn't cover the
             * module/address/device selector rows when not typing). */
            extern void init_button_edit_bindings(void);
            init_button_edit_bindings();
        }
        lvgl_port_unlock();
    }
#endif

    /* 8. WiFi bring-up NOW (after touch is proven up). */
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_got_ip, NULL);

    err = esp_hosted_init();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "ESP-Hosted failed — continuing UI-only");
    } else {
        esp_netif_create_default_wifi_sta();
        wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&wifi_cfg));
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
        ESP_ERROR_CHECK(esp_wifi_start());
        esp_wifi_set_ps(WIFI_PS_NONE);

        /* Arm the C6 heartbeat monitor + auto-restart recovery. See
         * wifi_health.c — this catches the case where the C6's RPC
         * handler task hangs while SDIO stays alive (ESP-Hosted's own
         * transport-restart safety net only covers SDIO-layer failures).
         * Must run after esp_wifi_start so the ESP-Hosted transport is
         * fully up and the heartbeat RPC has somewhere to land. */
        wifi_health_init();

        /* Let C6 slave settle before scan/connect. */
        vTaskDelay(pdMS_TO_TICKS(2000));

        uint8_t mac[6] = {0};
        if (esp_wifi_get_mac(WIFI_IF_STA, mac) == ESP_OK) {
            ESP_LOGI(TAG, "STA MAC = %02X:%02X:%02X:%02X:%02X:%02X",
                     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#if UI_EXPORT_PRESENT
            if (lvgl_port_lock(0)) {
                char macbuf[18];
                snprintf(macbuf, sizeof(macbuf),
                         "%02X:%02X:%02X:%02X:%02X:%02X",
                         mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                set_var_mcu_mac_address(macbuf);
                lvgl_port_unlock();
            }
#endif
        }

#if UI_EXPORT_PRESENT
        /* 9. State machine + wifi_setup + auto-connect. */
        ESP_ERROR_CHECK(app_state_init());
        app_state_start_wifi();
#endif
    }

    ESP_LOGI(TAG, "Setup done — entering main loop");

    uint32_t last_tick = 0;
    while (1) {
#if UI_EXPORT_PRESENT
        /* mqtt_client_process_messages() is now a no-op — MQTT dispatch
         * lives on its own FreeRTOS task ("mqtt_dispatch") so status
         * updates arrive within a few ms of the network delivery instead
         * of waiting for a main-loop iteration. Watchdogs stay here
         * (independent of the queue drain). */
        mqtt_client_check_watchdogs();

        uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
        if (now - last_tick >= 1000) {
            last_tick = now;
            if (lvgl_port_lock(0)) {
                update_clock_display();
                lvgl_port_unlock();
            }
        }
#endif
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
