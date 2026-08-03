idf_component_register(SRCS "main.c" "display_min.c"
                       REQUIRES bsp_display bsp_i2c driver esp_timer esp_lcd)
