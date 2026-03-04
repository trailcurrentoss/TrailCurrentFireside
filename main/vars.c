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
void set_var_satellite_count(int32_t value) {
  satellite_count = value;
  if (objects.label_gnss_num_sats_value) {
    char buf[8];
    sprintf(buf, "%d", (int)value);
    lv_label_set_text(objects.label_gnss_num_sats_value, buf);
  }
}

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

/* Last MQTT-received brightness per device (0-255 scale, index 0 = device 1).
 * Written on every incoming status message so the brightness slider can be
 * initialised to the correct position when the dialog opens. */
#define MAX_DEVICES 8
static int32_t device_brightness[MAX_DEVICES] = {0};

int32_t get_var_device_brightness(int32_t device_id) {
    if (device_id < 1 || device_id > MAX_DEVICES) return 0;
    return device_brightness[device_id - 1];
}

/* Helper: update a device button's checked state.
 * Only the button state is set — children inherit from the parent.
 * MQTT is the single source of truth. */
static void update_device_status(lv_obj_t *button, int32_t value) {
    if (!button) return;
    if (value > 0) {
        lv_obj_add_state(button, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(button, LV_STATE_CHECKED);
    }
}

void set_var_device01_status(int32_t value) {
    device_brightness[0] = value;
    update_device_status(objects.btn_device01, value);
}
void set_var_device02_status(int32_t value) {
    device_brightness[1] = value;
    update_device_status(objects.btn_device02, value);
}
void set_var_device03_status(int32_t value) {
    device_brightness[2] = value;
    update_device_status(objects.btn_device03, value);
}
void set_var_device04_status(int32_t value) {
    device_brightness[3] = value;
    update_device_status(objects.btn_device04, value);
}
void set_var_device05_status(int32_t value) {
    device_brightness[4] = value;
    update_device_status(objects.btn_device05, value);
}
void set_var_device06_status(int32_t value) {
    device_brightness[5] = value;
    update_device_status(objects.btn_device06, value);
}
void set_var_device07_status(int32_t value) {
    device_brightness[6] = value;
    update_device_status(objects.btn_device07, value);
}
void set_var_device08_status(int32_t value) {
    device_brightness[7] = value;
    update_device_status(objects.btn_device08, value);
}

/* Battery SOC */
static int32_t battery_soc = 0;
void set_var_battery_soc(int32_t percent) {
    battery_soc = percent;
    if (objects.bar_battery_soc)
        lv_bar_set_value(objects.bar_battery_soc, percent, LV_ANIM_ON);
    if (objects.label_power_battery_percentage_value) {
        char buf[16];
        sprintf(buf, "%d%%", (int)percent);
        lv_label_set_text(objects.label_power_battery_percentage_value, buf);
    }
}

/* Battery voltage */
static float battery_voltage_val = 0.0f;
void set_var_battery_voltage(float volts) {
    battery_voltage_val = volts;
    if (objects.label_battery_voltage_value) {
        char buf[16];
        sprintf(buf, "%.1fV", volts);
        lv_label_set_text(objects.label_battery_voltage_value, buf);
    }
}

/* Solar watts */
static int32_t solar_watts_val = 0;
void set_var_solar_watts(int32_t watts) {
    solar_watts_val = watts;
    if (objects.label_solar_power_wattage_value) {
        char buf[16];
        sprintf(buf, "%dW", (int)watts);
        lv_label_set_text(objects.label_solar_power_wattage_value, buf);
    }
}

/* Solar / charge status */
static char solar_status_str[64] = {0};
void set_var_solar_status(const char *status) {
    strncpy(solar_status_str, status, sizeof(solar_status_str) - 1);
    solar_status_str[sizeof(solar_status_str) - 1] = '\0';
    if (objects.label_charge_status_value_s)
        lv_label_set_text(objects.label_charge_status_value_s, status);
}

/* GPS latitude */
static float gps_latitude = 0.0f;
void set_var_latitude(float lat) {
    gps_latitude = lat;
    if (objects.label_latitude_value) {
        char buf[16];
        sprintf(buf, "%.6f", lat);
        lv_label_set_text(objects.label_latitude_value, buf);
    }
}

/* GPS longitude */
static float gps_longitude = 0.0f;
void set_var_longitude(float lon) {
    gps_longitude = lon;
    if (objects.label_longitude_value) {
        char buf[16];
        sprintf(buf, "%.6f", lon);
        lv_label_set_text(objects.label_longitude_value, buf);
    }
}

/* GPS altitude */
static float gps_altitude = 0.0f;
void set_var_altitude(float feet) {
    gps_altitude = feet;
    if (objects.label_gnss_elevation_value) {
        char buf[16];
        sprintf(buf, "%.0f ft", feet);
        lv_label_set_text(objects.label_gnss_elevation_value, buf);
    }
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
    if (objects.label_gnss_mode_value) {
        lv_label_set_text(objects.label_gnss_mode_value, gnss_mode_str);
    }
}

/* Humidity */
static float humidity_val = 0.0f;
void set_var_humidity(float percent) {
    humidity_val = percent;
}

/* GPS time → system clock */
static bool system_time_set = false;

void set_var_gps_time(int year, int month, int day, int hour, int minute, int second) {
    /* Convert UTC fields to epoch via mktime with TZ temporarily set to UTC */
    setenv("TZ", "UTC0", 1);
    tzset();

    struct tm tm = {0};
    tm.tm_year = year - 1900;
    tm.tm_mon  = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min  = minute;
    tm.tm_sec  = second;

    time_t t = mktime(&tm);
    struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
    settimeofday(&tv, NULL);
    system_time_set = true;

    update_clock_display();
}

static const char *month_names[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

void update_clock_display(void) {
    if (!system_time_set) return;

    /* Apply user-selected timezone */
    const char *tz = get_var_current_time_zone_string();
    if (tz && strlen(tz) > 0) {
        setenv("TZ", tz, 1);
    } else {
        setenv("TZ", "UTC0", 1);
    }
    tzset();

    time_t now;
    time(&now);
    struct tm ti;
    localtime_r(&now, &ti);

    /* Date: "January 01, 2026" */
    char date_buf[32];
    snprintf(date_buf, sizeof(date_buf), "%s %02d, %d",
             month_names[ti.tm_mon], ti.tm_mday, ti.tm_year + 1900);
    lv_label_set_text(objects.lbl_date, date_buf);

    /* Hours (12-hour) */
    int h12 = ti.tm_hour % 12;
    if (h12 == 0) h12 = 12;
    char hour_buf[4];
    snprintf(hour_buf, sizeof(hour_buf), "%d", h12);
    lv_label_set_text(objects.lbl_time_hour, hour_buf);

    /* Minutes */
    char min_buf[4];
    snprintf(min_buf, sizeof(min_buf), "%02d", ti.tm_min);
    lv_label_set_text(objects.lbl_time_minutes, min_buf);

    /* AM/PM */
    lv_label_set_text(objects.obj6, ti.tm_hour >= 12 ? "PM" : "AM");
}

/* Battery consumption watts (instantaneous power from Victron BMV P field) */
void set_var_consumption_watts(int32_t watts) {
    if (objects.label_power_consumption_wattage_value) {
        char buf[16];
        sprintf(buf, "%d", (int)watts);
        lv_label_set_text(objects.label_power_consumption_wattage_value, buf);
    }
}

/* Battery time-to-go (from Victron BMV TTG, arrives as minutes).
 * 0 or 0xFFFF from the shunt = no valid data. */
void set_var_time_remaining(int32_t minutes) {
    if (minutes <= 0 || minutes >= 0xFFFF) {
        /* No valid TTG data */
        if (objects.label_power_remaining_time_to_go_value)
            lv_label_set_text(objects.label_power_remaining_time_to_go_value, "-");
        if (objects.label_time_to_go_measurement_type)
            lv_label_set_text(objects.label_time_to_go_measurement_type, "");
        if (objects.power_arc_remaining_hours)
            lv_arc_set_value(objects.power_arc_remaining_hours, 0);
        return;
    }

    char value_buf[16];
    const char *unit;

    if (minutes >= 1440) {
        /* > 1 day: show days */
        int days = minutes / 1440;
        int hrs  = (minutes % 1440) / 60;
        snprintf(value_buf, sizeof(value_buf), "%dd %dh", days, hrs);
        unit = "Days";
        if (objects.power_arc_remaining_hours) {
            lv_arc_set_range(objects.power_arc_remaining_hours, 0, 30);
            lv_arc_set_value(objects.power_arc_remaining_hours, days > 30 ? 30 : days);
        }
    } else if (minutes >= 60) {
        /* > 1 hour: show hours */
        int hrs  = minutes / 60;
        int mins = minutes % 60;
        snprintf(value_buf, sizeof(value_buf), "%d:%02d", hrs, mins);
        unit = "Hrs";
        if (objects.power_arc_remaining_hours) {
            lv_arc_set_range(objects.power_arc_remaining_hours, 0, 24);
            lv_arc_set_value(objects.power_arc_remaining_hours, hrs);
        }
    } else {
        /* < 1 hour: show minutes */
        snprintf(value_buf, sizeof(value_buf), "%d", (int)minutes);
        unit = "Min";
        if (objects.power_arc_remaining_hours) {
            lv_arc_set_range(objects.power_arc_remaining_hours, 0, 60);
            lv_arc_set_value(objects.power_arc_remaining_hours, minutes);
        }
    }

    if (objects.label_power_remaining_time_to_go_value)
        lv_label_set_text(objects.label_power_remaining_time_to_go_value, value_buf);
    if (objects.label_time_to_go_measurement_type)
        lv_label_set_text(objects.label_time_to_go_measurement_type, unit);
}

/* MQTT connected status */
static bool mqtt_connected = false;
void set_var_mqtt_connected(bool connected) {
    mqtt_connected = connected;
    ESP_LOGI(TAG, "MQTT %s", connected ? "connected" : "disconnected");
}