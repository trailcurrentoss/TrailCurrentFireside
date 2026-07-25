#ifndef FIRESIDE_AUDIO_H
#define FIRESIDE_AUDIO_H

/*
 * Fireside audio — I2S TTS playback for the Waveshare ESP32-P4-WIFI6-Touch-LCD-7B.
 *
 * The Waveshare board has an ES8311 codec (I2S DAC) with an on-board class-D
 * PA. Access goes through the Waveshare BSP's esp_codec_dev abstraction:
 *
 *     bsp_audio_codec_speaker_init()   -> returns esp_codec_dev_handle_t
 *     esp_codec_dev_open(..., &fs)
 *     esp_codec_dev_set_out_vol(...)   -> volume 0..100
 *     esp_codec_dev_write(..., pcm, n) -> blocking I2S write
 *     esp_codec_dev_close(...)
 *
 * BSP pin map (documented in the Waveshare BSP header):
 *     I2S MCLK = GPIO 13, BCLK = 12, LRCK = 10, DOUT = 9, DIN = 11
 *     I2C SDA  = GPIO  7, SCL = 8 (shared with GT911 touch)
 *     PA EN    = GPIO 53
 *
 * Playback assets: pre-generated TTS phrases embedded as C arrays in
 * audio_assets.c (16-bit signed PCM, mono, 22.05 kHz). Regenerate via
 * `python3 tmp/gen_tts_assets.py` (uses ChatterboxTTS via local ComfyUI).
 *
 * Fires on the rising edge of alarms_active_count() (see paint_notif_badge
 * in vars.c). Playback is dispatched to a FreeRTOS task so the caller
 * doesn't block on I2S DMA.
 */

#include <stdbool.h>
#include "audio_assets.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Bring up the ES8311 codec via the Waveshare BSP and spawn the audio task.
 * Call once from main.c after I2C is initialized (bsp_i2c_init) and LVGL
 * is up. The codec is opened lazily on the first playback so I2C timing
 * doesn't collide with the initial touch enumeration. */
void audio_init(void);

/* Enqueue a phrase for playback. Non-blocking; drops the request if the
 * queue is full (previous clip still playing). Safe from any task. */
void audio_play_phrase(audio_phrase_t id);

/* Volume 0..100 percent. Passed to esp_codec_dev_set_out_vol on next
 * playback. Default 30 (the ES8311's hardware gain path can distort at
 * 100 % with these normalized PCM samples). Persisted to NVS
 * "fireside.vol". */
void    audio_set_volume(uint8_t pct);
uint8_t audio_get_volume(void);

/* ----------------------------------------------------------------------- */
/* Microphone capture (ES7210 codec via Waveshare BSP).                    */
/* ----------------------------------------------------------------------- */

/* Sample rate captured and expected by the streaming playback path. Matches
 * the speaker configuration so the I2S peripheral doesn't need to be
 * reconfigured between mic and speaker use. */
#define AUDIO_MIC_SAMPLE_RATE 22050

/* Bring up the ES7210 microphone codec. Idempotent — safe to call from
 * peregrine_voice_init(). Returns true on success; on failure the PTT
 * button surface still exists but capture will report empty. */
bool audio_mic_init(void);

/* Begin capturing mic samples into an internal PSRAM buffer. Non-blocking:
 * spawns a helper task that reads I2S chunks until audio_mic_capture_stop().
 * Returns false if mic init failed or a prior capture is still in progress. */
bool audio_mic_capture_start(void);

/* Stop capture and hand back the captured PCM. The pointer stays valid
 * (owned by the audio module) until the next audio_mic_capture_start().
 * *out_bytes is set to the length in bytes (int16_t mono at AUDIO_MIC_SAMPLE_RATE). */
void audio_mic_capture_stop(const int16_t **out_pcm, size_t *out_bytes);

/* ----------------------------------------------------------------------- */
/* Streaming playback (for the Peregrine /api/voice reply).                */
/* ----------------------------------------------------------------------- */

/* Open the speaker for a streamed reply. Applies the current stream
 * loudness. Safe to call from any task. Returns true on success. */
bool audio_play_stream_start(void);

/* Feed a chunk of raw PCM (int16_t mono at AUDIO_MIC_SAMPLE_RATE) into the
 * speaker as it arrives from the HTTP body. Blocks on I2S DMA. Uses a
 * static internal-SRAM scratch buffer for volume scaling — no per-chunk
 * malloc (see LessonsLearned #33). */
bool audio_play_stream_write(const void *pcm, size_t byte_len);

/* Signal that the streaming reply is finished. Does not close the codec
 * (it stays open for subsequent alarm phrases). */
void audio_play_stream_end(void);

/* Loudness attenuator applied to STREAMED replies only. 0..100 %. Combined
 * with the main volume as gain = volume × loudness / 10000. Default 50 %
 * because Piper TTS peaks hotter than the pre-baked ChatterboxTTS alarm
 * assets (see LessonsLearned #34). */
void    audio_set_stream_loudness_pct(uint8_t pct);
uint8_t audio_get_stream_loudness_pct(void);

#ifdef __cplusplus
}
#endif

#endif /* FIRESIDE_AUDIO_H */
