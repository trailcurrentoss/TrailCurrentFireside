#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: ArcDefault
//

void init_style_arc_default_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
};

lv_style_t *get_style_arc_default_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_default_KNOB_DEFAULT(style);
    }
    return style;
};

void init_style_arc_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_arc_width(style, 20);
};

lv_style_t *get_style_arc_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_arc_default_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_arc_width(style, 20);
};

lv_style_t *get_style_arc_default_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_arc_default_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_arc_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_arc_default_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_arc_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_arc_default_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_arc_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_arc_default_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_arc_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_arc_default_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: BarDefault
//

void init_style_bar_default_INDICATOR_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
};

lv_style_t *get_style_bar_default_INDICATOR_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_default_INDICATOR_CHECKED(style);
    }
    return style;
};

void init_style_bar_default_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
};

lv_style_t *get_style_bar_default_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_default_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_bar_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
};

lv_style_t *get_style_bar_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_bar_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_bar_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_bar_default_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_bar_default_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_bar_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_bar_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_bar_default_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_bar_default_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_bar_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ButtonCircle
//

void init_style_button_circle_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_radius(style, 50);
    lv_style_set_min_width(style, 55);
    lv_style_set_max_width(style, 55);
    lv_style_set_min_height(style, 55);
    lv_style_set_max_height(style, 55);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_text_font(style, &ui_font_fa32);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_button_circle_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_circle_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_button_circle(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_circle_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_button_circle(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_circle_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ButtonListMenuItem
//

void init_style_button_list_menu_item_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_align(style, LV_TEXT_ALIGN_LEFT);
    lv_style_set_pad_top(style, 10);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 5);
    lv_style_set_pad_right(style, 0);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_text_font(style, &lv_font_montserrat_16);
    lv_style_set_min_height(style, 60);
    lv_style_set_max_height(style, 60);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_opa(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_align(style, LV_ALIGN_LEFT_MID);
};

lv_style_t *get_style_button_list_menu_item_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_list_menu_item_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_list_menu_item_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_radius(style, 0);
};

lv_style_t *get_style_button_list_menu_item_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_list_menu_item_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_button_list_menu_item(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_list_menu_item_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_list_menu_item_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_button_list_menu_item(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_list_menu_item_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_list_menu_item_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ButtonNavbarActive
//

void init_style_button_navbar_active_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_fa32);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_border_width(style, 3);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_border_side(style, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
};

lv_style_t *get_style_button_navbar_active_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_navbar_active_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_navbar_active_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
};

lv_style_t *get_style_button_navbar_active_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_navbar_active_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_button_navbar_active(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_navbar_active_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_navbar_active_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_button_navbar_active(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_navbar_active_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_navbar_active_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: ButtonNavbarInActive
//

void init_style_button_navbar_in_active_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_fa32);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_radius(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_border_opa(style, 0);
};

lv_style_t *get_style_button_navbar_in_active_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_navbar_in_active_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_navbar_in_active_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_border_opa(style, 255);
    lv_style_set_border_width(style, 3);
    lv_style_set_border_side(style, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
};

lv_style_t *get_style_button_navbar_in_active_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_navbar_in_active_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_button_navbar_in_active(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_navbar_in_active_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_navbar_in_active_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_button_navbar_in_active(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_navbar_in_active_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_navbar_in_active_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: CheckboxDefault
//

void init_style_checkbox_default_INDICATOR_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_text_letter_space(style, 0);
    lv_style_set_text_line_space(style, 0);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
};

lv_style_t *get_style_checkbox_default_INDICATOR_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_checkbox_default_INDICATOR_CHECKED(style);
    }
    return style;
};

void init_style_checkbox_default_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
};

lv_style_t *get_style_checkbox_default_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_checkbox_default_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_checkbox_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_checkbox_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_checkbox_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_checkbox_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_checkbox_default_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_checkbox_default_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_checkbox_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_checkbox_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_checkbox_default_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_checkbox_default_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_checkbox_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: DefaultButtonMatrix
//

void init_style_default_button_matrix_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_line_width(style, 0);
    lv_style_set_pad_top(style, 10);
    lv_style_set_pad_bottom(style, 10);
    lv_style_set_pad_left(style, 10);
    lv_style_set_pad_right(style, 10);
    lv_style_set_pad_row(style, 10);
    lv_style_set_pad_column(style, 10);
    lv_style_set_border_width(style, 0);
    lv_style_set_border_opa(style, 0);
    lv_style_set_text_font(style, &ui_font_fa32);
};

lv_style_t *get_style_default_button_matrix_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_default_button_matrix_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_default_button_matrix_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
};

lv_style_t *get_style_default_button_matrix_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_default_button_matrix_ITEMS_DEFAULT(style);
    }
    return style;
};

void init_style_default_button_matrix_ITEMS_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
};

lv_style_t *get_style_default_button_matrix_ITEMS_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_default_button_matrix_ITEMS_CHECKED(style);
    }
    return style;
};

void add_style_default_button_matrix(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_default_button_matrix_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_default_button_matrix_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_default_button_matrix_ITEMS_CHECKED(), LV_PART_ITEMS | LV_STATE_CHECKED);
};

void remove_style_default_button_matrix(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_default_button_matrix_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_default_button_matrix_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_default_button_matrix_ITEMS_CHECKED(), LV_PART_ITEMS | LV_STATE_CHECKED);
};

//
// Style: DropdownDefault
//

void add_style_dropdown_default(lv_obj_t *obj) {
    (void)obj;
};

void remove_style_dropdown_default(lv_obj_t *obj) {
    (void)obj;
};

//
// Style: KeyboardDefault
//

void init_style_keyboard_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_text_font(style, &lv_font_montserrat_32);
};

lv_style_t *get_style_keyboard_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_keyboard_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_keyboard_default_ITEMS_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
};

lv_style_t *get_style_keyboard_default_ITEMS_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_keyboard_default_ITEMS_DEFAULT(style);
    }
    return style;
};

void add_style_keyboard_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_keyboard_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_keyboard_default_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
};

void remove_style_keyboard_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_keyboard_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_keyboard_default_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
};

//
// Style: LEDDefault
//

void init_style_led_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
};

lv_style_t *get_style_led_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_led_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_led_default_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
};

lv_style_t *get_style_led_default_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_led_default_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_led_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_led_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_led_default_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_led_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_led_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_led_default_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: LabelCircleButton
//

void init_style_label_circle_button_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_fa32);
};

lv_style_t *get_style_label_circle_button_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_circle_button_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_circle_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_circle_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_circle_button(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_circle_button_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelDefault
//

void init_style_label_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_font(style, &lv_font_montserrat_24);
};

lv_style_t *get_style_label_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_label_default_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_font(style, &lv_font_montserrat_14);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
};

lv_style_t *get_style_label_default_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_default_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_label_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_label_default_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_label_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_label_default_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: LabelFa16Icon
//

void init_style_label_fa16_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_fa16);
    lv_style_set_align(style, LV_ALIGN_TOP_MID);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_label_fa16_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_fa16_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_label_fa16_icon_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_fa16);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
};

lv_style_t *get_style_label_fa16_icon_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_fa16_icon_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_label_fa16_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_fa16_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_label_fa16_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_label_fa16_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_fa16_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_label_fa16_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: LabelFa32Icon
//

void init_style_label_fa32_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_fa32);
    lv_style_set_align(style, LV_ALIGN_TOP_MID);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_label_fa32_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_fa32_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_label_fa32_icon_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_fa32);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
};

lv_style_t *get_style_label_fa32_icon_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_fa32_icon_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_label_fa32_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_fa32_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_label_fa32_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_label_fa32_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_fa32_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_label_fa32_icon_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: LabelFaIconActive
//

void init_style_label_fa_icon_active_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_text_font(style, &ui_font_fa32);
    lv_style_set_align(style, LV_ALIGN_TOP_MID);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
};

lv_style_t *get_style_label_fa_icon_active_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_fa_icon_active_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_label_fa_icon_active(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_fa_icon_active_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_label_fa_icon_active(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_fa_icon_active_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: LabelListMenuItem
//

void init_style_label_list_menu_item_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_align(style, LV_TEXT_ALIGN_LEFT);
    lv_style_set_text_font(style, &lv_font_montserrat_18);
    lv_style_set_pad_top(style, 10);
    lv_style_set_pad_bottom(style, 10);
    lv_style_set_align(style, LV_ALIGN_LEFT_MID);
};

lv_style_t *get_style_label_list_menu_item_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_list_menu_item_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_list_menu_item(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_list_menu_item_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_list_menu_item(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_list_menu_item_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelNavText
//

void init_style_label_nav_text_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
    lv_style_set_text_opa(style, 255);
    lv_style_set_text_font(style, &lv_font_montserrat_14);
};

lv_style_t *get_style_label_nav_text_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_nav_text_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_nav_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_nav_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_nav_text(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_nav_text_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelNavTextAlternate
//

void init_style_label_nav_text_alternate_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_text_opa(style, 255);
    lv_style_set_text_font(style, &lv_font_montserrat_14);
};

lv_style_t *get_style_label_nav_text_alternate_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_nav_text_alternate_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_nav_text_alternate(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_nav_text_alternate_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_nav_text_alternate(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_nav_text_alternate_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: LabelSecondary
//

void init_style_label_secondary_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
};

lv_style_t *get_style_label_secondary_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_label_secondary_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_label_secondary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_secondary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_secondary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_secondary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ListDefault
//

void init_style_list_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_list_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_list_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_list_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_list_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_list_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_list_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ListMenu
//

void init_style_list_menu_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_outline_pad(style, 0);
    lv_style_set_border_width(style, 2);
    lv_style_set_border_side(style, LV_BORDER_SIDE_RIGHT);
    lv_style_set_radius(style, 0);
    lv_style_set_text_font(style, &lv_font_montserrat_32);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_LEFT);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_bg_opa(style, 0);
};

lv_style_t *get_style_list_menu_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_list_menu_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_list_menu(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_list_menu_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_list_menu(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_list_menu_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: MessageBoxDefault
//

void init_style_message_box_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_pad_top(style, 10);
    lv_style_set_pad_bottom(style, 10);
    lv_style_set_pad_left(style, 10);
    lv_style_set_pad_right(style, 10);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_shadow_width(style, 0);
};

lv_style_t *get_style_message_box_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_message_box_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_message_box_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_message_box_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_message_box_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_message_box_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: NavBarIcon
//

void init_style_nav_bar_icon_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
    lv_style_set_text_font(style, &ui_font_fa32);
};

lv_style_t *get_style_nav_bar_icon_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_nav_bar_icon_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_nav_bar_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_nav_bar_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_nav_bar_icon(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_nav_bar_icon_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: PanelNavBar
//

void add_style_panel_nav_bar(lv_obj_t *obj) {
    (void)obj;
};

void remove_style_panel_nav_bar(lv_obj_t *obj) {
    (void)obj;
};

//
// Style: PanelNavBarTop
//

void init_style_panel_nav_bar_top_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_align(style, LV_ALIGN_TOP_LEFT);
    lv_style_set_min_width(style, 800);
    lv_style_set_max_width(style, 60);
    lv_style_set_min_height(style, 60);
    lv_style_set_max_height(style, 60);
    lv_style_set_radius(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
    lv_style_set_text_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_panel_nav_bar_top_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_nav_bar_top_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel_nav_bar_top(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_nav_bar_top_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel_nav_bar_top(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_nav_bar_top_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: PanelScreenContent
//

void init_style_panel_screen_content_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_min_width(style, 800);
    lv_style_set_max_width(style, 800);
    lv_style_set_min_height(style, 360);
    lv_style_set_max_height(style, 360);
    lv_style_set_align(style, LV_ALIGN_CENTER);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_pad_row(style, 0);
    lv_style_set_pad_column(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_bg_opa(style, 255);
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
};

lv_style_t *get_style_panel_screen_content_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_screen_content_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel_screen_content(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_screen_content_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel_screen_content(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_screen_content_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: RollerDefault
//

void init_style_roller_default_SELECTED_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
};

lv_style_t *get_style_roller_default_SELECTED_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_roller_default_SELECTED_DEFAULT(style);
    }
    return style;
};

void init_style_roller_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_roller_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_roller_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_roller_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_roller_default_SELECTED_DEFAULT(), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_roller_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_roller_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_roller_default_SELECTED_DEFAULT(), LV_PART_SELECTED | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_roller_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: SpinnerDefault
//

void init_style_spinner_default_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
};

lv_style_t *get_style_spinner_default_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_spinner_default_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_spinner_default_INDICATOR_FOCUSED(lv_style_t *style) {
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
};

lv_style_t *get_style_spinner_default_INDICATOR_FOCUSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_spinner_default_INDICATOR_FOCUSED(style);
    }
    return style;
};

void init_style_spinner_default_INDICATOR_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
};

lv_style_t *get_style_spinner_default_INDICATOR_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_spinner_default_INDICATOR_CHECKED(style);
    }
    return style;
};

void init_style_spinner_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
};

lv_style_t *get_style_spinner_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_spinner_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_spinner_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_spinner_default_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_spinner_default_INDICATOR_FOCUSED(), LV_PART_INDICATOR | LV_STATE_FOCUSED);
    lv_obj_add_style(obj, get_style_spinner_default_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_style(obj, get_style_spinner_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_spinner_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_spinner_default_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_spinner_default_INDICATOR_FOCUSED(), LV_PART_INDICATOR | LV_STATE_FOCUSED);
    lv_obj_remove_style(obj, get_style_spinner_default_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_remove_style(obj, get_style_spinner_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: StyleButtonDefault
//

void init_style_style_button_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_radius(style, 5);
    lv_style_set_shadow_width(style, 0);
};

lv_style_t *get_style_style_button_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_button_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_style_button_default_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_border_width(style, 3);
    lv_style_set_border_side(style, LV_BORDER_SIDE_BOTTOM);
};

lv_style_t *get_style_style_button_default_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_button_default_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_style_button_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_button_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_style_button_default_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_style_button_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_button_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_style_button_default_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: StyleButtonSucces
//

void add_style_style_button_succes(lv_obj_t *obj) {
    (void)obj;
};

void remove_style_style_button_succes(lv_obj_t *obj) {
    (void)obj;
};

//
// Style: StyleContentPanel
//

void init_style_style_content_panel_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_pad_top(style, 5);
    lv_style_set_pad_bottom(style, 5);
    lv_style_set_pad_left(style, 5);
    lv_style_set_pad_right(style, 5);
    lv_style_set_border_opa(style, 0);
    lv_style_set_border_width(style, 0);
};

lv_style_t *get_style_style_content_panel_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_content_panel_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_style_content_panel(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_content_panel_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_style_content_panel(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_content_panel_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: StyleDefaultSlider
//

void init_style_style_default_slider_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
};

lv_style_t *get_style_style_default_slider_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_default_slider_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_style_default_slider_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
};

lv_style_t *get_style_style_default_slider_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_default_slider_KNOB_DEFAULT(style);
    }
    return style;
};

void init_style_style_default_slider_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
};

lv_style_t *get_style_style_default_slider_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_default_slider_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_style_default_slider(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_default_slider_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_style_default_slider_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_style_default_slider_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_style_default_slider(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_default_slider_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_style_default_slider_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_style_default_slider_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: StyleDeviceStatusIndOff
//

void init_style_style_device_status_ind_off_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_fa32);
    lv_style_set_align(style, LV_ALIGN_TOP_MID);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_opa(style, 255);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_style_device_status_ind_off_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_device_status_ind_off_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_style_device_status_ind_off(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_device_status_ind_off_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_style_device_status_ind_off(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_device_status_ind_off_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: StyleDeviceStatusIndOn
//

void init_style_style_device_status_ind_on_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_fa32);
    lv_style_set_align(style, LV_ALIGN_TOP_MID);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
    lv_style_set_text_opa(style, 255);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_style_device_status_ind_on_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_device_status_ind_on_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_style_device_status_ind_on_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xff3eb117));
};

lv_style_t *get_style_style_device_status_ind_on_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_device_status_ind_on_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_style_device_status_ind_on(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_device_status_ind_on_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_style_device_status_ind_on_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_style_device_status_ind_on(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_device_status_ind_on_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_style_device_status_ind_on_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: StylePanelDefault
//

void init_style_style_panel_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 0);
};

lv_style_t *get_style_style_panel_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_panel_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_style_panel_default_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_radius(style, 0);
};

lv_style_t *get_style_style_panel_default_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_panel_default_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_style_panel_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_panel_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_style_panel_default_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_style_panel_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_panel_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_style_panel_default_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: StylePanelDialog
//

void init_style_style_panel_dialog_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 5);
};

lv_style_t *get_style_style_panel_dialog_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_panel_dialog_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_style_panel_dialog(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_panel_dialog_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_style_panel_dialog(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_panel_dialog_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: StylePanelModalBlocker
//

void init_style_style_panel_modal_blocker_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_bg_opa(style, 230);
};

lv_style_t *get_style_style_panel_modal_blocker_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_panel_modal_blocker_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_style_panel_modal_blocker(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_panel_modal_blocker_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_style_panel_modal_blocker(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_panel_modal_blocker_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: StylePanelNavBarBottom
//

void init_style_style_panel_nav_bar_bottom_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_align(style, LV_ALIGN_BOTTOM_LEFT);
    lv_style_set_min_width(style, 1024);
    lv_style_set_max_width(style, 1024);
    lv_style_set_min_height(style, 60);
    lv_style_set_max_height(style, 60);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_style_panel_nav_bar_bottom_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_panel_nav_bar_bottom_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_style_panel_nav_bar_bottom(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_panel_nav_bar_bottom_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_style_panel_nav_bar_bottom(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_panel_nav_bar_bottom_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: StyleScreenDefault
//

void init_style_style_screen_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_text_color(style, lv_color_hex(0xff000000));
    lv_style_set_text_font(style, &lv_font_montserrat_14);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
};

lv_style_t *get_style_style_screen_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_screen_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_style_screen_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_screen_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_style_screen_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_screen_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: StyleTopNavTextNormal
//

void init_style_style_top_nav_text_normal_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(0xffffffff));
    lv_style_set_text_opa(style, 255);
    lv_style_set_text_font(style, &lv_font_montserrat_32);
    lv_style_set_pad_top(style, 16);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_bg_color(style, lv_color_hex(0xff000000));
};

lv_style_t *get_style_style_top_nav_text_normal_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_top_nav_text_normal_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_style_top_nav_text_normal(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_top_nav_text_normal_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_style_top_nav_text_normal(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_top_nav_text_normal_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: SwitchDefault
//

void init_style_switch_default_INDICATOR_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][0]));
};

lv_style_t *get_style_switch_default_INDICATOR_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_switch_default_INDICATOR_CHECKED(style);
    }
    return style;
};

void add_style_switch_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_switch_default_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
};

void remove_style_switch_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_switch_default_INDICATOR_CHECKED(), LV_PART_INDICATOR | LV_STATE_CHECKED);
};

//
// Style: TabDefault
//

void add_style_tab_default(lv_obj_t *obj) {
    (void)obj;
};

void remove_style_tab_default(lv_obj_t *obj) {
    (void)obj;
};

//
// Style: TabViewDefault
//

void add_style_tab_view_default(lv_obj_t *obj) {
    (void)obj;
};

void remove_style_tab_view_default(lv_obj_t *obj) {
    (void)obj;
};

//
// Style: StyleBadge
//

void init_style_style_badge_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &lv_font_montserrat_12);
    lv_style_set_text_align(style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_radius(style, 12);
    lv_style_set_pad_top(style, 2);
    lv_style_set_pad_bottom(style, 2);
    lv_style_set_pad_left(style, 4);
    lv_style_set_pad_right(style, 4);
    lv_style_set_border_width(style, 0);
};

lv_style_t *get_style_style_badge_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_style_badge_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_style_badge(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_style_badge_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_style_badge(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_style_badge_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_arc_default,
        add_style_bar_default,
        add_style_button_circle,
        add_style_button_list_menu_item,
        add_style_button_navbar_active,
        add_style_button_navbar_in_active,
        add_style_checkbox_default,
        add_style_default_button_matrix,
        add_style_dropdown_default,
        add_style_keyboard_default,
        add_style_led_default,
        add_style_label_circle_button,
        add_style_label_default,
        add_style_label_fa16_icon,
        add_style_label_fa32_icon,
        add_style_label_fa_icon_active,
        add_style_label_list_menu_item,
        add_style_label_nav_text,
        add_style_label_nav_text_alternate,
        add_style_label_secondary,
        add_style_list_default,
        add_style_list_menu,
        add_style_message_box_default,
        add_style_nav_bar_icon,
        add_style_panel_nav_bar,
        add_style_panel_nav_bar_top,
        add_style_panel_screen_content,
        add_style_roller_default,
        add_style_spinner_default,
        add_style_style_button_default,
        add_style_style_button_succes,
        add_style_style_content_panel,
        add_style_style_default_slider,
        add_style_style_device_status_ind_off,
        add_style_style_device_status_ind_on,
        add_style_style_panel_default,
        add_style_style_panel_dialog,
        add_style_style_panel_modal_blocker,
        add_style_style_panel_nav_bar_bottom,
        add_style_style_screen_default,
        add_style_style_top_nav_text_normal,
        add_style_switch_default,
        add_style_tab_default,
        add_style_tab_view_default,
        add_style_style_badge,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_arc_default,
        remove_style_bar_default,
        remove_style_button_circle,
        remove_style_button_list_menu_item,
        remove_style_button_navbar_active,
        remove_style_button_navbar_in_active,
        remove_style_checkbox_default,
        remove_style_default_button_matrix,
        remove_style_dropdown_default,
        remove_style_keyboard_default,
        remove_style_led_default,
        remove_style_label_circle_button,
        remove_style_label_default,
        remove_style_label_fa16_icon,
        remove_style_label_fa32_icon,
        remove_style_label_fa_icon_active,
        remove_style_label_list_menu_item,
        remove_style_label_nav_text,
        remove_style_label_nav_text_alternate,
        remove_style_label_secondary,
        remove_style_list_default,
        remove_style_list_menu,
        remove_style_message_box_default,
        remove_style_nav_bar_icon,
        remove_style_panel_nav_bar,
        remove_style_panel_nav_bar_top,
        remove_style_panel_screen_content,
        remove_style_roller_default,
        remove_style_spinner_default,
        remove_style_style_button_default,
        remove_style_style_button_succes,
        remove_style_style_content_panel,
        remove_style_style_default_slider,
        remove_style_style_device_status_ind_off,
        remove_style_style_device_status_ind_on,
        remove_style_style_panel_default,
        remove_style_style_panel_dialog,
        remove_style_style_panel_modal_blocker,
        remove_style_style_panel_nav_bar_bottom,
        remove_style_style_screen_default,
        remove_style_style_top_nav_text_normal,
        remove_style_switch_default,
        remove_style_tab_default,
        remove_style_tab_view_default,
        remove_style_style_badge,
    };
    remove_style_funcs[styleIndex](obj);
}

