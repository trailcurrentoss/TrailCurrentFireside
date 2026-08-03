#pragma once

/*
 * sd_config — boot-time reader for /sdcard/environment.conf.
 *
 * Mounts the SD card via the Waveshare BSP, opens
 * `/sdcard/environment.conf`, parses KEY=VALUE lines, dispatches recognized
 * keys to their consumer modules, then unmounts.
 *
 * File format (KEY=VALUE per line, '#' starts a line comment):
 *
 *     # Peregrine voice terminal
 *     PEREGRINE_URL=http://peregrine.local:8081
 *     PEREGRINE_VOICE_TOKEN=8cb78a5aad34...
 *
 * Recognized keys:
 *   PEREGRINE_URL           -> peregrine_voice_set_url()
 *   PEREGRINE_VOICE_TOKEN   -> peregrine_voice_set_token()
 *
 * Unrecognized keys are logged (info) and skipped — the same file can carry
 * MQTT / Headwaters / tile-server keys later without breaking here.
 *
 * If the SD card is absent, unformatted, or the file is missing, this is a
 * no-op — the compile-time Kconfig defaults for the consumers stay in effect.
 * Returns ESP_OK on any successful boot-config pass (including "no card").
 */

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t sd_config_load(void);

/* True only when the most recent sd_config_load() found BOTH
 * PEREGRINE_URL and PEREGRINE_VOICE_TOKEN with non-empty values in
 * /sdcard/environment.conf. False when the card is absent, the file is
 * missing, or either key is missing/blank.
 *
 * main.c uses this to decide whether the PageHome TALK button is visible:
 * without a URL + token there is nothing for push-to-talk to talk to, so
 * the button is hidden rather than offered and left dead. */
bool sd_config_peregrine_present(void);

#ifdef __cplusplus
}
#endif
