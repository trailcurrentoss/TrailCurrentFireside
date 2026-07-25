# TrailCurrent Fireside — Waveshare ESP32-P4-WIFI6-Touch-LCD-7B
#
# 7" 1024x600 IPS + capacitive touch. Waveshare's esp32_p4_wifi6_touch_lcd_7b
# component provides display, touch, and audio codec (ES8311 + ES7210)
# bring-up. Battery is an ADC-divider read on GPIO 20 (no dedicated
# fuel-gauge IC).

cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(fireside)
