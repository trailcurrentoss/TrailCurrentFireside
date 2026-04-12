#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_change_screen_brightness(lv_event_t * e);
extern void action_rotate_screen(lv_event_t * e);
extern void action_change_screen(lv_event_t * e);
extern void action_send_device_command(lv_event_t * e);
extern void action_change_desired_temperature(lv_event_t * e);
extern void action_change_fm_radio_station(lv_event_t * e);
extern void action_go_to_preset(lv_event_t * e);
extern void action_change_theme(lv_event_t * e);
extern void action_timeout_changed(lv_event_t * e);
extern void action_timezone_change(lv_event_t * e);
extern void action_set_device_brightness_level(lv_event_t * e);
extern void action_show_device_brightness_dialog(lv_event_t * e);
extern void action_close_dialog(lv_event_t * e);
extern void action_temperature_unit_change(lv_event_t * e);
extern void action_all_on_off(lv_event_t * e);
extern void action_navigate_to_button_edit(lv_event_t * e);
extern void action_navigate_to_device_assign(lv_event_t * e);
extern void action_select_button_icon(lv_event_t * e);
extern void action_save_button_appearance(lv_event_t * e);
extern void action_assign_channel(lv_event_t * e);
extern void action_select_device_instance(lv_event_t * e);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/