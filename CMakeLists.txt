# TrailCurrent Fireside — Waveshare ESP32-P4-WIFI6-Touch-LCD-7B
#
# 7" 1024x600 IPS + capacitive touch. Waveshare's esp32_p4_wifi6_touch_lcd_7b
# component provides display, touch, and audio codec (ES8311 + ES7210)
# bring-up. Battery is an ADC-divider read on GPIO 20 (no dedicated
# fuel-gauge IC).

cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "components")

# Skip the component manager's "new versions available" check. It re-solves
# against the newest registry releases, and lvgl/lvgl 9.6.0 carries
# $CONFIG{LV_USE_LIBPNG}-style rules that LVGL 8.4's Kconfig never defines,
# which aborts configuration with "Missing required kconfig option after
# retry". Run `idf.py update-dependencies` deliberately instead.
set(ENV{IDF_COMPONENT_CHECK_NEW_VERSION} 0)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(fireside)
