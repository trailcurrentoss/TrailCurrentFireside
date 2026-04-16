#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>
#include "bsp/esp-bsp.h"
#include "bsp/display.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_GLOBAL_VARIABLE_INTEGER = 0,
    FLOW_GLOBAL_VARIABLE_GLOBAL_VARIABLE_FLOAT = 1,
    FLOW_GLOBAL_VARIABLE_GLOBAL_VARIABLE_DOUBLE = 2,
    FLOW_GLOBAL_VARIABLE_GLOBAL_VARIABLE_BOOLEAN = 3,
    FLOW_GLOBAL_VARIABLE_GLOBAL_VARIABLE_STRING = 4,
    FLOW_GLOBAL_VARIABLE_ROTATION_DEGREES = 5,
    FLOW_GLOBAL_VARIABLE_SATELLITE_COUNT = 6,
    FLOW_GLOBAL_VARIABLE_DESIRED_TEMPERATURE = 7,
    FLOW_GLOBAL_VARIABLE_CURRENT_INTERIOR_TEMPERATURE = 8,
    FLOW_GLOBAL_VARIABLE_CURRENT_EXTERIOR_TEMPERATURE = 9,
    FLOW_GLOBAL_VARIABLE_DESIRED_FM_RADIO_STATION = 10,
    FLOW_GLOBAL_VARIABLE_USER_SETTINGS_CHANGED = 11,
    FLOW_GLOBAL_VARIABLE_CURRENT_TIME_ZONE_STRING = 12,
    FLOW_GLOBAL_VARIABLE_SCREEN_TIMEOUT_VALUE = 13,
    FLOW_GLOBAL_VARIABLE_SELECTED_THEME = 14,
    FLOW_GLOBAL_VARIABLE_CURRENT_DEVICE_BRIGHTNESS_IDENTIFIER = 15,
    FLOW_GLOBAL_VARIABLE_EDIT_BTN_NUMBER = 16,
    FLOW_GLOBAL_VARIABLE_EDIT_LABEL_TEXT = 17,
    FLOW_GLOBAL_VARIABLE_EDIT_ICON_CODEPOINT = 18,
    FLOW_GLOBAL_VARIABLE_ASSIGN_MODULE_TYPE = 19,
    FLOW_GLOBAL_VARIABLE_ASSIGN_INSTANCE = 20,
    FLOW_GLOBAL_VARIABLE_EDIT_CHANNEL = 21
};

// Native global variables

extern int32_t get_var_global_variable_integer();
extern void set_var_global_variable_integer(int32_t value);
extern float get_var_global_variable_float();
extern void set_var_global_variable_float(float value);
extern double get_var_global_variable_double();
extern void set_var_global_variable_double(double value);
extern bool get_var_global_variable_boolean();
extern void set_var_global_variable_boolean(bool value);
extern const char *get_var_global_variable_string();
extern void set_var_global_variable_string(const char *value);
extern int32_t get_var_rotation_degrees();
extern void set_var_rotation_degrees(int32_t value);
extern int32_t get_var_satellite_count();
extern void set_var_satellite_count(int32_t value);
extern int32_t get_var_desired_temperature();
extern void set_var_desired_temperature(int32_t value);
extern int32_t get_var_current_interior_temperature();
extern void set_var_current_interior_temperature(int32_t value);
extern int32_t get_var_current_exterior_temperature();
extern void set_var_current_exterior_temperature(int32_t value);
extern float get_var_desired_fm_radio_station();
extern void set_var_desired_fm_radio_station(float value);
extern bool get_var_user_settings_changed();
extern void set_var_user_settings_changed(bool value);
extern const char *get_var_current_time_zone_string();
extern void set_var_current_time_zone_string(const char *value);
extern int32_t get_var_gateway_mac_address_byte1();
extern void set_var_gateway_mac_address_byte1(int32_t value);
extern int32_t get_var_gateway_mac_address_byte2();
extern void set_var_gateway_mac_address_byte2(int32_t value);
extern int32_t get_var_gateway_mac_address_byte3();
extern void set_var_gateway_mac_address_byte3(int32_t value);
extern int32_t get_var_gateway_mac_address_byte4();
extern void set_var_gateway_mac_address_byte4(int32_t value);
extern int32_t get_var_gateway_mac_address_byte5();
extern void set_var_gateway_mac_address_byte5(int32_t value);
extern int32_t get_var_gateway_mac_address_byte6();
extern void set_var_gateway_mac_address_byte6(int32_t value);
extern int32_t get_var_screen_timeout_value();
extern void set_var_screen_timeout_value(int32_t value);
extern int32_t get_var_selected_theme();
extern void set_var_selected_theme(int32_t value);
extern int32_t get_var_current_device_brightness_identifier();
extern void set_var_current_device_brightness_identifier(int32_t value);
extern int32_t get_var_wifi_scan_status();
extern void set_var_wifi_scan_status(int32_t value);
extern int32_t get_var_wifi_network_count();
extern void set_var_wifi_network_count(int32_t value);
extern int32_t get_var_selected_wifi_network_index();
extern void set_var_selected_wifi_network_index(int32_t value);
extern const char *get_var_selected_wifi_network_name();
extern void set_var_selected_wifi_network_name(const char *value);
extern int32_t get_var_wifi_connection_status();
extern void set_var_wifi_connection_status(int32_t value);
extern int32_t get_var_edit_btn_number();
extern void set_var_edit_btn_number(int32_t value);
extern const char *get_var_edit_label_text();
extern void set_var_edit_label_text(const char *value);
extern int32_t get_var_edit_icon_codepoint();
extern void set_var_edit_icon_codepoint(int32_t value);
extern int32_t get_var_assign_module_type();
extern void set_var_assign_module_type(int32_t value);
extern int32_t get_var_assign_instance();
extern void set_var_assign_instance(int32_t value);
extern int32_t get_var_edit_channel();
extern void set_var_edit_channel(int32_t value);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/