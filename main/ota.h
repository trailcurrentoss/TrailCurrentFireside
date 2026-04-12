#pragma once

#include <stdbool.h>

#define OTA_TIMEOUT_MS 180000

void ota_init(void);
void ota_handle_trigger(void);
bool ota_is_running(void);
