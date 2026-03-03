idf_component_register(
    SRCS "src/mqtt_client.c"
    INCLUDE_DIRS "include"
    REQUIRES mqtt esp_event nvs_flash json esp_wifi
)
