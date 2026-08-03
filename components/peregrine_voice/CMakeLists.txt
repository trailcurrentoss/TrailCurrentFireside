idf_component_register(
    SRCS         "src/peregrine_voice.c"
    INCLUDE_DIRS "include"
    REQUIRES     esp_http_client esp_event freertos
    PRIV_REQUIRES esp_timer log
)
