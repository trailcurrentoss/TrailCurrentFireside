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

/* ----------------------------------------------------------------------- */
/* Microphone capture (on-board PDM mic: CLK GPIO24, DATA GPIO26, I2S0).   */
/* ----------------------------------------------------------------------- */

/* Capture rate. 16 kHz is the CrowPanel PDM path's native config (vendor
 * Lesson11) and Whisper's native rate — the Peregrine server replies with
 * PCM at whatever rate the request WAV advertises, so playback streaming
 * also runs at this rate (the speaker I2S clock is switched for the
 * duration of the streamed reply). */
#define AUDIO_MIC_SAMPLE_RATE 16000

#include <stdint.h>
#include <stddef.h>

/* Bring up the PDM RX channel. Idempotent — safe to call from
 * peregrine_voice_init(). Returns true on success; on failure the PTT
 * button surface still exists but capture will report empty. */
bool audio_mic_init(void);

/* Begin capturing mic samples into an internal PSRAM buffer. Non-blocking:
 * spawns a helper task that reads I2S chunks until audio_mic_capture_stop().
 * Returns false if mic init failed or a prior capture is still in progress. */
bool audio_mic_capture_start(void);

/* Stop capture and hand back the captured PCM. The pointer stays valid
 * (owned by the audio module) until the next audio_mic_capture_start().
 * *out_bytes is set to the length in bytes (int16_t mono at
 * AUDIO_MIC_SAMPLE_RATE). */
void audio_mic_capture_stop(const int16_t **out_pcm, size_t *out_bytes);

/* ----------------------------------------------------------------------- */
/* Streaming playback (for the Peregrine /api/voice reply).                */
/* ----------------------------------------------------------------------- */

/* Switch the speaker path to the reply's sample rate (from its WAV
 * header — the Peregrine server resamples to its own configured
 * PEREGRINE_VOICE_TARGET_SR, which need not match our mic rate) and
 * power the amp. Alarm phrases are dropped while a stream is active. */
bool audio_play_stream_start(uint32_t sample_rate_hz);

/* Write a chunk of 16-bit mono PCM (at the rate given to
 * audio_play_stream_start) to the speaker, scaled by volume × stream
 * loudness. Blocking on I2S DMA. */
bool audio_play_stream_write(const void *pcm, size_t byte_len);

/* End the stream: drain, restore the 22.05 kHz phrase clock, power off. */
void audio_play_stream_end(void);

/* Loudness trim for streamed replies, independent of the alarm volume.
 * Default 50 % — Piper output peaks hotter than the pre-baked phrases. */
void    audio_set_stream_loudness_pct(uint8_t pct);
uint8_t audio_get_stream_loudness_pct(void);

#ifdef __cplusplus
}
#endif

#endif /* FIRESIDE_AUDIO_H */
