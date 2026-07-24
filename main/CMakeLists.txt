# Main component — Fireside CrowPanel port for the Elecrow ESP32-P4 10.1".
#
# The `ui/*` files come from EEZ Studio's export (Ctrl+B). They must appear in
# SRCS or the linker won't find their symbols. Because font conversion is
# owned by tmp/gen_eez_project.py (not EEZ Studio's auto-embed), we glob for
# every `ui_font_*.c` and `ui_image_*.c` the generator emits.
file(GLOB UI_FONT_SRCS "ui/ui_font_*.c")
file(GLOB UI_IMAGE_SRCS "ui/ui_image_*.c")

# The core EEZ export produces screens.c / styles.c / images.c / ui.c.
# These only exist AFTER the user runs EEZ Studio → Build (Ctrl+B) on the
# .eez-project. On the first build (before export), the glob returns empty
# and main/*.c uses `#if __has_include("ui/screens.h")` guards to avoid
# referencing the missing symbols — so the project still links (just shows
# a blank display until the user does the export).
file(GLOB UI_CORE_SRCS "ui/screens.c" "ui/styles.c" "ui/images.c" "ui/ui.c")

idf_component_register(
    SRCS
        main.c
        actions.c
        vars.c
        app_state.c
        alarms.c
        audio.c
        audio_assets.c
        battery.c
        ${UI_CORE_SRCS}
        ${UI_FONT_SRCS}
        ${UI_IMAGE_SRCS}
    INCLUDE_DIRS
        .
        ui
    REQUIRES
        # CrowPanel BSP
        bsp_i2c bsp_display bsp_illuminate bsp_extra
        # LVGL + LCD drivers
        lvgl__lvgl
        espressif__esp_lvgl_port
        espressif__esp_lcd_ek79007
        espressif__esp_lcd_touch_gt911
        # Fireside connectivity stack (copied into local components/)
        wifi_setup mqtt_client fireside_config button_config
        # WiFi over ESP-Hosted (C6 coprocessor on SDIO)
        espressif__esp_hosted
        # IDF stack for MQTT/WiFi/NVS/HTTP
        esp_wifi esp_netif esp_event esp_timer nvs_flash json
        esp_app_format app_update esp_partition
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE
    MODULE_TYPE="fireside-crowpanel-p4-10in"
)

# At -O2 (CONFIG_COMPILER_OPTIMIZATION_PERF) GCC's range analysis assumes
# every `long` printed with %ld could span the full int32 range and errors
# on a dozen `char buf[8]` sites in vars.c / actions.c whose values are
# physically bounded (percent 0-100, small temperatures, etc.). snprintf
# still truncates safely at runtime; this only demotes the compile-time
# error. Fix the individual buffers later if we want to reinstate the
# check.
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-format-truncation)

# LVGL simple header include — matches every other TrailCurrent EEZ project.
idf_component_get_property(LVGL_LIB lvgl__lvgl COMPONENT_LIB)
if(LVGL_LIB)
    target_compile_options(${LVGL_LIB} PRIVATE
        -DLV_LVGL_H_INCLUDE_SIMPLE)
endif()
