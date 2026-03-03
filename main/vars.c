#include "vars.h"
#include "mqtt_vars.h"
#include "bsp/display.h"
#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "lvgl.h"
#include "screens.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

static const char *TAG = "VARS";

char date_time_utc[100] = {0};
char date_time_local[100] = {0};

char mcu_mac_address[100] = {0};
const char *get_var_mcu_mac_address() { return mcu_mac_address; }
void set_var_mcu_mac_address(const char *value) {
  strncpy(mcu_mac_address, value, sizeof(mcu_mac_address) / sizeof(char));
  mcu_mac_address[sizeof(mcu_mac_address) / sizeof(char) - 1] = 0;
  lv_label_set_text(objects.mcu_mac_address_value, value);
}

bool user_settings_changed;
bool get_var_user_settings_changed() { return user_settings_changed; }
void set_var_user_settings_changed(bool value) {
  user_settings_changed = value;
}

int32_t selected_theme;
int32_t get_var_selected_theme() { return selected_theme; }
void set_var_selected_theme(int32_t value) {
  selected_theme = value;
  lv_obj_clear_state(objects.btn_theme_dark, LV_STATE_CHECKED);
  lv_obj_clear_state(objects.btn_theme_light, LV_STATE_CHECKED);
  if (selected_theme == 0) {
    change_color_theme(THEME_ID_DEFAULT);
    lv_obj_add_state(objects.btn_theme_light, LV_STATE_CHECKED);
  } else if (selected_theme == 1) {
    change_color_theme(THEME_ID_DARK);
    lv_obj_add_state(objects.btn_theme_dark, LV_STATE_CHECKED);
  }
}

int32_t global_variable_integer;
int32_t get_var_global_variable_integer() { return global_variable_integer; }
void set_var_global_variable_integer(int32_t value) {
  global_variable_integer = value;
}

float global_variable_float;
float get_var_global_variable_float() { return global_variable_float; }
void set_var_global_variable_float(float value) {
  global_variable_float = value;
}

double global_variable_double;
double get_var_global_variable_double() { return global_variable_double; }
void set_var_global_variable_double(double value) {
  global_variable_double = value;
}

bool global_variable_boolean;
bool get_var_global_variable_boolean() { return global_variable_boolean; }
void set_var_global_variable_boolean(bool value) {
  global_variable_boolean = value;
}

char global_variable_string[100] = {0};
const char *get_var_global_variable_string() { return global_variable_string; }
void set_var_global_variable_string(const char *value) {
  strncpy(global_variable_string, value,
          sizeof(global_variable_string) / sizeof(char));
  global_variable_string[sizeof(global_variable_string) / sizeof(char) - 1] = 0;
}

int32_t rotation_degrees;
int32_t get_var_rotation_degrees() { return rotation_degrees; }
void set_var_rotation_degrees(int32_t value) { rotation_degrees = value; }

int32_t satellite_count;
int32_t get_var_satellite_count() { return satellite_count; }
void set_var_satellite_count(int32_t value) { satellite_count = value; }

/* Current interior temperature */
int32_t current_interior_temperature;
int32_t get_var_current_interior_temperature() {
  return current_interior_temperature;
}
void set_var_current_interior_temperature(int32_t value) {
  current_interior_temperature = value;
  char buffer[20];
  sprintf(buffer, "%d", value);
  lv_label_set_text(objects.label_current_interior_temperature, buffer);
}

/* Desired temperature */
int32_t desired_temperature;
int32_t get_var_desired_temperature() { return desired_temperature; }
void set_var_desired_temperature(int32_t value) {
  desired_temperature = value;
  char buffer[20];
  sprintf(buffer, "%d", value);
  lv_label_set_text(objects.label_desired_temperature_value, buffer);
  /* Check to see if heat or ac is required or if both can be turned off */
  lv_obj_clear_state(objects.label_heat_activated_icon, LV_STATE_CHECKED);
  lv_obj_clear_state(objects.label_ac_activated_icon, LV_STATE_CHECKED);
  if (desired_temperature > current_interior_temperature) {
    lv_obj_add_state(objects.label_heat_activated_icon, LV_STATE_CHECKED);
    /* TODO: Figure out why checked arc style doesn't work for this one, checked
     * state is a workd around */
    lv_obj_add_state(objects.arc_thermostat, LV_STATE_CHECKED);
  } else if (desired_temperature < current_interior_temperature) {
    lv_obj_add_state(objects.label_ac_activated_icon, LV_STATE_CHECKED);
    lv_obj_clear_state(objects.arc_thermostat, LV_STATE_CHECKED);
    /* TODO: Figure out how to get the color from the current theme, right now
    it won't be noticable because coo is the same color code for default and
    dark themes, but that could change in the future. */
    lv_obj_set_style_arc_color(objects.arc_thermostat,
                               lv_palette_main(COLOR_ID_COOL),
                               LV_PART_INDICATOR);
  }
}

/* Exterior Temperature */
int32_t current_exterior_temperature;
int32_t get_var_current_exterior_temperature() {
  return current_exterior_temperature;
}
void set_var_current_exterior_temperature(int32_t value) {
  current_exterior_temperature = value;
  char buffer[20];
  sprintf(buffer, "%d", value);
  lv_label_set_text(objects.label_current_exterior_temperature, buffer);
}

/* Deisred FM Radio Station */
float desired_fm_radio_station;
float get_var_desired_fm_radio_station() { return desired_fm_radio_station; }
void set_var_desired_fm_radio_station(float value) {
  desired_fm_radio_station = value;
  char buf[16]; // Make sure buffer is large enough
  sprintf(buf, "%.1f", value);
  lv_label_set_text(objects.label_tuned_station, buf);
}

char current_time_zone_string[100] = {0};
const char *get_var_current_time_zone_string() {
  return current_time_zone_string;
}
void set_var_current_time_zone_string(const char *value) {
  strncpy(current_time_zone_string, value,
          sizeof(current_time_zone_string) / sizeof(char));
  current_time_zone_string[sizeof(current_time_zone_string) / sizeof(char) -
                           1] = 0;
}

int32_t gateway_mac_address_byte1;
int32_t get_var_gateway_mac_address_byte1() {
  return gateway_mac_address_byte1;
}
void set_var_gateway_mac_address_byte1(int32_t value) {
  gateway_mac_address_byte1 = value;
}

int32_t gateway_mac_address_byte2;
int32_t get_var_gateway_mac_address_byte2() {
  return gateway_mac_address_byte2;
}
void set_var_gateway_mac_address_byte2(int32_t value) {
  gateway_mac_address_byte2 = value;
}

int32_t gateway_mac_address_byte3;
int32_t get_var_gateway_mac_address_byte3() {
  return gateway_mac_address_byte3;
}
void set_var_gateway_mac_address_byte3(int32_t value) {
  gateway_mac_address_byte3 = value;
}

int32_t gateway_mac_address_byte4;
int32_t get_var_gateway_mac_address_byte4() {
  return gateway_mac_address_byte4;
}
void set_var_gateway_mac_address_byte4(int32_t value) {
  gateway_mac_address_byte4 = value;
}

int32_t gateway_mac_address_byte5;
int32_t get_var_gateway_mac_address_byte5() {
  return gateway_mac_address_byte5;
}
void set_var_gateway_mac_address_byte5(int32_t value) {
  gateway_mac_address_byte5 = value;
}

int32_t gateway_mac_address_byte6;
int32_t get_var_gateway_mac_address_byte6() {
  return gateway_mac_address_byte6;
}
void set_var_gateway_mac_address_byte6(int32_t value) {
  gateway_mac_address_byte6 = value;
}

int32_t screen_timeout_value;
int32_t get_var_screen_timeout_value() { return screen_timeout_value; }
void set_var_screen_timeout_value(int32_t value) {
  screen_timeout_value = value;
  char buffer[20];
  sprintf(buffer, "%d", value);
  lv_label_set_text(objects.label_screen_timeout_value, buffer);
}


int32_t current_device_brightness_identifier;
int32_t get_var_current_device_brightness_identifier() {
    return current_device_brightness_identifier;
}
void set_var_current_device_brightness_identifier(int32_t value) {
    current_device_brightness_identifier = value;
}

/* ===== MQTT-sourced variables ===== */

/* Helper: update a device button and its icon indicator.
 * Button uses LV_STATE_CHECKED for EEZ Studio checked-state styles.
 * Indicator swaps between on/off EEZ Studio shared styles for icon color. */
static void update_device_status(lv_obj_t *button, lv_obj_t *indicator, int32_t value) {
    if (!button || !indicator) return;
    if (value > 0) {
        lv_obj_add_state(button, LV_STATE_CHECKED);
        lv_obj_add_state(indicator, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(button, LV_STATE_CHECKED);
        lv_obj_clear_state(indicator, LV_STATE_CHECKED);
    }
}

/* Device light status (0=off, >0=on with brightness) */
static int32_t device_status[8] = {0};

void set_var_device01_status(int32_t value) {
    device_status[0] = value;
    update_device_status(objects.btn_device01, objects.lbl_device01_status_ind, value);
}
void set_var_device02_status(int32_t value) {
    device_status[1] = value;
    update_device_status(objects.btn_device02, objects.lbl_device02_status_ind, value);
}
void set_var_device03_status(int32_t value) {
    device_status[2] = value;
    update_device_status(objects.btn_device03, objects.lbl_device03_status_ind, value);
}
void set_var_device04_status(int32_t value) {
    device_status[3] = value;
    update_device_status(objects.btn_device04, objects.lbl_device04_status_ind, value);
}
void set_var_device05_status(int32_t value) {
    device_status[4] = value;
    update_device_status(objects.btn_device05, objects.lbl_device05_status_ind, value);
}
void set_var_device06_status(int32_t value) {
    device_status[5] = value;
    update_device_status(objects.btn_device06, objects.lbl_device06_status_ind, value);
}
void set_var_device07_status(int32_t value) {
    device_status[6] = value;
    update_device_status(objects.btn_device07, objects.lbl_device07_status_ind, value);
}
void set_var_device08_status(int32_t value) {
    device_status[7] = value;
    update_device_status(objects.btn_device08, objects.lbl_device08_status_ind, value);
}

/* Battery SOC */
static int32_t battery_soc = 0;
void set_var_battery_soc(int32_t percent) {
    battery_soc = percent;
    if (objects.bar_battery_soc)
        lv_bar_set_value(objects.bar_battery_soc, percent, LV_ANIM_ON);
    if (objects.label_power_battery_percentage) {
        char buf[16];
        sprintf(buf, "%d%%", (int)percent);
        lv_label_set_text(objects.label_power_battery_percentage, buf);
    }
}

/* Battery voltage */
static float battery_voltage_val = 0.0f;
void set_var_battery_voltage(float volts) {
    battery_voltage_val = volts;
    if (objects.label_battery_voltage) {
        char buf[16];
        sprintf(buf, "%.1fV", volts);
        lv_label_set_text(objects.label_battery_voltage, buf);
    }
}

/* Solar watts */
static int32_t solar_watts_val = 0;
void set_var_solar_watts(int32_t watts) {
    solar_watts_val = watts;
    if (objects.label_remaining_cacpity_2) {
        char buf[16];
        sprintf(buf, "%dW", (int)watts);
        lv_label_set_text(objects.label_remaining_cacpity_2, buf);
    }
}

/* Solar / charge status */
static char solar_status_str[64] = {0};
void set_var_solar_status(const char *status) {
    strncpy(solar_status_str, status, sizeof(solar_status_str) - 1);
    solar_status_str[sizeof(solar_status_str) - 1] = '\0';
    if (objects.label_solar_status)
        lv_label_set_text(objects.label_solar_status, status);
}

/* GPS latitude */
static float gps_latitude = 0.0f;
void set_var_latitude(float lat) {
    gps_latitude = lat;
}

/* GPS longitude */
static float gps_longitude = 0.0f;
void set_var_longitude(float lon) {
    gps_longitude = lon;
}

/* GPS altitude */
static float gps_altitude = 0.0f;
void set_var_altitude(float feet) {
    gps_altitude = feet;
}

/* GPS speed */
static float gps_speed = 0.0f;
void set_var_speed(float knots) {
    gps_speed = knots;
}

/* GPS course */
static float gps_course = 0.0f;
void set_var_course(float degrees) {
    gps_course = degrees;
}

/* GNSS mode */
static char gnss_mode_str[32] = {0};
void set_var_gnss_mode(const char *mode) {
    strncpy(gnss_mode_str, mode, sizeof(gnss_mode_str) - 1);
    gnss_mode_str[sizeof(gnss_mode_str) - 1] = '\0';
}

/* Humidity */
static float humidity_val = 0.0f;
void set_var_humidity(float percent) {
    humidity_val = percent;
}

/* MQTT connected status */
static bool mqtt_connected = false;
void set_var_mqtt_connected(bool connected) {
    mqtt_connected = connected;
    ESP_LOGI(TAG, "MQTT %s", connected ? "connected" : "disconnected");
}