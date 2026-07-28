/*
 * TrailCurrent Fireside — Waveshare ESP32-P4-WIFI6-Touch-LCD-7B.
 *
 * The board-specific pieces (bring-up order, panel driver, codec IC,
 * battery-sense path) live in:
 *
 *   - components/bsp_shim       — set_lcd_blight -> bsp_display_brightness_set
 *   - main/battery.c            — ADC divider on GPIO20
 *   - main/audio.c              — esp_codec_dev(ES8311)
 *   - waveshare BSP component   — display, touch, I2C, I2S, PA all in one
 *
 * Bring-up order — the Waveshare BSP does most of it for us:
 *   1. NVS + fireside_config + alarms
 *   2. default event loop
 *   3. bsp_i2c_init()               (Waveshare BSP)
 *   4. battery_init                 (ADC on GPIO 20 — safe to fail)
 *   5. bsp_display_start_with_config()
 *        -> creates LDO channel for DSI PHY, EK79007 panel, GT911 touch,
 *           and LVGL port with touch indev, all in one call.
 *   6. bsp_display_backlight_on(); (default 100 %)
 *   7. ui_init  — EEZ screens (under bsp_display_lock)
 *      + button_config_apply, restore_user_settings, init_*_poll(),
 *        init_battery_poll (defined in battery.c), audio_init, etc.
 *   8. WiFi via esp_hosted / esp_wifi_remote (ESP-Hosted SDIO defaults,
 *      no pin overrides needed)
 *   9. app_state_init + app_state_start_wifi
 *  10. Main loop: MQTT watchdog + clock tick
 *
 * NB: we do NOT acquire the LDO channels ourselves — bsp_display_new()
 * acquires LDO channel 3 for the DSI PHY internally
 * (BSP_MIPI_DSI_PHY_PWR_LDO_CHAN). LDO 4 is used by the SD card path
 * (bsp_sdcard_mount) if we ever call it; not needed for display
 * bring-up.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_app_desc.h"
#include "esp_wifi.h"
#include "esp_hosted.h"
#include "esp_heap_caps.h"

#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "bsp/esp32_p4_wifi6_touch_lcd_7b.h"

#include "bsp_shim.h"          /* set_lcd_blight compatibility */

#include "battery.h"
#include "fireside_config.h"
#include "app_mqtt.h"
#include "mqtt_vars.h"
#include "button_config.h"
#include "wifi_health.h"
#include "peregrine_voice.h"
#include "sd_config.h"
#include "discovery.h"

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

/* mqtt_client component pulls this in — OTA isn't ported to Fireside yet.
 * discovery_handle_trigger() is provided by main/discovery.c. */
void ota_handle_trigger(void)       {}

extern void restore_user_settings(void);

static void init_fail(const char *module, esp_err_t err) {
    while (1) {
        ESP_LOGE(TAG, "[%s] init failed: %s", module, esp_err_to_name(err));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void ip_got_ip(void *arg, esp_event_base_t base, int32_t id, void *data) {
    (void)arg; (void)base; (void)id; (void)data;
    static bool mdns_started = false;
    /* No SNTP here: Fireside is offline-first and the wall clock comes from
     * Bearing (GNSS → MQTT local/gps/time). MQTT's TLS handshake works with
     * an unset RTC because CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY and the
     * absence of CONFIG_MBEDTLS_HAVE_TIME_DATE together disable X.509
     * validity-date checks — so we don't need a pre-connection time source
     * to bootstrap MQTT. If cert validity checking is ever re-enabled,
     * re-introduce a time source (SNTP or a build-time epoch floor) here. */
    if (!mdns_started) {
        /* mDNS resolver needs netif to be up — first IP is the earliest
         * safe point to start it. Advertising the _trailcurrent service is
         * deferred until discovery_handle_trigger() fires (via MQTT). */
        discovery_mdns_init();
        mdns_started = true;
    }
    if (fireside_config_has_mqtt()) {
        mqtt_client_load_settings();
        mqtt_client_connect();
    }
}

void app_main(void) {
    esp_err_t err;
    ESP_LOGI(TAG, "Waveshare P4-7B boot (UI %s, WiFi LAST)",
             UI_EXPORT_PRESENT ? "present" : "MISSING");

    /* 1. NVS + fireside_config + alarms config. */
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    ESP_ERROR_CHECK(fireside_config_init());
    /* Alarms module reads per-mode arm config from NVS ns "fireside_alarm".
     * Empty on first boot — no alarms fire until user configures via
     * Settings > Alarms. */
    extern void alarms_init(void);
    alarms_init();

    /* 2. Default event loop (WiFi needs it; the BSP doesn't create one). */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* 3. I2C bus — shared between GT911 touch and ES8311 codec. Waveshare
     *    BSP's own touch/codec init calls this too; it's idempotent. */
    ESP_ERROR_CHECK(bsp_i2c_init());

    /* 4. Battery — ADC on GPIO 20, safe to fail. */
    err = battery_init();
    if (err != ESP_OK) ESP_LOGW(TAG, "battery_init: %s", esp_err_to_name(err));

    /* 5. Display + LVGL + touch — all in one BSP call.
     *
     *    task_affinity 1 pins the LVGL port task to core 1 so WiFi/MQTT
     *    (default core 0) don't share the framebuffer cadence. Draw buffer
     *    100 lines * 1024 px = 100 KB from PSRAM; double-buffered. sw_rotate
     *    stays true so we can rotate 180° for enclosures where the display
     *    is mounted upside-down. */
    bsp_display_cfg_t disp_cfg = {
        .lvgl_port_cfg = {
            .task_priority     = configMAX_PRIORITIES - 4,
            .task_stack        = 8192 * 2,
            .task_affinity     = 1,
            .task_max_sleep_ms = 10,
            .task_stack_caps   = MALLOC_CAP_INTERNAL | MALLOC_CAP_DEFAULT,
            .timer_period_ms   = 5,
        },
        .buffer_size   = BSP_LCD_H_RES * 100,
        .double_buffer = true,
        .flags = {
            .buff_dma    = false,
            .buff_spiram = true,
            .sw_rotate   = true,
        },
    };
    lv_display_t *disp = bsp_display_start_with_config(&disp_cfg);
    if (!disp) init_fail("display", ESP_FAIL);
    /* Rotate 180°. The Waveshare 7B ships with the display connector on
     * the top edge; when the board is mounted in an enclosure with the
     * MicroSD slot / USB ports facing DOWN (matches the CAD orientation
     * used by every other TrailCurrent product), the panel physically
     * ends up upside-down and the UI has to compensate. sw_rotate=true in
     * disp_cfg above is the prerequisite; the BSP calls
     * lv_disp_set_rotation under the hood so LVGL does the rotate on
     * every flush. If you're bench-testing bare-board with the panel
     * connector at the bottom instead, change this to
     * LV_DISPLAY_ROTATION_0. */
    bsp_display_rotate(disp, LV_DISPLAY_ROTATION_180);
    /* Match Fireside boot brightness (80 %) — BSP defaults to on/100. */
    set_lcd_blight(80);

#if UI_EXPORT_PRESENT
    /* 6. ui_init under LVGL lock, land on page_home. */
    if (bsp_display_lock(0)) {
        ui_init();
        set_var_rotation_degrees(0);
        const esp_app_desc_t *app = esp_app_get_description();
        if (app && objects.label_version_number) {
            lv_label_set_text(objects.label_version_number, app->version);
        }
        restore_user_settings();
        button_config_init();
        button_config_apply_to_ui();
        if (objects.page_home) lv_scr_load(objects.page_home);
        init_clock_blink();
        init_metric_charts();
        init_wifi_rssi_poll();
        init_battery_poll();
        init_notif_icon_ack_taps();
        init_touch_target_hit_areas();
        {
            extern void perf_init(void);
            perf_init();
        }
        init_screen_timeout();
        {
            extern void audio_init(void);
            audio_init();
            /* Push-to-talk voice terminal — spawns its own worker task,
             * bound to the TALK button on PageHome via
             * action_ptt_pressed / action_ptt_released. Safe to call
             * after audio_init since it uses the same BSP codec paths. */
            peregrine_voice_init();
            /* Overlay Peregrine URL + token from /sdcard/environment.conf
             * on top of the Kconfig defaults. Silently no-ops when the SD
             * card isn't present or the file isn't there. */
            sd_config_load();
            /* No card / no Peregrine keys -> nothing for push-to-talk to
             * talk to, so the PageHome TALK button stays hidden rather
             * than sitting there dead. Still inside the display lock and
             * before the first flush, so it never flashes into view. */
            extern void apply_ptt_availability(bool available);
            apply_ptt_availability(sd_config_peregrine_present());
            /* Pulsating "thinking" ring while an utterance is in flight. */
            extern void init_ptt_glow(void);
            init_ptt_glow();
        }
        reset_placeholders();
        {
            extern void init_mqtt_setup_bindings(void);
            init_mqtt_setup_bindings();
            extern void init_button_edit_bindings(void);
            init_button_edit_bindings();
        }
        bsp_display_unlock();
    }
#endif

    /* 7. WiFi bring-up. Waveshare uses ESP-Hosted defaults (no pin overrides
     *    in sdkconfig.defaults.esp32p4), so this is straightforward. */
    ESP_ERROR_CHECK(esp_netif_init());
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

        /* Arm the C6 heartbeat monitor + auto-restart recovery. */
        wifi_health_init();

        /* Let C6 slave settle before scan/connect. */
        vTaskDelay(pdMS_TO_TICKS(2000));

        uint8_t mac[6] = {0};
        if (esp_wifi_get_mac(WIFI_IF_STA, mac) == ESP_OK) {
            ESP_LOGI(TAG, "STA MAC = %02X:%02X:%02X:%02X:%02X:%02X",
                     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#if UI_EXPORT_PRESENT
            if (bsp_display_lock(0)) {
                char macbuf[18];
                snprintf(macbuf, sizeof(macbuf),
                         "%02X:%02X:%02X:%02X:%02X:%02X",
                         mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                set_var_mcu_mac_address(macbuf);
                bsp_display_unlock();
            }
#endif
        }

#if UI_EXPORT_PRESENT
        /* 8. State machine + wifi_setup + auto-connect. */
        ESP_ERROR_CHECK(app_state_init());
        app_state_start_wifi();
#endif
        /* Discovery — armed here so the MQTT subscription (in mqtt_client.c)
         * has a real handler once WiFi/MQTT come up. The mDNS resolver and
         * per-request advert are started later (see ip_got_ip and
         * discovery_task_fn). */
        discovery_init();
    }

    ESP_LOGI(TAG, "Setup done — entering main loop");

    uint32_t last_tick = 0;
    while (1) {
#if UI_EXPORT_PRESENT
        /* MQTT dispatch runs on its own FreeRTOS task ("mqtt_dispatch");
         * this call is now a no-op left as a hook. Watchdogs stay here. */
        mqtt_client_check_watchdogs();

        uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
        if (now - last_tick >= 1000) {
            last_tick = now;
            if (bsp_display_lock(0)) {
                update_clock_display();
                bsp_display_unlock();
            }
        }
#endif
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
