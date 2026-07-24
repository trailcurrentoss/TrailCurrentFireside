#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: ScreenDefault
lv_style_t *get_style_screen_default_MAIN_DEFAULT();
void add_style_screen_default(lv_obj_t *obj);
void remove_style_screen_default(lv_obj_t *obj);

// Style: ScreenWizard
lv_style_t *get_style_screen_wizard_MAIN_DEFAULT();
void add_style_screen_wizard(lv_obj_t *obj);
void remove_style_screen_wizard(lv_obj_t *obj);

// Style: PanelTopBar
lv_style_t *get_style_panel_top_bar_MAIN_DEFAULT();
void add_style_panel_top_bar(lv_obj_t *obj);
void remove_style_panel_top_bar(lv_obj_t *obj);

// Style: PanelNavBar
lv_style_t *get_style_panel_nav_bar_MAIN_DEFAULT();
void add_style_panel_nav_bar(lv_obj_t *obj);
void remove_style_panel_nav_bar(lv_obj_t *obj);

// Style: Card
lv_style_t *get_style_card_MAIN_DEFAULT();
void add_style_card(lv_obj_t *obj);
void remove_style_card(lv_obj_t *obj);

// Style: CardAccent
lv_style_t *get_style_card_accent_MAIN_DEFAULT();
void add_style_card_accent(lv_obj_t *obj);
void remove_style_card_accent(lv_obj_t *obj);

// Style: CardAccentSoft
lv_style_t *get_style_card_accent_soft_MAIN_DEFAULT();
void add_style_card_accent_soft(lv_obj_t *obj);
void remove_style_card_accent_soft(lv_obj_t *obj);

// Style: PanelTransparent
lv_style_t *get_style_panel_transparent_MAIN_DEFAULT();
void add_style_panel_transparent(lv_obj_t *obj);
void remove_style_panel_transparent(lv_obj_t *obj);

// Style: ButtonDefault
lv_style_t *get_style_button_default_MAIN_DEFAULT();
lv_style_t *get_style_button_default_MAIN_PRESSED();
lv_style_t *get_style_button_default_MAIN_CHECKED();
void add_style_button_default(lv_obj_t *obj);
void remove_style_button_default(lv_obj_t *obj);

// Style: ButtonNav
lv_style_t *get_style_button_nav_MAIN_DEFAULT();
lv_style_t *get_style_button_nav_MAIN_PRESSED();
lv_style_t *get_style_button_nav_MAIN_CHECKED();
void add_style_button_nav(lv_obj_t *obj);
void remove_style_button_nav(lv_obj_t *obj);

// Style: ButtonSeg
lv_style_t *get_style_button_seg_MAIN_DEFAULT();
lv_style_t *get_style_button_seg_MAIN_PRESSED();
lv_style_t *get_style_button_seg_MAIN_CHECKED();
void add_style_button_seg(lv_obj_t *obj);
void remove_style_button_seg(lv_obj_t *obj);

// Style: ButtonPill
lv_style_t *get_style_button_pill_MAIN_DEFAULT();
lv_style_t *get_style_button_pill_MAIN_PRESSED();
lv_style_t *get_style_button_pill_MAIN_CHECKED();
void add_style_button_pill(lv_obj_t *obj);
void remove_style_button_pill(lv_obj_t *obj);

// Style: ButtonPrimary
lv_style_t *get_style_button_primary_MAIN_DEFAULT();
lv_style_t *get_style_button_primary_MAIN_PRESSED();
void add_style_button_primary(lv_obj_t *obj);
void remove_style_button_primary(lv_obj_t *obj);

// Style: ButtonDanger
lv_style_t *get_style_button_danger_MAIN_DEFAULT();
lv_style_t *get_style_button_danger_MAIN_PRESSED();
void add_style_button_danger(lv_obj_t *obj);
void remove_style_button_danger(lv_obj_t *obj);

// Style: ButtonStepper
lv_style_t *get_style_button_stepper_MAIN_DEFAULT();
lv_style_t *get_style_button_stepper_MAIN_PRESSED();
void add_style_button_stepper(lv_obj_t *obj);
void remove_style_button_stepper(lv_obj_t *obj);

// Style: LabelDefault
lv_style_t *get_style_label_default_MAIN_DEFAULT();
void add_style_label_default(lv_obj_t *obj);
void remove_style_label_default(lv_obj_t *obj);

// Style: SliderDefault
lv_style_t *get_style_slider_default_MAIN_DEFAULT();
lv_style_t *get_style_slider_default_INDICATOR_DEFAULT();
lv_style_t *get_style_slider_default_KNOB_DEFAULT();
void add_style_slider_default(lv_obj_t *obj);
void remove_style_slider_default(lv_obj_t *obj);

// Style: BarDefault
lv_style_t *get_style_bar_default_MAIN_DEFAULT();
void add_style_bar_default(lv_obj_t *obj);
void remove_style_bar_default(lv_obj_t *obj);

// Style: TextareaDefault
lv_style_t *get_style_textarea_default_MAIN_DEFAULT();
lv_style_t *get_style_textarea_default_MAIN_FOCUSED();
lv_style_t *get_style_textarea_default_CURSOR_DEFAULT();
void add_style_textarea_default(lv_obj_t *obj);
void remove_style_textarea_default(lv_obj_t *obj);

// Style: KeyboardDefault
lv_style_t *get_style_keyboard_default_MAIN_DEFAULT();
lv_style_t *get_style_keyboard_default_ITEMS_DEFAULT();
lv_style_t *get_style_keyboard_default_ITEMS_PRESSED();
lv_style_t *get_style_keyboard_default_ITEMS_CHECKED();
void add_style_keyboard_default(lv_obj_t *obj);
void remove_style_keyboard_default(lv_obj_t *obj);

// Style: DropdownDefault
lv_style_t *get_style_dropdown_default_MAIN_DEFAULT();
lv_style_t *get_style_dropdown_default_INDICATOR_DEFAULT();
void add_style_dropdown_default(lv_obj_t *obj);
void remove_style_dropdown_default(lv_obj_t *obj);

// Style: ListDefault
lv_style_t *get_style_list_default_MAIN_DEFAULT();
void add_style_list_default(lv_obj_t *obj);
void remove_style_list_default(lv_obj_t *obj);

// Style: SwitchDefault
lv_style_t *get_style_switch_default_MAIN_DEFAULT();
lv_style_t *get_style_switch_default_MAIN_CHECKED();
void add_style_switch_default(lv_obj_t *obj);
void remove_style_switch_default(lv_obj_t *obj);

// Style: ArcDefault
lv_style_t *get_style_arc_default_MAIN_DEFAULT();
void add_style_arc_default(lv_obj_t *obj);
void remove_style_arc_default(lv_obj_t *obj);



#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/