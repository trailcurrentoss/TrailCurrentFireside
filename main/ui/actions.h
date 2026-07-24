#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_nav_home(lv_event_t * e);
extern void action_nav_trailer(lv_event_t * e);
extern void action_nav_power(lv_event_t * e);
extern void action_nav_water(lv_event_t * e);
extern void action_nav_air(lv_event_t * e);
extern void action_nav_settings(lv_event_t * e);
extern void action_toggle_theme(lv_event_t * e);
extern void action_toggle_device(lv_event_t * e);
extern void action_set_temp_unit_f(lv_event_t * e);
extern void action_set_temp_unit_c(lv_event_t * e);
extern void action_set_power_variant(lv_event_t * e);
extern void action_set_air_variant(lv_event_t * e);
extern void action_select_power_metric(lv_event_t * e);
extern void action_select_air_metric(lv_event_t * e);
extern void action_toggle_pump(lv_event_t * e);
extern void action_brightness_changed(lv_event_t * e);
extern void action_volume_changed(lv_event_t * e);
extern void action_timeout_up(lv_event_t * e);
extern void action_timeout_down(lv_event_t * e);
extern void action_set_timezone(lv_event_t * e);
extern void action_reset_connection(lv_event_t * e);
extern void action_factory_reset(lv_event_t * e);
extern void action_wifi_scan(lv_event_t * e);
extern void action_wifi_select(lv_event_t * e);
extern void action_wifi_cancel_password(lv_event_t * e);
extern void action_wifi_submit_password(lv_event_t * e);
extern void action_mqtt_submit(lv_event_t * e);
extern void action_mqtt_skip(lv_event_t * e);
extern void action_open_alarms(lv_event_t * e);
extern void action_alarms_back(lv_event_t * e);
extern void action_toggle_battery_alarm(lv_event_t * e);
extern void action_battery_threshold(lv_event_t * e);
extern void action_navigate_to_button_edit(lv_event_t * e);
extern void action_select_button_icon(lv_event_t * e);
extern void action_save_button_appearance(lv_event_t * e);
extern void action_back_to_edit_buttons(lv_event_t * e);
extern void action_open_edit_buttons(lv_event_t * e);
extern void action_select_module(lv_event_t * e);
extern void action_select_address(lv_event_t * e);
extern void action_select_channel(lv_event_t * e);
extern void action_show_factory_reset_confirm(lv_event_t * e);
extern void action_cancel_factory_reset_confirm(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/