idf_component_register(
    SRCS "src/sd_config.c"
    INCLUDE_DIRS "include"
    REQUIRES fatfs sdmmc driver vfs nvs_flash
)
