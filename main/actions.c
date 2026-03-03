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
#include "mqtt_vars.h"
#include "nvs.h"
#include <stdint.h>
#include <string.h>

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
      /* Connect MQTT now that we have network */
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

void action_send_device_command(lv_event_t *e) {
  int device_id = (int)lv_event_get_user_data(e);
  char topic[64];
  snprintf(topic, sizeof(topic), "local/lights/%d/command", device_id);
  const char *payload = "{\"state\":1}";
  if (mqtt_client_publish(topic, payload, strlen(payload)) < 0) {
    ESP_LOGW(TAG, "MQTT not connected, cannot send command for device %d", device_id);
  }
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
  set_var_screen_timeout_value(currentTimeoutValue);

  nvs_handle_t nvs;
  if (nvs_open(USER_SETTINGS_NVS_NAMESPACE, NVS_READWRITE, &nvs) == ESP_OK) {
    nvs_set_i8(nvs, "timeout", (int8_t)currentTimeoutValue);
    nvs_commit(nvs);
    nvs_close(nvs);
  }
}

void action_show_device_brightness_dialog(lv_event_t *e) {
  lv_obj_clear_flag(objects.container_modal_background, LV_OBJ_FLAG_HIDDEN);
  int eventValue = (int)lv_event_get_user_data(e);
  set_var_current_device_brightness_identifier(eventValue);
}

void action_close_dialog(lv_event_t *e) {
  lv_obj_add_flag(objects.container_modal_background, LV_OBJ_FLAG_HIDDEN);
  set_var_current_device_brightness_identifier(-1);
}

void action_set_device_brightness_level(lv_event_t *e) {
  /* Only publish on release, not while the slider is being dragged */
  if (lv_event_get_code(e) != LV_EVENT_RELEASED) return;

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
