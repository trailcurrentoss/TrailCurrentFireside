# TrailCurrent Fireside — Elecrow CrowPanel Advance ESP32-P4 HMI (10.1", 1024x600)
cmake_minimum_required(VERSION 3.16)

# Local components live under components/ (bsp_i2c, bsp_display, bsp_illuminate,
# bsp_extra, bsp_sd, bsp_compat, plus the Fireside-imported button_config,
# can_bus, fireside_config, mqtt_client, wifi_setup). Generated LVGL UI files
# come from EEZ Studio into main/ui/.
set(EXTRA_COMPONENT_DIRS "components")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(fireside-elecrow-p4-10in)
