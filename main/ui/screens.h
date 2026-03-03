#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *page_home;
    lv_obj_t *page_trailer;
    lv_obj_t *page_power_management;
    lv_obj_t *page_air_quality;
    lv_obj_t *page_water;
    lv_obj_t *page_settings;
    lv_obj_t *widget_home_page_bottom_nav_bar;
    lv_obj_t *widget_home_page_bottom_nav_bar__bottom_nav_bar_button_home;
    lv_obj_t *widget_home_page_bottom_nav_bar__bottom_nav_bar_button_trailer;
    lv_obj_t *widget_home_page_bottom_nav_bar__bottom_nav_bar_button_power;
    lv_obj_t *widget_home_page_bottom_nav_bar__bottom_nav_bar_button_water;
    lv_obj_t *widget_home_page_bottom_nav_bar__bottom_nav_bar_button_air_quality;
    lv_obj_t *widget_home_page_bottom_nav_bar__bottom_nav_bar_button_settings;
    lv_obj_t *panel_control_buttons;
    lv_obj_t *btn_device01;
    lv_obj_t *lbl_device01_status_ind;
    lv_obj_t *lbl_device01_label;
    lv_obj_t *btn_device02;
    lv_obj_t *lbl_device02_status_ind;
    lv_obj_t *lbl_device02_label;
    lv_obj_t *btn_device03;
    lv_obj_t *lbl_device03_status_ind;
    lv_obj_t *lbl_device03_label;
    lv_obj_t *btn_device04;
    lv_obj_t *lbl_device04_status_ind;
    lv_obj_t *lbl_device04_label;
    lv_obj_t *btn_device05;
    lv_obj_t *lbl_device05_status_ind;
    lv_obj_t *lbl_device05_label;
    lv_obj_t *btn_device06;
    lv_obj_t *lbl_device06_status_ind;
    lv_obj_t *lbl_device06_label;
    lv_obj_t *btn_device07;
    lv_obj_t *lbl_device07_status_ind;
    lv_obj_t *lbl_device07_label;
    lv_obj_t *btn_device08;
    lv_obj_t *lbl_device08_status_ind;
    lv_obj_t *lbl_device08_label;
    lv_obj_t *arc_thermostat;
    lv_obj_t *temp_bg;
    lv_obj_t *obj0;
    lv_obj_t *label_desired_temperature_value;
    lv_obj_t *obj1;
    lv_obj_t *label_current_interior_temperature;
    lv_obj_t *obj2;
    lv_obj_t *label_heat_activated_icon;
    lv_obj_t *label_ac_activated_icon;
    lv_obj_t *lbl_date;
    lv_obj_t *obj3;
    lv_obj_t *lbl_time_hour;
    lv_obj_t *obj4;
    lv_obj_t *lbl_time_minutes;
    lv_obj_t *obj5;
    lv_obj_t *obj6;
    lv_obj_t *container_modal_background;
    lv_obj_t *panel_device_brighness_level;
    lv_obj_t *slider_device_brightness_level;
    lv_obj_t *widget_trailer_page_bottom_nav_bar;
    lv_obj_t *widget_trailer_page_bottom_nav_bar__bottom_nav_bar_button_home;
    lv_obj_t *widget_trailer_page_bottom_nav_bar__bottom_nav_bar_button_trailer;
    lv_obj_t *widget_trailer_page_bottom_nav_bar__bottom_nav_bar_button_power;
    lv_obj_t *widget_trailer_page_bottom_nav_bar__bottom_nav_bar_button_water;
    lv_obj_t *widget_trailer_page_bottom_nav_bar__bottom_nav_bar_button_air_quality;
    lv_obj_t *widget_trailer_page_bottom_nav_bar__bottom_nav_bar_button_settings;
    lv_obj_t *panel_trailer_side_view;
    lv_obj_t *obj7;
    lv_obj_t *label_front_level_value;
    lv_obj_t *label_front_level_fa_indicator;
    lv_obj_t *label_back_level_value;
    lv_obj_t *label_front_leve_label_1;
    lv_obj_t *label_back_level_fa_indicator;
    lv_obj_t *label_front_leve_label;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *label_gnss_elevation_value;
    lv_obj_t *obj10;
    lv_obj_t *obj11;
    lv_obj_t *label_gnss_num_sats_value;
    lv_obj_t *label_gnss_mode_value;
    lv_obj_t *label_latitude_value;
    lv_obj_t *label_longitude_value;
    lv_obj_t *panel_trailer_back;
    lv_obj_t *obj12;
    lv_obj_t *label_left_side_level_fa_indicator;
    lv_obj_t *label_left_side_level_value;
    lv_obj_t *label_left_side_leve_label;
    lv_obj_t *label_right_side_level_value;
    lv_obj_t *label_right_side_leve_label;
    lv_obj_t *label_right_side_level_fa_indicator;
    lv_obj_t *widget_power_management_page_bottom_nav_bar;
    lv_obj_t *widget_power_management_page_bottom_nav_bar__bottom_nav_bar_button_home;
    lv_obj_t *widget_power_management_page_bottom_nav_bar__bottom_nav_bar_button_trailer;
    lv_obj_t *widget_power_management_page_bottom_nav_bar__bottom_nav_bar_button_power;
    lv_obj_t *widget_power_management_page_bottom_nav_bar__bottom_nav_bar_button_water;
    lv_obj_t *widget_power_management_page_bottom_nav_bar__bottom_nav_bar_button_air_quality;
    lv_obj_t *widget_power_management_page_bottom_nav_bar__bottom_nav_bar_button_settings;
    lv_obj_t *panel_solar_input;
    lv_obj_t *label_solar_power_wattage_value;
    lv_obj_t *panel_charge_type;
    lv_obj_t *label_charge_status_value_s;
    lv_obj_t *panel_shore_power;
    lv_obj_t *label_power_shore_power_heading;
    lv_obj_t *label_shore_power_connection_status;
    lv_obj_t *panel_shore_power_indicator_background;
    lv_obj_t *panel_shore_power_indicator_foreground;
    lv_obj_t *panel_power_battery_stats;
    lv_obj_t *label_power_remaining_cacpity_1;
    lv_obj_t *bar_battery_soc;
    lv_obj_t *obj13;
    lv_obj_t *label_power_battery_percentage_value;
    lv_obj_t *label_battery_voltage_value;
    lv_obj_t *panel_power_remaining_time;
    lv_obj_t *label_power_remaining_cacpity;
    lv_obj_t *power_remaining_arc_group;
    lv_obj_t *power_arc_remaining_hours;
    lv_obj_t *panel_power_remaining_center;
    lv_obj_t *label_time_to_go_measurement_type;
    lv_obj_t *label_power_remaining_time_to_go_value;
    lv_obj_t *label_power_remaining;
    lv_obj_t *label_power_consumption_wattage_value;
    lv_obj_t *widget_air_quality_page_bottom_nav_bar;
    lv_obj_t *widget_air_quality_page_bottom_nav_bar__bottom_nav_bar_button_home;
    lv_obj_t *widget_air_quality_page_bottom_nav_bar__bottom_nav_bar_button_trailer;
    lv_obj_t *widget_air_quality_page_bottom_nav_bar__bottom_nav_bar_button_power;
    lv_obj_t *widget_air_quality_page_bottom_nav_bar__bottom_nav_bar_button_water;
    lv_obj_t *widget_air_quality_page_bottom_nav_bar__bottom_nav_bar_button_air_quality;
    lv_obj_t *widget_air_quality_page_bottom_nav_bar__bottom_nav_bar_button_settings;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *widget_water_page_bottom_nav_bar;
    lv_obj_t *widget_water_page_bottom_nav_bar__bottom_nav_bar_button_home;
    lv_obj_t *widget_water_page_bottom_nav_bar__bottom_nav_bar_button_trailer;
    lv_obj_t *widget_water_page_bottom_nav_bar__bottom_nav_bar_button_power;
    lv_obj_t *widget_water_page_bottom_nav_bar__bottom_nav_bar_button_water;
    lv_obj_t *widget_water_page_bottom_nav_bar__bottom_nav_bar_button_air_quality;
    lv_obj_t *widget_water_page_bottom_nav_bar__bottom_nav_bar_button_settings;
    lv_obj_t *panel_fresh_water;
    lv_obj_t *label_fresh_water_icon;
    lv_obj_t *label_fresh_water_text;
    lv_obj_t *bar_fresh_water_value;
    lv_obj_t *label_fresh_water_value;
    lv_obj_t *obj17;
    lv_obj_t *obj18;
    lv_obj_t *obj19;
    lv_obj_t *obj20;
    lv_obj_t *obj21;
    lv_obj_t *obj22;
    lv_obj_t *widget_settings_page_bottom_nav_bar;
    lv_obj_t *widget_settings_page_bottom_nav_bar__bottom_nav_bar_button_home;
    lv_obj_t *widget_settings_page_bottom_nav_bar__bottom_nav_bar_button_trailer;
    lv_obj_t *widget_settings_page_bottom_nav_bar__bottom_nav_bar_button_power;
    lv_obj_t *widget_settings_page_bottom_nav_bar__bottom_nav_bar_button_water;
    lv_obj_t *widget_settings_page_bottom_nav_bar__bottom_nav_bar_button_air_quality;
    lv_obj_t *widget_settings_page_bottom_nav_bar__bottom_nav_bar_button_settings;
    lv_obj_t *obj23;
    lv_obj_t *obj24;
    lv_obj_t *btn_theme_light;
    lv_obj_t *lbl_device08_status_ind_5;
    lv_obj_t *btn_theme_dark;
    lv_obj_t *lbl_device08_status_ind_6;
    lv_obj_t *label_screen_brightness;
    lv_obj_t *slider_screen_brightness;
    lv_obj_t *label_screen_timeout;
    lv_obj_t *button_screen_timeout_decrease;
    lv_obj_t *label_screen_timeout_value;
    lv_obj_t *button_screen_timeout_increase;
    lv_obj_t *tab_connectivity;
    lv_obj_t *label_wifi_connection_status;
    lv_obj_t *button_settings_demo_light_sequence_start;
    lv_obj_t *button_settings_demo_light_sequence_stop;
    lv_obj_t *label_time_zone_header;
    lv_obj_t *drop_down_selected_time_zone;
    lv_obj_t *label_about_header;
    lv_obj_t *label_version_number;
    lv_obj_t *mcu_mac_address_value;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_PAGE_HOME = 1,
    SCREEN_ID_PAGE_TRAILER = 2,
    SCREEN_ID_PAGE_POWER_MANAGEMENT = 3,
    SCREEN_ID_PAGE_AIR_QUALITY = 4,
    SCREEN_ID_PAGE_WATER = 5,
    SCREEN_ID_PAGE_SETTINGS = 6,
};

void create_screen_page_home();
void tick_screen_page_home();

void create_screen_page_trailer();
void tick_screen_page_trailer();

void create_screen_page_power_management();
void tick_screen_page_power_management();

void create_screen_page_air_quality();
void tick_screen_page_air_quality();

void create_screen_page_water();
void tick_screen_page_water();

void create_screen_page_settings();
void tick_screen_page_settings();

void create_user_widget_bottom_nav_bar(lv_obj_t *parent_obj, int startWidgetIndex);
void tick_user_widget_bottom_nav_bar(int startWidgetIndex);

enum Themes {
    THEME_ID_DEFAULT,
    THEME_ID_DARK,
};
enum Colors {
    COLOR_ID_ACCENT_COLOR,
    COLOR_ID_BACKGROUND_BLACK,
    COLOR_ID_BACKGROUND_CONTENT,
    COLOR_ID_BACKGROUND_NOT_SELECTED,
    COLOR_ID_BACKGROUND_PANEL,
    COLOR_ID_BACKGROUND_SELECTED,
    COLOR_ID_COOL,
    COLOR_ID_FOREGROUND_WHITE,
    COLOR_ID_HOT,
    COLOR_ID_PRIMARY_TEXT_COLOR,
    COLOR_ID_SECONDARY_TEXT_COLOR,
    COLOR_ID_SUCCESS,
};
void change_color_theme(uint32_t themeIndex);
extern uint32_t theme_colors[2][12];
extern uint32_t active_theme_index;

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/