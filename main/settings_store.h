/*
 * settings_store — deferred NVS persistence for UI-driven settings.
 *
 * LVGL event callbacks must never block on flash: an nvs_commit() in a
 * slider's VALUE_CHANGED handler stalls the LVGL task (which also polls
 * the touch controller), producing dropped/teleporting touch samples.
 * Callbacks call the setters below, which record the value in RAM and
 * return immediately; a low-priority task batches the writes to NVS
 * ~1 s after the last change.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

void settings_store_init(void);

void settings_store_set_i32(const char *ns, const char *key, int32_t v);
void settings_store_set_u8(const char *ns, const char *key, uint8_t v);
void settings_store_set_u16(const char *ns, const char *key, uint16_t v);
void settings_store_set_str(const char *ns, const char *key, const char *v);
void settings_store_erase(const char *ns, const char *key);

/* Discard anything not yet flushed (factory reset: wipe must win over a
 * pending write that would resurrect a key after nvs_erase_all). */
void settings_store_drop_pending(void);
