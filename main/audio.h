#ifndef FIRESIDE_AUDIO_H
#define FIRESIDE_AUDIO_H

/*
 * Fireside audio — I2S TTS playback for CrowPanel Advance 10.1" ESP32-P4.
 *
 * Pin map (fixed by the board, from CrowPanel reference board_config.h):
 *   AUDIO_POWER_EN  = GPIO 30  (active LOW)
 *   I2S LRCLK / WS  = GPIO 21
 *   I2S BCLK / SCK  = GPIO 22
 *   I2S SDATA / DIN = GPIO 23
 *
 * Playback assets: pre-generated TTS phrases embedded as C arrays in
 * audio_assets.c (16-bit signed PCM, mono, 22.05 kHz — matches the I2S
 * config so no runtime resampling is needed). Regenerate via
 * `python3 tmp/gen_tts_assets.py` (uses ChatterboxTTS via local ComfyUI).
 *
 * Fires on the rising edge of alarms_active_count() (see
 * paint_notif_badge in vars.c). Playback is dispatched to a FreeRTOS
 * task so the caller doesn't block on I2S DMA.
 */

#include <stdbool.h>
#include "audio_assets.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Bring up I2S TX + power-enable GPIO. Spawns the audio task. Call once
 * from main.c after LVGL is up. (CrowPanel-P4 boot-order rule: after
 * touch_init, before WiFi — audio doesn't hit the same regression, but
 * we keep the ordering consistent.) */
void audio_init(void);

/* Enqueue a phrase for playback. Non-blocking; drops the request if the
 * queue is full (previous clip still playing). Safe from any task. */
void audio_play_phrase(audio_phrase_t id);

/* Volume 0..100 percent. Applied as a multiplicative gain to samples
 * before they hit the DAC. Default 100. Persisted to NVS "fireside.vol". */
void    audio_set_volume(uint8_t pct);
uint8_t audio_get_volume(void);

#ifdef __cplusplus
}
#endif

#endif /* FIRESIDE_AUDIO_H */
