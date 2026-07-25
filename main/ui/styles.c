#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: ScreenDefault
//

void init_style_screen_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr14);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
};

lv_style_t *get_style_screen_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_screen_default_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_screen_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_screen_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_screen_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_screen_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ScreenWizard
//

void init_style_screen_wizard_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][1]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr14);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
};

lv_style_t *get_style_screen_wizard_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_screen_wizard_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_screen_wizard(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_screen_wizard_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_screen_wizard(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_screen_wizard_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: PanelTopBar
//

void init_style_panel_top_bar_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_side(style, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_min_width(style, 1024);
    lv_style_set_max_width(style, 1024);
    lv_style_set_min_height(style, 38);
    lv_style_set_max_height(style, 38);
    lv_style_set_align(style, LV_ALIGN_TOP_LEFT);
};

lv_style_t *get_style_panel_top_bar_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_top_bar_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel_top_bar(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_top_bar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel_top_bar(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_top_bar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: PanelNavBar
//

void init_style_panel_nav_bar_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][4]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_side(style, LV_BORDER_SIDE_TOP);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_min_width(style, 1024);
    lv_style_set_max_width(style, 1024);
    lv_style_set_min_height(style, 48);
    lv_style_set_max_height(style, 48);
    lv_style_set_align(style, LV_ALIGN_TOP_LEFT);
};

lv_style_t *get_style_panel_nav_bar_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_nav_bar_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel_nav_bar(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_nav_bar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel_nav_bar(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_nav_bar_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: Card
//

void init_style_card_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr14);
};

lv_style_t *get_style_card_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_card_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_card(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_card_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_card(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_card_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: CardAccent
//

void init_style_card_accent_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
};

lv_style_t *get_style_card_accent_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_card_accent_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_card_accent(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_card_accent_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_card_accent(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_card_accent_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: CardAccentSoft
//

void init_style_card_accent_soft_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 12);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
};

lv_style_t *get_style_card_accent_soft_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_card_accent_soft_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_card_accent_soft(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_card_accent_soft_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_card_accent_soft(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_card_accent_soft_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: PanelTransparent
//

void init_style_panel_transparent_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
};

lv_style_t *get_style_panel_transparent_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_panel_transparent_MAIN_DEFAULT(style);
    }
    return style;
};

void add_style_panel_transparent(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_panel_transparent_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_panel_transparent(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_panel_transparent_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: ButtonDefault
//

void init_style_button_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 10);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rm11);
};

lv_style_t *get_style_button_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_default_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
};

lv_style_t *get_style_button_default_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_default_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_button_default_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_shadow_width(style, 15);
    lv_style_set_shadow_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_shadow_opa(style, 77);
    lv_style_set_shadow_spread(style, 0);
};

lv_style_t *get_style_button_default_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_default_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_button_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_default_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_button_default_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_button_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_default_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_button_default_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ButtonNav
//

void init_style_button_nav_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 8);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rm11);
};

lv_style_t *get_style_button_nav_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_nav_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_nav_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_button_nav_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_nav_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_button_nav_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][26]));
};

lv_style_t *get_style_button_nav_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_nav_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_button_nav(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_nav_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_nav_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_button_nav_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_button_nav(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_nav_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_nav_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_button_nav_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ButtonSeg
//

void init_style_button_seg_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 8);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rm11);
};

lv_style_t *get_style_button_seg_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_seg_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_seg_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
};

lv_style_t *get_style_button_seg_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_seg_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_button_seg_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][26]));
};

lv_style_t *get_style_button_seg_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_seg_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_button_seg(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_seg_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_seg_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_button_seg_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_button_seg(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_seg_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_seg_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_button_seg_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ButtonPill
//

void init_style_button_pill_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 999);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][7]));
    lv_style_set_text_font(style, &ui_font_rm11);
};

lv_style_t *get_style_button_pill_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_pill_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_pill_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_bg_opa(style, 255);
};

lv_style_t *get_style_button_pill_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_pill_MAIN_PRESSED(style);
    }
    return style;
};

void init_style_button_pill_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][26]));
};

lv_style_t *get_style_button_pill_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_pill_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_button_pill(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_pill_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_pill_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_button_pill_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_button_pill(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_pill_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_pill_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_button_pill_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ButtonPrimary
//

void init_style_button_primary_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 10);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][26]));
    lv_style_set_text_font(style, &ui_font_rm14);
};

lv_style_t *get_style_button_primary_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_primary_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_primary_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_opa(style, 220);
};

lv_style_t *get_style_button_primary_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_primary_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_button_primary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_primary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_primary_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_button_primary(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_primary_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_primary_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: ButtonDanger
//

void init_style_button_danger_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][14]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 10);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][26]));
    lv_style_set_text_font(style, &ui_font_rm14);
};

lv_style_t *get_style_button_danger_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_danger_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_danger_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_opa(style, 220);
};

lv_style_t *get_style_button_danger_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_danger_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_button_danger(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_danger_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_danger_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_button_danger(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_danger_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_danger_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: ButtonStepper
//

void init_style_button_stepper_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 8);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rm14);
};

lv_style_t *get_style_button_stepper_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_stepper_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_button_stepper_MAIN_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
};

lv_style_t *get_style_button_stepper_MAIN_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_button_stepper_MAIN_PRESSED(style);
    }
    return style;
};

void add_style_button_stepper(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_button_stepper_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_button_stepper_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

void remove_style_button_stepper(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_button_stepper_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_button_stepper_MAIN_PRESSED(), LV_PART_MAIN | LV_STATE_PRESSED);
};

//
// Style: LabelDefault
//

void init_style_label_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr14);
    lv_style_set_bg_opa(style, 0);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
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

void add_style_label_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_label_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_label_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_label_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: SliderDefault
//

void init_style_slider_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 999);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
};

lv_style_t *get_style_slider_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_slider_default_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 999);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
};

lv_style_t *get_style_slider_default_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_default_INDICATOR_DEFAULT(style);
    }
    return style;
};

void init_style_slider_default_KNOB_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 999);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
};

lv_style_t *get_style_slider_default_KNOB_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_slider_default_KNOB_DEFAULT(style);
    }
    return style;
};

void add_style_slider_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_slider_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_default_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_slider_default_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

void remove_style_slider_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_slider_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_default_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_slider_default_KNOB_DEFAULT(), LV_PART_KNOB | LV_STATE_DEFAULT);
};

//
// Style: BarDefault
//

void init_style_bar_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_width(style, 0);
    lv_style_set_radius(style, 6);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
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
    lv_obj_add_style(obj, get_style_bar_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_bar_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_bar_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
// Style: TextareaDefault
//

void init_style_textarea_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 8);
    lv_style_set_pad_top(style, 8);
    lv_style_set_pad_bottom(style, 8);
    lv_style_set_pad_left(style, 10);
    lv_style_set_pad_right(style, 10);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr14);
};

lv_style_t *get_style_textarea_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_textarea_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_textarea_default_MAIN_FOCUSED(lv_style_t *style) {
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 2);
    lv_style_set_border_opa(style, 255);
};

lv_style_t *get_style_textarea_default_MAIN_FOCUSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_textarea_default_MAIN_FOCUSED(style);
    }
    return style;
};

void init_style_textarea_default_CURSOR_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_opa(style, 0);
    lv_style_set_border_opa(style, 0);
};

lv_style_t *get_style_textarea_default_CURSOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_textarea_default_CURSOR_DEFAULT(style);
    }
    return style;
};

void add_style_textarea_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_textarea_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_textarea_default_MAIN_FOCUSED(), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_add_style(obj, get_style_textarea_default_CURSOR_DEFAULT(), LV_PART_CURSOR | LV_STATE_DEFAULT);
};

void remove_style_textarea_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_textarea_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_textarea_default_MAIN_FOCUSED(), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_remove_style(obj, get_style_textarea_default_CURSOR_DEFAULT(), LV_PART_CURSOR | LV_STATE_DEFAULT);
};

//
// Style: KeyboardDefault
//

void init_style_keyboard_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 8);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &lv_font_montserrat_14);
    lv_style_set_align(style, LV_ALIGN_TOP_LEFT);
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
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_radius(style, 6);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_outline_width(style, 0);
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

void init_style_keyboard_default_ITEMS_PRESSED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_keyboard_default_ITEMS_PRESSED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_keyboard_default_ITEMS_PRESSED(style);
    }
    return style;
};

void init_style_keyboard_default_ITEMS_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][10]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_width(style, 1);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_keyboard_default_ITEMS_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_keyboard_default_ITEMS_CHECKED(style);
    }
    return style;
};

void add_style_keyboard_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_keyboard_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_keyboard_default_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_keyboard_default_ITEMS_PRESSED(), LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_add_style(obj, get_style_keyboard_default_ITEMS_CHECKED(), LV_PART_ITEMS | LV_STATE_CHECKED);
};

void remove_style_keyboard_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_keyboard_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_keyboard_default_ITEMS_DEFAULT(), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_keyboard_default_ITEMS_PRESSED(), LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_remove_style(obj, get_style_keyboard_default_ITEMS_CHECKED(), LV_PART_ITEMS | LV_STATE_CHECKED);
};

//
// Style: DropdownDefault
//

void init_style_dropdown_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 8);
    lv_style_set_pad_top(style, 8);
    lv_style_set_pad_bottom(style, 8);
    lv_style_set_pad_left(style, 10);
    lv_style_set_pad_right(style, 10);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr14);
};

lv_style_t *get_style_dropdown_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dropdown_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_dropdown_default_INDICATOR_DEFAULT(lv_style_t *style) {
    lv_style_set_text_font(style, &ui_font_fa16);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][8]));
};

lv_style_t *get_style_dropdown_default_INDICATOR_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_dropdown_default_INDICATOR_DEFAULT(style);
    }
    return style;
};

void add_style_dropdown_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_dropdown_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_dropdown_default_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

void remove_style_dropdown_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_dropdown_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_dropdown_default_INDICATOR_DEFAULT(), LV_PART_INDICATOR | LV_STATE_DEFAULT);
};

//
// Style: ListDefault
//

void init_style_list_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][2]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 8);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
    lv_style_set_text_color(style, lv_color_hex(theme_colors[active_theme_index][6]));
    lv_style_set_text_font(style, &ui_font_rr14);
    lv_style_set_align(style, LV_ALIGN_TOP_LEFT);
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
// Style: SwitchDefault
//

void init_style_switch_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_radius(style, 999);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
};

lv_style_t *get_style_switch_default_MAIN_DEFAULT() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_switch_default_MAIN_DEFAULT(style);
    }
    return style;
};

void init_style_switch_default_MAIN_CHECKED(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][9]));
};

lv_style_t *get_style_switch_default_MAIN_CHECKED() {
    static lv_style_t *style;
    if (!style) {
        style = (lv_style_t *)lv_mem_alloc(sizeof(lv_style_t));
        lv_style_init(style);
        init_style_switch_default_MAIN_CHECKED(style);
    }
    return style;
};

void add_style_switch_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_switch_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(obj, get_style_switch_default_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

void remove_style_switch_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_switch_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_remove_style(obj, get_style_switch_default_MAIN_CHECKED(), LV_PART_MAIN | LV_STATE_CHECKED);
};

//
// Style: ArcDefault
//

void init_style_arc_default_MAIN_DEFAULT(lv_style_t *style) {
    lv_style_set_bg_color(style, lv_color_hex(theme_colors[active_theme_index][3]));
    lv_style_set_bg_opa(style, 255);
    lv_style_set_border_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_border_width(style, 1);
    lv_style_set_border_opa(style, 255);
    lv_style_set_arc_color(style, lv_color_hex(theme_colors[active_theme_index][5]));
    lv_style_set_arc_width(style, 2);
    lv_style_set_arc_opa(style, 255);
    lv_style_set_radius(style, 999);
    lv_style_set_pad_top(style, 0);
    lv_style_set_pad_bottom(style, 0);
    lv_style_set_pad_left(style, 0);
    lv_style_set_pad_right(style, 0);
    lv_style_set_shadow_width(style, 0);
    lv_style_set_shadow_opa(style, 0);
    lv_style_set_shadow_spread(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_opa(style, 0);
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

void add_style_arc_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_add_style(obj, get_style_arc_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

void remove_style_arc_default(lv_obj_t *obj) {
    (void)obj;
    lv_obj_remove_style(obj, get_style_arc_default_MAIN_DEFAULT(), LV_PART_MAIN | LV_STATE_DEFAULT);
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_screen_default,
        add_style_screen_wizard,
        add_style_panel_top_bar,
        add_style_panel_nav_bar,
        add_style_card,
        add_style_card_accent,
        add_style_card_accent_soft,
        add_style_panel_transparent,
        add_style_button_default,
        add_style_button_nav,
        add_style_button_seg,
        add_style_button_pill,
        add_style_button_primary,
        add_style_button_danger,
        add_style_button_stepper,
        add_style_label_default,
        add_style_slider_default,
        add_style_bar_default,
        add_style_textarea_default,
        add_style_keyboard_default,
        add_style_dropdown_default,
        add_style_list_default,
        add_style_switch_default,
        add_style_arc_default,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_screen_default,
        remove_style_screen_wizard,
        remove_style_panel_top_bar,
        remove_style_panel_nav_bar,
        remove_style_card,
        remove_style_card_accent,
        remove_style_card_accent_soft,
        remove_style_panel_transparent,
        remove_style_button_default,
        remove_style_button_nav,
        remove_style_button_seg,
        remove_style_button_pill,
        remove_style_button_primary,
        remove_style_button_danger,
        remove_style_button_stepper,
        remove_style_label_default,
        remove_style_slider_default,
        remove_style_bar_default,
        remove_style_textarea_default,
        remove_style_keyboard_default,
        remove_style_dropdown_default,
        remove_style_list_default,
        remove_style_switch_default,
        remove_style_arc_default,
    };
    remove_style_funcs[styleIndex](obj);
}

