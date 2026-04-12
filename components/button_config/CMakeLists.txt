idf_component_register(
    SRCS "src/button_config.c"
    INCLUDE_DIRS "include"
    REQUIRES nvs_flash
    PRIV_REQUIRES esp_common
)
