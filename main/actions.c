/*
 * actions.c — EEZ-declared action handlers.
 *
 * Every action in `.eez-project`'s actions[] array produces an
 * `extern void action_<Name>(lv_event_t *e);` in `main/ui/actions.h`. The
 * project won't link until this file provides an implementation for each of
 * the 26 declared actions. Handlers dispatch to app_state, mqtt_vars, and
 * (Phase 2) to the mqtt_client + wifi_setup components.
 *
 * All action handlers run on the LVGL task with the display lock held.
 * Anything that would touch MQTT or NVS gets bounced onto a background task
 * (Phase 2 wiring: use xTaskCreate + FreeRTOS notifications). For Phase 1
 * we log the action and update local UI state only.
 */
#include "esp_log.h"
#include "lvgl.h"

#if __has_include("ui/screens.h")
#include "screens.h"
#include "actions.h"
#include "ui.h"
#include "vars.h"
#endif

#include "app_state.h"
#include "mqtt_vars.h"

#include "fireside_config.h"
#include "wifi_setup.h"
#include "app_mqtt.h"
#include "button_config.h"
#include "peregrine_voice.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const char *TAG = "ACT";

/* Track the SSID the user tapped on PageWifiSetup so
 * action_wifi_submit_password knows which network to connect to. */
static char s_selected_ssid[33] = {0};
static bool s_selected_locked = false;

/* ============================================================
 * Navigation
 * ============================================================ */

#if __has_include("ui/screens.h")
/* BottomNav is a user widget instanced on every page. Each instance's
 * children get flattened into `objects` with the prefix `<page>_nav__`,
 * so `objects.home_nav__nav_home` is the Home tab button rendered on the
 * Home page, `objects.trailer_nav__nav_home` is the same tab button on
 * the Trailer page, etc.
 *
 * When navigating, we (1) update the CHECKED state on the six nav buttons
 * of the DESTINATION page's nav bar so its highlighted tab matches the
 * page the user is looking at, and (2) call lv_scr_load. */

typedef enum { TAB_HOME=0, TAB_TRAILER, TAB_POWER, TAB_WATER, TAB_AIR, TAB_SETTINGS, TAB_COUNT } tab_id_t;

/* [page_index][tab_index] → button object. Row order MUST match the
 * `PAGE_SCREENS` array below and NAV_TABS in the generator. */
static lv_obj_t *nav_btn(int page, int tab) {
    switch (page) {
    case TAB_HOME:
        switch (tab) {
        case TAB_HOME:     return objects.home_nav__nav_home;
        case TAB_TRAILER:  return objects.home_nav__nav_trailer;
        case TAB_POWER:    return objects.home_nav__nav_power;
        case TAB_WATER:    return objects.home_nav__nav_water;
        case TAB_AIR:      return objects.home_nav__nav_air;
        case TAB_SETTINGS: return objects.home_nav__nav_settings;
        } break;
    case TAB_TRAILER:
        switch (tab) {
        case TAB_HOME:     return objects.trailer_nav__nav_home;
        case TAB_TRAILER:  return objects.trailer_nav__nav_trailer;
        case TAB_POWER:    return objects.trailer_nav__nav_power;
        case TAB_WATER:    return objects.trailer_nav__nav_water;
        case TAB_AIR:      return objects.trailer_nav__nav_air;
        case TAB_SETTINGS: return objects.trailer_nav__nav_settings;
        } break;
    case TAB_POWER:
        switch (tab) {
        case TAB_HOME:     return objects.power_nav__nav_home;
        case TAB_TRAILER:  return objects.power_nav__nav_trailer;
        case TAB_POWER:    return objects.power_nav__nav_power;
        case TAB_WATER:    return objects.power_nav__nav_water;
        case TAB_AIR:      return objects.power_nav__nav_air;
        case TAB_SETTINGS: return objects.power_nav__nav_settings;
        } break;
    case TAB_WATER:
        switch (tab) {
        case TAB_HOME:     return objects.water_nav__nav_home;
        case TAB_TRAILER:  return objects.water_nav__nav_trailer;
        case TAB_POWER:    return objects.water_nav__nav_power;
        case TAB_WATER:    return objects.water_nav__nav_water;
        case TAB_AIR:      return objects.water_nav__nav_air;
        case TAB_SETTINGS: return objects.water_nav__nav_settings;
        } break;
    case TAB_AIR:
        switch (tab) {
        case TAB_HOME:     return objects.air_nav__nav_home;
        case TAB_TRAILER:  return objects.air_nav__nav_trailer;
        case TAB_POWER:    return objects.air_nav__nav_power;
        case TAB_WATER:    return objects.air_nav__nav_water;
        case TAB_AIR:      return objects.air_nav__nav_air;
        case TAB_SETTINGS: return objects.air_nav__nav_settings;
        } break;
    case TAB_SETTINGS:
        switch (tab) {
        case TAB_HOME:     return objects.settings_nav__nav_home;
        case TAB_TRAILER:  return objects.settings_nav__nav_trailer;
        case TAB_POWER:    return objects.settings_nav__nav_power;
        case TAB_WATER:    return objects.settings_nav__nav_water;
        case TAB_AIR:      return objects.settings_nav__nav_air;
        case TAB_SETTINGS: return objects.settings_nav__nav_settings;
        } break;
    }
    return NULL;
}

static void set_active_tab(int page) {
    for (int t = 0; t < TAB_COUNT; t++) {
        lv_obj_t *b = nav_btn(page, t);
        if (!b) continue;
        if (t == page) lv_obj_add_state(b, LV_STATE_CHECKED);
        else           lv_obj_clear_state(b, LV_STATE_CHECKED);
    }
}

static void nav_to(lv_obj_t *scr, const char *name, int tab_id) {
    if (!scr) { ESP_LOGW(TAG, "nav %s: page not exported yet", name); return; }
    set_active_tab(tab_id);
    lv_scr_load(scr);
}
#endif

void action_nav_home(lv_event_t *e)     { (void)e;
#if __has_include("ui/screens.h")
    nav_to(objects.page_home, "home", TAB_HOME);
#endif
}
void action_nav_trailer(lv_event_t *e)  { (void)e;
#if __has_include("ui/screens.h")
    nav_to(objects.page_trailer, "trailer", TAB_TRAILER);
#endif
}
void action_nav_power(lv_event_t *e)    { (void)e;
#if __has_include("ui/screens.h")
    nav_to(objects.page_power, "power", TAB_POWER);
#endif
}
void action_nav_water(lv_event_t *e)    { (void)e;
#if __has_include("ui/screens.h")
    nav_to(objects.page_water, "water", TAB_WATER);
#endif
}
void action_nav_air(lv_event_t *e)      { (void)e;
#if __has_include("ui/screens.h")
    nav_to(objects.page_air, "air", TAB_AIR);
#endif
}
void action_nav_settings(lv_event_t *e) { (void)e;
#if __has_include("ui/screens.h")
    nav_to(objects.page_settings, "settings", TAB_SETTINGS);
#endif
}

/* ============================================================
 * Theme toggle — Light/Dark. Persisted in NVS ("theme").
 * EEZ Studio-authored themes switch via lv_theme_default_init.
 * ============================================================ */

static bool s_dark = false;

void action_toggle_theme(lv_event_t *e) {
    (void)e;
    s_dark = !s_dark;
#if __has_include("ui/screens.h")
    /* EEZ Studio's C export emits change_color_theme(idx) which rewrites
     * every named style with the new theme's colors and refreshes all
     * widgets. Theme 0 = Default (light), Theme 1 = Dark (matching my
     * themes[] array in the .eez-project). */
    change_color_theme(s_dark ? 1 : 0);
#endif
    nvs_handle_t nvs;
    if (nvs_open("fireside", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_i32(nvs, "theme", s_dark ? 1 : 0);
        nvs_commit(nvs);
        nvs_close(nvs);
    }
    ESP_LOGI(TAG, "theme = %s", s_dark ? "dark" : "light");
}

/* ============================================================
 * Device toggle — 8 home-screen tiles, userData carries index 0..7
 * ============================================================ */

/* Publish a CAN toggle envelope to Headwaters' can/outbound topic.
 * Matches the shape of Spotter's device_alarms_send_toggle and
 * Headwaters' publishCanMessage — 8 sub-arrays of 8 bits each, MSB
 * first. Byte 0 carries the channel index; bytes 1..7 are zero. Base
 * CAN IDs match the Torrent / Switchback toggle IDs already declared
 * in button_config.c (TORRENT_TOGGLE_ID[], SWITCHBACK_TOGGLE_ID[]). */
static void publish_can_toggle(uint32_t can_id, uint8_t channel) {
    char payload[640];
    int n = snprintf(payload, sizeof(payload),
        "{\"identifier\":\"0x%x\",\"data_length_code\":1,\"data\":["
        "[%d,%d,%d,%d,%d,%d,%d,%d],"
        "[0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0],"
        "[0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0],"
        "[0,0,0,0,0,0,0,0]"
        "],\"extd\":0,\"rtr\":0,\"ss\":0,\"self\":0}",
        (unsigned)can_id,
        (channel >> 7) & 1, (channel >> 6) & 1, (channel >> 5) & 1,
        (channel >> 4) & 1, (channel >> 3) & 1, (channel >> 2) & 1,
        (channel >> 1) & 1, (channel >> 0) & 1);
    if (n < 0 || n >= (int)sizeof(payload)) {
        ESP_LOGE(TAG, "publish_can_toggle: payload format failed n=%d", n);
        return;
    }
    mqtt_client_publish("can/outbound", payload, 0);
    ESP_LOGI(TAG, "can/outbound id=0x%x ch=%u", (unsigned)can_id,
             (unsigned)channel);
}

void action_toggle_device(lv_event_t *e) {
    intptr_t idx = (intptr_t)lv_event_get_user_data(e);
    if (idx < 0 || idx > 7) return;

    const btn_config_t *b = &g_buttons[idx];

    /* Configured tile → send the CAN toggle envelope. Headwaters relays it
     * to the physical CAN bus; the module then publishes its new state on
     * local/lights/{id}/status or local/relays/{id}/status, which routes
     * back through apply_module_status → set_var_deviceNN_status →
     * flips the CHECKED state on this tile. */
    if (b->module_type == MOD_TORRENT && b->instance <= 2) {
        publish_can_toggle(TORRENT_TOGGLE_ID[b->instance], b->channel);
        return;
    }
    if (b->module_type == MOD_SWITCHBACK && b->instance <= 2) {
        publish_can_toggle(SWITCHBACK_TOGGLE_ID[b->instance], b->channel);
        return;
    }

    /* Unconfigured tile — no CAN module bound. Do NOT flip local UI
     * state: a button's visible state must ONLY reflect the last MQTT
     * status message for the device it's bound to. Faking a toggle
     * here would leave the tile "on" indefinitely (no MQTT status
     * will ever arrive to correct it) and lie to the user about
     * device state.
     *
     * Log-only. When the user visits Settings > Edit Buttons and
     * binds this slot to a module, subsequent taps will publish CAN
     * toggles and the UI will update from the status roundtrip. */
    ESP_LOGI(TAG, "device %ld tap ignored — button unconfigured "
             "(Settings > Edit Buttons to bind)",
             (long)(idx + 1));
}

/* ============================================================
 * Temperature unit — F / C
 * ============================================================ */

void action_set_temp_unit_f(lv_event_t *e) { (void)e; set_var_temperature_unit(0);
#if __has_include("ui/screens.h")
    if (objects.settings_f_btn) lv_obj_add_state(objects.settings_f_btn, LV_STATE_CHECKED);
    if (objects.settings_c_btn) lv_obj_clear_state(objects.settings_c_btn, LV_STATE_CHECKED);
#endif
    nvs_handle_t nvs;
    if (nvs_open("fireside", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_i32(nvs, "tempunit", 0);
        nvs_commit(nvs); nvs_close(nvs);
    }
}
void action_set_temp_unit_c(lv_event_t *e) { (void)e; set_var_temperature_unit(1);
#if __has_include("ui/screens.h")
    if (objects.settings_c_btn) lv_obj_add_state(objects.settings_c_btn, LV_STATE_CHECKED);
    if (objects.settings_f_btn) lv_obj_clear_state(objects.settings_f_btn, LV_STATE_CHECKED);
#endif
    nvs_handle_t nvs;
    if (nvs_open("fireside", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_i32(nvs, "tempunit", 1);
        nvs_commit(nvs); nvs_close(nvs);
    }
}

/* ============================================================
 * Power / Air variant toggles + metric hero selection
 * Variant pills were removed from the JSON (only Variant A / Sparks was
 * ever implemented). Handlers stay as no-op stubs so the EEZ export's
 * extern declarations still resolve at link time — if the variant UI
 * ever comes back, put the seg_check() logic here.
 * ============================================================ */

void action_set_power_variant(lv_event_t *e) {
    intptr_t idx = (intptr_t)lv_event_get_user_data(e);
    (void)idx;
}
void action_set_air_variant(lv_event_t *e) {
    intptr_t idx = (intptr_t)lv_event_get_user_data(e);
    (void)idx;
}
void action_select_power_metric(lv_event_t *e) {
    intptr_t idx = (intptr_t)lv_event_get_user_data(e);
    ESP_LOGI(TAG, "power metric -> %ld", (long)idx);
    /* Phase 2: repaint hero chart from metric_history_get(). */
}
void action_select_air_metric(lv_event_t *e) {
    intptr_t idx = (intptr_t)lv_event_get_user_data(e);
    ESP_LOGI(TAG, "air metric -> %ld", (long)idx);
}

/* ============================================================
 * Water pump toggle
 * ============================================================ */

void action_toggle_pump(lv_event_t *e) {
    (void)e;
    /* Same rule as home device tiles: the button's visible state must
     * only reflect MQTT status coming back for the pump — not
     * optimistically flip on tap. Phase 2 wires the actual publish
     * (mqtt_client_publish("local/water/pump/set", ...)) and a matching
     * status subscription that calls into a set_var_pump_status setter
     * in vars.c. Until then this is a no-op with a log so the user
     * doesn't see a fake state change. */
    ESP_LOGI(TAG, "pump tap ignored — publish/status roundtrip not yet wired");
}

/* ============================================================
 * Settings — brightness / timeout / timezone / reset connection
 * ============================================================ */

void action_brightness_changed(lv_event_t *e) {
    (void)e;
#if __has_include("ui/screens.h")
    if (!objects.slider_screen_brightness) return;
    int32_t v = lv_slider_get_value(objects.slider_screen_brightness);
    if (v < 10) v = 10;
    set_var_screen_brightness(v);
    if (objects.settings_brightness_pct) {
        char buf[16]; snprintf(buf, sizeof(buf), "%ld%%", (long)v);
        lv_label_set_text(objects.settings_brightness_pct, buf);
    }
    /* Waveshare BSP backlight PWM (0..100). */
    extern esp_err_t set_lcd_blight(uint32_t brightness);
    set_lcd_blight((uint32_t)v);
    nvs_handle_t nvs;
    if (nvs_open("fireside", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_i32(nvs, "brightness", v);
        nvs_commit(nvs); nvs_close(nvs);
    }
#endif
}

#include "audio.h"

void action_volume_changed(lv_event_t *e) {
    (void)e;
#if __has_include("ui/screens.h")
    if (!objects.slider_speaker_volume) return;
    int32_t v = lv_slider_get_value(objects.slider_speaker_volume);
    if (v < 0)   v = 0;
    if (v > 100) v = 100;
    audio_set_volume((uint8_t)v);
    if (objects.settings_volume_pct) {
        char buf[16]; snprintf(buf, sizeof(buf), "%ld%%", (long)v);
        lv_label_set_text(objects.settings_volume_pct, buf);
    }
    nvs_handle_t nvs;
    if (nvs_open("fireside", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_i32(nvs, "volume", v);
        nvs_commit(nvs); nvs_close(nvs);
    }
#endif
}

/* paint_screen_timeout_label lives in vars.c (next to s_timeout_min). */
extern void paint_screen_timeout_label(void);

void action_timeout_up(lv_event_t *e)   { (void)e;
    int32_t v = get_var_screen_timeout_minutes(); if (v < 60) v++;
    set_var_screen_timeout_minutes(v);
    paint_screen_timeout_label();
    nvs_handle_t nvs;
    if (nvs_open("fireside", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_i32(nvs, "timeout", v);
        nvs_commit(nvs); nvs_close(nvs);
    }
}
void action_timeout_down(lv_event_t *e) { (void)e;
    int32_t v = get_var_screen_timeout_minutes(); if (v > 0) v--;
    set_var_screen_timeout_minutes(v);
    paint_screen_timeout_label();
    nvs_handle_t nvs;
    if (nvs_open("fireside", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_i32(nvs, "timeout", v);
        nvs_commit(nvs); nvs_close(nvs);
    }
}

void action_set_timezone(lv_event_t *e) {
    (void)e;
#if __has_include("ui/screens.h")
    if (!objects.settings_timezone_dd) return;
    uint16_t idx = lv_dropdown_get_selected(objects.settings_timezone_dd);
    char sel[32] = {0};
    lv_dropdown_get_selected_str(objects.settings_timezone_dd, sel, sizeof(sel));
    bool applied = apply_timezone(sel);
    nvs_handle_t nvs;
    if (nvs_open("fireside", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_str(nvs, "tz", sel);
        nvs_commit(nvs); nvs_close(nvs);
    }
    ESP_LOGI(TAG, "timezone -> %s (idx=%u)%s", sel, (unsigned)idx,
             applied ? "" : " [unmapped, TZ unchanged]");
#endif
}

void action_reset_connection(lv_event_t *e) {
    (void)e;
    app_state_reset_connection_and_reenter();
}

/* ============================================================
 * Alarms wizard — mode picker + battery threshold on PageAlarms.
 * Per-sensor arming (Phase 2b) will land here too.
 * ============================================================ */

#include "alarms.h"

/* On PageAlarms open: paint the current arm-config state into the switch
 * + slider from NVS so they reflect the saved values. */
static void alarms_paint_state(void) {
#if __has_include("ui/screens.h")
    nvs_handle_t nvs;
    if (nvs_open("fireside_alarm", NVS_READONLY, &nvs) == ESP_OK) {
        uint8_t en = 0;
        nvs_get_u8(nvs, "batt_en", &en);
        uint8_t thr = 20;
        nvs_get_u8(nvs, "batt_thr", &thr);
        nvs_close(nvs);
        if (objects.alarms_bat_switch) {
            if (en) lv_obj_add_state(objects.alarms_bat_switch, LV_STATE_CHECKED);
            else    lv_obj_clear_state(objects.alarms_bat_switch, LV_STATE_CHECKED);
        }
        if (objects.alarms_bat_slider)
            lv_slider_set_value(objects.alarms_bat_slider, thr, LV_ANIM_OFF);
        if (objects.alarms_bat_current) {
            char buf[8]; snprintf(buf, sizeof(buf), "%u%%", (unsigned)thr);
            lv_label_set_text(objects.alarms_bat_current, buf);
        }
    }
#endif
}

/* ---- Re-alert snooze row (dynamic — not authored in EEZ) --------------
 *
 * Adds a "Re-alert every N seconds" slider row at the bottom of the
 * sensor arming card. The row is rebuilt each time PageAlarms opens so
 * it always shows the current alarms.c value; the sensor list above is
 * shrunk by SNOOZE_ROW_HEIGHT so nothing overlaps.
 *
 * The label pointer is stashed in a file-static so the slider callback
 * can repaint the current-value text without walking children. Both
 * widgets are children of alarms_sensor_card and get destroyed with
 * the card on screen tear-down.
 *
 * Why this lives here (not in .eez-project): it's a small runtime
 * control that Fireside adds on top of the existing EEZ-authored
 * PageAlarms. Shipping this without a full EEZ Studio rebuild lets us
 * ship the alarm-persistence fix without touching the canvas — the
 * sensor list is authored at 238 px tall, resized to 188 px at open
 * time to make room. The 50 px gap → snooze row. */
#if __has_include("ui/screens.h")
#define SNOOZE_ROW_HEIGHT   50
#define SENSOR_LIST_HEIGHT_WITH_SNOOZE  188   /* was 238; shrunk 50 for the row */

static lv_obj_t *s_snooze_current_lbl = NULL;
static lv_obj_t *s_snooze_slider      = NULL;

static void snooze_slider_cb(lv_event_t *e) {
    lv_obj_t *sl = lv_event_get_target(e);
    int32_t v = lv_slider_get_value(sl);
    alarms_set_snooze_secs((int)v);
    if (s_snooze_current_lbl) {
        char buf[24];
        int secs = alarms_get_snooze_secs();
        if (secs < 60) snprintf(buf, sizeof(buf), "%d sec", secs);
        else           snprintf(buf, sizeof(buf), "%d min %02d sec",
                                secs / 60, secs % 60);
        lv_label_set_text(s_snooze_current_lbl, buf);
    }
}

static void alarms_build_snooze_row(void) {
    lv_obj_t *card = objects.alarms_sensor_card;
    lv_obj_t *list = objects.alarms_sensor_list;
    if (!card || !list) return;

    /* Shrink the sensor list to free vertical space for the snooze row.
     * Sensor list is scrollable so nothing is lost — just fewer visible
     * rows before scrolling. Called every open (idempotent — resetting
     * to the same size is a no-op). */
    lv_obj_set_size(list, 976, SENSOR_LIST_HEIGHT_WITH_SNOOZE);

    /* Snooze row container — pinned inside the sensor card, below the
     * (now-shorter) sensor list. Rebuilt on each open. */
    static lv_obj_t *s_snooze_row = NULL;
    if (s_snooze_row) {
        lv_obj_del(s_snooze_row);
        s_snooze_row = NULL;
        s_snooze_slider = NULL;
        s_snooze_current_lbl = NULL;
    }

    s_snooze_row = lv_obj_create(card);
    lv_obj_set_size(s_snooze_row, 976, SNOOZE_ROW_HEIGHT);
    lv_obj_set_pos(s_snooze_row, 12, 56 + SENSOR_LIST_HEIGHT_WITH_SNOOZE + 4);
    lv_obj_set_style_bg_opa(s_snooze_row, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(s_snooze_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(s_snooze_row, 4, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(s_snooze_row, 0, LV_PART_MAIN);
    lv_obj_clear_flag(s_snooze_row, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(s_snooze_row);
    lv_label_set_text(title, "Re-alert while active every");
    lv_obj_set_style_text_color(title, lv_color_hex(0x1a1a1a), LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 0, -10);

    s_snooze_current_lbl = lv_label_create(s_snooze_row);
    lv_obj_set_style_text_color(s_snooze_current_lbl,
                                lv_color_hex(0x52A441), LV_PART_MAIN);
    lv_obj_align(s_snooze_current_lbl, LV_ALIGN_LEFT_MID, 240, -10);

    s_snooze_slider = lv_slider_create(s_snooze_row);
    /* Range = seconds, stepping by ALARM_SNOOZE_SECS_MIN so the tap
     * granularity matches the min. Value labeled by the current-label
     * text next to the title. */
    lv_slider_set_range(s_snooze_slider,
                        ALARM_SNOOZE_SECS_MIN, ALARM_SNOOZE_SECS_MAX);
    lv_slider_set_value(s_snooze_slider, alarms_get_snooze_secs(),
                        LV_ANIM_OFF);
    lv_obj_set_size(s_snooze_slider, 620, 10);
    lv_obj_align(s_snooze_slider, LV_ALIGN_LEFT_MID, 340, -10);
    lv_obj_add_event_cb(s_snooze_slider, snooze_slider_cb,
                        LV_EVENT_VALUE_CHANGED, NULL);
    /* Match SliderDefault (EEZ-authored style): track = BgCardHover grey,
     * indicator + knob = AccentPrimary green. Without this the runtime-
     * created slider uses LVGL's built-in theme (bright blue), which
     * clashes with the rest of the sliders on this screen. */
    lv_obj_set_style_bg_color(s_snooze_slider,
                              lv_color_hex(0xEDEDED), LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_snooze_slider,
                              lv_color_hex(0x52A441), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(s_snooze_slider,
                              lv_color_hex(0x52A441), LV_PART_KNOB);

    /* Sync label text on build. */
    {
        char buf[24];
        int secs = alarms_get_snooze_secs();
        if (secs < 60) snprintf(buf, sizeof(buf), "%d sec", secs);
        else           snprintf(buf, sizeof(buf), "%d min %02d sec",
                                secs / 60, secs % 60);
        lv_label_set_text(s_snooze_current_lbl, buf);
    }
}
#endif

/* Populate the alarms_sensor_list container with one row per known sensor.
 * Called from action_open_alarms every time the user enters PageAlarms —
 * cleans + rebuilds so freshly-discovered boards appear. */
#if __has_include("ui/screens.h")

typedef struct { alarm_src_t src; uint8_t addr; uint8_t sensor; } sensor_key_t;

static void sensor_switch_cb(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    sensor_key_t *k = (sensor_key_t *)lv_event_get_user_data(e);
    if (!k || !sw) return;
    bool armed = lv_obj_has_state(sw, LV_STATE_CHECKED);
    alarms_set_armed(k->src, k->addr, k->sensor, armed);
}

/* --- Rename modal ------------------------------------------------------
 *
 * Full-screen overlay + centered panel + textarea + keyboard. Built in C
 * as a child of the active screen (page_alarms) rather than authored in
 * the .eez-project — modals don't touch canvas-device parity because
 * there's no `objects.rename_modal` for EEZ Studio to render differently.
 *
 * State is stashed on the modal root's user_data as a sensor_key_t so the
 * Save handler knows which sensor to update. Cancel/Save both delete the
 * modal + repopulate the sensor list so the row shows the new label. */

typedef struct {
    sensor_key_t key;
    lv_obj_t    *textarea;
    lv_obj_t    *label_row;   /* the label widget in the source row, so
                               * Save can repaint it without a full list
                               * rebuild (avoids scrolling reset). */
} rename_ctx_t;

static void alarms_ui_populate(void);   /* forward decl */

static void rename_close(lv_obj_t *modal_root) {
    rename_ctx_t *ctx = (rename_ctx_t *)lv_obj_get_user_data(modal_root);
    if (ctx) free(ctx);
    lv_obj_del(modal_root);
}

static void rename_cancel_cb(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *modal_root = (lv_obj_t *)lv_event_get_user_data(e);
    (void)btn;
    if (modal_root) rename_close(modal_root);
}

static void rename_save_cb(lv_event_t *e) {
    lv_obj_t *modal_root = (lv_obj_t *)lv_event_get_user_data(e);
    if (!modal_root) return;
    rename_ctx_t *ctx = (rename_ctx_t *)lv_obj_get_user_data(modal_root);
    if (!ctx || !ctx->textarea) { rename_close(modal_root); return; }

    const char *txt = lv_textarea_get_text(ctx->textarea);
    /* Empty text clears the custom label — alarms_set_label with NULL/""
     * erases the NVS key so the row falls back to the default "SB0 · S3"
     * rendering next tick. That's the intended way to "unname" a sensor. */
    alarms_set_label(ctx->key.src, ctx->key.addr, ctx->key.sensor, txt);

    /* Repaint just this row's label instead of rebuilding the list —
     * keeps the scroll position where the user tapped. */
    if (ctx->label_row) {
        char buf[ALARM_LABEL_MAX];
        alarms_get_label(ctx->key.src, ctx->key.addr, ctx->key.sensor,
                         buf, sizeof(buf));
        lv_label_set_text(ctx->label_row, buf);
    }
    rename_close(modal_root);
}

/* Keyboard READY event (Enter/Ok key) = same as pressing Save. Both
 * callbacks pull `modal_root` out of the event's user_data, so we can
 * forward the real event directly rather than fabricating one. */
static void rename_kb_ready_cb(lv_event_t *e) {
    rename_save_cb(e);
}

static void alarms_open_rename_modal(alarm_src_t src, uint8_t addr,
                                     uint8_t sensor, lv_obj_t *row_label) {
    lv_obj_t *scr = lv_scr_act();
    if (!scr) return;

    /* Full-screen dim scrim — swallows touches so nothing behind is
     * accidentally toggled. */
    lv_obj_t *root = lv_obj_create(scr);
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(root, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(root, LV_OPA_60, LV_PART_MAIN);
    lv_obj_set_style_border_width(root, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(root, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(root, 0, LV_PART_MAIN);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(root, LV_OBJ_FLAG_CLICKABLE);

    /* Center card holding the textarea + buttons. Positioned above where
     * the keyboard will land (keyboard docks bottom, so keep card in the
     * top half). */
    lv_obj_t *card = lv_obj_create(root);
    lv_obj_set_size(card, 640, 180);
    lv_obj_align(card, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_set_style_bg_color(card, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_radius(card, 10, LV_PART_MAIN);
    lv_obj_set_style_border_color(card, lv_color_hex(0xc8c8c8), LV_PART_MAIN);
    lv_obj_set_style_border_width(card, 1, LV_PART_MAIN);
    lv_obj_set_style_pad_all(card, 16, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(card, 18, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(card, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(card, LV_OPA_20, LV_PART_MAIN);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(card);
    lv_label_set_text(title, "Rename sensor");
    lv_obj_set_style_text_color(title, lv_color_hex(0x1a1a1a), LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *hint = lv_label_create(card);
    lv_label_set_text(hint,
        "Name shown in the alarms list. Words like 'cabinet', 'door', "
        "'window', 'fridge' pick the matching TTS phrase.");
    lv_label_set_long_mode(hint, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(hint, 608);
    lv_obj_set_style_text_color(hint, lv_color_hex(0x666666), LV_PART_MAIN);
    lv_obj_align(hint, LV_ALIGN_TOP_LEFT, 0, 26);

    lv_obj_t *ta = lv_textarea_create(card);
    lv_obj_set_size(ta, 608, 44);
    lv_textarea_set_one_line(ta, true);
    lv_textarea_set_max_length(ta, ALARM_LABEL_MAX - 1);
    lv_textarea_set_placeholder_text(ta, "e.g. Kitchen cabinet");
    /* Pre-fill with the current label (custom or default). Empty entries
     * clear the custom name. */
    char cur[ALARM_LABEL_MAX];
    alarms_get_label(src, addr, sensor, cur, sizeof(cur));
    lv_textarea_set_text(ta, cur);
    lv_obj_align(ta, LV_ALIGN_TOP_LEFT, 0, 76);

    lv_obj_t *btn_cancel = lv_btn_create(card);
    lv_obj_set_size(btn_cancel, 100, 36);
    lv_obj_align(btn_cancel, LV_ALIGN_BOTTOM_RIGHT, -110, 0);
    lv_obj_set_style_bg_color(btn_cancel, lv_color_hex(0xededed), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn_cancel, 0, LV_PART_MAIN);
    lv_obj_t *lbl_c = lv_label_create(btn_cancel);
    lv_label_set_text(lbl_c, "Cancel");
    lv_obj_set_style_text_color(lbl_c, lv_color_hex(0x1a1a1a), LV_PART_MAIN);
    lv_obj_center(lbl_c);

    lv_obj_t *btn_save = lv_btn_create(card);
    lv_obj_set_size(btn_save, 100, 36);
    lv_obj_align(btn_save, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_bg_color(btn_save, lv_color_hex(0x52A441), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn_save, 0, LV_PART_MAIN);
    lv_obj_t *lbl_s = lv_label_create(btn_save);
    lv_label_set_text(lbl_s, "Save");
    lv_obj_set_style_text_color(lbl_s, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_center(lbl_s);

    /* Keyboard docked at bottom, full width. */
    lv_obj_t *kb = lv_keyboard_create(root);
    lv_obj_set_size(kb, LV_PCT(100), 220);
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_keyboard_set_textarea(kb, ta);

    /* The theme cascades rr14 (Roboto) onto every child, but Roboto is
     * ASCII-only. Every LVGL LV_SYMBOL_* on the keyboard (backspace,
     * shift, keyboard, OK) is a codepoint in the built-in Montserrat 14
     * subset, so pin the buttonmatrix items to that font or they render
     * as rectangles. */
    lv_obj_set_style_text_font(kb, &lv_font_montserrat_14,
                               LV_PART_ITEMS);

    /* LVGL's default keyboard maps use three symbols that AREN'T in the
     * Montserrat 14 built-in subset: LV_SYMBOL_LEFT (0xF060), _RIGHT
     * (0xF061), and _NEW_LINE (0xF149). They render as rectangles.
     * Override the lc/uc/spec/num maps to drop LEFT+RIGHT (bottom-row
     * cursor nav — user can still tap in the textarea to place the
     * caret) and replace NEW_LINE with LV_SYMBOL_OK (both submit — the
     * textarea is single-line so newline has no semantic meaning). */
    static const char *fireside_kb_map_lc[] = {
        "1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", LV_SYMBOL_BACKSPACE, "\n",
        "ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", LV_SYMBOL_OK, "\n",
        "_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
        LV_SYMBOL_KEYBOARD, " ", LV_SYMBOL_OK, ""
    };
    static const lv_btnmatrix_ctrl_t fireside_kb_ctrl_lc[] = {
        LV_KEYBOARD_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, LV_BTNMATRIX_CTRL_CHECKED | 7,
        LV_KEYBOARD_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, LV_BTNMATRIX_CTRL_CHECKED | 7,
        LV_BTNMATRIX_CTRL_CHECKED | 1, LV_BTNMATRIX_CTRL_CHECKED | 1, 1, 1, 1, 1, 1, 1, 1, LV_BTNMATRIX_CTRL_CHECKED | 1, LV_BTNMATRIX_CTRL_CHECKED | 1, LV_BTNMATRIX_CTRL_CHECKED | 1,
        LV_KEYBOARD_CTRL_BTN_FLAGS | 2, 8, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
    };
    static const char *fireside_kb_map_uc[] = {
        "1#", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", LV_SYMBOL_BACKSPACE, "\n",
        "abc", "A", "S", "D", "F", "G", "H", "J", "K", "L", LV_SYMBOL_OK, "\n",
        "_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
        LV_SYMBOL_KEYBOARD, " ", LV_SYMBOL_OK, ""
    };
    static const char *fireside_kb_map_spec[] = {
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", LV_SYMBOL_BACKSPACE, "\n",
        "abc", "+", "&", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
        "\\",  "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
        LV_SYMBOL_KEYBOARD, " ", LV_SYMBOL_OK, ""
    };
    static const lv_btnmatrix_ctrl_t fireside_kb_ctrl_spec[] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, LV_BTNMATRIX_CTRL_CHECKED | 2,
        LV_KEYBOARD_CTRL_BTN_FLAGS | 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        LV_KEYBOARD_CTRL_BTN_FLAGS | 2, 8, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
    };
    static const char *fireside_kb_map_num[] = {
        "1", "2", "3", LV_SYMBOL_KEYBOARD, "\n",
        "4", "5", "6", LV_SYMBOL_OK, "\n",
        "7", "8", "9", LV_SYMBOL_BACKSPACE, "\n",
        "+/-", "0", ".", ""
    };
    static const lv_btnmatrix_ctrl_t fireside_kb_ctrl_num[] = {
        1, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
        1, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
        1, 1, 1, 2,
        1, 1, 1
    };
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_TEXT_LOWER,
                        fireside_kb_map_lc, fireside_kb_ctrl_lc);
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_TEXT_UPPER,
                        fireside_kb_map_uc, fireside_kb_ctrl_lc);
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_SPECIAL,
                        fireside_kb_map_spec, fireside_kb_ctrl_spec);
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_NUMBER,
                        fireside_kb_map_num, fireside_kb_ctrl_num);

    /* Stash the sensor identity + which label widget to repaint on save. */
    rename_ctx_t *ctx = (rename_ctx_t *)malloc(sizeof(*ctx));
    if (!ctx) { lv_obj_del(root); return; }
    ctx->key.src    = src;
    ctx->key.addr   = addr;
    ctx->key.sensor = sensor;
    ctx->textarea   = ta;
    ctx->label_row  = row_label;
    lv_obj_set_user_data(root, ctx);

    lv_obj_add_event_cb(btn_cancel, rename_cancel_cb,
                        LV_EVENT_CLICKED, root);
    lv_obj_add_event_cb(btn_save,   rename_save_cb,
                        LV_EVENT_CLICKED, root);
    lv_obj_add_event_cb(kb,         rename_kb_ready_cb,
                        LV_EVENT_READY,   root);
}

/* Edit-button tap on a sensor row → open the rename modal for that
 * sensor. The row's label pointer is stashed on the button's user_data
 * (as `key + label` via a small paired malloc — reuses the same lifetime
 * as the sensor_key_t so DELETE cb frees it). */
typedef struct { sensor_key_t key; lv_obj_t *label; } rename_btn_ctx_t;

static void rename_btn_click_cb(lv_event_t *e) {
    rename_btn_ctx_t *c =
        (rename_btn_ctx_t *)lv_event_get_user_data(e);
    if (!c) return;
    alarms_open_rename_modal(c->key.src, c->key.addr, c->key.sensor,
                             c->label);
}

static void rename_btn_delete_cb(lv_event_t *e) {
    rename_btn_ctx_t *c =
        (rename_btn_ctx_t *)lv_event_get_user_data(e);
    if (c) free(c);
}

/* Polarity switch handler — CHECKED means "inverted" (fire on LOW). Also
 * repaints the caption label so the user sees "TRIGGER: HIGH/LOW" flip
 * in sync. The caption label pointer is stashed in the switch's
 * user_data (via lv_obj_set_user_data) at row-build time. */
static void sensor_polarity_cb(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    sensor_key_t *k = (sensor_key_t *)lv_event_get_user_data(e);
    if (!k || !sw) return;
    bool inverted = lv_obj_has_state(sw, LV_STATE_CHECKED);
    alarms_set_inverted(k->src, k->addr, k->sensor, inverted);
    lv_obj_t *cap = (lv_obj_t *)lv_obj_get_user_data(sw);
    if (cap) lv_label_set_text(cap, inverted ? "TRIGGER: LOW" : "TRIGGER: HIGH");
}

/* Free the sensor_key_t we heap-allocated for each row's user_data when
 * the switch is deleted (rebuild replaces the whole list). */
static void sensor_switch_delete_cb(lv_event_t *e) {
    sensor_key_t *k = (sensor_key_t *)lv_event_get_user_data(e);
    if (k) free(k);
}

static void alarms_ui_populate(void) {
    lv_obj_t *list = objects.alarms_sensor_list;
    if (!list) return;

    /* Wipe every child from a prior open. LVGL deletes bottom-up which
     * invokes each switch's DELETE cb → frees its user_data. */
    lv_obj_clean(list);

    uint8_t sw_boards[8], pk_boards[8];
    int n_sw = alarms_get_known_boards(ALARM_SRC_SWITCHBACK, sw_boards, 8);
    int n_pk = alarms_get_known_boards(ALARM_SRC_PICKET,     pk_boards, 8);

    if (objects.alarms_sensor_hint) {
        if (n_sw + n_pk == 0) {
            lv_obj_clear_flag(objects.alarms_sensor_hint, LV_OBJ_FLAG_HIDDEN);
            return;   /* nothing to render below */
        } else {
            lv_obj_add_flag(objects.alarms_sensor_hint, LV_OBJ_FLAG_HIDDEN);
        }
    }

    /* Make the container scrollable + column-flex so rows stack naturally. */
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_add_flag(list, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_row(list, 4, LV_PART_MAIN);
    lv_obj_set_style_pad_all(list, 4, LV_PART_MAIN);

    const int SW_SENSORS = 8;
    const int PK_SENSORS = 12;

    /* One row layout — three horizontal cells (from left):
     *   1. Custom label (or default "SB0 · S3") + tiny caption "Trigger on"
     *   2. Polarity switch labeled HIGH / LOW (ON = LOW = "power-lost" style)
     *   3. Armed switch (green when on)
     * Row height 58 to accommodate a caption above each right-side switch. */
    for (int side = 0; side < 2; side++) {
        alarm_src_t src = (side == 0) ? ALARM_SRC_SWITCHBACK
                                      : ALARM_SRC_PICKET;
        int n_boards      = (side == 0) ? n_sw : n_pk;
        uint8_t *boards   = (side == 0) ? sw_boards : pk_boards;
        int sensors_per   = (side == 0) ? SW_SENSORS : PK_SENSORS;

        for (int b = 0; b < n_boards; b++) {
            uint8_t addr = boards[b];
            for (int s = 0; s < sensors_per; s++) {
                lv_obj_t *row = lv_obj_create(list);
                lv_obj_set_size(row, LV_PCT(100), 58);
                lv_obj_set_style_bg_color(row, lv_color_hex(0xffffff), LV_PART_MAIN);
                lv_obj_set_style_bg_opa(row, LV_OPA_COVER, LV_PART_MAIN);
                lv_obj_set_style_border_color(row, lv_color_hex(0xc8c8c8), LV_PART_MAIN);
                lv_obj_set_style_border_width(row, 1, LV_PART_MAIN);
                lv_obj_set_style_radius(row, 6, LV_PART_MAIN);
                lv_obj_set_style_pad_all(row, 8, LV_PART_MAIN);
                lv_obj_set_style_shadow_width(row, 0, LV_PART_MAIN);
                lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

                /* Label column (grows to fill remaining space) */
                char lbl_buf[ALARM_LABEL_MAX];
                alarms_get_label(src, addr, (uint8_t)s, lbl_buf, sizeof(lbl_buf));
                lv_obj_t *lbl = lv_label_create(row);
                lv_label_set_text(lbl, lbl_buf);
                lv_obj_set_style_text_color(lbl, lv_color_hex(0x1a1a1a), LV_PART_MAIN);
                lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 0, 0);

                /* Edit (pencil) button — opens rename modal. Placed
                 * immediately right of the label, before the two switches.
                 * 42×32 gives a comfortable tap target on the 10-in glass. */
                lv_obj_t *btn_edit = lv_btn_create(row);
                lv_obj_set_size(btn_edit, 42, 32);
                lv_obj_align(btn_edit, LV_ALIGN_RIGHT_MID, -140, 0);
                lv_obj_set_style_bg_color(btn_edit, lv_color_hex(0xededed),
                                          LV_PART_MAIN);
                lv_obj_set_style_shadow_width(btn_edit, 0, LV_PART_MAIN);
                lv_obj_t *edit_lbl = lv_label_create(btn_edit);
                lv_label_set_text(edit_lbl, LV_SYMBOL_EDIT);
                /* LV_SYMBOL_EDIT (U+F304) is only in LVGL's Montserrat
                 * built-ins; the theme's rr14 (Roboto Regular) is ASCII-
                 * only, so without an explicit font override the glyph
                 * renders as a rectangle. */
                lv_obj_set_style_text_font(edit_lbl, &lv_font_montserrat_14,
                                           LV_PART_MAIN);
                lv_obj_set_style_text_color(edit_lbl, lv_color_hex(0x1a1a1a),
                                            LV_PART_MAIN);
                lv_obj_center(edit_lbl);

                rename_btn_ctx_t *rc =
                    (rename_btn_ctx_t *)malloc(sizeof(*rc));
                if (rc) {
                    rc->key.src = src;
                    rc->key.addr = addr;
                    rc->key.sensor = (uint8_t)s;
                    rc->label = lbl;
                    lv_obj_add_event_cb(btn_edit, rename_btn_click_cb,
                                        LV_EVENT_CLICKED, rc);
                    lv_obj_add_event_cb(btn_edit, rename_btn_delete_cb,
                                        LV_EVENT_DELETE, rc);
                }

                /* Polarity switch — labeled TRIGGER, caption above shows
                 * current polarity: HIGH (default, arms on rising edge)
                 * vs LOW (inverted, arms on falling edge). Positions
                 * shifted left by 40 px to make room for the edit button
                 * (was -68/-10, now -88/-10 for polarity/arm). */
                bool inverted = alarms_is_inverted(src, addr, (uint8_t)s);
                lv_obj_t *pol_cap = lv_label_create(row);
                lv_label_set_text(pol_cap, inverted ? "TRIGGER: LOW" : "TRIGGER: HIGH");
                lv_obj_set_style_text_color(pol_cap, lv_color_hex(0x888888), LV_PART_MAIN);
                lv_obj_align(pol_cap, LV_ALIGN_RIGHT_MID, -88, -14);

                lv_obj_t *sw_pol = lv_switch_create(row);
                lv_obj_set_size(sw_pol, 46, 20);
                lv_obj_set_style_bg_color(sw_pol, lv_color_hex(0xededed), LV_PART_MAIN);
                lv_obj_set_style_bg_color(sw_pol, lv_color_hex(0xFFC107),
                                          LV_PART_INDICATOR | LV_STATE_CHECKED);
                if (inverted) lv_obj_add_state(sw_pol, LV_STATE_CHECKED);
                lv_obj_align(sw_pol, LV_ALIGN_RIGHT_MID, -88, 8);

                /* Armed switch (green when on) */
                lv_obj_t *arm_cap = lv_label_create(row);
                lv_label_set_text(arm_cap, "ARMED");
                lv_obj_set_style_text_color(arm_cap, lv_color_hex(0x888888), LV_PART_MAIN);
                lv_obj_align(arm_cap, LV_ALIGN_RIGHT_MID, -10, -14);

                lv_obj_t *sw_arm = lv_switch_create(row);
                lv_obj_set_size(sw_arm, 46, 20);
                lv_obj_set_style_bg_color(sw_arm, lv_color_hex(0xededed), LV_PART_MAIN);
                lv_obj_set_style_bg_color(sw_arm, lv_color_hex(0x52A441),
                                          LV_PART_INDICATOR | LV_STATE_CHECKED);
                if (alarms_is_armed(src, addr, (uint8_t)s))
                    lv_obj_add_state(sw_arm, LV_STATE_CHECKED);

                /* Heap key per switch (freed by DELETE cb). One instance
                 * per switch — armed + polarity share the same key struct
                 * type but need distinct instances. */
                sensor_key_t *k_arm = (sensor_key_t *)malloc(sizeof(*k_arm));
                sensor_key_t *k_pol = (sensor_key_t *)malloc(sizeof(*k_pol));
                if (k_arm) {
                    k_arm->src = src; k_arm->addr = addr; k_arm->sensor = (uint8_t)s;
                    lv_obj_add_event_cb(sw_arm, sensor_switch_cb,
                                        LV_EVENT_VALUE_CHANGED, k_arm);
                    lv_obj_add_event_cb(sw_arm, sensor_switch_delete_cb,
                                        LV_EVENT_DELETE, k_arm);
                }
                if (k_pol) {
                    k_pol->src = src; k_pol->addr = addr; k_pol->sensor = (uint8_t)s;
                    /* Polarity cb needs its own handler — captures pol_cap
                     * so it can repaint "TRIGGER: HIGH/LOW" on toggle. */
                    lv_obj_set_user_data(sw_pol, pol_cap);
                    lv_obj_add_event_cb(sw_pol, sensor_polarity_cb,
                                        LV_EVENT_VALUE_CHANGED, k_pol);
                    lv_obj_add_event_cb(sw_pol, sensor_switch_delete_cb,
                                        LV_EVENT_DELETE, k_pol);
                }
                lv_obj_align(sw_arm, LV_ALIGN_RIGHT_MID, -10, 8);
            }
        }
    }
}
#endif

void action_open_alarms(lv_event_t *e) {
    (void)e;
#if __has_include("ui/screens.h")
    if (objects.page_alarms) {
        alarms_paint_state();
        alarms_ui_populate();
        alarms_build_snooze_row();
        lv_scr_load(objects.page_alarms);
    }
#endif
}

void action_alarms_back(lv_event_t *e) {
    (void)e;
#if __has_include("ui/screens.h")
    if (objects.page_settings) lv_scr_load(objects.page_settings);
#endif
}

void action_toggle_battery_alarm(lv_event_t *e) {
#if __has_include("ui/screens.h")
    (void)e;
    if (!objects.alarms_bat_switch) return;
    bool en = lv_obj_has_state(objects.alarms_bat_switch, LV_STATE_CHECKED);
    nvs_handle_t nvs;
    if (nvs_open("fireside_alarm", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_u8(nvs, "batt_en", en ? 1 : 0);
        nvs_commit(nvs); nvs_close(nvs);
    }
    /* Reload alarms module so the new enabled flag takes effect
     * without needing a reboot. */
    alarms_init();
#else
    (void)e;
#endif
}

void action_battery_threshold(lv_event_t *e) {
#if __has_include("ui/screens.h")
    (void)e;
    if (!objects.alarms_bat_slider) return;
    int32_t v = lv_slider_get_value(objects.alarms_bat_slider);
    if (v < 0)   v = 0;
    if (v > 100) v = 100;
    nvs_handle_t nvs;
    if (nvs_open("fireside_alarm", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_u8(nvs, "batt_thr", (uint8_t)v);
        nvs_commit(nvs); nvs_close(nvs);
    }
    if (objects.alarms_bat_current) {
        char buf[8]; snprintf(buf, sizeof(buf), "%d%%", (int)v);
        lv_label_set_text(objects.alarms_bat_current, buf);
    }
    alarms_init();
#else
    (void)e;
#endif
}

/* Factory reset flow:
 *   Settings "Factory Reset" tile button  -> action_show_factory_reset_confirm
 *   Modal Cancel button / backdrop tap    -> action_cancel_factory_reset_confirm
 *   Modal "Factory Reset" (red) button    -> action_factory_reset (wipes+reboots)
 */

void action_show_factory_reset_confirm(lv_event_t *e) {
    (void)e;
#if __has_include("ui/screens.h")
    /* Modal is authored as the LAST child of the PageSettings screen in the
     * .eez-project, so it already sits on top in the widget tree. Simply
     * clearing HIDDEN reveals it. */
    if (objects.modal_factory_reset) {
        lv_obj_clear_flag(objects.modal_factory_reset, LV_OBJ_FLAG_HIDDEN);
    }
#endif
}

void action_cancel_factory_reset_confirm(lv_event_t *e) {
    (void)e;
#if __has_include("ui/screens.h")
    if (objects.modal_factory_reset) {
        lv_obj_add_flag(objects.modal_factory_reset, LV_OBJ_FLAG_HIDDEN);
    }
#endif
}

/* Factory reset — wipe every user-set value from the "fireside" NVS
 * namespace (WiFi creds, MQTT creds, brightness, theme, timeout, tempunit,
 * timezone) and reboot into a clean first-boot state. */
void action_factory_reset(lv_event_t *e) {
    (void)e;
    ESP_LOGW(TAG, "FACTORY RESET requested — wiping NVS");
    nvs_handle_t h;
    if (nvs_open("fireside", NVS_READWRITE, &h) == ESP_OK) {
        nvs_erase_all(h);
        nvs_commit(h);
        nvs_close(h);
    }
    /* Also nuke the top-level NVS default partition to catch anything the
     * WiFi stack, esp_hosted, or SNTP might have squirreled away. */
    nvs_flash_erase();
    ESP_LOGW(TAG, "NVS wiped — rebooting");
    vTaskDelay(pdMS_TO_TICKS(200));   /* let the log flush */
    esp_restart();
}

/* ============================================================
 * WiFi wizard actions
 * ============================================================ */

void action_wifi_scan(lv_event_t *e) {
    (void)e;
    ESP_LOGI(TAG, "wifi scan requested");
    /* Blank the list + set "Scanning..." status BEFORE kicking the scan so
     * the user gets immediate visual feedback. app_state_paint_wifi_rows
     * repopulates when the scan callback lands. */
    app_state_paint_wifi_scanning();
    wifi_setup_scan_start();
}

/* Stub — kept so the linker resolves the actions[] declaration EEZ
 * Studio still exports for the WifiSelect action name. Rows are now
 * created dynamically by app_state.c, which wires wifi_row_selected()
 * as the per-row click handler; this legacy entry point never fires. */
void action_wifi_select(lv_event_t *e) { (void)e; }

/* Called from app_state.c's dynamic row click callback when the user
 * taps a network in the scan list. Same behaviour as the old
 * action_wifi_select (open → connect, locked → password screen), but
 * without the row-index-into-authored-widgets lookup. */
void wifi_row_selected(const char *ssid, bool locked) {
    if (!ssid || !*ssid) return;
    strlcpy(s_selected_ssid, ssid, sizeof(s_selected_ssid));
    s_selected_locked = locked;
    ESP_LOGI(TAG, "wifi row selected: %s (%s)",
             s_selected_ssid, locked ? "locked" : "open");
    if (!locked) {
        app_state_set(APP_STATE_WIFI_CONNECTING);
        fireside_config_set_wifi(s_selected_ssid, "");
        wifi_setup_connect(s_selected_ssid, "");
    } else {
        app_state_wifi_show_password(s_selected_ssid);
    }
}

/* Show/hide toggle for the two password textareas. Flips
 * lv_textarea_set_password_mode and rewrites the button's child label
 * between "Show" (currently masked) and "Hide" (currently visible). */
#if __has_include("ui/screens.h")
static void toggle_password_visibility(lv_obj_t *textarea, lv_obj_t *btn_label) {
    if (!textarea) return;
    bool now_masked = lv_textarea_get_password_mode(textarea);
    lv_textarea_set_password_mode(textarea, !now_masked);
    if (btn_label) {
        /* If it was masked and we're revealing → label becomes "Hide".
         * If it was visible and we're masking → label becomes "Show". */
        lv_label_set_text(btn_label, now_masked ? "Hide" : "Show");
    }
}
#endif

void action_wifi_toggle_password(lv_event_t *e) {
    (void)e;
#if __has_include("ui/screens.h")
    toggle_password_visibility(objects.wifi_pw_input,
                               objects.wifi_pw_show_btn_lbl);
#endif
}

void action_mqtt_toggle_password(lv_event_t *e) {
    (void)e;
#if __has_include("ui/screens.h")
    toggle_password_visibility(objects.mqtt_pass_input,
                               objects.mqtt_pass_show_btn_lbl);
#endif
}

void action_wifi_cancel_password(lv_event_t *e) {
    (void)e;
    ESP_LOGI(TAG, "wifi cancel password");
    s_selected_ssid[0] = '\0';
    s_selected_locked = false;
#if __has_include("ui/screens.h")
    if (objects.wifi_pw_input) lv_textarea_set_text(objects.wifi_pw_input, "");
#endif
    /* Return to the network list. */
    app_state_wifi_show_list();
}

void action_wifi_submit_password(lv_event_t *e) {
    (void)e;
#if __has_include("ui/screens.h")
    if (!s_selected_ssid[0]) {
        ESP_LOGW(TAG, "wifi submit: no SSID selected");
        return;
    }
    const char *pw = objects.wifi_pw_input
                   ? lv_textarea_get_text(objects.wifi_pw_input)
                   : "";
    ESP_LOGI(TAG, "wifi submit SSID=%s (pw len=%u)",
             s_selected_ssid, (unsigned)strlen(pw));
    fireside_config_set_wifi(s_selected_ssid, pw);
    app_state_set(APP_STATE_WIFI_CONNECTING);
    wifi_setup_connect(s_selected_ssid, pw);
#endif
}

/* ============================================================
 * MQTT wizard actions
 * ============================================================ */

/* Tap-to-focus keyboard binding for the MQTT setup wizard textareas. The
 * keyboard widget doesn't magically follow focus — every textarea needs
 * lv_keyboard_set_textarea() called when the user taps it. Wired up once
 * from init_mqtt_setup_bindings() (called from main.c after ui_init). */
#if __has_include("ui/screens.h")
static void mqtt_textarea_focus_cb(lv_event_t *e) {
    /* Use current_target (the widget the handler was registered on) rather
     * than target (which can be a child if events bubbled). If the label
     * or cursor inside the textarea absorbs the tap, target would return
     * that child and lv_keyboard_set_textarea(kb, label) silently unbinds
     * the keyboard — which is exactly the "keys stop responding" symptom. */
    lv_obj_t *ta = lv_event_get_current_target(e);
    if (!objects.mqtt_setup_kb || !ta) return;
    /* Guard: only bind if it's one of our authored textareas. */
    if (ta != objects.mqtt_host_input &&
        ta != objects.mqtt_user_input &&
        ta != objects.mqtt_pass_input) return;
    /* Manually manage LV_STATE_FOCUSED across the three textareas so
     * the CURSOR.FOCUSED and MAIN.FOCUSED styles in TextareaDefault fire
     * on ONLY the field being typed into. Without this every field
     * looks focused simultaneously (they all draw a cursor + highlighted
     * border). */
    lv_obj_t *all[3] = { objects.mqtt_host_input,
                         objects.mqtt_user_input,
                         objects.mqtt_pass_input };
    for (int i = 0; i < 3; i++) {
        if (all[i]) lv_obj_clear_state(all[i], LV_STATE_FOCUSED);
    }
    lv_obj_add_state(ta, LV_STATE_FOCUSED);
    lv_keyboard_set_textarea(objects.mqtt_setup_kb, ta);
}
#endif

/* Button-edit textarea → keyboard binding. The keyboard on PageButtonEdit
 * is authored HIDDEN so it doesn't obscure the four selector rows when
 * the user isn't typing. Tap the textarea → reveal keyboard + bind it.
 * Tap the keyboard's OK (READY event) or the ESC (CANCEL event) → hide.
 * Wired once from init_button_edit_bindings() called after ui_init(). */
#if __has_include("ui/screens.h")
static void button_edit_ta_focus_cb(lv_event_t *e) {
    (void)e;
    if (!objects.kb_button_edit || !objects.ta_button_edit_label) return;
    lv_keyboard_set_textarea(objects.kb_button_edit,
                             objects.ta_button_edit_label);
    lv_obj_clear_flag(objects.kb_button_edit, LV_OBJ_FLAG_HIDDEN);
}
static void button_edit_kb_dismiss_cb(lv_event_t *e) {
    (void)e;
    if (!objects.kb_button_edit) return;
    lv_obj_add_flag(objects.kb_button_edit, LV_OBJ_FLAG_HIDDEN);
}
#endif

void init_button_edit_bindings(void) {
#if __has_include("ui/screens.h")
    if (objects.ta_button_edit_label) {
        /* CLICKED covers touch; FOCUSED covers programmatic focus. */
        lv_obj_add_event_cb(objects.ta_button_edit_label,
                            button_edit_ta_focus_cb,
                            LV_EVENT_CLICKED, NULL);
        lv_obj_add_event_cb(objects.ta_button_edit_label,
                            button_edit_ta_focus_cb,
                            LV_EVENT_FOCUSED, NULL);
    }
    if (objects.kb_button_edit) {
        /* Keyboard's built-in OK (READY) and ESC (CANCEL) hide it. */
        lv_obj_add_event_cb(objects.kb_button_edit,
                            button_edit_kb_dismiss_cb,
                            LV_EVENT_READY, NULL);
        lv_obj_add_event_cb(objects.kb_button_edit,
                            button_edit_kb_dismiss_cb,
                            LV_EVENT_CANCEL, NULL);
    }
#endif
}

void init_mqtt_setup_bindings(void) {
#if __has_include("ui/screens.h")
    lv_obj_t *tas[3] = {
        objects.mqtt_host_input,
        objects.mqtt_user_input,
        objects.mqtt_pass_input,
    };
    for (int i = 0; i < 3; i++) {
        if (!tas[i]) continue;
        /* CLICKED covers touch and pointer; FOCUSED covers keyboard/nav.
         * Both re-bind so whichever textarea the user last touched is the
         * one receiving keystrokes. */
        lv_obj_add_event_cb(tas[i], mqtt_textarea_focus_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_add_event_cb(tas[i], mqtt_textarea_focus_cb, LV_EVENT_FOCUSED, NULL);
    }
    /* Pre-bind to the host field so typing works the moment the wizard
     * loads (before any tap). */
    if (objects.mqtt_setup_kb && objects.mqtt_host_input) {
        lv_keyboard_set_textarea(objects.mqtt_setup_kb, objects.mqtt_host_input);
    }
#endif
}

void action_mqtt_submit(lv_event_t *e) {
    (void)e;
#if __has_include("ui/screens.h")
    const char *host = objects.mqtt_host_input
                     ? lv_textarea_get_text(objects.mqtt_host_input) : "";
    const char *user = objects.mqtt_user_input
                     ? lv_textarea_get_text(objects.mqtt_user_input) : "";
    const char *pass = objects.mqtt_pass_input
                     ? lv_textarea_get_text(objects.mqtt_pass_input) : "";
    if (!host || !*host) {
        ESP_LOGW(TAG, "mqtt submit: host required");
        return;
    }
    ESP_LOGI(TAG, "mqtt submit host=%s user=%s", host, user);
    /* Default TrailCurrent broker port. Wizard can be extended later to
     * allow a per-device port override. */
    fireside_config_set_mqtt(host, user, pass, 8883);
    mqtt_client_load_settings();
    app_state_set(APP_STATE_MQTT_CONNECTING);
    mqtt_client_connect();

    if (objects.mqtt_connecting_host)
        lv_label_set_text(objects.mqtt_connecting_host, host);
#endif
}

void action_mqtt_skip(lv_event_t *e) {
    (void)e;
    /* Skip MQTT setup — land on Home without a broker connection. */
    ESP_LOGI(TAG, "mqtt skip");
    app_state_set(APP_STATE_READY);
}

/* ============================================================
 * Button/Device configuration wizard
 *
 * Mirrors Milepost's PageButtonEdit + PageDeviceAssign flow, adapted for
 * Fireside's MQTT-fed data model. Backend (components/button_config) is
 * a straight port from Milepost — only the UI wiring and the outbound
 * publish differ. Edit config lives in file-static state so we don't need
 * to expose new EEZ variables just for the wizard.
 * ============================================================ */

static int32_t s_edit_btn_number     = 0;
static uint16_t s_edit_icon_codepoint = 0;
static uint8_t  s_assign_module_type = MOD_NONE;
static uint8_t  s_assign_instance    = 0;

#if __has_include("ui/screens.h")

/* Refresh the highlight ring on the icon-picker slot that matches
 * s_edit_icon_codepoint. Slots are named BtnIconSlot00..BtnIconSlot<N-1>
 * and the C export makes each one available as objects.btn_icon_slot<NN>. */
static void highlight_selected_icon(uint16_t cp) {
    lv_obj_t *slots[NUM_CURATED_ICONS];
    /* EEZ Studio exports identifiers as lower-snake-case. The macro block
     * below fetches each slot by name; if the export hasn't happened yet
     * (Fireside built pre-EEZ-Studio-Ctrl+B) the pointers are NULL and
     * this quietly no-ops. Kept as raw list because the struct order can't
     * be walked programmatically. */
    #define S(n, sym) slots[n] = objects.sym
    S(0,  btn_icon_slot00); S(1,  btn_icon_slot01); S(2,  btn_icon_slot02);
    S(3,  btn_icon_slot03); S(4,  btn_icon_slot04); S(5,  btn_icon_slot05);
    S(6,  btn_icon_slot06); S(7,  btn_icon_slot07); S(8,  btn_icon_slot08);
    S(9,  btn_icon_slot09); S(10, btn_icon_slot10); S(11, btn_icon_slot11);
    S(12, btn_icon_slot12); S(13, btn_icon_slot13); S(14, btn_icon_slot14);
    S(15, btn_icon_slot15); S(16, btn_icon_slot16); S(17, btn_icon_slot17);
    S(18, btn_icon_slot18); S(19, btn_icon_slot19); S(20, btn_icon_slot20);
    S(21, btn_icon_slot21); S(22, btn_icon_slot22); S(23, btn_icon_slot23);
    S(24, btn_icon_slot24); S(25, btn_icon_slot25); S(26, btn_icon_slot26);
    S(27, btn_icon_slot27); S(28, btn_icon_slot28); S(29, btn_icon_slot29);
    S(30, btn_icon_slot30); S(31, btn_icon_slot31); S(32, btn_icon_slot32);
    S(33, btn_icon_slot33); S(34, btn_icon_slot34); S(35, btn_icon_slot35);
    S(36, btn_icon_slot36); S(37, btn_icon_slot37); S(38, btn_icon_slot38);
    S(39, btn_icon_slot39); S(40, btn_icon_slot40); S(41, btn_icon_slot41);
    S(42, btn_icon_slot42); S(43, btn_icon_slot43); S(44, btn_icon_slot44);
    S(45, btn_icon_slot45); S(46, btn_icon_slot46); S(47, btn_icon_slot47);
    S(48, btn_icon_slot48); S(49, btn_icon_slot49); S(50, btn_icon_slot50);
    S(51, btn_icon_slot51); S(52, btn_icon_slot52); S(53, btn_icon_slot53);
    S(54, btn_icon_slot54); S(55, btn_icon_slot55); S(56, btn_icon_slot56);
    S(57, btn_icon_slot57); S(58, btn_icon_slot58); S(59, btn_icon_slot59);
    S(60, btn_icon_slot60); S(61, btn_icon_slot61); S(62, btn_icon_slot62);
    S(63, btn_icon_slot63); S(64, btn_icon_slot64); S(65, btn_icon_slot65);
    S(66, btn_icon_slot66); S(67, btn_icon_slot67); S(68, btn_icon_slot68);
    S(69, btn_icon_slot69); S(70, btn_icon_slot70); S(71, btn_icon_slot71);
    S(72, btn_icon_slot72); S(73, btn_icon_slot73); S(74, btn_icon_slot74);
    S(75, btn_icon_slot75); S(76, btn_icon_slot76); S(77, btn_icon_slot77);
    S(78, btn_icon_slot78); S(79, btn_icon_slot79); S(80, btn_icon_slot80);
    S(81, btn_icon_slot81); S(82, btn_icon_slot82); S(83, btn_icon_slot83);
    #undef S
    for (int i = 0; i < NUM_CURATED_ICONS; i++) {
        if (!slots[i]) continue;
        if (CURATED_ICONS[i] == cp) lv_obj_add_state(slots[i], LV_STATE_CHECKED);
        else                        lv_obj_clear_state(slots[i], LV_STATE_CHECKED);
    }
}

/* Per-button-edit selector state — mirrors the button's mapping while
 * the user is on PageButtonEdit. Committed to g_buttons[] via
 * button_config_assign when the user taps Save. */
static uint8_t s_edit_channel = 0;    /* 0..7 device slot within (mod, addr) */

/* CHECKED-state helpers for the three single-select rows. Each takes the
 * currently-selected value and paints CHECKED on the matching button. */
static void highlight_module(uint8_t mod) {
    /* Torrent = MOD_TORRENT (1), Switchback = MOD_SWITCHBACK (2). */
    lv_obj_t *torrent = objects.btn_module_torrent;
    lv_obj_t *swback  = objects.btn_module_switchback;
    if (torrent) {
        if (mod == MOD_TORRENT) lv_obj_add_state(torrent, LV_STATE_CHECKED);
        else                    lv_obj_clear_state(torrent, LV_STATE_CHECKED);
    }
    if (swback) {
        if (mod == MOD_SWITCHBACK) lv_obj_add_state(swback, LV_STATE_CHECKED);
        else                       lv_obj_clear_state(swback, LV_STATE_CHECKED);
    }
}

static void highlight_address(uint8_t addr) {
    lv_obj_t *btns[3] = { objects.btn_addr0, objects.btn_addr1,
                          objects.btn_addr2 };
    for (int i = 0; i < 3; i++) {
        if (!btns[i]) continue;
        if (i == addr) lv_obj_add_state(btns[i], LV_STATE_CHECKED);
        else           lv_obj_clear_state(btns[i], LV_STATE_CHECKED);
    }
}

static void highlight_channel(uint8_t ch) {
    lv_obj_t *btns[8] = {
        objects.btn_ch0, objects.btn_ch1, objects.btn_ch2, objects.btn_ch3,
        objects.btn_ch4, objects.btn_ch5, objects.btn_ch6, objects.btn_ch7,
    };
    for (int i = 0; i < 8; i++) {
        if (!btns[i]) continue;
        if (i == ch) lv_obj_add_state(btns[i], LV_STATE_CHECKED);
        else         lv_obj_clear_state(btns[i], LV_STATE_CHECKED);
    }
}

/* Strong override of the weak stub in components/button_config/. This
 * lives in main/ so we can reach objects.* without cross-component
 * header visibility. Called on:
 *   - boot, from main.c after ui_init(), to hydrate the saved state
 *   - action_save_button_appearance, right after button_config_assign
 *   - action_open_edit_buttons, before landing on the tile grid
 */
static void format_sub_line(const btn_config_t *b, char *out, size_t sz) {
    if (b->module_type == MOD_NONE) {
        snprintf(out, sz, "Unassigned");
        return;
    }
    const char *mod = (b->module_type == MOD_TORRENT)    ? "Torrent"
                    : (b->module_type == MOD_SWITCHBACK) ? "Switchback"
                    : "?";
    snprintf(out, sz, "%s %u \xc2\xb7 Dev %u",
             mod, (unsigned)b->instance, (unsigned)b->channel);
}

void button_config_apply_to_ui(void) {
    lv_obj_t *home_texts[NUM_BUTTONS] = {
        objects.home_dev1_text, objects.home_dev2_text,
        objects.home_dev3_text, objects.home_dev4_text,
        objects.home_dev5_text, objects.home_dev6_text,
        objects.home_dev7_text, objects.home_dev8_text,
    };
    lv_obj_t *home_icons[NUM_BUTTONS] = {
        objects.home_dev1_icon, objects.home_dev2_icon,
        objects.home_dev3_icon, objects.home_dev4_icon,
        objects.home_dev5_icon, objects.home_dev6_icon,
        objects.home_dev7_icon, objects.home_dev8_icon,
    };
    lv_obj_t *edit_labels[NUM_BUTTONS] = {
        objects.lbl_edit_btn01_label, objects.lbl_edit_btn02_label,
        objects.lbl_edit_btn03_label, objects.lbl_edit_btn04_label,
        objects.lbl_edit_btn05_label, objects.lbl_edit_btn06_label,
        objects.lbl_edit_btn07_label, objects.lbl_edit_btn08_label,
    };
    lv_obj_t *edit_icons[NUM_BUTTONS] = {
        objects.lbl_edit_btn01_icon, objects.lbl_edit_btn02_icon,
        objects.lbl_edit_btn03_icon, objects.lbl_edit_btn04_icon,
        objects.lbl_edit_btn05_icon, objects.lbl_edit_btn06_icon,
        objects.lbl_edit_btn07_icon, objects.lbl_edit_btn08_icon,
    };
    lv_obj_t *edit_subs[NUM_BUTTONS] = {
        objects.lbl_edit_btn01_sub, objects.lbl_edit_btn02_sub,
        objects.lbl_edit_btn03_sub, objects.lbl_edit_btn04_sub,
        objects.lbl_edit_btn05_sub, objects.lbl_edit_btn06_sub,
        objects.lbl_edit_btn07_sub, objects.lbl_edit_btn08_sub,
    };
    char icon_buf[5];
    char sub_buf[32];
    for (int i = 0; i < NUM_BUTTONS; i++) {
        const btn_config_t *b = &g_buttons[i];
        const char *icon = utf8_encode(b->icon_codepoint, icon_buf);
        if (home_texts[i])  lv_label_set_text(home_texts[i],  b->label);
        if (home_icons[i])  lv_label_set_text(home_icons[i],  icon);
        if (edit_labels[i]) lv_label_set_text(edit_labels[i], b->label);
        if (edit_icons[i])  lv_label_set_text(edit_icons[i],  icon);
        if (edit_subs[i]) {
            format_sub_line(b, sub_buf, sizeof(sub_buf));
            lv_label_set_text(edit_subs[i], sub_buf);
        }
    }
    ESP_LOGI(TAG, "button_config_apply_to_ui: painted 8 tiles");
}

#endif  /* screens.h */

void action_open_edit_buttons(lv_event_t *e) {
    (void)e;
#if __has_include("ui/screens.h")
    if (objects.page_edit_buttons) {
        button_config_apply_to_ui();   /* refresh sub-lines before landing */
        lv_scr_load(objects.page_edit_buttons);
    }
#endif
}

void action_navigate_to_button_edit(lv_event_t *e) {
    intptr_t btn = (intptr_t)lv_event_get_user_data(e);
    if (btn < 1 || btn > NUM_BUTTONS) return;
    s_edit_btn_number     = (int32_t)btn;
    const btn_config_t *cfg = &g_buttons[btn - 1];
    s_edit_icon_codepoint = cfg->icon_codepoint;
    /* Pre-populate the module/address/device pills from what this button
     * is currently mapped to. Fallback: no module set → default Torrent
     * addr=0 device=(btn-1) so first-time editors see a sensible starting
     * point instead of nothing highlighted. */
    if (cfg->module_type == MOD_NONE) {
        s_assign_module_type = MOD_TORRENT;
        s_assign_instance    = 0;
        s_edit_channel       = (uint8_t)(btn - 1);
    } else {
        s_assign_module_type = cfg->module_type;
        s_assign_instance    = cfg->instance;
        s_edit_channel       = cfg->channel;
    }
#if __has_include("ui/screens.h")
    if (objects.lbl_button_edit_header) {
        char hdr[24];
        snprintf(hdr, sizeof(hdr), "Button %ld", (long)btn);
        lv_label_set_text(objects.lbl_button_edit_header, hdr);
    }
    if (objects.ta_button_edit_label) {
        lv_textarea_set_text(objects.ta_button_edit_label, cfg->label);
        if (objects.kb_button_edit) {
            lv_keyboard_set_textarea(objects.kb_button_edit,
                                     objects.ta_button_edit_label);
        }
    }
    highlight_selected_icon(cfg->icon_codepoint);
    highlight_module(s_assign_module_type);
    highlight_address(s_assign_instance);
    highlight_channel(s_edit_channel);
    if (objects.page_button_edit) lv_scr_load(objects.page_button_edit);
#endif
    ESP_LOGI(TAG, "edit button %ld: mod=%u addr=%u dev=%u",
             (long)btn, (unsigned)s_assign_module_type,
             (unsigned)s_assign_instance, (unsigned)s_edit_channel);
}

void action_select_button_icon(lv_event_t *e) {
    uint32_t cp = (uint32_t)(uintptr_t)lv_event_get_user_data(e);
    if (cp == 0) return;
    s_edit_icon_codepoint = (uint16_t)cp;
#if __has_include("ui/screens.h")
    highlight_selected_icon((uint16_t)cp);
#endif
    ESP_LOGI(TAG, "icon selected U+%04lX", (unsigned long)cp);
}

void action_save_button_appearance(lv_event_t *e) {
    (void)e;
    int btn = s_edit_btn_number;
    if (btn < 1 || btn > NUM_BUTTONS) return;
    const char *lbl = NULL;
    char buf[BTN_LABEL_MAX];
#if __has_include("ui/screens.h")
    if (objects.ta_button_edit_label) {
        const char *cur = lv_textarea_get_text(objects.ta_button_edit_label);
        if (cur) {
            strncpy(buf, cur, sizeof(buf) - 1);
            buf[sizeof(buf) - 1] = '\0';
            lbl = buf;
        }
    }
#endif
    /* One-shot save: label + icon + full (module, address, device) mapping.
     * button_config_assign clears any other button that currently points at
     * the same (module, addr, device) tuple, keeping the mapping bijective
     * — matches Milepost semantics. */
    button_config_set_appearance((uint8_t)btn,
                                 (lbl && lbl[0]) ? lbl : NULL,
                                 s_edit_icon_codepoint);
    if (s_assign_module_type == MOD_TORRENT ||
        s_assign_module_type == MOD_SWITCHBACK) {
        button_config_assign((module_type_t)s_assign_module_type,
                             s_assign_instance,
                             s_edit_channel,
                             (uint8_t)btn);
    }
    button_config_apply_to_ui();
#if __has_include("ui/screens.h")
    /* Hide the keyboard so it's not still visible on next entry. */
    if (objects.kb_button_edit)
        lv_obj_add_flag(objects.kb_button_edit, LV_OBJ_FLAG_HIDDEN);
    if (objects.page_edit_buttons) lv_scr_load(objects.page_edit_buttons);
#endif
    ESP_LOGI(TAG, "saved btn=%d icon=U+%04X mod=%u addr=%u dev=%u",
             btn, (unsigned)s_edit_icon_codepoint,
             (unsigned)s_assign_module_type,
             (unsigned)s_assign_instance,
             (unsigned)s_edit_channel);
}

/* Module / Address / Device single-select handlers. Only update the
 * transient edit state + repaint the CHECKED pill; nothing is persisted
 * until the user taps Save. */

void action_select_module(lv_event_t *e) {
    intptr_t mod = (intptr_t)lv_event_get_user_data(e);
    if (mod != MOD_TORRENT && mod != MOD_SWITCHBACK) return;
    s_assign_module_type = (uint8_t)mod;
#if __has_include("ui/screens.h")
    highlight_module((uint8_t)mod);
#endif
}

void action_select_address(lv_event_t *e) {
    intptr_t addr = (intptr_t)lv_event_get_user_data(e);
    if (addr < 0 || addr > 2) return;
    s_assign_instance = (uint8_t)addr;
#if __has_include("ui/screens.h")
    highlight_address((uint8_t)addr);
#endif
}

void action_select_channel(lv_event_t *e) {
    intptr_t ch = (intptr_t)lv_event_get_user_data(e);
    if (ch < 0 || ch >= 8) return;
    s_edit_channel = (uint8_t)ch;
#if __has_include("ui/screens.h")
    highlight_channel((uint8_t)ch);
#endif
}

void action_back_to_edit_buttons(lv_event_t *e) {
    (void)e;
#if __has_include("ui/screens.h")
    /* Hide the keyboard on exit so the next PageButtonEdit entry starts
     * with it dismissed and the selector rows fully visible. */
    if (objects.kb_button_edit)
        lv_obj_add_flag(objects.kb_button_edit, LV_OBJ_FLAG_HIDDEN);
    lv_obj_t *scr = lv_scr_act();
    if (scr == objects.page_button_edit && objects.page_edit_buttons) {
        lv_scr_load(objects.page_edit_buttons);
        return;
    }
    if (objects.page_settings) lv_scr_load(objects.page_settings);
#endif
}

/* Push-to-talk (TALK) button on PageHome. Two separate actions bound to
 * PRESSED and RELEASED (plus PRESS_LOST -> released, so a finger drag off
 * the button doesn't leave the mic capturing). Both handlers run under
 * the LVGL lock; peregrine_voice hands the HTTP work off to its own task
 * so the UI stays responsive during upload + playback. */
void action_ptt_pressed(lv_event_t *e) {
    (void)e;
    peregrine_voice_press();
}

void action_ptt_released(lv_event_t *e) {
    (void)e;
    peregrine_voice_release();
}

/* ============================================================
 * TALK button "thinking" glow.
 *
 * While the released utterance is in flight to Peregrine (state ==
 * PRG_VOICE_UPLOADING) the button wears a pulsating green ring so the user
 * knows the request is being worked on rather than dropped.
 *
 * Appearance is entirely EEZ-authored: home_ptt_glow1..6 are six concentric
 * ring overlays on the button, each a frame of the pulse (border thickness
 * 2..7 px, opacity 70..255, AccentPrimary). All six are authored HIDDEN, so
 * the EEZ Studio canvas shows the idle button exactly as the device does at
 * rest. This code only un-hides one frame at a time — no lv_obj_set_style_*
 * calls, per the CLAUDE.md "code controls STATE, EEZ Studio controls
 * APPEARANCE" rule.
 *
 * Frame order 1-2-3-4-5-6-5-4-3-2 at 90 ms = ~900 ms breathe cycle.
 * ============================================================ */

#if __has_include("ui/screens.h")

#define PTT_GLOW_FRAMES 6
#define PTT_GLOW_STEP_MS 90

/* Ping-pong ramp: index into the frame array, up then back down. */
static const uint8_t s_ptt_glow_seq[] = { 0, 1, 2, 3, 4, 5, 4, 3, 2, 1 };

static lv_obj_t *ptt_glow_frame(int i) {
    switch (i) {
    case 0: return objects.home_ptt_glow1;
    case 1: return objects.home_ptt_glow2;
    case 2: return objects.home_ptt_glow3;
    case 3: return objects.home_ptt_glow4;
    case 4: return objects.home_ptt_glow5;
    case 5: return objects.home_ptt_glow6;
    default: return NULL;
    }
}

static void ptt_glow_hide_all(void) {
    for (int i = 0; i < PTT_GLOW_FRAMES; i++) {
        lv_obj_t *o = ptt_glow_frame(i);
        if (o) lv_obj_add_flag(o, LV_OBJ_FLAG_HIDDEN);
    }
}

static void ptt_glow_cb(lv_timer_t *t) {
    (void)t;
    static bool     active = false;
    static uint32_t step   = 0;

    /* "Waiting for the LLM" is precisely the upload/response window: the
     * button has been released, the WAV is on the wire, nothing has come
     * back yet. LISTENING already has its own PRESSED look, and PLAYING
     * means the answer is arriving. */
    bool want = (peregrine_voice_get_state() == PRG_VOICE_UPLOADING);

    if (!want) {
        if (active) {
            ptt_glow_hide_all();
            active = false;
        }
        step = 0;
        return;
    }

    if (!active) {
        active = true;
        step   = 0;
    }

    int prev = s_ptt_glow_seq[step % (sizeof(s_ptt_glow_seq) / sizeof(*s_ptt_glow_seq))];
    step++;
    int cur  = s_ptt_glow_seq[step % (sizeof(s_ptt_glow_seq) / sizeof(*s_ptt_glow_seq))];

    if (prev != cur) {
        lv_obj_t *po = ptt_glow_frame(prev);
        if (po) lv_obj_add_flag(po, LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_t *co = ptt_glow_frame(cur);
    if (co) lv_obj_clear_flag(co, LV_OBJ_FLAG_HIDDEN);
}

#endif /* __has_include("ui/screens.h") */

/* Called once from app_main, under the display lock. */
void init_ptt_glow(void) {
#if __has_include("ui/screens.h")
    ptt_glow_hide_all();
    lv_timer_create(ptt_glow_cb, PTT_GLOW_STEP_MS, NULL);
#endif
}

/* Show or hide the PageHome TALK button based on whether the microSD card
 * supplied a Peregrine URL + token. Called once from app_main immediately
 * after sd_config_load(), before the first flush, so the button never
 * flashes into view on a card-less boot. */
void apply_ptt_availability(bool available) {
#if __has_include("ui/screens.h")
    if (!objects.home_ptt_btn) return;
    if (available) {
        lv_obj_clear_flag(objects.home_ptt_btn, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(objects.home_ptt_btn, LV_OBJ_FLAG_HIDDEN);
    }
    ESP_LOGI(TAG, "TALK button %s (Peregrine SD config %s)",
             available ? "visible" : "hidden",
             available ? "found" : "missing");
#else
    (void)available;
#endif
}
