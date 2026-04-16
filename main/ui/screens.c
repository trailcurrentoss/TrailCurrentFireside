#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

void create_screen_page_home() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_home = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    add_style_style_screen_default(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // WidgetHomePageBottomNavBar
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.widget_home_page_bottom_nav_bar = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 60);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_bottom_nav_bar(obj, 9);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // PanelControlButtons
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_control_buttons = obj;
            lv_obj_set_pos(obj, -10, 10);
            lv_obj_set_size(obj, 520, 266);
            add_style_style_content_panel(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // BtnDevice01
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_device01 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 120, 120);
                    lv_obj_add_event_cb(obj, action_send_device_command, LV_EVENT_SHORT_CLICKED, (void *)1);
                    lv_obj_add_event_cb(obj, action_show_device_brightness_dialog, LV_EVENT_LONG_PRESSED, (void *)1);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_ON_FOCUS|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lblDevice01StatusInd
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device01_status_ind = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf7b6");
                        }
                        {
                            // lblDevice01Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device01_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Under\nCabinet");
                        }
                    }
                }
                {
                    // BtnDevice02
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_device02 = obj;
                    lv_obj_set_pos(obj, -65, 0);
                    lv_obj_set_size(obj, 120, 120);
                    lv_obj_add_event_cb(obj, action_send_device_command, LV_EVENT_SHORT_CLICKED, (void *)2);
                    lv_obj_add_event_cb(obj, action_show_device_brightness_dialog, LV_EVENT_LONG_PRESSED, (void *)2);
                    add_style_style_button_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lblDevice02StatusInd
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device02_status_ind = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf2e7");
                        }
                        {
                            // lblDevice02Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device02_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Kitchen\nLights");
                        }
                    }
                }
                {
                    // BtnDevice03
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_device03 = obj;
                    lv_obj_set_pos(obj, -130, 0);
                    lv_obj_set_size(obj, 120, 120);
                    lv_obj_add_event_cb(obj, action_send_device_command, LV_EVENT_SHORT_CLICKED, (void *)3);
                    lv_obj_add_event_cb(obj, action_show_device_brightness_dialog, LV_EVENT_LONG_PRESSED, (void *)3);
                    add_style_style_button_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lblDevice03StatusInd
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device03_status_ind = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf52a");
                        }
                        {
                            // lblDevice03Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device03_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Entry\nLights");
                        }
                    }
                }
                {
                    // BtnDevice04
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_device04 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 120, 120);
                    lv_obj_add_event_cb(obj, action_send_device_command, LV_EVENT_SHORT_CLICKED, (void *)4);
                    lv_obj_add_event_cb(obj, action_show_device_brightness_dialog, LV_EVENT_LONG_PRESSED, (void *)4);
                    add_style_style_button_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lblDevice04StatusInd
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device04_status_ind = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf0eb");
                        }
                        {
                            // lblDevice04Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device04_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Main\nLights");
                        }
                    }
                }
                {
                    // BtnDevice05
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_device05 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 120, 120);
                    lv_obj_add_event_cb(obj, action_send_device_command, LV_EVENT_SHORT_CLICKED, (void *)5);
                    lv_obj_add_event_cb(obj, action_show_device_brightness_dialog, LV_EVENT_LONG_PRESSED, (void *)5);
                    add_style_style_button_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lblDevice05StatusInd
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device05_status_ind = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf54f");
                        }
                        {
                            // lblDevice05Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device05_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Porch\nLight");
                        }
                    }
                }
                {
                    // BtnDevice06
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_device06 = obj;
                    lv_obj_set_pos(obj, 130, 0);
                    lv_obj_set_size(obj, 120, 120);
                    lv_obj_add_event_cb(obj, action_send_device_command, LV_EVENT_SHORT_CLICKED, (void *)6);
                    lv_obj_add_event_cb(obj, action_show_device_brightness_dialog, LV_EVENT_LONG_PRESSED, (void *)6);
                    add_style_style_button_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lblDevice06StatusInd
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device06_status_ind = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf8ff");
                        }
                        {
                            // lblDevice06Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device06_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Trailer\nLights");
                        }
                    }
                }
                {
                    // BtnDevice07
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_device07 = obj;
                    lv_obj_set_pos(obj, -130, 0);
                    lv_obj_set_size(obj, 120, 120);
                    lv_obj_add_event_cb(obj, action_send_device_command, LV_EVENT_SHORT_CLICKED, (void *)7);
                    lv_obj_add_event_cb(obj, action_show_device_brightness_dialog, LV_EVENT_LONG_PRESSED, (void *)7);
                    add_style_style_button_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lblDevice07StatusInd
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device07_status_ind = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf773");
                        }
                        {
                            // lblDevice07Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device07_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Water\nPump");
                        }
                    }
                }
                {
                    // BtnDevice08
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_device08 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 120, 120);
                    lv_obj_add_event_cb(obj, action_send_device_command, LV_EVENT_SHORT_CLICKED, (void *)8);
                    lv_obj_add_event_cb(obj, action_show_device_brightness_dialog, LV_EVENT_LONG_PRESSED, (void *)8);
                    add_style_style_button_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lblDevice08StatusInd
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device08_status_ind = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf1e6");
                        }
                        {
                            // lblDevice08Label
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_device08_label = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Power\nOutlet");
                        }
                    }
                }
            }
        }
        {
            // ArcThermostat
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.arc_thermostat = obj;
            lv_obj_set_pos(obj, 10, 44);
            lv_obj_set_size(obj, 433, 433);
            lv_arc_set_range(obj, 35, 100);
            lv_arc_set_value(obj, 65);
            lv_obj_add_event_cb(obj, action_change_desired_temperature, LV_EVENT_PRESSING, (void *)0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SNAPPABLE);
            add_style_arc_default(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 500, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_color(obj, lv_color_hex(theme_colors[active_theme_index][5]), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_width(obj, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_rounded(obj, true, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_width(obj, 30, LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_INDICATOR | LV_STATE_CHECKED);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // TempBg
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.temp_bg = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 330, 330);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_style_panel_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 330, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_grad_color(obj, lv_color_hex(theme_colors[active_theme_index][2]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(obj, lv_color_hex(theme_colors[active_theme_index][1]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_width(obj, 80, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_x(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_shadow_ofs_y(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.obj0 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 250, 250);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_style_panel_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 250, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][5]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_grad_color(obj, lv_color_hex(theme_colors[active_theme_index][4]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_width(obj, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(theme_colors[active_theme_index][1]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // LabelDesiredTemperatureValue
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_desired_temperature_value = obj;
                                    lv_obj_set_pos(obj, -30, 6);
                                    lv_obj_set_size(obj, 90, 63);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "0");
                                }
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.obj1 = obj;
                                    lv_obj_set_pos(obj, 30, 1);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "°");
                                }
                                {
                                    // LabelCurrentInteriorTemperature
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_current_interior_temperature = obj;
                                    lv_obj_set_pos(obj, -23, 16);
                                    lv_obj_set_size(obj, 63, 63);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "0");
                                }
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.obj2 = obj;
                                    lv_obj_set_pos(obj, 21, 12);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "°");
                                }
                                {
                                    // LabelHeatActivatedIcon
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_heat_activated_icon = obj;
                                    lv_obj_set_pos(obj, -16, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &ui_font_fa32, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &ui_font_fa32, LV_PART_MAIN | LV_STATE_CHECKED);
                                    lv_label_set_text(obj, "\uf7e4");
                                }
                                {
                                    // LabelAcActivatedIcon
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_ac_activated_icon = obj;
                                    lv_obj_set_pos(obj, 16, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &ui_font_fa32, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][6]), LV_PART_MAIN | LV_STATE_CHECKED);
                                    lv_obj_set_style_text_font(obj, &ui_font_fa32, LV_PART_MAIN | LV_STATE_CHECKED);
                                    lv_label_set_text(obj, "\uf2dc");
                                }
                            }
                        }
                    }
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 494, 287);
            lv_obj_set_size(obj, 515, 225);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_panel_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // lblDate
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.lbl_date = obj;
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "----- --, ----");
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj3 = obj;
                    lv_obj_set_pos(obj, 53, 60);
                    lv_obj_set_size(obj, 124, 134);
                    add_style_style_content_panel(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][5]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lblTimeHour
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_time_hour = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "--");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj4 = obj;
                    lv_obj_set_pos(obj, 191, 60);
                    lv_obj_set_size(obj, 124, 134);
                    add_style_style_content_panel(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][5]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // lblTimeMinutes
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.lbl_time_minutes = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "--");
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj5 = obj;
                    lv_obj_set_pos(obj, 334, 60);
                    lv_obj_set_size(obj, 124, 134);
                    add_style_style_content_panel(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj6 = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][0]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "PM");
                        }
                    }
                }
            }
        }
        {
            // ContainerModalBackground
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.container_modal_background = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_PCT(100), 600);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][1]), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 190, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // PanelDeviceBrighnessLevel
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_device_brighness_level = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 640, 140);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_style_panel_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            lv_obj_set_pos(obj, 5, -10);
                            lv_obj_set_size(obj, 100, 50);
                            lv_obj_add_event_cb(obj, action_close_dialog, LV_EVENT_CLICKED, (void *)0);
                            add_style_button_circle(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_fa32_icon(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "\uf057");
                                }
                            }
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 30);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_label_default(obj);
                            lv_label_set_text(obj, "Device Brightness");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_label_fa16_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf185");
                        }
                        {
                            // SliderDeviceBrightnessLevel
                            lv_obj_t *obj = lv_slider_create(parent_obj);
                            objects.slider_device_brightness_level = obj;
                            lv_obj_set_pos(obj, -22, -4);
                            lv_obj_set_size(obj, 476, 10);
                            lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                            lv_obj_add_event_cb(obj, action_set_device_brightness_level, LV_EVENT_PRESSING, (void *)0);
                            add_style_style_default_slider(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, -40, 10);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf185");
                        }
                    }
                }
            }
        }
    }
    
    tick_screen_page_home();
}

void tick_screen_page_home() {
    tick_user_widget_bottom_nav_bar(9);
}

void create_screen_page_trailer() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_trailer = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    add_style_style_screen_default(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // WidgetTrailerPageBottomNavBar
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.widget_trailer_page_bottom_nav_bar = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 60);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_bottom_nav_bar(obj, 60);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // PanelTrailerSideView
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_trailer_side_view = obj;
            lv_obj_set_pos(obj, 10, 10);
            lv_obj_set_size(obj, 571, 330);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_panel_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Front to Back");
                }
                {
                    lv_obj_t *obj = lv_arc_create(parent_obj);
                    objects.obj7 = obj;
                    lv_obj_set_pos(obj, 2, 27);
                    lv_obj_set_size(obj, 150, 172);
                    lv_arc_set_range(obj, 0, 180);
                    lv_arc_set_value(obj, 90);
                    lv_arc_set_bg_start_angle(obj, 180);
                    lv_arc_set_bg_end_angle(obj, 0);
                    add_style_arc_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_width(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_opa(obj, 128, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_opa(obj, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_img_src(obj, &img_img_chevron_up32px, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_img_recolor(obj, lv_color_hex(theme_colors[active_theme_index][0]), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_img_opa(obj, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_img_recolor_opa(obj, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 23, -127);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Front");
                }
                {
                    // LabelFrontLevelValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_front_level_value = obj;
                    lv_obj_set_pos(obj, 1, 66);
                    lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0.00");
                }
                {
                    // LabelFrontLevelFaIndicator
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_front_level_fa_indicator = obj;
                    lv_obj_set_pos(obj, 10, 27);
                    lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][11]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_label_set_text(obj, "\uf078");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -20, -123);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Rear");
                }
                {
                    // LabelBackLevelValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_back_level_value = obj;
                    lv_obj_set_pos(obj, -14, 70);
                    lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0.00");
                }
                {
                    // LabelFrontLeveLabel_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_front_leve_label_1 = obj;
                    lv_obj_set_pos(obj, 0, 70);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\"");
                }
                {
                    // LabelBackLevelFaIndicator
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_back_level_fa_indicator = obj;
                    lv_obj_set_pos(obj, -5, 31);
                    lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][11]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_label_set_text(obj, "\uf078");
                }
                {
                    // LabelFrontLeveLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_front_leve_label = obj;
                    lv_obj_set_pos(obj, 75, 70);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\"");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj8 = obj;
            lv_obj_set_pos(obj, 5, -75);
            lv_obj_set_size(obj, 576, 169);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_panel_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj9 = obj;
                    lv_obj_set_pos(obj, 5, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uf6fc");
                }
                {
                    // LabelGnssElevationValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_gnss_elevation_value = obj;
                    lv_obj_set_pos(obj, 45, 10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "-");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj10 = obj;
                    lv_obj_set_pos(obj, 95, 10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "ft");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj11 = obj;
                    lv_obj_set_pos(obj, -40, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uf7c0");
                }
                {
                    // LabelGnssNumSatsValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_gnss_num_sats_value = obj;
                    lv_obj_set_pos(obj, -5, 10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "-");
                }
                {
                    // LabelGnssModeValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_gnss_mode_value = obj;
                    lv_obj_set_pos(obj, 5, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Not Available");
                }
                {
                    // LabelLatitudeValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_latitude_value = obj;
                    lv_obj_set_pos(obj, 0, 60);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_label_set_text(obj, "--");
                }
                {
                    // LabelLongitudeValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_longitude_value = obj;
                    lv_obj_set_pos(obj, 0, 60);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "--");
                }
            }
        }
        {
            // PanelTrailerBack
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_trailer_back = obj;
            lv_obj_set_pos(obj, -10, 10);
            lv_obj_set_size(obj, 416, 515);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_panel_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Side to Side");
                }
                {
                    lv_obj_t *obj = lv_arc_create(parent_obj);
                    objects.obj12 = obj;
                    lv_obj_set_pos(obj, 5, 45);
                    lv_obj_set_size(obj, 161, 172);
                    lv_arc_set_range(obj, 0, 180);
                    lv_arc_set_value(obj, 90);
                    lv_arc_set_bg_start_angle(obj, 180);
                    lv_arc_set_bg_end_angle(obj, 0);
                    add_style_arc_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_width(obj, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_opa(obj, 128, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_opa(obj, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_arc_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_img_src(obj, &img_img_chevron_up32px, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_img_recolor(obj, lv_color_hex(theme_colors[active_theme_index][0]), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_img_opa(obj, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_img_recolor_opa(obj, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 4, -36);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Drivers Side");
                }
                {
                    // LabelLeftSideLevelFaIndicator
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_left_side_level_fa_indicator = obj;
                    lv_obj_set_pos(obj, 4, -20);
                    lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][11]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_label_set_text(obj, "\uf078");
                }
                {
                    // LabelLeftSideLevelValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_left_side_level_value = obj;
                    lv_obj_set_pos(obj, -5, 19);
                    lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0.00");
                }
                {
                    // LabelLeftSideLeveLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_left_side_leve_label = obj;
                    lv_obj_set_pos(obj, 65, 19);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\"");
                }
                {
                    // LabelRightSideLevelValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_right_side_level_value = obj;
                    lv_obj_set_pos(obj, -5, 19);
                    lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0.00");
                }
                {
                    // LabelRightSideLeveLabel
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_right_side_leve_label = obj;
                    lv_obj_set_pos(obj, 5, 19);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\"");
                }
                {
                    // LabelRightSideLevelFaIndicator
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_right_side_level_fa_indicator = obj;
                    lv_obj_set_pos(obj, 4, -20);
                    lv_obj_set_size(obj, 70, LV_SIZE_CONTENT);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    lv_obj_add_state(obj, LV_STATE_CHECKED);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][11]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_CHECKED);
                    lv_label_set_text(obj, "\uf077");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 5, -36);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Passenger Side");
                }
            }
        }
        {
            lv_obj_t *obj = lv_img_create(parent_obj);
            lv_obj_set_pos(obj, 139, 143);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_trailer_side);
        }
        {
            lv_obj_t *obj = lv_img_create(parent_obj);
            lv_obj_set_pos(obj, 663, 150);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_img_set_src(obj, &img_trailer_back);
        }
    }
    
    tick_screen_page_trailer();
}

void tick_screen_page_trailer() {
    tick_user_widget_bottom_nav_bar(60);
}

void create_screen_page_power_management() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_power_management = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    add_style_style_screen_default(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // WidgetPowerManagementPageBottomNavBar
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.widget_power_management_page_bottom_nav_bar = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 60);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_bottom_nav_bar(obj, 92);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // PanelSolarInput
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_solar_input = obj;
            lv_obj_set_pos(obj, 10, 10);
            lv_obj_set_size(obj, 328, 149);
            add_style_style_content_panel(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 25, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uf185");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 20);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uf5ba");
                }
                {
                    // LabelSolarPowerWattageValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_solar_power_wattage_value = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_nav_text_alternate(obj);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "WATTS");
                }
            }
        }
        {
            // PanelChargeType
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_charge_type = obj;
            lv_obj_set_pos(obj, 12, -81);
            lv_obj_set_size(obj, 328, 96);
            add_style_style_content_panel(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][5]), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // LabelChargeStatusValueS
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_charge_status_value_s = obj;
                    lv_obj_set_pos(obj, 43, -1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_nav_text_alternate(obj);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "UNKOWN");
                }
            }
        }
        {
            // PanelShorePower
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_shore_power = obj;
            lv_obj_set_pos(obj, 11, -70);
            lv_obj_set_size(obj, 328, 259);
            add_style_style_content_panel(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][4]), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // LabelPowerShorePowerHeading
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_power_shore_power_heading = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Shore Power");
                }
                {
                    // LabelShorePowerConnectionStatus
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_shore_power_connection_status = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Not Connected");
                }
                {
                    // PanelShorePowerIndicatorBackground
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.panel_shore_power_indicator_background = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 60, 60);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_style_panel_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][5]), LV_PART_MAIN | LV_STATE_CHECKED);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // PanelShorePowerIndicatorForeground
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.panel_shore_power_indicator_foreground = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][0]), LV_PART_MAIN | LV_STATE_CHECKED);
                            lv_label_set_text(obj, "\uf1e6");
                        }
                    }
                }
            }
        }
        {
            // PanelPowerBatteryStats
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_power_battery_stats = obj;
            lv_obj_set_pos(obj, 0, 10);
            lv_obj_set_size(obj, 328, 520);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_content_panel(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // LabelPowerRemainingCacpity_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_power_remaining_cacpity_1 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Battery Status");
                }
                {
                    // BarBatterySoc
                    lv_obj_t *obj = lv_bar_create(parent_obj);
                    objects.bar_battery_soc = obj;
                    lv_obj_set_pos(obj, 0, 20);
                    lv_obj_set_size(obj, 240, 400);
                    add_style_bar_default(obj);
                    lv_obj_set_style_radius(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_outline_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_outline_color(obj, lv_color_hex(theme_colors[active_theme_index][5]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_bottom(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_top(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_right(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 25, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][5]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_outline_width(obj, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.obj13 = obj;
                            lv_obj_set_pos(obj, 30, -20);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_label_default(obj);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "%");
                        }
                        {
                            // LabelPowerBatteryPercentageValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.label_power_battery_percentage_value = obj;
                            lv_obj_set_pos(obj, -10, -20);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // LabelBatteryVoltageValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.label_battery_voltage_value = obj;
                            lv_obj_set_pos(obj, -10, 30);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 30, 30);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_label_nav_text_alternate(obj);
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "V");
                        }
                    }
                }
            }
        }
        {
            // PanelPowerRemainingTime
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_power_remaining_time = obj;
            lv_obj_set_pos(obj, -4, 10);
            lv_obj_set_size(obj, 328, 515);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_content_panel(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // LabelPowerRemainingCacpity
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_power_remaining_cacpity = obj;
                    lv_obj_set_pos(obj, 5, 5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Battery Consumption");
                }
                {
                    // PowerRemainingArcGroup
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.power_remaining_arc_group = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 350, 350);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_style_panel_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // PowerArcCurrentConsumption
                            lv_obj_t *obj = lv_arc_create(parent_obj);
                            objects.power_arc_current_consumption = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 300, 300);
                            lv_arc_set_range(obj, 0, 2000);
                            lv_arc_set_value(obj, 200);
                            add_style_arc_default(obj);
                            lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][0]), LV_PART_KNOB | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_width(obj, 20, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_color(obj, lv_color_hex(theme_colors[active_theme_index][0]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_width(obj, 25, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_arc_color(obj, lv_color_hex(0xff635c5c), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff746e6e), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 250, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_top(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_bottom(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_left(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_right(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_color(obj, lv_color_hex(0xff625d5d), LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_border_width(obj, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // PanelPowerRemainingCenter
                            lv_obj_t *obj = lv_obj_create(parent_obj);
                            objects.panel_power_remaining_center = obj;
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 180, 180);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_style_panel_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_radius(obj, 300, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff7c7777), LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // LabelPowerRemainingTimeToGoValue
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.label_power_remaining_time_to_go_value = obj;
                            lv_obj_set_pos(obj, -5, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_style_top_nav_text_normal(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "0");
                        }
                        {
                            // LabelPowerRemaining
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.label_power_remaining = obj;
                            lv_obj_set_pos(obj, 0, -100);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_default(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "Remaining");
                        }
                    }
                }
                {
                    // LabelPowerConsumptionWattageValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_power_consumption_wattage_value = obj;
                    lv_obj_set_pos(obj, -50, -5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 50, -5);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_nav_text_alternate(obj);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "WATTS");
                }
            }
        }
    }
    
    tick_screen_page_power_management();
}

void tick_screen_page_power_management() {
    tick_user_widget_bottom_nav_bar(92);
}

void create_screen_page_air_quality() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_air_quality = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    add_style_style_screen_default(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // WidgetAirQualityPageBottomNavBar
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.widget_air_quality_page_bottom_nav_bar = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 60);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_bottom_nav_bar(obj, 122);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 10, 10);
            lv_obj_set_size(obj, 495, 255);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_panel_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj14 = obj;
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uf2c7");
                }
                {
                    // LabelAirQualityTemperatureValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_air_quality_temperature_value = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "--");
                }
                {
                    // LabelTempValueIndicator
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_temp_value_indicator = obj;
                    lv_obj_set_pos(obj, 40, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "°F");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Temperature");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, -10, 10);
            lv_obj_set_size(obj, 495, 255);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_panel_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj15 = obj;
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][6]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uf043");
                }
                {
                    // LabelAirQualityHumdityValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_air_quality_humdity_value = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "--");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj16 = obj;
                    lv_obj_set_pos(obj, 40, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "%");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Humidity");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, -10, -70);
            lv_obj_set_size(obj, 495, 255);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_panel_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj17 = obj;
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][1]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uf0c2");
                }
                {
                    // label_air_quality_co2_value
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_air_quality_co2_value = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "--");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj18 = obj;
                    lv_obj_set_pos(obj, 70, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "ppm");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "CO");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 20, 2);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "2");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 10, -71);
            lv_obj_set_size(obj, 495, 255);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_panel_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj19 = obj;
                    lv_obj_set_pos(obj, 0, 10);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][1]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uf72e");
                }
                {
                    // label_air_quality_tvoc_value
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_air_quality_tvoc_value = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "--");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "TVOC");
                }
            }
        }
    }
    
    tick_screen_page_air_quality();
}

void tick_screen_page_air_quality() {
    tick_user_widget_bottom_nav_bar(122);
}

void create_screen_page_water() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_water = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    add_style_style_screen_default(obj);
    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(obj, lv_color_hex(0xffd62b2b), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(obj, 30, LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // WidgetWaterPageBottomNavBar
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.widget_water_page_bottom_nav_bar = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 60);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_bottom_nav_bar(obj, 140);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // PanelFreshWater
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.panel_fresh_water = obj;
            lv_obj_set_pos(obj, 10, 10);
            lv_obj_set_size(obj, 325, 447);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_panel_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // LabelFreshWaterIcon
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_fresh_water_icon = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][6]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uf043");
                }
                {
                    // LabelFreshWaterText
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_fresh_water_text = obj;
                    lv_obj_set_pos(obj, 101, 43);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "FRESH");
                }
                {
                    // BarFreshWaterValue
                    lv_obj_t *obj = lv_bar_create(parent_obj);
                    objects.bar_fresh_water_value = obj;
                    lv_obj_set_pos(obj, 2, -36);
                    lv_obj_set_size(obj, 160, 300);
                    lv_bar_set_value(obj, 1, LV_ANIM_OFF);
                    add_style_bar_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_img_src(obj, &img_water_level_bg, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][6]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                }
                {
                    // LabelFreshWaterValue
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_fresh_water_value = obj;
                    lv_obj_set_pos(obj, 122, 382);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 144, 382);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_secondary(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "%");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 10);
            lv_obj_set_size(obj, 325, 447);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_panel_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.obj20 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_fa32_icon(obj);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "\uf043");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 110, 44);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "GREY");
                }
                {
                    // BarGreyWaterLevel
                    lv_obj_t *obj = lv_bar_create(parent_obj);
                    objects.bar_grey_water_level = obj;
                    lv_obj_set_pos(obj, 2, -36);
                    lv_obj_set_size(obj, 160, 300);
                    lv_bar_set_value(obj, 1, LV_ANIM_OFF);
                    add_style_bar_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_img_src(obj, &img_water_level_bg, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 122, 382);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 144, 382);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_secondary(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "%");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, -10, 10);
            lv_obj_set_size(obj, 325, 447);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_panel_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_fa32_icon(obj);
                    lv_label_set_text(obj, "\uf043");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 101, 43);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "BLACK");
                }
                {
                    // BarBlackWaterLevel
                    lv_obj_t *obj = lv_bar_create(parent_obj);
                    objects.bar_black_water_level = obj;
                    lv_obj_set_pos(obj, 2, -36);
                    lv_obj_set_size(obj, 160, 300);
                    lv_bar_set_value(obj, 1, LV_ANIM_OFF);
                    add_style_bar_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_img_src(obj, &img_water_level_bg, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][1]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 122, 382);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 144, 382);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_secondary(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "%");
                }
            }
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 1, -76);
            lv_obj_set_size(obj, 1004, 57);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_panel_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj21 = obj;
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 20, 20);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_style_panel_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][6]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 30, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_LEFT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Fresh - refill when low");
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj22 = obj;
                    lv_obj_set_pos(obj, -130, 0);
                    lv_obj_set_size(obj, 20, 20);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_style_panel_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -1, -1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Grey - empty when high");
                }
                {
                    lv_obj_t *obj = lv_obj_create(parent_obj);
                    objects.obj23 = obj;
                    lv_obj_set_pos(obj, -240, 0);
                    lv_obj_set_size(obj, 20, 20);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_style_panel_default(obj);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][1]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_radius(obj, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -18, -1);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_RIGHT_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Black - empty when high");
                }
            }
        }
    }
    
    tick_screen_page_water();
}

void tick_screen_page_water() {
    tick_user_widget_bottom_nav_bar(140);
}

void create_screen_page_settings() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_settings = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    add_style_style_screen_default(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // WidgetSettingsPageBottomNavBar
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.widget_settings_page_bottom_nav_bar = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 60);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            create_user_widget_bottom_nav_bar(obj, 158);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_tabview_create(parent_obj, LV_DIR_LEFT, 32);
            objects.obj24 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 1024, 540);
            add_style_tab_view_default(obj);
            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][4]), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_tabview_get_tab_btns(parent_obj);
                    objects.obj25 = obj;
                    lv_obj_set_style_min_width(obj, 250, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_width(obj, 250, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][5]), LV_PART_ITEMS | LV_STATE_CHECKED);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][7]), LV_PART_ITEMS | LV_STATE_CHECKED);
                    lv_obj_set_style_border_color(obj, lv_color_hex(theme_colors[active_theme_index][0]), LV_PART_ITEMS | LV_STATE_CHECKED);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][4]), LV_PART_ITEMS | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_22, LV_PART_ITEMS | LV_STATE_DEFAULT);
                    lv_obj_set_style_pad_left(obj, 0, LV_PART_ITEMS | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(obj, lv_color_hex(theme_colors[active_theme_index][9]), LV_PART_ITEMS | LV_STATE_DEFAULT);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_tabview_add_tab(lv_obj_get_parent(parent_obj), "Display");
                            add_style_tab_default(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 40);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_default(obj);
                                    lv_label_set_text(obj, "Display Theme");
                                }
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 6);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_secondary(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "Displays Settings");
                                }
                                {
                                    // BtnThemeLight
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.btn_theme_light = obj;
                                    lv_obj_set_pos(obj, 0, 80);
                                    lv_obj_set_size(obj, 100, 50);
                                    lv_obj_add_event_cb(obj, action_change_theme, LV_EVENT_PRESSED, (void *)0);
                                    lv_obj_add_state(obj, LV_STATE_CHECKED);
                                    add_style_style_button_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // lblDevice08StatusInd_5
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_device08_status_ind_5 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 32, 32);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_fa32_icon(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "\uf185");
                                        }
                                    }
                                }
                                {
                                    // BtnThemeDark
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.btn_theme_dark = obj;
                                    lv_obj_set_pos(obj, 100, 80);
                                    lv_obj_set_size(obj, 100, 50);
                                    lv_obj_add_event_cb(obj, action_change_theme, LV_EVENT_PRESSED, (void *)1);
                                    add_style_style_button_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // lblDevice08StatusInd_6
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_device08_status_ind_6 = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 32, 32);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_fa32_icon(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "\uf186");
                                        }
                                    }
                                }
                                {
                                    // LabelScreenBrightness
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_screen_brightness = obj;
                                    lv_obj_set_pos(obj, 0, 140);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "Brightness");
                                }
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 205);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_fa16_icon(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "\uf185");
                                }
                                {
                                    // SliderScreenBrightness
                                    lv_obj_t *obj = lv_slider_create(parent_obj);
                                    objects.slider_screen_brightness = obj;
                                    lv_obj_set_pos(obj, 30, 210);
                                    lv_obj_set_size(obj, 640, 10);
                                    lv_slider_set_value(obj, 25, LV_ANIM_OFF);
                                    lv_obj_add_event_cb(obj, action_change_screen_brightness, LV_EVENT_PRESSING, (void *)0);
                                    add_style_style_default_slider(obj);
                                }
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 700, 200);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_fa32_icon(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "\uf185");
                                }
                                {
                                    // LabelScreenTimeout
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_screen_timeout = obj;
                                    lv_obj_set_pos(obj, -1, 250);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "Screen Timeout (Minutes)");
                                }
                                {
                                    // ButtonScreenTimeoutDecrease
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.button_screen_timeout_decrease = obj;
                                    lv_obj_set_pos(obj, 0, 300);
                                    lv_obj_set_size(obj, 100, 50);
                                    lv_obj_add_event_cb(obj, action_timeout_changed, LV_EVENT_PRESSED, (void *)0);
                                    add_style_style_button_default(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa32, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "\uf068");
                                        }
                                    }
                                }
                                {
                                    // LabelScreenTimeoutValue
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_screen_timeout_value = obj;
                                    lv_obj_set_pos(obj, 135, 310);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "1");
                                }
                                {
                                    // ButtonScreenTimeoutIncrease
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.button_screen_timeout_increase = obj;
                                    lv_obj_set_pos(obj, 189, 300);
                                    lv_obj_set_size(obj, 100, 50);
                                    lv_obj_add_event_cb(obj, action_timeout_changed, LV_EVENT_PRESSED, (void *)1);
                                    add_style_style_button_default(obj);
                                    lv_obj_set_style_text_font(obj, &ui_font_fa32, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_font(obj, &ui_font_fa32, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "+");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            // TabConnectivity
                            lv_obj_t *obj = lv_tabview_add_tab(lv_obj_get_parent(parent_obj), "Connectivity\n");
                            objects.tab_connectivity = obj;
                            add_style_tab_default(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // LabelWifiConnectionStatus
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_wifi_connection_status = obj;
                                    lv_obj_set_pos(obj, 0, 10);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_default(obj);
                                    lv_label_set_text(obj, "Status: Not connected");
                                }
                            }
                        }
                        {
                            lv_obj_t *obj = lv_tabview_add_tab(lv_obj_get_parent(parent_obj), "Locale");
                            add_style_tab_default(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                    add_style_label_list_menu_item(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "Date/Time");
                                }
                                {
                                    // LabelTimeZoneHeader
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_time_zone_header = obj;
                                    lv_obj_set_pos(obj, 0, 50);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_CHECKED);
                                    lv_label_set_text(obj, "TimeZone");
                                }
                                {
                                    // DropDownSelectedTimeZone
                                    lv_obj_t *obj = lv_dropdown_create(parent_obj);
                                    objects.drop_down_selected_time_zone = obj;
                                    lv_obj_set_pos(obj, 0, 80);
                                    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                                    lv_dropdown_set_options(obj, "Alaska\nChicago, Illinois\nDenver, Colorado\nHawaii\nLos Angeles\nNew York\nPhoenix");
                                    lv_dropdown_set_selected(obj, 0);
                                    lv_obj_add_event_cb(obj, action_timezone_change, LV_EVENT_VALUE_CHANGED, (void *)0);
                                    add_style_dropdown_default(obj);
                                }
                                {
                                    // LabelTemperatureReadingsHeader
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_temperature_readings_header = obj;
                                    lv_obj_set_pos(obj, 0, 140);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_CHECKED);
                                    lv_label_set_text(obj, "Temperature Readings");
                                }
                                {
                                    // BtnTempFahrenheit
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.btn_temp_fahrenheit = obj;
                                    lv_obj_set_pos(obj, 0, 170);
                                    lv_obj_set_size(obj, 100, 50);
                                    lv_obj_add_event_cb(obj, action_temperature_unit_change, LV_EVENT_PRESSED, (void *)0);
                                    lv_obj_add_state(obj, LV_STATE_CHECKED);
                                    add_style_style_button_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // LblTempFahrenheitIcon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_temp_fahrenheit_icon = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 32, 32);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "°F");
                                        }
                                    }
                                }
                                {
                                    // BtnTempCelsius
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.btn_temp_celsius = obj;
                                    lv_obj_set_pos(obj, 100, 170);
                                    lv_obj_set_size(obj, 100, 50);
                                    lv_obj_add_event_cb(obj, action_temperature_unit_change, LV_EVENT_PRESSED, (void *)1);
                                    add_style_style_button_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // LblTempCelsiusIcon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_temp_celsius_icon = obj;
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 32, 32);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "°C");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            lv_obj_t *obj = lv_tabview_add_tab(lv_obj_get_parent(parent_obj), "Buttons");
                            add_style_tab_default(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 20, 10);
                                    lv_obj_set_size(obj, 600, 36);
                                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                    add_style_label_default(obj);
                                    lv_label_set_text(obj, "Edit Button Labels & Icons");
                                }
                                {
                                    // BtnEditAppearance01
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.btn_edit_appearance01 = obj;
                                    lv_obj_set_pos(obj, 30, 60);
                                    lv_obj_set_size(obj, 140, 140);
                                    lv_obj_add_event_cb(obj, action_navigate_to_button_edit, LV_EVENT_SHORT_CLICKED, (void *)1);
                                    add_style_style_button_default(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // LblEditBtn01Icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn01_icon = obj;
                                            lv_obj_set_pos(obj, 0, 15);
                                            lv_obj_set_size(obj, 32, 32);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_fa32_icon(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // LblEditBtn01Label
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn01_label = obj;
                                            lv_obj_set_pos(obj, 0, 55);
                                            lv_obj_set_size(obj, 120, 80);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Device 1");
                                        }
                                    }
                                }
                                {
                                    // BtnEditAppearance02
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.btn_edit_appearance02 = obj;
                                    lv_obj_set_pos(obj, 184, 60);
                                    lv_obj_set_size(obj, 140, 140);
                                    lv_obj_add_event_cb(obj, action_navigate_to_button_edit, LV_EVENT_SHORT_CLICKED, (void *)2);
                                    add_style_style_button_default(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // LblEditBtn02Icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn02_icon = obj;
                                            lv_obj_set_pos(obj, 0, 15);
                                            lv_obj_set_size(obj, 32, 32);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_fa32_icon(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // LblEditBtn02Label
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn02_label = obj;
                                            lv_obj_set_pos(obj, 0, 55);
                                            lv_obj_set_size(obj, 120, 80);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Device 2");
                                        }
                                    }
                                }
                                {
                                    // BtnEditAppearance03
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.btn_edit_appearance03 = obj;
                                    lv_obj_set_pos(obj, 338, 60);
                                    lv_obj_set_size(obj, 140, 140);
                                    lv_obj_add_event_cb(obj, action_navigate_to_button_edit, LV_EVENT_SHORT_CLICKED, (void *)3);
                                    add_style_style_button_default(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // LblEditBtn03Icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn03_icon = obj;
                                            lv_obj_set_pos(obj, 0, 15);
                                            lv_obj_set_size(obj, 32, 32);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_fa32_icon(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // LblEditBtn03Label
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn03_label = obj;
                                            lv_obj_set_pos(obj, 0, 55);
                                            lv_obj_set_size(obj, 120, 80);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Device 3");
                                        }
                                    }
                                }
                                {
                                    // BtnEditAppearance04
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.btn_edit_appearance04 = obj;
                                    lv_obj_set_pos(obj, 30, 214);
                                    lv_obj_set_size(obj, 140, 140);
                                    lv_obj_add_event_cb(obj, action_navigate_to_button_edit, LV_EVENT_SHORT_CLICKED, (void *)4);
                                    add_style_style_button_default(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // LblEditBtn04Icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn04_icon = obj;
                                            lv_obj_set_pos(obj, 0, 15);
                                            lv_obj_set_size(obj, 32, 32);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_fa32_icon(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // LblEditBtn04Label
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn04_label = obj;
                                            lv_obj_set_pos(obj, 0, 55);
                                            lv_obj_set_size(obj, 120, 80);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Device 4");
                                        }
                                    }
                                }
                                {
                                    // BtnEditAppearance05
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.btn_edit_appearance05 = obj;
                                    lv_obj_set_pos(obj, 184, 214);
                                    lv_obj_set_size(obj, 140, 140);
                                    lv_obj_add_event_cb(obj, action_navigate_to_button_edit, LV_EVENT_SHORT_CLICKED, (void *)5);
                                    add_style_style_button_default(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // LblEditBtn05Icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn05_icon = obj;
                                            lv_obj_set_pos(obj, 0, 15);
                                            lv_obj_set_size(obj, 32, 32);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_fa32_icon(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // LblEditBtn05Label
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn05_label = obj;
                                            lv_obj_set_pos(obj, 0, 55);
                                            lv_obj_set_size(obj, 120, 80);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Device 5");
                                        }
                                    }
                                }
                                {
                                    // BtnEditAppearance06
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.btn_edit_appearance06 = obj;
                                    lv_obj_set_pos(obj, 338, 214);
                                    lv_obj_set_size(obj, 140, 140);
                                    lv_obj_add_event_cb(obj, action_navigate_to_button_edit, LV_EVENT_SHORT_CLICKED, (void *)6);
                                    add_style_style_button_default(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // LblEditBtn06Icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn06_icon = obj;
                                            lv_obj_set_pos(obj, 0, 15);
                                            lv_obj_set_size(obj, 32, 32);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_fa32_icon(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // LblEditBtn06Label
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn06_label = obj;
                                            lv_obj_set_pos(obj, 0, 55);
                                            lv_obj_set_size(obj, 120, 80);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Device 6");
                                        }
                                    }
                                }
                                {
                                    // BtnEditAppearance07
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.btn_edit_appearance07 = obj;
                                    lv_obj_set_pos(obj, 30, 368);
                                    lv_obj_set_size(obj, 140, 140);
                                    lv_obj_add_event_cb(obj, action_navigate_to_button_edit, LV_EVENT_SHORT_CLICKED, (void *)7);
                                    add_style_style_button_default(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // LblEditBtn07Icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn07_icon = obj;
                                            lv_obj_set_pos(obj, 0, 15);
                                            lv_obj_set_size(obj, 32, 32);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_fa32_icon(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // LblEditBtn07Label
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn07_label = obj;
                                            lv_obj_set_pos(obj, 0, 55);
                                            lv_obj_set_size(obj, 120, 80);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Device 7");
                                        }
                                    }
                                }
                                {
                                    // BtnEditAppearance08
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.btn_edit_appearance08 = obj;
                                    lv_obj_set_pos(obj, 184, 368);
                                    lv_obj_set_size(obj, 140, 140);
                                    lv_obj_add_event_cb(obj, action_navigate_to_button_edit, LV_EVENT_SHORT_CLICKED, (void *)8);
                                    add_style_style_button_default(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            // LblEditBtn08Icon
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn08_icon = obj;
                                            lv_obj_set_pos(obj, 0, 15);
                                            lv_obj_set_size(obj, 32, 32);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_fa32_icon(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "");
                                        }
                                        {
                                            // LblEditBtn08Label
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            objects.lbl_edit_btn08_label = obj;
                                            lv_obj_set_pos(obj, 0, 55);
                                            lv_obj_set_size(obj, 120, 80);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_TOP_MID, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Device 8");
                                        }
                                    }
                                }
                                {
                                    // BtnResetAllButtons
                                    lv_obj_t *obj = lv_btn_create(parent_obj);
                                    objects.btn_reset_all_buttons = obj;
                                    lv_obj_set_pos(obj, 614, 6);
                                    lv_obj_set_size(obj, 150, 44);
                                    lv_obj_add_event_cb(obj, action_reset_all_buttons, LV_EVENT_SHORT_CLICKED, (void *)0);
                                    add_style_style_button_default(obj);
                                    {
                                        lv_obj_t *parent_obj = obj;
                                        {
                                            lv_obj_t *obj = lv_label_create(parent_obj);
                                            lv_obj_set_pos(obj, 0, 0);
                                            lv_obj_set_size(obj, 140, 30);
                                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                                            add_style_label_default(obj);
                                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                            lv_label_set_text(obj, "Reset All");
                                        }
                                    }
                                }
                            }
                        }
                        {
                            lv_obj_t *obj = lv_tabview_add_tab(lv_obj_get_parent(parent_obj), "About");
                            add_style_tab_default(obj);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    // LabelAboutHeader
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_about_header = obj;
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, LV_PART_MAIN | LV_STATE_CHECKED);
                                    lv_label_set_text(obj, "Current Version");
                                }
                                {
                                    // LabelVersionNumber
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.label_version_number = obj;
                                    lv_obj_set_pos(obj, 0, 40);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_default(obj);
                                    lv_label_set_text(obj, "0.0.0");
                                }
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 85);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_default(obj);
                                    lv_label_set_text(obj, "Mac Address");
                                }
                                {
                                    // McuMacAddressValue
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    objects.mcu_mac_address_value = obj;
                                    lv_obj_set_pos(obj, 0, 120);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    add_style_label_default(obj);
                                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "00:00:00:00:00:00");
                                }
                            }
                        }
                    }
                }
                {
                    lv_obj_t *obj = lv_tabview_get_content(parent_obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_TOP_RIGHT, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
                    lv_obj_set_style_min_width(obj, 774, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
                    lv_obj_set_style_max_width(obj, 774, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
                }
            }
        }
    }
    
    tick_screen_page_settings();
}

void tick_screen_page_settings() {
    tick_user_widget_bottom_nav_bar(158);
}

void create_screen_page_button_edit() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_button_edit = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE);
    add_style_style_screen_default(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 30, 10);
            lv_obj_set_size(obj, 300, 40);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_label_set_text(obj, "Edit Button");
        }
        {
            // BtnButtonEditCancel
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_button_edit_cancel = obj;
            lv_obj_set_pos(obj, 850, 10);
            lv_obj_set_size(obj, 140, 48);
            lv_obj_add_event_cb(obj, action_change_screen, LV_EVENT_SHORT_CLICKED, (void *)5);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 100, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Cancel");
                }
            }
        }
        {
            // LblButtonEditHeader
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_button_edit_header = obj;
            lv_obj_set_pos(obj, 360, 18);
            lv_obj_set_size(obj, 400, 32);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_secondary(obj);
            lv_label_set_text(obj, "Button ?");
        }
        {
            // LblButtonEditText
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_button_edit_text = obj;
            lv_obj_set_pos(obj, 30, 60);
            lv_obj_set_size(obj, 960, 45);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_obj_set_style_border_width(obj, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_color(obj, lv_color_hex(theme_colors[active_theme_index][0]), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 12, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(theme_colors[active_theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Type on keyboard...");
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 30, 115);
            lv_obj_set_size(obj, 960, 152);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            lv_obj_set_scroll_dir(obj, LV_DIR_VER);
            add_style_style_panel_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // BtnIconSlot00
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot00 = obj;
                    lv_obj_set_pos(obj, 4, 4);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)63414);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot01
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot01 = obj;
                    lv_obj_set_pos(obj, 80, 4);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61675);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot02
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot02 = obj;
                    lv_obj_set_pos(obj, 156, 4);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61829);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot03
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot03 = obj;
                    lv_obj_set_pos(obj, 232, 4);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61830);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot04
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot04 = obj;
                    lv_obj_set_pos(obj, 308, 4);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61671);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot05
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot05 = obj;
                    lv_obj_set_pos(obj, 384, 4);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61926);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot06
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot06 = obj;
                    lv_obj_set_pos(obj, 460, 4);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62016);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot07
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot07 = obj;
                    lv_obj_set_pos(obj, 536, 4);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62172);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot08
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot08 = obj;
                    lv_obj_set_pos(obj, 612, 4);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61549);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot09
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot09 = obj;
                    lv_obj_set_pos(obj, 688, 4);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62183);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot10
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot10 = obj;
                    lv_obj_set_pos(obj, 764, 4);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)63228);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot11
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot11 = obj;
                    lv_obj_set_pos(obj, 840, 4);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62951);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot12
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot12 = obj;
                    lv_obj_set_pos(obj, 4, 80);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61634);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot13
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot13 = obj;
                    lv_obj_set_pos(obj, 80, 80);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)63424);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot14
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot14 = obj;
                    lv_obj_set_pos(obj, 156, 80);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)63460);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot15
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot15 = obj;
                    lv_obj_set_pos(obj, 232, 80);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62890);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot16
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot16 = obj;
                    lv_obj_set_pos(obj, 308, 80);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)63587);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot17
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot17 = obj;
                    lv_obj_set_pos(obj, 384, 80);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62799);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot18
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot18 = obj;
                    lv_obj_set_pos(obj, 460, 80);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61684);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot19
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot19 = obj;
                    lv_obj_set_pos(obj, 536, 80);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61461);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot20
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot20 = obj;
                    lv_obj_set_pos(obj, 612, 80);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61869);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot21
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot21 = obj;
                    lv_obj_set_pos(obj, 688, 80);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62762);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot22
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot22 = obj;
                    lv_obj_set_pos(obj, 764, 80);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62198);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot23
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot23 = obj;
                    lv_obj_set_pos(obj, 840, 80);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61475);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot24
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot24 = obj;
                    lv_obj_set_pos(obj, 4, 156);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61572);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot25
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot25 = obj;
                    lv_obj_set_pos(obj, 80, 156);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61883);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot26
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot26 = obj;
                    lv_obj_set_pos(obj, 156, 156);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62973);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot27
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot27 = obj;
                    lv_obj_set_pos(obj, 232, 156);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61649);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot28
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot28 = obj;
                    lv_obj_set_pos(obj, 308, 156);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62612);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot29
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot29 = obj;
                    lv_obj_set_pos(obj, 384, 156);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61881);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot30
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot30 = obj;
                    lv_obj_set_pos(obj, 460, 156);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61958);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot31
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot31 = obj;
                    lv_obj_set_pos(obj, 536, 156);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61548);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot32
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot32 = obj;
                    lv_obj_set_pos(obj, 612, 156);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61612);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot33
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot33 = obj;
                    lv_obj_set_pos(obj, 688, 156);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61673);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot34
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot34 = obj;
                    lv_obj_set_pos(obj, 764, 156);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61571);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot35
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot35 = obj;
                    lv_obj_set_pos(obj, 840, 156);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61448);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot36
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot36 = obj;
                    lv_obj_set_pos(obj, 4, 232);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61441);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot37
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot37 = obj;
                    lv_obj_set_pos(obj, 80, 232);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61477);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot38
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot38 = obj;
                    lv_obj_set_pos(obj, 156, 232);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61515);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot39
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot39 = obj;
                    lv_obj_set_pos(obj, 232, 232);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61514);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot40
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot40 = obj;
                    lv_obj_set_pos(obj, 308, 232);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61517);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot41
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot41 = obj;
                    lv_obj_set_pos(obj, 384, 232);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61723);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot42
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot42 = obj;
                    lv_obj_set_pos(obj, 460, 232);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61589);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot43
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot43 = obj;
                    lv_obj_set_pos(obj, 536, 232);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61632);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot44
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot44 = obj;
                    lv_obj_set_pos(obj, 612, 232);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61447);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot45
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot45 = obj;
                    lv_obj_set_pos(obj, 688, 232);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61827);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot46
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot46 = obj;
                    lv_obj_set_pos(obj, 764, 232);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61683);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot47
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot47 = obj;
                    lv_obj_set_pos(obj, 840, 232);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61557);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot48
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot48 = obj;
                    lv_obj_set_pos(obj, 4, 308);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61459);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot49
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot49 = obj;
                    lv_obj_set_pos(obj, 80, 308);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61918);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot50
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot50 = obj;
                    lv_obj_set_pos(obj, 156, 308);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61444);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot51
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot51 = obj;
                    lv_obj_set_pos(obj, 232, 308);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61445);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot52
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot52 = obj;
                    lv_obj_set_pos(obj, 308, 308);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61796);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot53
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot53 = obj;
                    lv_obj_set_pos(obj, 384, 308);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61463);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot54
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot54 = obj;
                    lv_obj_set_pos(obj, 460, 308);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61747);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot55
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot55 = obj;
                    lv_obj_set_pos(obj, 536, 308);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61555);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot56
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot56 = obj;
                    lv_obj_set_pos(obj, 612, 308);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61442);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot57
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot57 = obj;
                    lv_obj_set_pos(obj, 688, 308);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61530);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot58
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot58 = obj;
                    lv_obj_set_pos(obj, 764, 308);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61597);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot59
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot59 = obj;
                    lv_obj_set_pos(obj, 840, 308);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61617);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot60
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot60 = obj;
                    lv_obj_set_pos(obj, 4, 384);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61638);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot61
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot61 = obj;
                    lv_obj_set_pos(obj, 80, 384);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61485);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot62
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot62 = obj;
                    lv_obj_set_pos(obj, 156, 384);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62006);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot63
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot63 = obj;
                    lv_obj_set_pos(obj, 232, 384);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62157);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot64
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot64 = obj;
                    lv_obj_set_pos(obj, 308, 384);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62803);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot65
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot65 = obj;
                    lv_obj_set_pos(obj, 384, 384);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)63380);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot66
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot66 = obj;
                    lv_obj_set_pos(obj, 460, 384);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62096);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot67
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot67 = obj;
                    lv_obj_set_pos(obj, 536, 384);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61681);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot68
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot68 = obj;
                    lv_obj_set_pos(obj, 612, 384);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61979);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot69
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot69 = obj;
                    lv_obj_set_pos(obj, 688, 384);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61550);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot70
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot70 = obj;
                    lv_obj_set_pos(obj, 764, 384);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61746);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot71
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot71 = obj;
                    lv_obj_set_pos(obj, 840, 384);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)62405);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot72
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot72 = obj;
                    lv_obj_set_pos(obj, 4, 460);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61505);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot73
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot73 = obj;
                    lv_obj_set_pos(obj, 80, 460);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61554);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot74
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot74 = obj;
                    lv_obj_set_pos(obj, 156, 460);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61749);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot75
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot75 = obj;
                    lv_obj_set_pos(obj, 232, 460);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61678);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot76
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot76 = obj;
                    lv_obj_set_pos(obj, 308, 460);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61547);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot77
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot77 = obj;
                    lv_obj_set_pos(obj, 384, 460);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61944);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot78
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot78 = obj;
                    lv_obj_set_pos(obj, 460, 460);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61541);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot79
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot79 = obj;
                    lv_obj_set_pos(obj, 536, 460);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61931);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot80
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot80 = obj;
                    lv_obj_set_pos(obj, 612, 460);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)61507);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot81
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot81 = obj;
                    lv_obj_set_pos(obj, 688, 460);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)57349);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot82
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot82 = obj;
                    lv_obj_set_pos(obj, 4, 536);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)63743);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
                {
                    // BtnIconSlot83
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    objects.btn_icon_slot83 = obj;
                    lv_obj_set_pos(obj, 80, 536);
                    lv_obj_set_size(obj, 68, 68);
                    lv_obj_add_event_cb(obj, action_select_button_icon, LV_EVENT_SHORT_CLICKED, (void *)63347);
                    add_style_style_button_default(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                            add_style_label_fa32_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "");
                        }
                    }
                }
            }
        }
        {
            // KbButtonEdit
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.kb_button_edit = obj;
            lv_obj_set_pos(obj, 0, 368);
            lv_obj_set_size(obj, 1024, 215);
            add_style_keyboard_default(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 30, 278);
            lv_obj_set_size(obj, 85, 28);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_label_set_text(obj, "Device:");
        }
        {
            // BtnEditDeviceNone
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_device_none = obj;
            lv_obj_set_pos(obj, 120, 270);
            lv_obj_set_size(obj, 110, 44);
            lv_obj_add_event_cb(obj, action_select_edit_device_type, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 90, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "None");
                }
            }
        }
        {
            // BtnEditDeviceTorrent
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_device_torrent = obj;
            lv_obj_set_pos(obj, 240, 270);
            lv_obj_set_size(obj, 130, 44);
            lv_obj_add_event_cb(obj, action_select_edit_device_type, LV_EVENT_SHORT_CLICKED, (void *)1);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 110, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Torrent");
                }
            }
        }
        {
            // BtnEditDeviceSwitchback
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_device_switchback = obj;
            lv_obj_set_pos(obj, 380, 270);
            lv_obj_set_size(obj, 155, 44);
            lv_obj_add_event_cb(obj, action_select_edit_device_type, LV_EVENT_SHORT_CLICKED, (void *)2);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 135, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Switchback");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 562, 278);
            lv_obj_set_size(obj, 60, 28);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_label_set_text(obj, "Addr:");
        }
        {
            // BtnEditAddr0
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_addr0 = obj;
            lv_obj_set_pos(obj, 630, 270);
            lv_obj_set_size(obj, 100, 44);
            lv_obj_add_event_cb(obj, action_select_edit_address, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 80, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
            }
        }
        {
            // BtnEditAddr1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_addr1 = obj;
            lv_obj_set_pos(obj, 740, 270);
            lv_obj_set_size(obj, 100, 44);
            lv_obj_add_event_cb(obj, action_select_edit_address, LV_EVENT_SHORT_CLICKED, (void *)1);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 80, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "1");
                }
            }
        }
        {
            // BtnEditAddr2
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_addr2 = obj;
            lv_obj_set_pos(obj, 850, 270);
            lv_obj_set_size(obj, 100, 44);
            lv_obj_add_event_cb(obj, action_select_edit_address, LV_EVENT_SHORT_CLICKED, (void *)2);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 80, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "2");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 30, 329);
            lv_obj_set_size(obj, 62, 28);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_label_set_text(obj, "Chan:");
        }
        {
            // BtnEditCh0
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_ch0 = obj;
            lv_obj_set_pos(obj, 100, 320);
            lv_obj_set_size(obj, 108, 44);
            lv_obj_add_event_cb(obj, action_select_edit_channel, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 98, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "0");
                }
            }
        }
        {
            // BtnEditCh1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_ch1 = obj;
            lv_obj_set_pos(obj, 213, 320);
            lv_obj_set_size(obj, 108, 44);
            lv_obj_add_event_cb(obj, action_select_edit_channel, LV_EVENT_SHORT_CLICKED, (void *)1);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 98, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "1");
                }
            }
        }
        {
            // BtnEditCh2
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_ch2 = obj;
            lv_obj_set_pos(obj, 326, 320);
            lv_obj_set_size(obj, 108, 44);
            lv_obj_add_event_cb(obj, action_select_edit_channel, LV_EVENT_SHORT_CLICKED, (void *)2);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 98, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "2");
                }
            }
        }
        {
            // BtnEditCh3
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_ch3 = obj;
            lv_obj_set_pos(obj, 439, 320);
            lv_obj_set_size(obj, 108, 44);
            lv_obj_add_event_cb(obj, action_select_edit_channel, LV_EVENT_SHORT_CLICKED, (void *)3);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 98, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "3");
                }
            }
        }
        {
            // BtnEditCh4
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_ch4 = obj;
            lv_obj_set_pos(obj, 552, 320);
            lv_obj_set_size(obj, 108, 44);
            lv_obj_add_event_cb(obj, action_select_edit_channel, LV_EVENT_SHORT_CLICKED, (void *)4);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 98, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "4");
                }
            }
        }
        {
            // BtnEditCh5
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_ch5 = obj;
            lv_obj_set_pos(obj, 665, 320);
            lv_obj_set_size(obj, 108, 44);
            lv_obj_add_event_cb(obj, action_select_edit_channel, LV_EVENT_SHORT_CLICKED, (void *)5);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 98, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "5");
                }
            }
        }
        {
            // BtnEditCh6
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_ch6 = obj;
            lv_obj_set_pos(obj, 778, 320);
            lv_obj_set_size(obj, 108, 44);
            lv_obj_add_event_cb(obj, action_select_edit_channel, LV_EVENT_SHORT_CLICKED, (void *)6);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 98, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "6");
                }
            }
        }
        {
            // BtnEditCh7
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_edit_ch7 = obj;
            lv_obj_set_pos(obj, 891, 320);
            lv_obj_set_size(obj, 108, 44);
            lv_obj_add_event_cb(obj, action_select_edit_channel, LV_EVENT_SHORT_CLICKED, (void *)7);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 98, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "7");
                }
            }
        }
        {
            // BtnButtonEditClear
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_button_edit_clear = obj;
            lv_obj_set_pos(obj, 700, 10);
            lv_obj_set_size(obj, 140, 48);
            lv_obj_add_event_cb(obj, action_clear_button, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 130, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Clear");
                }
            }
        }
    }
    
    tick_screen_page_button_edit();
}

void tick_screen_page_button_edit() {
}

void create_screen_page_device_assign() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.page_device_assign = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 1024, 600);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE);
    add_style_style_screen_default(obj);
    {
        lv_obj_t *parent_obj = obj;
        {
            // LblDeviceAssignHeader
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_device_assign_header = obj;
            lv_obj_set_pos(obj, 30, 20);
            lv_obj_set_size(obj, 700, 40);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_label_set_text(obj, "Assign Channels");
        }
        {
            // BtnDeviceInstance0
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_device_instance0 = obj;
            lv_obj_set_pos(obj, 30, 75);
            lv_obj_set_size(obj, 170, 60);
            lv_obj_add_event_cb(obj, action_select_device_instance, LV_EVENT_SHORT_CLICKED, (void *)0);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 140, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Address 0");
                }
            }
        }
        {
            // BtnDeviceInstance1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_device_instance1 = obj;
            lv_obj_set_pos(obj, 220, 75);
            lv_obj_set_size(obj, 170, 60);
            lv_obj_add_event_cb(obj, action_select_device_instance, LV_EVENT_SHORT_CLICKED, (void *)1);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 140, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Address 1");
                }
            }
        }
        {
            // BtnDeviceInstance2
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_device_instance2 = obj;
            lv_obj_set_pos(obj, 410, 75);
            lv_obj_set_size(obj, 170, 60);
            lv_obj_add_event_cb(obj, action_select_device_instance, LV_EVENT_SHORT_CLICKED, (void *)2);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 140, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Address 2");
                }
            }
        }
        {
            // BtnDeviceAssignBack
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.btn_device_assign_back = obj;
            lv_obj_set_pos(obj, 830, 75);
            lv_obj_set_size(obj, 170, 60);
            lv_obj_add_event_cb(obj, action_change_screen, LV_EVENT_SHORT_CLICKED, (void *)5);
            add_style_style_button_default(obj);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, 100, 30);
                    lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
                    add_style_label_default(obj);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "Back");
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 30, 166);
            lv_obj_set_size(obj, 100, 40);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_label_set_text(obj, "Ch 0");
        }
        {
            // DdChannel0Button
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.dd_channel0_button = obj;
            lv_obj_set_pos(obj, 140, 160);
            lv_obj_set_size(obj, 340, 44);
            lv_dropdown_set_options(obj, "None\nButton 1\nButton 2\nButton 3\nButton 4\nButton 5\nButton 6\nButton 7\nButton 8");
            lv_dropdown_set_selected(obj, 0);
            lv_obj_add_event_cb(obj, action_assign_channel, LV_EVENT_VALUE_CHANGED, (void *)0);
            add_style_dropdown_default(obj);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 30, 218);
            lv_obj_set_size(obj, 100, 40);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_label_set_text(obj, "Ch 1");
        }
        {
            // DdChannel1Button
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.dd_channel1_button = obj;
            lv_obj_set_pos(obj, 140, 212);
            lv_obj_set_size(obj, 340, 44);
            lv_dropdown_set_options(obj, "None\nButton 1\nButton 2\nButton 3\nButton 4\nButton 5\nButton 6\nButton 7\nButton 8");
            lv_dropdown_set_selected(obj, 0);
            lv_obj_add_event_cb(obj, action_assign_channel, LV_EVENT_VALUE_CHANGED, (void *)1);
            add_style_dropdown_default(obj);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 30, 270);
            lv_obj_set_size(obj, 100, 40);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_label_set_text(obj, "Ch 2");
        }
        {
            // DdChannel2Button
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.dd_channel2_button = obj;
            lv_obj_set_pos(obj, 140, 264);
            lv_obj_set_size(obj, 340, 44);
            lv_dropdown_set_options(obj, "None\nButton 1\nButton 2\nButton 3\nButton 4\nButton 5\nButton 6\nButton 7\nButton 8");
            lv_dropdown_set_selected(obj, 0);
            lv_obj_add_event_cb(obj, action_assign_channel, LV_EVENT_VALUE_CHANGED, (void *)2);
            add_style_dropdown_default(obj);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 30, 322);
            lv_obj_set_size(obj, 100, 40);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_label_set_text(obj, "Ch 3");
        }
        {
            // DdChannel3Button
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.dd_channel3_button = obj;
            lv_obj_set_pos(obj, 140, 316);
            lv_obj_set_size(obj, 340, 44);
            lv_dropdown_set_options(obj, "None\nButton 1\nButton 2\nButton 3\nButton 4\nButton 5\nButton 6\nButton 7\nButton 8");
            lv_dropdown_set_selected(obj, 0);
            lv_obj_add_event_cb(obj, action_assign_channel, LV_EVENT_VALUE_CHANGED, (void *)3);
            add_style_dropdown_default(obj);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 30, 374);
            lv_obj_set_size(obj, 100, 40);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_label_set_text(obj, "Ch 4");
        }
        {
            // DdChannel4Button
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.dd_channel4_button = obj;
            lv_obj_set_pos(obj, 140, 368);
            lv_obj_set_size(obj, 340, 44);
            lv_dropdown_set_options(obj, "None\nButton 1\nButton 2\nButton 3\nButton 4\nButton 5\nButton 6\nButton 7\nButton 8");
            lv_dropdown_set_selected(obj, 0);
            lv_obj_add_event_cb(obj, action_assign_channel, LV_EVENT_VALUE_CHANGED, (void *)4);
            add_style_dropdown_default(obj);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 30, 426);
            lv_obj_set_size(obj, 100, 40);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_label_set_text(obj, "Ch 5");
        }
        {
            // DdChannel5Button
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.dd_channel5_button = obj;
            lv_obj_set_pos(obj, 140, 420);
            lv_obj_set_size(obj, 340, 44);
            lv_dropdown_set_options(obj, "None\nButton 1\nButton 2\nButton 3\nButton 4\nButton 5\nButton 6\nButton 7\nButton 8");
            lv_dropdown_set_selected(obj, 0);
            lv_obj_add_event_cb(obj, action_assign_channel, LV_EVENT_VALUE_CHANGED, (void *)5);
            add_style_dropdown_default(obj);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 30, 478);
            lv_obj_set_size(obj, 100, 40);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_label_set_text(obj, "Ch 6");
        }
        {
            // DdChannel6Button
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.dd_channel6_button = obj;
            lv_obj_set_pos(obj, 140, 472);
            lv_obj_set_size(obj, 340, 44);
            lv_dropdown_set_options(obj, "None\nButton 1\nButton 2\nButton 3\nButton 4\nButton 5\nButton 6\nButton 7\nButton 8");
            lv_dropdown_set_selected(obj, 0);
            lv_obj_add_event_cb(obj, action_assign_channel, LV_EVENT_VALUE_CHANGED, (void *)6);
            add_style_dropdown_default(obj);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 30, 530);
            lv_obj_set_size(obj, 100, 40);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_label_default(obj);
            lv_label_set_text(obj, "Ch 7");
        }
        {
            // DdChannel7Button
            lv_obj_t *obj = lv_dropdown_create(parent_obj);
            objects.dd_channel7_button = obj;
            lv_obj_set_pos(obj, 140, 524);
            lv_obj_set_size(obj, 340, 44);
            lv_dropdown_set_options(obj, "None\nButton 1\nButton 2\nButton 3\nButton 4\nButton 5\nButton 6\nButton 7\nButton 8");
            lv_dropdown_set_selected(obj, 0);
            lv_obj_add_event_cb(obj, action_assign_channel, LV_EVENT_VALUE_CHANGED, (void *)7);
            add_style_dropdown_default(obj);
        }
    }
    
    tick_screen_page_device_assign();
}

void tick_screen_page_device_assign() {
}

void create_user_widget_bottom_nav_bar(lv_obj_t *parent_obj, int startWidgetIndex) {
    (void)startWidgetIndex;
    lv_obj_t *obj = parent_obj;
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, LV_PCT(100), 60);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
            add_style_style_panel_nav_bar_bottom(obj);
            lv_obj_set_style_align(obj, LV_ALIGN_BOTTOM_LEFT, LV_PART_MAIN | LV_STATE_CHECKED);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // BottomNavBarButtonHome
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 0] = obj;
                    lv_obj_set_pos(obj, 61, 5);
                    lv_obj_set_size(obj, 100, 50);
                    lv_obj_add_event_cb(obj, action_change_screen, LV_EVENT_PRESSED, (void *)0);
                    lv_obj_add_state(obj, LV_STATE_CHECKED);
                    add_style_button_navbar_in_active(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            add_style_nav_bar_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf015");
                        }
                    }
                }
                {
                    // BottomNavBarButtonTrailer
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 1] = obj;
                    lv_obj_set_pos(obj, 221, 5);
                    lv_obj_set_size(obj, 100, 50);
                    lv_obj_add_event_cb(obj, action_change_screen, LV_EVENT_PRESSED, (void *)1);
                    add_style_button_navbar_in_active(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            add_style_nav_bar_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf8ff");
                        }
                    }
                }
                {
                    // BottomNavBarButtonPower
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 2] = obj;
                    lv_obj_set_pos(obj, 382, 5);
                    lv_obj_set_size(obj, 100, 50);
                    lv_obj_add_event_cb(obj, action_change_screen, LV_EVENT_PRESSED, (void *)2);
                    add_style_button_navbar_in_active(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            add_style_nav_bar_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf0e7");
                        }
                    }
                }
                {
                    // BottomNavBarButtonWater
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 3] = obj;
                    lv_obj_set_pos(obj, 542, 5);
                    lv_obj_set_size(obj, 100, 50);
                    lv_obj_add_event_cb(obj, action_change_screen, LV_EVENT_PRESSED, (void *)4);
                    add_style_button_navbar_in_active(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, -2);
                            lv_obj_set_size(obj, 32, 32);
                            add_style_nav_bar_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf043");
                        }
                    }
                }
                {
                    // BottomNavBarButtonAirQuality
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 4] = obj;
                    lv_obj_set_pos(obj, 703, 5);
                    lv_obj_set_size(obj, 100, 50);
                    lv_obj_add_event_cb(obj, action_change_screen, LV_EVENT_PRESSED, (void *)3);
                    add_style_button_navbar_in_active(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            add_style_nav_bar_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf72e\n");
                        }
                    }
                }
                {
                    // BottomNavBarButtonSettings
                    lv_obj_t *obj = lv_btn_create(parent_obj);
                    ((lv_obj_t **)&objects)[startWidgetIndex + 5] = obj;
                    lv_obj_set_pos(obj, 863, 5);
                    lv_obj_set_size(obj, 100, 50);
                    lv_obj_add_event_cb(obj, action_change_screen, LV_EVENT_PRESSED, (void *)6);
                    add_style_button_navbar_in_active(obj);
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            lv_obj_set_pos(obj, 0, 0);
                            lv_obj_set_size(obj, 32, 32);
                            add_style_nav_bar_icon(obj);
                            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "\uf013");
                        }
                    }
                }
            }
        }
    }
}

void tick_user_widget_bottom_nav_bar(int startWidgetIndex) {
    (void)startWidgetIndex;
}

void change_color_theme(uint32_t theme_index) {
    active_theme_index = theme_index;
    
    lv_style_set_bg_color(get_style_arc_default_KNOB_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    
    lv_style_set_arc_color(get_style_arc_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    
    lv_style_set_arc_color(get_style_arc_default_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_bg_color(get_style_bar_default_INDICATOR_CHECKED(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_bg_color(get_style_bar_default_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_bg_color(get_style_bar_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    
    lv_style_set_bg_color(get_style_button_circle_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    
    lv_style_set_text_color(get_style_button_circle_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    
    lv_style_set_text_color(get_style_button_list_menu_item_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    
    lv_style_set_bg_color(get_style_button_list_menu_item_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][5]));
    
    lv_style_set_text_color(get_style_button_navbar_active_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    
    lv_style_set_border_color(get_style_button_navbar_active_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_text_color(get_style_button_navbar_active_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][7]));
    
    lv_style_set_text_color(get_style_button_navbar_in_active_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    
    lv_style_set_border_color(get_style_button_navbar_in_active_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_bg_color(get_style_button_navbar_in_active_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][1]));
    
    lv_style_set_bg_color(get_style_checkbox_default_INDICATOR_CHECKED(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_border_color(get_style_checkbox_default_INDICATOR_CHECKED(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_text_color(get_style_checkbox_default_INDICATOR_CHECKED(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_border_color(get_style_checkbox_default_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    
    lv_style_set_text_color(get_style_checkbox_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    
    lv_style_set_bg_color(get_style_default_button_matrix_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    
    lv_style_set_text_color(get_style_default_button_matrix_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    
    lv_style_set_bg_color(get_style_default_button_matrix_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    
    lv_style_set_bg_color(get_style_default_button_matrix_ITEMS_CHECKED(), lv_color_hex(theme_colors[theme_index][5]));
    
    lv_style_set_bg_color(get_style_keyboard_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    
    lv_style_set_bg_color(get_style_keyboard_default_ITEMS_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    
    lv_style_set_bg_color(get_style_led_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    
    lv_style_set_bg_color(get_style_led_default_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][5]));
    
    lv_style_set_text_color(get_style_label_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    
    lv_style_set_text_color(get_style_label_default_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][7]));
    
    lv_style_set_text_color(get_style_label_fa16_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    
    lv_style_set_text_color(get_style_label_fa16_icon_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][7]));
    
    lv_style_set_text_color(get_style_label_fa32_icon_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    
    lv_style_set_text_color(get_style_label_fa32_icon_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][7]));
    
    lv_style_set_text_color(get_style_label_fa_icon_active_MAIN_PRESSED(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_text_color(get_style_label_nav_text_alternate_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_text_color(get_style_label_secondary_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][10]));
    
    lv_style_set_bg_color(get_style_message_box_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    
    lv_style_set_bg_color(get_style_panel_screen_content_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    
    lv_style_set_bg_color(get_style_roller_default_SELECTED_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    
    lv_style_set_bg_color(get_style_roller_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    
    lv_style_set_text_color(get_style_roller_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    
    lv_style_set_bg_color(get_style_spinner_default_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_arc_color(get_style_spinner_default_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_arc_color(get_style_spinner_default_INDICATOR_FOCUSED(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_bg_color(get_style_spinner_default_INDICATOR_CHECKED(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_bg_color(get_style_spinner_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_arc_color(get_style_spinner_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    
    lv_style_set_bg_color(get_style_style_button_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    
    lv_style_set_bg_color(get_style_style_button_default_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][5]));
    
    lv_style_set_border_color(get_style_style_button_default_MAIN_CHECKED(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_bg_color(get_style_style_content_panel_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    
    lv_style_set_bg_color(get_style_style_default_slider_INDICATOR_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_bg_color(get_style_style_default_slider_KNOB_DEFAULT(), lv_color_hex(theme_colors[theme_index][7]));
    
    lv_style_set_bg_color(get_style_style_default_slider_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][3]));
    
    lv_style_set_text_color(get_style_style_device_status_ind_off_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][9]));
    
    lv_style_set_text_color(get_style_style_device_status_ind_on_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_style_set_bg_color(get_style_style_panel_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][4]));
    
    lv_style_set_bg_color(get_style_style_panel_dialog_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][5]));
    
    lv_style_set_bg_color(get_style_style_panel_modal_blocker_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    
    lv_style_set_bg_color(get_style_style_screen_default_MAIN_DEFAULT(), lv_color_hex(theme_colors[theme_index][2]));
    
    lv_style_set_bg_color(get_style_switch_default_INDICATOR_CHECKED(), lv_color_hex(theme_colors[theme_index][0]));
    
    lv_obj_set_style_text_color(objects.lbl_device01_status_ind, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device01_label, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device02_status_ind, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device02_label, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device03_status_ind, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device03_label, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device04_status_ind, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device04_label, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device05_status_ind, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device05_label, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device06_status_ind, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device06_label, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device07_status_ind, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device07_label, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device08_status_ind, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_device08_label, lv_color_hex(theme_colors[theme_index][9]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.arc_thermostat, lv_color_hex(theme_colors[theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_arc_color(objects.arc_thermostat, lv_color_hex(theme_colors[theme_index][5]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_arc_color(objects.arc_thermostat, lv_color_hex(theme_colors[theme_index][10]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    
    lv_obj_set_style_arc_color(objects.arc_thermostat, lv_color_hex(theme_colors[theme_index][8]), LV_PART_INDICATOR | LV_STATE_CHECKED);
    
    lv_obj_set_style_bg_color(objects.temp_bg, lv_color_hex(theme_colors[theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_grad_color(objects.temp_bg, lv_color_hex(theme_colors[theme_index][2]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_border_color(objects.temp_bg, lv_color_hex(theme_colors[theme_index][1]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.obj0, lv_color_hex(theme_colors[theme_index][5]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_grad_color(objects.obj0, lv_color_hex(theme_colors[theme_index][4]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_border_color(objects.obj0, lv_color_hex(theme_colors[theme_index][1]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_desired_temperature_value, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.obj1, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj1, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_current_interior_temperature, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.obj2, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj2, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_heat_activated_icon, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_heat_activated_icon, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_CHECKED);
    
    lv_obj_set_style_text_color(objects.label_ac_activated_icon, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_ac_activated_icon, lv_color_hex(theme_colors[theme_index][6]), LV_PART_MAIN | LV_STATE_CHECKED);
    
    lv_obj_set_style_bg_color(objects.obj3, lv_color_hex(theme_colors[theme_index][5]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_time_hour, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.obj4, lv_color_hex(theme_colors[theme_index][5]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.lbl_time_minutes, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.obj5, lv_color_hex(theme_colors[theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj6, lv_color_hex(theme_colors[theme_index][0]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.container_modal_background, lv_color_hex(theme_colors[theme_index][1]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.panel_trailer_side_view, lv_color_hex(theme_colors[theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_arc_color(objects.obj7, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_img_recolor(objects.obj7, lv_color_hex(theme_colors[theme_index][0]), LV_PART_KNOB | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_front_level_fa_indicator, lv_color_hex(theme_colors[theme_index][11]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_front_level_fa_indicator, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_CHECKED);
    
    lv_obj_set_style_text_color(objects.label_front_leve_label_1, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_back_level_fa_indicator, lv_color_hex(theme_colors[theme_index][11]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_back_level_fa_indicator, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_CHECKED);
    
    lv_obj_set_style_text_color(objects.label_front_leve_label, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.obj8, lv_color_hex(theme_colors[theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj9, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj10, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj11, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.panel_trailer_back, lv_color_hex(theme_colors[theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_arc_color(objects.obj12, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_arc_color(objects.obj12, lv_color_hex(theme_colors[theme_index][10]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_img_recolor(objects.obj12, lv_color_hex(theme_colors[theme_index][0]), LV_PART_KNOB | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_left_side_level_fa_indicator, lv_color_hex(theme_colors[theme_index][11]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_left_side_level_fa_indicator, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_CHECKED);
    
    lv_obj_set_style_text_color(objects.label_left_side_leve_label, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_right_side_leve_label, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_right_side_level_fa_indicator, lv_color_hex(theme_colors[theme_index][11]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_right_side_level_fa_indicator, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_CHECKED);
    
    lv_obj_set_style_bg_color(objects.panel_solar_input, lv_color_hex(theme_colors[theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.panel_charge_type, lv_color_hex(theme_colors[theme_index][5]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_charge_status_value_s, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.panel_shore_power, lv_color_hex(theme_colors[theme_index][4]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.panel_shore_power_indicator_background, lv_color_hex(theme_colors[theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.panel_shore_power_indicator_background, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.panel_shore_power_indicator_background, lv_color_hex(theme_colors[theme_index][5]), LV_PART_MAIN | LV_STATE_CHECKED);
    
    lv_obj_set_style_text_color(objects.panel_shore_power_indicator_foreground, lv_color_hex(theme_colors[theme_index][0]), LV_PART_MAIN | LV_STATE_CHECKED);
    
    lv_obj_set_style_bg_color(objects.panel_power_battery_stats, lv_color_hex(theme_colors[theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_outline_color(objects.bar_battery_soc, lv_color_hex(theme_colors[theme_index][5]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.bar_battery_soc, lv_color_hex(theme_colors[theme_index][5]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj13, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_power_battery_percentage_value, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_battery_voltage_value, lv_color_hex(theme_colors[theme_index][7]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.panel_power_remaining_time, lv_color_hex(theme_colors[theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.power_arc_current_consumption, lv_color_hex(theme_colors[theme_index][0]), LV_PART_KNOB | LV_STATE_DEFAULT);
    
    lv_obj_set_style_arc_color(objects.power_arc_current_consumption, lv_color_hex(theme_colors[theme_index][0]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj14, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_temp_value_indicator, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj15, lv_color_hex(theme_colors[theme_index][6]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj16, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj17, lv_color_hex(theme_colors[theme_index][1]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj18, lv_color_hex(theme_colors[theme_index][10]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj19, lv_color_hex(theme_colors[theme_index][1]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.label_fresh_water_icon, lv_color_hex(theme_colors[theme_index][6]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.bar_fresh_water_value, lv_color_hex(theme_colors[theme_index][6]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj20, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.bar_grey_water_level, lv_color_hex(theme_colors[theme_index][8]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.bar_black_water_level, lv_color_hex(theme_colors[theme_index][1]), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.obj21, lv_color_hex(theme_colors[theme_index][6]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.obj22, lv_color_hex(theme_colors[theme_index][8]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.obj23, lv_color_hex(theme_colors[theme_index][1]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.obj24, lv_color_hex(theme_colors[theme_index][4]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.obj25, lv_color_hex(theme_colors[theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.obj25, lv_color_hex(theme_colors[theme_index][5]), LV_PART_ITEMS | LV_STATE_CHECKED);
    
    lv_obj_set_style_text_color(objects.obj25, lv_color_hex(theme_colors[theme_index][7]), LV_PART_ITEMS | LV_STATE_CHECKED);
    
    lv_obj_set_style_border_color(objects.obj25, lv_color_hex(theme_colors[theme_index][0]), LV_PART_ITEMS | LV_STATE_CHECKED);
    
    lv_obj_set_style_bg_color(objects.obj25, lv_color_hex(theme_colors[theme_index][4]), LV_PART_ITEMS | LV_STATE_DEFAULT);
    
    lv_obj_set_style_text_color(objects.obj25, lv_color_hex(theme_colors[theme_index][9]), LV_PART_ITEMS | LV_STATE_DEFAULT);
    
    lv_obj_set_style_border_color(objects.lbl_button_edit_text, lv_color_hex(theme_colors[theme_index][0]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(objects.lbl_button_edit_text, lv_color_hex(theme_colors[theme_index][3]), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_invalidate(objects.page_home);
    lv_obj_invalidate(objects.page_trailer);
    lv_obj_invalidate(objects.page_power_management);
    lv_obj_invalidate(objects.page_air_quality);
    lv_obj_invalidate(objects.page_water);
    lv_obj_invalidate(objects.page_settings);
    lv_obj_invalidate(objects.page_button_edit);
    lv_obj_invalidate(objects.page_device_assign);
}

uint32_t theme_colors[2][12] = {
    { 0xff52a441, 0xff000000, 0xffebebeb, 0xffd0e2c7, 0xffe3f3dc, 0xff5e635a, 0xff48e6fe, 0xffffffff, 0xffff5453, 0xff000000, 0xff83a79c, 0xff74fe00 },
    { 0xff00d004, 0xff000000, 0xff646667, 0xff859195, 0xff5b6768, 0xff9db8bb, 0xff48e6fe, 0xffffffff, 0xffff5453, 0xffffffff, 0xffe3f3ee, 0xff74fe00 },
};


typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_page_home,
    tick_screen_page_trailer,
    tick_screen_page_power_management,
    tick_screen_page_air_quality,
    tick_screen_page_water,
    tick_screen_page_settings,
    tick_screen_page_button_edit,
    tick_screen_page_device_assign,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_page_home();
    create_screen_page_trailer();
    create_screen_page_power_management();
    create_screen_page_air_quality();
    create_screen_page_water();
    create_screen_page_settings();
    create_screen_page_button_edit();
    create_screen_page_device_assign();
}
