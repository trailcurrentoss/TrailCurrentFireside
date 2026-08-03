# Phase 0 touch baseline — vendor Lesson05 touch pipeline, unmodified,
# plus a logging harness and a backlight-PWM noise sweep in main/.
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXTRA_COMPONENT_DIRS "peripheral")
project(TouchBaseline)
