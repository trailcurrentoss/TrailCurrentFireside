#pragma once

#include <stdbool.h>

#define DISCOVERY_TIMEOUT_MS 180000

void discovery_mdns_init(void);
void discovery_init(void);
void discovery_handle_trigger(void);
bool discovery_is_running(void);
