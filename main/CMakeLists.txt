idf_component_register(
    SRCS
        "main.c"
        "ui/images.c"
        "ui/screens.c"
        "ui/styles.c"
        "ui/ui_font_fa16.c"
        "ui/ui_font_fa32.c"
        "ui/ui_font_robot60.c"
        "ui/ui_image_img_chevron_up32px.c"
        "ui/ui_image_needle.c"
        "ui/ui_image_trailer_back"
        "ui/ui_image_trailer_side"
        "ui/ui_image_tuning_background.c"
        "ui/ui.c"
        "actions.c"
        "vars.c"
    INCLUDE_DIRS . ./ui
    REQUIRES esp_wifi mqtt_client sd_config json nvs_flash waveshare__esp32_p4_wifi6_touch_lcd_7b)

idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
target_compile_options(
    ${LVGL_LIB} 
    PRIVATE 
        -DLV_LVGL_H_INCLUDE_SIMPLE
        -DLV_USE_DEMO_MUSIC
)
