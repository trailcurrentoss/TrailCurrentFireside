#include "actions.h"
#include "bsp/display.h"
#include "bsp/esp-bsp.h"
#include "core/lv_event.h"
#include "core/lv_obj.h"
#include "esp_log.h"
#include "lvgl.h"
#include "screens.h"
#include "styles.h"
#include "ui.h"
#include "vars.h"
#include "app_mqtt.h"
#include "discovery.h"
#include "button_config.h"
#include "mqtt_vars.h"
#include "nvs.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* Check if WiFi is enabled (ESP-Hosted for ESP32-P4) */
#if defined(CONFIG_ESP_HOSTED_ENABLED)
#define WIFI_ENABLED 1
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#else
#define WIFI_ENABLED 0
#endif

#define USER_SETTINGS_NVS_NAMESPACE "user_settings"

#if WIFI_ENABLED
static char wifi_connected_ip[20] = {0};
static int wifi_retry_count = 0;
#define WIFI_MAX_RETRIES 5
static esp_timer_handle_t s_rssi_timer = NULL;

static void rssi_poll_cb(void *arg) {
  wifi_ap_record_t ap;
  if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) {
    bsp_display_lock(0);
    set_var_wifi_rssi((int32_t)ap.rssi);
    bsp_display_unlock();
  }
}

static void start_rssi_polling(void) {
  wifi_ap_record_t ap;
  int32_t initial_rssi = -50;
  if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) {
    initial_rssi = (int32_t)ap.rssi;
  }
  bsp_display_lock(0);
  set_var_wifi_rssi(initial_rssi);
  bsp_display_unlock();

  if (s_rssi_timer == NULL) {
    esp_timer_create_args_t args = {
      .callback = rssi_poll_cb,
      .name = "rssi_poll"
    };
    esp_timer_create(&args, &s_rssi_timer);
  }
  esp_timer_start_periodic(s_rssi_timer, 10ULL * 1000000ULL);
}

static void stop_rssi_polling(void) {
  if (s_rssi_timer) {
    esp_timer_stop(s_rssi_timer);
  }
  bsp_display_lock(0);
  set_var_wifi_rssi(-100);
  bsp_display_unlock();
}

/**
 * @brief WiFi event handler to update UI on connection status changes
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  if (event_base == WIFI_EVENT) {
    switch (event_id) {
      case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI("WIFI_EVENT", "Connected to AP");
        wifi_retry_count = 0;
        bsp_display_lock(0);
        lv_label_set_text(objects.label_wifi_connection_status, "Status: Connected, getting IP...");
        bsp_display_unlock();
        break;
      case WIFI_EVENT_STA_DISCONNECTED: {
        wifi_event_sta_disconnected_t *disconn = (wifi_event_sta_disconnected_t *)event_data;
        ESP_LOGW("WIFI_EVENT", "Disconnected from AP, reason: %d", disconn->reason);
        stop_rssi_polling();
        if (wifi_retry_count < WIFI_MAX_RETRIES) {
          wifi_retry_count++;
          ESP_LOGI("WIFI_EVENT", "Retrying connection (%d/%d)...", wifi_retry_count, WIFI_MAX_RETRIES);
          esp_wifi_connect();
          char retry_msg[64];
          snprintf(retry_msg, sizeof(retry_msg), "Status: Retry %d/%d...", wifi_retry_count, WIFI_MAX_RETRIES);
          bsp_display_lock(0);
          lv_label_set_text(objects.label_wifi_connection_status, retry_msg);
          bsp_display_unlock();
        } else {
          ESP_LOGW("WIFI_EVENT", "Max retries reached, giving up");
          bsp_display_lock(0);
          lv_label_set_text(objects.label_wifi_connection_status, "Status: Connection failed");
          bsp_display_unlock();
        }
        break;
      }
      default:
        break;
    }
  } else if (event_base == IP_EVENT) {
    if (event_id == IP_EVENT_STA_GOT_IP) {
      ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
      snprintf(wifi_connected_ip, sizeof(wifi_connected_ip), IPSTR, IP2STR(&event->ip_info.ip));
      ESP_LOGI("WIFI_EVENT", "Got IP: %s", wifi_connected_ip);
      char status_msg[64];
      snprintf(status_msg, sizeof(status_msg), "Connected: %s", wifi_connected_ip);
      bsp_display_lock(0);
      lv_label_set_text(objects.label_wifi_connection_status, status_msg);
      bsp_display_unlock();
      start_rssi_polling();
      /* Start mDNS (enables .local resolution) then connect MQTT */
      discovery_mdns_init();
      mqtt_client_connect();
    }
  }
}

/**
 * @brief Initialize WiFi event handlers for UI updates
 * Call this once after WiFi is initialized
 */
void wifi_event_handler_init(void) {
  esp_event_handler_instance_t wifi_handler;
  esp_event_handler_instance_t ip_handler;

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                       ESP_EVENT_ANY_ID,
                                                       &wifi_event_handler,
                                                       NULL,
                                                       &wifi_handler));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                       IP_EVENT_STA_GOT_IP,
                                                       &wifi_event_handler,
                                                       NULL,
                                                       &ip_handler));
  ESP_LOGI("ACTIONS", "WiFi event handlers registered for UI updates");
}
#endif

static const char *TAG = "ACTIONS";

static int32_t last_persisted_brightness = -1;

void action_change_screen_brightness(lv_event_t *e) {
  lv_obj_t *slider = lv_event_get_target(e);
  int32_t value = lv_slider_get_value(slider);
  bsp_display_brightness_set(value);

  /* Persist only when the value actually changes (event is LV_EVENT_PRESSING,
     so this fires repeatedly while dragging — deduplicate to reduce NVS writes) */
  if (value != last_persisted_brightness) {
    last_persisted_brightness = value;
    nvs_handle_t nvs;
    if (nvs_open(USER_SETTINGS_NVS_NAMESPACE, NVS_READWRITE, &nvs) == ESP_OK) {
      nvs_set_u8(nvs, "brightness", (uint8_t)value);
      nvs_commit(nvs);
      nvs_close(nvs);
    }
  }
}

void action_change_theme(lv_event_t *e) {
  set_var_user_settings_changed(true);
  int themeIndex = (int)lv_event_get_user_data(e);
  set_var_selected_theme(themeIndex);

  nvs_handle_t nvs;
  if (nvs_open(USER_SETTINGS_NVS_NAMESPACE, NVS_READWRITE, &nvs) == ESP_OK) {
    nvs_set_u8(nvs, "theme", (uint8_t)themeIndex);
    nvs_commit(nvs);
    nvs_close(nvs);
  }
}

void action_temperature_unit_change(lv_event_t *e) {
  set_var_user_settings_changed(true);
  int unit = (int)lv_event_get_user_data(e);
  set_var_temperature_unit(unit);

  nvs_handle_t nvs;
  if (nvs_open(USER_SETTINGS_NVS_NAMESPACE, NVS_READWRITE, &nvs) == ESP_OK) {
    nvs_set_u8(nvs, "tempUnit", (uint8_t)unit);
    nvs_commit(nvs);
    nvs_close(nvs);
  }
}

/* Nav button lookup: nav_buttons[page][btn] where btn index matches page index
 * for the "active" button (e.g. nav_buttons[0][0] = home page's home button).
 * Populated by init_nav_lookup() after ui_init(). */
#define NUM_PAGES 6
static lv_obj_t *nav_buttons[NUM_PAGES][NUM_PAGES];
static lv_obj_t *page_screens[NUM_PAGES];

void init_nav_lookup(void) {
  page_screens[0] = objects.page_home;
  page_screens[1] = objects.page_trailer;
  page_screens[2] = objects.page_power_management;
  page_screens[3] = objects.page_air_quality;
  page_screens[4] = objects.page_water;
  page_screens[5] = objects.page_settings;

  /* Home page nav buttons */
  nav_buttons[0][0] = objects.widget_home_page_bottom_nav_bar__bottom_nav_bar_button_home;
  nav_buttons[0][1] = objects.widget_home_page_bottom_nav_bar__bottom_nav_bar_button_trailer;
  nav_buttons[0][2] = objects.widget_home_page_bottom_nav_bar__bottom_nav_bar_button_power;
  nav_buttons[0][3] = objects.widget_home_page_bottom_nav_bar__bottom_nav_bar_button_air_quality;
  nav_buttons[0][4] = objects.widget_home_page_bottom_nav_bar__bottom_nav_bar_button_water;
  nav_buttons[0][5] = objects.widget_home_page_bottom_nav_bar__bottom_nav_bar_button_settings;

  /* Trailer page nav buttons */
  nav_buttons[1][0] = objects.widget_trailer_page_bottom_nav_bar__bottom_nav_bar_button_home;
  nav_buttons[1][1] = objects.widget_trailer_page_bottom_nav_bar__bottom_nav_bar_button_trailer;
  nav_buttons[1][2] = objects.widget_trailer_page_bottom_nav_bar__bottom_nav_bar_button_power;
  nav_buttons[1][3] = objects.widget_trailer_page_bottom_nav_bar__bottom_nav_bar_button_air_quality;
  nav_buttons[1][4] = objects.widget_trailer_page_bottom_nav_bar__bottom_nav_bar_button_water;
  nav_buttons[1][5] = objects.widget_trailer_page_bottom_nav_bar__bottom_nav_bar_button_settings;

  /* Power management page nav buttons */
  nav_buttons[2][0] = objects.widget_power_management_page_bottom_nav_bar__bottom_nav_bar_button_home;
  nav_buttons[2][1] = objects.widget_power_management_page_bottom_nav_bar__bottom_nav_bar_button_trailer;
  nav_buttons[2][2] = objects.widget_power_management_page_bottom_nav_bar__bottom_nav_bar_button_power;
  nav_buttons[2][3] = objects.widget_power_management_page_bottom_nav_bar__bottom_nav_bar_button_air_quality;
  nav_buttons[2][4] = objects.widget_power_management_page_bottom_nav_bar__bottom_nav_bar_button_water;
  nav_buttons[2][5] = objects.widget_power_management_page_bottom_nav_bar__bottom_nav_bar_button_settings;

  /* Air quality page nav buttons */
  nav_buttons[3][0] = objects.widget_air_quality_page_bottom_nav_bar__bottom_nav_bar_button_home;
  nav_buttons[3][1] = objects.widget_air_quality_page_bottom_nav_bar__bottom_nav_bar_button_trailer;
  nav_buttons[3][2] = objects.widget_air_quality_page_bottom_nav_bar__bottom_nav_bar_button_power;
  nav_buttons[3][3] = objects.widget_air_quality_page_bottom_nav_bar__bottom_nav_bar_button_air_quality;
  nav_buttons[3][4] = objects.widget_air_quality_page_bottom_nav_bar__bottom_nav_bar_button_water;
  nav_buttons[3][5] = objects.widget_air_quality_page_bottom_nav_bar__bottom_nav_bar_button_settings;

  /* Water page nav buttons */
  nav_buttons[4][0] = objects.widget_water_page_bottom_nav_bar__bottom_nav_bar_button_home;
  nav_buttons[4][1] = objects.widget_water_page_bottom_nav_bar__bottom_nav_bar_button_trailer;
  nav_buttons[4][2] = objects.widget_water_page_bottom_nav_bar__bottom_nav_bar_button_power;
  nav_buttons[4][3] = objects.widget_water_page_bottom_nav_bar__bottom_nav_bar_button_air_quality;
  nav_buttons[4][4] = objects.widget_water_page_bottom_nav_bar__bottom_nav_bar_button_water;
  nav_buttons[4][5] = objects.widget_water_page_bottom_nav_bar__bottom_nav_bar_button_settings;

  /* Settings page nav buttons */
  nav_buttons[5][0] = objects.widget_settings_page_bottom_nav_bar__bottom_nav_bar_button_home;
  nav_buttons[5][1] = objects.widget_settings_page_bottom_nav_bar__bottom_nav_bar_button_trailer;
  nav_buttons[5][2] = objects.widget_settings_page_bottom_nav_bar__bottom_nav_bar_button_power;
  nav_buttons[5][3] = objects.widget_settings_page_bottom_nav_bar__bottom_nav_bar_button_air_quality;
  nav_buttons[5][4] = objects.widget_settings_page_bottom_nav_bar__bottom_nav_bar_button_water;
  nav_buttons[5][5] = objects.widget_settings_page_bottom_nav_bar__bottom_nav_bar_button_settings;

  /* Set home button checked on the home page (initial state) */
  lv_obj_add_state(nav_buttons[0][0], LV_STATE_CHECKED);
}

/* Map EEZ Studio user_data values to internal page indices.
 * EEZ values: 0=home, 1=trailer, 2=power, 3=air_quality, 4=water, 6=settings
 * (5 was the removed entertainment page) */
static int eez_to_page_idx(int eez_id) {
  switch (eez_id) {
    case 0: return 0;
    case 1: return 1;
    case 2: return 2;
    case 3: return 3;
    case 4: return 4;
    default: return 5; /* settings (user_data=6) */
  }
}

void action_change_screen(lv_event_t *e) {
  int eez_id = (int)lv_event_get_user_data(e);
  int to_idx = eez_to_page_idx(eez_id);

  /* Clear all nav buttons on the target page, then set the active one */
  for (int btn = 0; btn < NUM_PAGES; btn++) {
    lv_obj_clear_state(nav_buttons[to_idx][btn], LV_STATE_CHECKED);
  }
  lv_obj_add_state(nav_buttons[to_idx][to_idx], LV_STATE_CHECKED);

  /* Animated screen transition — fast fade */
  lv_scr_load_anim(page_screens[to_idx], LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, false);
}

void action_rotate_screen(lv_event_t *e) {
  uint32_t currentDegrees = get_var_rotation_degrees();
  lv_disp_t *disp = lv_disp_get_default();
  int *btn_no_ptr = (int *)lv_event_get_user_data(e);
  if (btn_no_ptr == 0) {
    if (currentDegrees == 0) {
      bsp_display_rotate(disp, LV_DISPLAY_ROTATION_90);
      set_var_rotation_degrees(90);
    } else if (currentDegrees == 90) {
      bsp_display_rotate(disp, LV_DISPLAY_ROTATION_180);
      set_var_rotation_degrees(180);
    } else if (currentDegrees == 180) {
      bsp_display_rotate(disp, LV_DISPLAY_ROTATION_270);
      set_var_rotation_degrees(270);
    } else {
      bsp_display_rotate(disp, LV_DISPLAY_ROTATION_0);
      set_var_rotation_degrees(0);
    }
  } else {
    if (currentDegrees == 270) {
      bsp_display_rotate(disp, LV_DISPLAY_ROTATION_180);
      set_var_rotation_degrees(180);
    } else if (currentDegrees == 180) {
      bsp_display_rotate(disp, LV_DISPLAY_ROTATION_90);
      set_var_rotation_degrees(90);
    } else if (currentDegrees == 90) {
      bsp_display_rotate(disp, LV_DISPLAY_ROTATION_0);
      set_var_rotation_degrees(0);
    } else {
      bsp_display_rotate(disp, LV_DISPLAY_ROTATION_270);
      set_var_rotation_degrees(270);
    }
  }
}

/* Build a CAN-frame JSON payload matching Headwaters' publishCanMessage()
 * format and publish it to "can/outbound". Data bytes are serialized as
 * 8 arrays of 8 bits (MSB-first), padded to 8 bytes. */
static void publish_can_frame(uint16_t can_id, const uint8_t *data, int dlc) {
  char payload[512];
  int n = snprintf(payload, sizeof(payload),
                   "{\"identifier\":\"0x%x\",\"data_length_code\":%d,\"data\":[",
                   can_id, dlc);
  for (int b = 0; b < 8; b++) {
    uint8_t byte = (b < dlc) ? data[b] : 0;
    n += snprintf(payload + n, sizeof(payload) - n,
                  "%s[%d,%d,%d,%d,%d,%d,%d,%d]",
                  b == 0 ? "" : ",",
                  (byte >> 7) & 1, (byte >> 6) & 1, (byte >> 5) & 1,
                  (byte >> 4) & 1, (byte >> 3) & 1, (byte >> 2) & 1,
                  (byte >> 1) & 1, (byte >> 0) & 1);
  }
  n += snprintf(payload + n, sizeof(payload) - n,
                "],\"extd\":0,\"rtr\":0,\"ss\":0,\"self\":0}");
  if (mqtt_client_publish("can/outbound", payload, n) < 0) {
    ESP_LOGW(TAG, "MQTT not connected, cannot publish CAN 0x%x", can_id);
  }
}

void action_send_device_command(lv_event_t *e) {
  int btn = (int)lv_event_get_user_data(e);
  if (btn < 1 || btn > NUM_BUTTONS) return;
  const btn_config_t *cfg = &g_buttons[btn - 1];
  if (cfg->instance > 2 || cfg->channel > 7) return;

  uint8_t data[1] = { cfg->channel };
  uint32_t can_id;
  switch ((module_type_t)cfg->module_type) {
    case MOD_TORRENT:    can_id = TORRENT_TOGGLE_ID[cfg->instance];    break;
    case MOD_SWITCHBACK: can_id = SWITCHBACK_TOGGLE_ID[cfg->instance]; break;
    default:
      ESP_LOGI(TAG, "button %d unmapped, ignoring press", btn);
      return;
  }
  publish_can_frame(can_id, data, 1);
}

void action_change_desired_temperature(lv_event_t *e) {
  int new_desired_temperature = lv_arc_get_value(objects.arc_thermostat);
  set_var_desired_temperature(new_desired_temperature);
}

void action_go_to_preset(lv_event_t *e) { ESP_LOGE(TAG, "Got here prset"); }

/* NOTE: These have to match the order of the drop down items in the UI */
static const char *timezoneItems[] = {
    "ASKT9AKDT,M3.2.0/2:00:00,M11.1.0/2:00:00",
    "CST6CDT,M3.2.0/2:00:00,M11.1.0/2:00:00",
    "MST7MDT,M3.2.0/2:00:00,M11.1.0/2:00:00",
    "HST11HDT,M3.2.0/2:00:00,M11.1.0/2:00:00",
    "PST8PDT,M3.2.0/2:00:00,M11.1.0/2:00:00",
    "EST5EDT,M3.2.0/2:00:00,M11.1.0/2:00:00",
    "MST7",
};
#define TIMEZONE_COUNT (sizeof(timezoneItems) / sizeof(timezoneItems[0]))

void restore_user_settings(void) {
  nvs_handle_t nvs;
  if (nvs_open(USER_SETTINGS_NVS_NAMESPACE, NVS_READONLY, &nvs) != ESP_OK)
    return;

  uint8_t tzIndex = 0;
  if (nvs_get_u8(nvs, "tzIndex", &tzIndex) == ESP_OK && tzIndex < TIMEZONE_COUNT) {
    lv_dropdown_set_selected(objects.drop_down_selected_time_zone, tzIndex);
    set_var_current_time_zone_string(timezoneItems[tzIndex]);
    ESP_LOGI(TAG, "Restored timezone index %d", tzIndex);
  }

  uint8_t theme = 0;
  if (nvs_get_u8(nvs, "theme", &theme) == ESP_OK) {
    set_var_selected_theme(theme);
    ESP_LOGI(TAG, "Restored theme %d", theme);
  }

  uint8_t brightness = 0;
  if (nvs_get_u8(nvs, "brightness", &brightness) == ESP_OK) {
    bsp_display_brightness_set(brightness);
    lv_slider_set_value(objects.slider_screen_brightness, brightness, LV_ANIM_OFF);
    ESP_LOGI(TAG, "Restored brightness %d", brightness);
  }

  int8_t timeout = 0;
  if (nvs_get_i8(nvs, "timeout", &timeout) == ESP_OK) {
    set_var_screen_timeout_value(timeout);
    ESP_LOGI(TAG, "Restored screen timeout %d", timeout);
  }

  uint8_t tempUnit = 0;
  if (nvs_get_u8(nvs, "tempUnit", &tempUnit) == ESP_OK) {
    set_var_temperature_unit(tempUnit);
    ESP_LOGI(TAG, "Restored temp unit %d", tempUnit);
  }

  nvs_close(nvs);
}
void action_timezone_change(lv_event_t *e) {
  uint8_t selectedIndex =
      lv_dropdown_get_selected(objects.drop_down_selected_time_zone);
  set_var_user_settings_changed(true);
  set_var_current_time_zone_string(timezoneItems[selectedIndex]);

  /* Immediately refresh the clock display with the new timezone */
  update_clock_display();

  /* Persist to NVS */
  nvs_handle_t nvs;
  if (nvs_open(USER_SETTINGS_NVS_NAMESPACE, NVS_READWRITE, &nvs) == ESP_OK) {
    nvs_set_u8(nvs, "tzIndex", selectedIndex);
    nvs_commit(nvs);
    nvs_close(nvs);
  }
}

void action_timeout_changed(lv_event_t *e) {
  set_var_user_settings_changed(true);
  int currentTimeoutValue = get_var_screen_timeout_value();
  int eventValue = (int)lv_event_get_user_data(e);
  if (eventValue == 0) {
    currentTimeoutValue--;
  } else {
    currentTimeoutValue++;
  }
  /* Clamp to 0-60 range (0 = disabled, 1-60 = minutes) */
  if (currentTimeoutValue < 0) currentTimeoutValue = 0;
  if (currentTimeoutValue > 60) currentTimeoutValue = 60;
  set_var_screen_timeout_value(currentTimeoutValue);

  nvs_handle_t nvs;
  if (nvs_open(USER_SETTINGS_NVS_NAMESPACE, NVS_READWRITE, &nvs) == ESP_OK) {
    nvs_set_i8(nvs, "timeout", (int8_t)currentTimeoutValue);
    nvs_commit(nvs);
    nvs_close(nvs);
  }
}

void action_show_device_brightness_dialog(lv_event_t *e) {
  int eventValue = (int)lv_event_get_user_data(e);
  set_var_current_device_brightness_identifier(eventValue);

  /* Initialise slider to the device's last known brightness (0-255 → 0-100) */
  int32_t brightness = get_var_device_brightness(eventValue);
  int32_t slider_val = (brightness > 0) ? brightness * 100 / 255 : 0;
  lv_slider_set_value(objects.slider_device_brightness_level, slider_val, LV_ANIM_OFF);

  lv_obj_clear_flag(objects.container_modal_background, LV_OBJ_FLAG_HIDDEN);
}

void action_close_dialog(lv_event_t *e) {
  lv_obj_add_flag(objects.container_modal_background, LV_OBJ_FLAG_HIDDEN);
  set_var_current_device_brightness_identifier(-1);
}

void action_set_device_brightness_level(lv_event_t *e) {
  /* EEZ Studio binds this to LV_EVENT_PRESSING — intentionally a no-op.
   * We send the brightness command only on release via the callback
   * registered by init_brightness_slider(). */
  (void)e;
}

/**
 * Called on LV_EVENT_RELEASED for the brightness slider.
 * Sends the brightness command only when the user lifts their finger.
 */
static void brightness_slider_released(lv_event_t *e) {
  int32_t device_id = get_var_current_device_brightness_identifier();
  if (device_id < 0) return;
  lv_obj_t *slider = lv_event_get_target(e);
  int32_t slider_val = lv_slider_get_value(slider);
  int32_t brightness = slider_val * 255 / 100; /* scale 0-100 to 0-255 */
  char topic[64];
  snprintf(topic, sizeof(topic), "local/lights/%d/brightness", (int)device_id);
  char payload[32];
  snprintf(payload, sizeof(payload), "{\"brightness\":%d}", (int)brightness);
  mqtt_client_publish(topic, payload, strlen(payload));
}

void init_brightness_slider(void) {
  lv_obj_add_event_cb(objects.slider_device_brightness_level,
                       brightness_slider_released, LV_EVENT_RELEASED, NULL);
}

/* ===========================================================================
 * Button configuration UI — PageButtonEdit + PageDeviceAssign
 * (ported from TrailCurrentMilepost/main/actions.c)
 * ===========================================================================*/

static int s_active_screen_id = 0; /* updated by navigate_* handlers */

static bool s_cursor_visible = true;

static void update_edit_text_display(void) {
  const char *txt = get_var_edit_label_text();
  if (!txt) txt = "";
  char buf[BTN_LABEL_MAX + 2];
  size_t n = strnlen(txt, BTN_LABEL_MAX - 1);
  memcpy(buf, txt, n);
  if (s_cursor_visible) buf[n++] = '|';
  buf[n] = '\0';
  if (objects.lbl_button_edit_text) {
    lv_label_set_text(objects.lbl_button_edit_text, buf);
  }
}

static void cursor_blink_cb(lv_timer_t *t) {
  (void)t;
  if (s_active_screen_id != SCREEN_ID_PAGE_BUTTON_EDIT) return;
  s_cursor_visible = !s_cursor_visible;
  update_edit_text_display();
}

static void start_cursor_blink(void) {
  static lv_timer_t *s_cursor_timer = NULL;
  if (s_cursor_timer == NULL) {
    s_cursor_timer = lv_timer_create(cursor_blink_cb, 500, NULL);
  }
  s_cursor_visible = true;
  update_edit_text_display();
}

static lv_obj_t *channel_dropdown(int ch) {
  switch (ch) {
    case 0: return objects.dd_channel0_button;
    case 1: return objects.dd_channel1_button;
    case 2: return objects.dd_channel2_button;
    case 3: return objects.dd_channel3_button;
    case 4: return objects.dd_channel4_button;
    case 5: return objects.dd_channel5_button;
    case 6: return objects.dd_channel6_button;
    case 7: return objects.dd_channel7_button;
    default: return NULL;
  }
}

static lv_obj_t *instance_button(int inst) {
  switch (inst) {
    case 0: return objects.btn_device_instance0;
    case 1: return objects.btn_device_instance1;
    case 2: return objects.btn_device_instance2;
    default: return NULL;
  }
}

static void refresh_channel_dropdowns(void) {
  module_type_t mod = (module_type_t)get_var_assign_module_type();
  uint8_t inst = (uint8_t)get_var_assign_instance();
  for (int ch = 0; ch < 8; ch++) {
    lv_obj_t *dd = channel_dropdown(ch);
    if (!dd) continue;
    uint8_t btn = button_config_find(mod, inst, (uint8_t)ch);
    lv_dropdown_set_selected(dd, btn);
  }
}

static void highlight_active_instance(void) {
  int active = get_var_assign_instance();
  for (int i = 0; i < 3; i++) {
    lv_obj_t *b = instance_button(i);
    if (!b) continue;
    if (i == active) lv_obj_add_state(b, LV_STATE_CHECKED);
    else              lv_obj_clear_state(b, LV_STATE_CHECKED);
  }
}

static lv_obj_t *edit_device_button(int mod) {
  switch (mod) {
    case MOD_NONE:       return objects.btn_edit_device_none;
    case MOD_TORRENT:    return objects.btn_edit_device_torrent;
    case MOD_SWITCHBACK: return objects.btn_edit_device_switchback;
    default:             return NULL;
  }
}

static lv_obj_t *edit_addr_button(int addr) {
  switch (addr) {
    case 0: return objects.btn_edit_addr0;
    case 1: return objects.btn_edit_addr1;
    case 2: return objects.btn_edit_addr2;
    default: return NULL;
  }
}

static void highlight_edit_device_type(int mod) {
  for (int m = MOD_NONE; m <= MOD_SWITCHBACK; m++) {
    lv_obj_t *b = edit_device_button(m);
    if (!b) continue;
    if (m == mod) lv_obj_add_state(b, LV_STATE_CHECKED);
    else          lv_obj_clear_state(b, LV_STATE_CHECKED);
  }
}

static void highlight_edit_address(int addr) {
  for (int i = 0; i < 3; i++) {
    lv_obj_t *b = edit_addr_button(i);
    if (!b) continue;
    if (i == addr) lv_obj_add_state(b, LV_STATE_CHECKED);
    else           lv_obj_clear_state(b, LV_STATE_CHECKED);
  }
}

static lv_obj_t *edit_ch_button(int ch) {
  switch (ch) {
    case 0: return objects.btn_edit_ch0;
    case 1: return objects.btn_edit_ch1;
    case 2: return objects.btn_edit_ch2;
    case 3: return objects.btn_edit_ch3;
    case 4: return objects.btn_edit_ch4;
    case 5: return objects.btn_edit_ch5;
    case 6: return objects.btn_edit_ch6;
    case 7: return objects.btn_edit_ch7;
    default: return NULL;
  }
}

static void highlight_edit_channel(int ch) {
  for (int i = 0; i < 8; i++) {
    lv_obj_t *b = edit_ch_button(i);
    if (!b) continue;
    if (i == ch) lv_obj_add_state(b, LV_STATE_CHECKED);
    else         lv_obj_clear_state(b, LV_STATE_CHECKED);
  }
}

static lv_obj_t *icon_slot(int i) {
  /* Indexed lookup via a static table saves ~300 lines of switch. */
  static lv_obj_t **slots = NULL;
  static lv_obj_t *table[NUM_CURATED_ICONS];
  if (!slots) {
    lv_obj_t **p = table;
    *p++ = objects.btn_icon_slot00; *p++ = objects.btn_icon_slot01;
    *p++ = objects.btn_icon_slot02; *p++ = objects.btn_icon_slot03;
    *p++ = objects.btn_icon_slot04; *p++ = objects.btn_icon_slot05;
    *p++ = objects.btn_icon_slot06; *p++ = objects.btn_icon_slot07;
    *p++ = objects.btn_icon_slot08; *p++ = objects.btn_icon_slot09;
    *p++ = objects.btn_icon_slot10; *p++ = objects.btn_icon_slot11;
    *p++ = objects.btn_icon_slot12; *p++ = objects.btn_icon_slot13;
    *p++ = objects.btn_icon_slot14; *p++ = objects.btn_icon_slot15;
    *p++ = objects.btn_icon_slot16; *p++ = objects.btn_icon_slot17;
    *p++ = objects.btn_icon_slot18; *p++ = objects.btn_icon_slot19;
    *p++ = objects.btn_icon_slot20; *p++ = objects.btn_icon_slot21;
    *p++ = objects.btn_icon_slot22; *p++ = objects.btn_icon_slot23;
    *p++ = objects.btn_icon_slot24; *p++ = objects.btn_icon_slot25;
    *p++ = objects.btn_icon_slot26; *p++ = objects.btn_icon_slot27;
    *p++ = objects.btn_icon_slot28; *p++ = objects.btn_icon_slot29;
    *p++ = objects.btn_icon_slot30; *p++ = objects.btn_icon_slot31;
    *p++ = objects.btn_icon_slot32; *p++ = objects.btn_icon_slot33;
    *p++ = objects.btn_icon_slot34; *p++ = objects.btn_icon_slot35;
    *p++ = objects.btn_icon_slot36; *p++ = objects.btn_icon_slot37;
    *p++ = objects.btn_icon_slot38; *p++ = objects.btn_icon_slot39;
    *p++ = objects.btn_icon_slot40; *p++ = objects.btn_icon_slot41;
    *p++ = objects.btn_icon_slot42; *p++ = objects.btn_icon_slot43;
    *p++ = objects.btn_icon_slot44; *p++ = objects.btn_icon_slot45;
    *p++ = objects.btn_icon_slot46; *p++ = objects.btn_icon_slot47;
    *p++ = objects.btn_icon_slot48; *p++ = objects.btn_icon_slot49;
    *p++ = objects.btn_icon_slot50; *p++ = objects.btn_icon_slot51;
    *p++ = objects.btn_icon_slot52; *p++ = objects.btn_icon_slot53;
    *p++ = objects.btn_icon_slot54; *p++ = objects.btn_icon_slot55;
    *p++ = objects.btn_icon_slot56; *p++ = objects.btn_icon_slot57;
    *p++ = objects.btn_icon_slot58; *p++ = objects.btn_icon_slot59;
    *p++ = objects.btn_icon_slot60; *p++ = objects.btn_icon_slot61;
    *p++ = objects.btn_icon_slot62; *p++ = objects.btn_icon_slot63;
    *p++ = objects.btn_icon_slot64; *p++ = objects.btn_icon_slot65;
    *p++ = objects.btn_icon_slot66; *p++ = objects.btn_icon_slot67;
    *p++ = objects.btn_icon_slot68; *p++ = objects.btn_icon_slot69;
    *p++ = objects.btn_icon_slot70; *p++ = objects.btn_icon_slot71;
    *p++ = objects.btn_icon_slot72; *p++ = objects.btn_icon_slot73;
    *p++ = objects.btn_icon_slot74; *p++ = objects.btn_icon_slot75;
    *p++ = objects.btn_icon_slot76; *p++ = objects.btn_icon_slot77;
    *p++ = objects.btn_icon_slot78; *p++ = objects.btn_icon_slot79;
    *p++ = objects.btn_icon_slot80; *p++ = objects.btn_icon_slot81;
    *p++ = objects.btn_icon_slot82; *p++ = objects.btn_icon_slot83;
    slots = table;
  }
  return (i >= 0 && i < NUM_CURATED_ICONS) ? slots[i] : NULL;
}

static void highlight_selected_icon(uint16_t selected_cp) {
  for (int i = 0; i < NUM_CURATED_ICONS; i++) {
    lv_obj_t *slot = icon_slot(i);
    if (!slot) continue;
    if (CURATED_ICONS[i] == selected_cp) lv_obj_add_state(slot, LV_STATE_CHECKED);
    else                                  lv_obj_clear_state(slot, LV_STATE_CHECKED);
  }
}

void action_navigate_to_button_edit(lv_event_t *e) {
  int btn = (int)(intptr_t)lv_event_get_user_data(e);
  if (btn < 1 || btn > NUM_BUTTONS) return;

  const btn_config_t *cfg = &g_buttons[btn - 1];
  set_var_edit_btn_number(btn);
  set_var_edit_label_text(cfg->label);
  set_var_edit_icon_codepoint((int32_t)cfg->icon_codepoint);
  set_var_assign_module_type((int32_t)cfg->module_type);
  set_var_assign_instance((int32_t)cfg->instance);
  set_var_edit_channel((int32_t)cfg->channel);

  if (objects.lbl_button_edit_header)
    lv_label_set_text_fmt(objects.lbl_button_edit_header, "Button %d", btn);
  highlight_selected_icon(cfg->icon_codepoint);
  highlight_edit_device_type((int)cfg->module_type);
  highlight_edit_address((int)cfg->instance);
  highlight_edit_channel((int)cfg->channel);

  s_active_screen_id = SCREEN_ID_PAGE_BUTTON_EDIT;
  lv_scr_load_anim(objects.page_button_edit, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, false);
  start_cursor_blink();
}

void action_navigate_to_device_assign(lv_event_t *e) {
  int mod = (int)(intptr_t)lv_event_get_user_data(e);
  if (mod != MOD_TORRENT && mod != MOD_SWITCHBACK) return;

  set_var_assign_module_type(mod);
  set_var_assign_instance(0);

  const char *hdr = (mod == MOD_TORRENT) ? "Assign Torrent Channels"
                                          : "Assign Switchback Relays";
  if (objects.lbl_device_assign_header)
    lv_label_set_text(objects.lbl_device_assign_header, hdr);

  refresh_channel_dropdowns();
  highlight_active_instance();

  s_active_screen_id = SCREEN_ID_PAGE_DEVICE_ASSIGN;
  lv_scr_load_anim(objects.page_device_assign, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, false);
}

void action_select_button_icon(lv_event_t *e) {
  uint32_t cp = (uint32_t)(intptr_t)lv_event_get_user_data(e);
  if (cp == 0) return;
  set_var_edit_icon_codepoint((int32_t)cp);
  highlight_selected_icon((uint16_t)cp);
}

void action_save_button_appearance(lv_event_t *e) {
  (void)e;
  int btn = get_var_edit_btn_number();
  const char *lbl = get_var_edit_label_text();
  uint16_t icon = (uint16_t)get_var_edit_icon_codepoint();

  if (btn >= 1 && btn <= NUM_BUTTONS) {
    module_type_t mod = (module_type_t)get_var_assign_module_type();
    uint8_t inst = (uint8_t)get_var_assign_instance();
    button_config_set_appearance((uint8_t)btn,
                                 (lbl && lbl[0]) ? lbl : NULL, icon);
    uint8_t ch = (uint8_t)get_var_edit_channel();
    button_config_set_mapping((uint8_t)btn, mod, inst, ch);
    button_config_apply_to_ui();
  }

  s_active_screen_id = SCREEN_ID_PAGE_SETTINGS;
  lv_scr_load_anim(objects.page_settings, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, false);
}

void action_assign_channel(lv_event_t *e) {
  int ch = (int)(intptr_t)lv_event_get_user_data(e);
  if (ch < 0 || ch >= 8) return;

  lv_obj_t *dd = channel_dropdown(ch);
  if (!dd) return;
  uint16_t sel = lv_dropdown_get_selected(dd); /* 0=None, 1..8=Button N */

  module_type_t mod = (module_type_t)get_var_assign_module_type();
  uint8_t inst = (uint8_t)get_var_assign_instance();

  button_config_assign(mod, inst, (uint8_t)ch, (uint8_t)sel);
  button_config_apply_to_ui();
  refresh_channel_dropdowns();
}

void action_select_device_instance(lv_event_t *e) {
  int inst = (int)(intptr_t)lv_event_get_user_data(e);
  if (inst < 0 || inst > 2) return;
  set_var_assign_instance(inst);
  refresh_channel_dropdowns();
  highlight_active_instance();
}

void action_reset_all_buttons(lv_event_t *e) {
  (void)e;
  button_config_reset_all();
  button_config_apply_to_ui();
}

void action_clear_button(lv_event_t *e) {
  (void)e;
  int btn = get_var_edit_btn_number();
  if (btn >= 1 && btn <= NUM_BUTTONS) {
    button_config_clear((uint8_t)btn);
    button_config_apply_to_ui();
  }
  s_active_screen_id = SCREEN_ID_PAGE_SETTINGS;
  lv_scr_load_anim(objects.page_settings, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, false);
}

void action_select_edit_device_type(lv_event_t *e) {
  int mod = (int)(intptr_t)lv_event_get_user_data(e);
  if (mod < MOD_NONE || mod > MOD_SWITCHBACK) return;
  set_var_assign_module_type(mod);
  highlight_edit_device_type(mod);
}

void action_select_edit_address(lv_event_t *e) {
  int addr = (int)(intptr_t)lv_event_get_user_data(e);
  if (addr < 0 || addr > 2) return;
  set_var_assign_instance(addr);
  highlight_edit_address(addr);
}

void action_select_edit_channel(lv_event_t *e) {
  int ch = (int)(intptr_t)lv_event_get_user_data(e);
  if (ch < 0 || ch > 7) return;
  set_var_edit_channel(ch);
  highlight_edit_channel(ch);
}

/* Keyboard event interceptor — drives EditLabelText buffer since
 * PageButtonEdit has no textarea widget. Wired once from main after ui_init. */
static void kb_button_edit_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *kb = lv_event_get_target(e);

  if (code == LV_EVENT_READY) { action_save_button_appearance(NULL); return; }
  if (code == LV_EVENT_CANCEL) {
    s_active_screen_id = SCREEN_ID_PAGE_SETTINGS;
    lv_scr_load_anim(objects.page_settings, LV_SCR_LOAD_ANIM_FADE_ON, 150, 0, false);
    return;
  }
  if (code != LV_EVENT_VALUE_CHANGED) return;

  uint16_t btn_id = lv_btnmatrix_get_selected_btn(kb);
  if (btn_id == LV_BTNMATRIX_BTN_NONE) return;
  const char *txt = lv_btnmatrix_get_btn_text(kb, btn_id);
  if (!txt) return;

  if (strcmp(txt, "abc") == 0 || strcmp(txt, "ABC") == 0 ||
      strcmp(txt, "1#") == 0  || strcmp(txt, "Abc") == 0) return;

  char buf[BTN_LABEL_MAX];
  const char *cur = get_var_edit_label_text();
  strncpy(buf, cur ? cur : "", sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = '\0';
  size_t len = strlen(buf);

  if (strcmp(txt, LV_SYMBOL_BACKSPACE) == 0) {
    if (len > 0) buf[len - 1] = '\0';
  } else if (strcmp(txt, LV_SYMBOL_OK) == 0 ||
             strcmp(txt, LV_SYMBOL_NEW_LINE) == 0) {
    action_save_button_appearance(NULL);
    return;
  } else if (strcmp(txt, LV_SYMBOL_KEYBOARD) == 0) {
    return;
  } else {
    size_t tl = strlen(txt);
    if (len + tl < sizeof(buf)) {
      memcpy(buf + len, txt, tl);
      buf[len + tl] = '\0';
    }
  }
  set_var_edit_label_text(buf);
  s_cursor_visible = true;
  update_edit_text_display();
}

void ui_bind_button_edit_keyboard(void) {
  if (objects.kb_button_edit) {
    lv_obj_add_event_cb(objects.kb_button_edit, kb_button_edit_event_cb,
                        LV_EVENT_ALL, NULL);
  }
}

void action_all_on_off(lv_event_t *e) {
  (void)e;
  static int64_t last_send_us = 0;
  int64_t now = esp_timer_get_time();
  if (now - last_send_us < 300000) return;
  last_send_us = now;

  bool any_on = false;
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (g_buttons[i].module_type != MOD_NONE && g_button_state[i] > 0) {
      any_on = true; break;
    }
  }
  bool want_off = any_on;

  for (int i = 0; i < NUM_BUTTONS; i++) {
    const btn_config_t *cfg = &g_buttons[i];
    if (cfg->module_type == MOD_NONE) continue;
    if (cfg->instance > 2 || cfg->channel > 7) continue;
    bool is_on = g_button_state[i] > 0;
    if (want_off != is_on) continue;
    uint32_t can_id = (cfg->module_type == MOD_TORRENT)
        ? TORRENT_TOGGLE_ID[cfg->instance]
        : SWITCHBACK_TOGGLE_ID[cfg->instance];
    uint8_t data[1] = { cfg->channel };
    publish_can_frame(can_id, data, 1);
    vTaskDelay(pdMS_TO_TICKS(30));
  }
}
