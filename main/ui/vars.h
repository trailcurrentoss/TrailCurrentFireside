#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_ROTATION_DEGREES = 0,
    FLOW_GLOBAL_VARIABLE_TEMPERATURE_UNIT_FAHRENHEIT = 1,
    FLOW_GLOBAL_VARIABLE_SCREEN_BRIGHTNESS = 2,
    FLOW_GLOBAL_VARIABLE_SCREEN_TIMEOUT_MINUTES = 3,
    FLOW_GLOBAL_VARIABLE_CURRENT_DEVICE_BRIGHTNESS_ID = 4,
    FLOW_GLOBAL_VARIABLE_EDIT_BTN_NUMBER = 5,
    FLOW_GLOBAL_VARIABLE_EDIT_LABEL_TEXT = 6,
    FLOW_GLOBAL_VARIABLE_EDIT_ICON_CODEPOINT = 7,
    FLOW_GLOBAL_VARIABLE_ASSIGN_MODULE_TYPE = 8,
    FLOW_GLOBAL_VARIABLE_ASSIGN_INSTANCE = 9
};

// Native global variables

extern int32_t get_var_rotation_degrees();
extern void set_var_rotation_degrees(int32_t value);
extern bool get_var_temperature_unit_fahrenheit();
extern void set_var_temperature_unit_fahrenheit(bool value);
extern int32_t get_var_screen_brightness();
extern void set_var_screen_brightness(int32_t value);
extern int32_t get_var_screen_timeout_minutes();
extern void set_var_screen_timeout_minutes(int32_t value);
extern int32_t get_var_current_device_brightness_id();
extern void set_var_current_device_brightness_id(int32_t value);
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


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/