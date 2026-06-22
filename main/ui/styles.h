#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: ArcDefault
lv_style_t *get_style_arc_default_KNOB_DEFAULT();
lv_style_t *get_style_arc_default_MAIN_DEFAULT();
lv_style_t *get_style_arc_default_INDICATOR_DEFAULT();
void add_style_arc_default(lv_obj_t *obj);
void remove_style_arc_default(lv_obj_t *obj);

// Style: BarDefault
lv_style_t *get_style_bar_default_INDICATOR_CHECKED();
lv_style_t *get_style_bar_default_INDICATOR_DEFAULT();
lv_style_t *get_style_bar_default_MAIN_DEFAULT();
void add_style_bar_default(lv_obj_t *obj);
void remove_style_bar_default(lv_obj_t *obj);

// Style: ButtonCircle
lv_style_t *get_style_button_circle_MAIN_DEFAULT();
void add_style_button_circle(lv_obj_t *obj);
void remove_style_button_circle(lv_obj_t *obj);

// Style: ButtonListMenuItem
lv_style_t *get_style_button_list_menu_item_MAIN_DEFAULT();
lv_style_t *get_style_button_list_menu_item_MAIN_CHECKED();
void add_style_button_list_menu_item(lv_obj_t *obj);
void remove_style_button_list_menu_item(lv_obj_t *obj);

// Style: ButtonNavbarActive
lv_style_t *get_style_button_navbar_active_MAIN_DEFAULT();
lv_style_t *get_style_button_navbar_active_MAIN_PRESSED();
void add_style_button_navbar_active(lv_obj_t *obj);
void remove_style_button_navbar_active(lv_obj_t *obj);

// Style: ButtonNavbarInActive
lv_style_t *get_style_button_navbar_in_active_MAIN_DEFAULT();
lv_style_t *get_style_button_navbar_in_active_MAIN_CHECKED();
void add_style_button_navbar_in_active(lv_obj_t *obj);
void remove_style_button_navbar_in_active(lv_obj_t *obj);

// Style: CheckboxDefault
lv_style_t *get_style_checkbox_default_INDICATOR_CHECKED();
lv_style_t *get_style_checkbox_default_INDICATOR_DEFAULT();
lv_style_t *get_style_checkbox_default_MAIN_DEFAULT();
void add_style_checkbox_default(lv_obj_t *obj);
void remove_style_checkbox_default(lv_obj_t *obj);

// Style: DefaultButtonMatrix
lv_style_t *get_style_default_button_matrix_MAIN_DEFAULT();
lv_style_t *get_style_default_button_matrix_ITEMS_DEFAULT();
lv_style_t *get_style_default_button_matrix_ITEMS_CHECKED();
void add_style_default_button_matrix(lv_obj_t *obj);
void remove_style_default_button_matrix(lv_obj_t *obj);

// Style: DropdownDefault
void add_style_dropdown_default(lv_obj_t *obj);
void remove_style_dropdown_default(lv_obj_t *obj);

// Style: KeyboardDefault
lv_style_t *get_style_keyboard_default_MAIN_DEFAULT();
lv_style_t *get_style_keyboard_default_ITEMS_DEFAULT();
void add_style_keyboard_default(lv_obj_t *obj);
void remove_style_keyboard_default(lv_obj_t *obj);

// Style: LEDDefault
lv_style_t *get_style_led_default_MAIN_DEFAULT();
lv_style_t *get_style_led_default_MAIN_CHECKED();
void add_style_led_default(lv_obj_t *obj);
void remove_style_led_default(lv_obj_t *obj);

// Style: LabelCircleButton
lv_style_t *get_style_label_circle_button_MAIN_DEFAULT();
void add_style_label_circle_button(lv_obj_t *obj);
void remove_style_label_circle_button(lv_obj_t *obj);

// Style: LabelDefault
lv_style_t *get_style_label_default_MAIN_DEFAULT();
lv_style_t *get_style_label_default_MAIN_CHECKED();
void add_style_label_default(lv_obj_t *obj);
void remove_style_label_default(lv_obj_t *obj);

// Style: LabelFa16Icon
lv_style_t *get_style_label_fa16_icon_MAIN_DEFAULT();
lv_style_t *get_style_label_fa16_icon_MAIN_CHECKED();
void add_style_label_fa16_icon(lv_obj_t *obj);
void remove_style_label_fa16_icon(lv_obj_t *obj);

// Style: LabelFa32Icon
lv_style_t *get_style_label_fa32_icon_MAIN_DEFAULT();
lv_style_t *get_style_label_fa32_icon_MAIN_CHECKED();
void add_style_label_fa32_icon(lv_obj_t *obj);
void remove_style_label_fa32_icon(lv_obj_t *obj);

// Style: LabelFaIconActive
lv_style_t *get_style_label_fa_icon_active_MAIN_PRESSED();
void add_style_label_fa_icon_active(lv_obj_t *obj);
void remove_style_label_fa_icon_active(lv_obj_t *obj);

// Style: LabelListMenuItem
lv_style_t *get_style_label_list_menu_item_MAIN_DEFAULT();
void add_style_label_list_menu_item(lv_obj_t *obj);
void remove_style_label_list_menu_item(lv_obj_t *obj);

// Style: LabelNavText
lv_style_t *get_style_label_nav_text_MAIN_DEFAULT();
void add_style_label_nav_text(lv_obj_t *obj);
void remove_style_label_nav_text(lv_obj_t *obj);

// Style: LabelNavTextAlternate
lv_style_t *get_style_label_nav_text_alternate_MAIN_DEFAULT();
void add_style_label_nav_text_alternate(lv_obj_t *obj);
void remove_style_label_nav_text_alternate(lv_obj_t *obj);

// Style: LabelSecondary
lv_style_t *get_style_label_secondary_MAIN_DEFAULT();
void add_style_label_secondary(lv_obj_t *obj);
void remove_style_label_secondary(lv_obj_t *obj);

// Style: ListDefault
lv_style_t *get_style_list_default_MAIN_DEFAULT();
void add_style_list_default(lv_obj_t *obj);
void remove_style_list_default(lv_obj_t *obj);

// Style: ListMenu
lv_style_t *get_style_list_menu_MAIN_DEFAULT();
void add_style_list_menu(lv_obj_t *obj);
void remove_style_list_menu(lv_obj_t *obj);

// Style: MessageBoxDefault
lv_style_t *get_style_message_box_default_MAIN_DEFAULT();
void add_style_message_box_default(lv_obj_t *obj);
void remove_style_message_box_default(lv_obj_t *obj);

// Style: NavBarIcon
lv_style_t *get_style_nav_bar_icon_MAIN_DEFAULT();
void add_style_nav_bar_icon(lv_obj_t *obj);
void remove_style_nav_bar_icon(lv_obj_t *obj);

// Style: PanelNavBar
void add_style_panel_nav_bar(lv_obj_t *obj);
void remove_style_panel_nav_bar(lv_obj_t *obj);

// Style: PanelNavBarTop
lv_style_t *get_style_panel_nav_bar_top_MAIN_DEFAULT();
void add_style_panel_nav_bar_top(lv_obj_t *obj);
void remove_style_panel_nav_bar_top(lv_obj_t *obj);

// Style: PanelScreenContent
lv_style_t *get_style_panel_screen_content_MAIN_DEFAULT();
void add_style_panel_screen_content(lv_obj_t *obj);
void remove_style_panel_screen_content(lv_obj_t *obj);

// Style: RollerDefault
lv_style_t *get_style_roller_default_SELECTED_DEFAULT();
lv_style_t *get_style_roller_default_MAIN_DEFAULT();
void add_style_roller_default(lv_obj_t *obj);
void remove_style_roller_default(lv_obj_t *obj);

// Style: SpinnerDefault
lv_style_t *get_style_spinner_default_INDICATOR_DEFAULT();
lv_style_t *get_style_spinner_default_INDICATOR_FOCUSED();
lv_style_t *get_style_spinner_default_INDICATOR_CHECKED();
lv_style_t *get_style_spinner_default_MAIN_DEFAULT();
void add_style_spinner_default(lv_obj_t *obj);
void remove_style_spinner_default(lv_obj_t *obj);

// Style: StyleButtonDefault
lv_style_t *get_style_style_button_default_MAIN_DEFAULT();
lv_style_t *get_style_style_button_default_MAIN_CHECKED();
void add_style_style_button_default(lv_obj_t *obj);
void remove_style_style_button_default(lv_obj_t *obj);

// Style: StyleButtonSucces
void add_style_style_button_succes(lv_obj_t *obj);
void remove_style_style_button_succes(lv_obj_t *obj);

// Style: StyleContentPanel
lv_style_t *get_style_style_content_panel_MAIN_DEFAULT();
void add_style_style_content_panel(lv_obj_t *obj);
void remove_style_style_content_panel(lv_obj_t *obj);

// Style: StyleDefaultSlider
lv_style_t *get_style_style_default_slider_INDICATOR_DEFAULT();
lv_style_t *get_style_style_default_slider_KNOB_DEFAULT();
lv_style_t *get_style_style_default_slider_MAIN_DEFAULT();
void add_style_style_default_slider(lv_obj_t *obj);
void remove_style_style_default_slider(lv_obj_t *obj);

// Style: StyleDeviceStatusIndOff
lv_style_t *get_style_style_device_status_ind_off_MAIN_DEFAULT();
void add_style_style_device_status_ind_off(lv_obj_t *obj);
void remove_style_style_device_status_ind_off(lv_obj_t *obj);

// Style: StyleDeviceStatusIndOn
lv_style_t *get_style_style_device_status_ind_on_MAIN_DEFAULT();
lv_style_t *get_style_style_device_status_ind_on_MAIN_PRESSED();
void add_style_style_device_status_ind_on(lv_obj_t *obj);
void remove_style_style_device_status_ind_on(lv_obj_t *obj);

// Style: StylePanelDefault
lv_style_t *get_style_style_panel_default_MAIN_DEFAULT();
lv_style_t *get_style_style_panel_default_MAIN_PRESSED();
void add_style_style_panel_default(lv_obj_t *obj);
void remove_style_style_panel_default(lv_obj_t *obj);

// Style: StylePanelDialog
lv_style_t *get_style_style_panel_dialog_MAIN_DEFAULT();
void add_style_style_panel_dialog(lv_obj_t *obj);
void remove_style_style_panel_dialog(lv_obj_t *obj);

// Style: StylePanelModalBlocker
lv_style_t *get_style_style_panel_modal_blocker_MAIN_DEFAULT();
void add_style_style_panel_modal_blocker(lv_obj_t *obj);
void remove_style_style_panel_modal_blocker(lv_obj_t *obj);

// Style: StylePanelNavBarBottom
lv_style_t *get_style_style_panel_nav_bar_bottom_MAIN_DEFAULT();
void add_style_style_panel_nav_bar_bottom(lv_obj_t *obj);
void remove_style_style_panel_nav_bar_bottom(lv_obj_t *obj);

// Style: StyleScreenDefault
lv_style_t *get_style_style_screen_default_MAIN_DEFAULT();
void add_style_style_screen_default(lv_obj_t *obj);
void remove_style_style_screen_default(lv_obj_t *obj);

// Style: StyleTopNavTextNormal
lv_style_t *get_style_style_top_nav_text_normal_MAIN_DEFAULT();
void add_style_style_top_nav_text_normal(lv_obj_t *obj);
void remove_style_style_top_nav_text_normal(lv_obj_t *obj);

// Style: SwitchDefault
lv_style_t *get_style_switch_default_INDICATOR_CHECKED();
void add_style_switch_default(lv_obj_t *obj);
void remove_style_switch_default(lv_obj_t *obj);

// Style: TabDefault
void add_style_tab_default(lv_obj_t *obj);
void remove_style_tab_default(lv_obj_t *obj);

// Style: TabViewDefault
void add_style_tab_view_default(lv_obj_t *obj);
void remove_style_tab_view_default(lv_obj_t *obj);

// Style: StyleBadge
lv_style_t *get_style_style_badge_MAIN_DEFAULT();
void add_style_style_badge(lv_obj_t *obj);
void remove_style_style_badge(lv_obj_t *obj);



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/