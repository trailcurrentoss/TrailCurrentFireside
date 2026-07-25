#pragma once

/*
 * peregrine_voice — push-to-talk client for the Peregrine voice terminal.
 *
 * Captures via the ES7210 mic and plays server-returned audio through
 * the ES8311 speaker path, both via the Waveshare BSP.
 *
 * Flow:
 *
 *   1. TALK button PRESSED    -> peregrine_voice_press()
 *        - audio_mic_capture_start()
 *        - state = LISTENING
 *
 *   2. TALK button RELEASED   -> peregrine_voice_release()
 *        - audio_mic_capture_stop() -> captured PCM
 *        - Spawn worker task that:
 *            a. Wraps the PCM in a WAV header
 *            b. HTTP POSTs to `${PEREGRINE_URL}/api/voice`
 *               with `Authorization: Bearer ${PEREGRINE_VOICE_TOKEN}`
 *            c. Streams the WAV response body straight into
 *               audio_play_stream_write()
 *
 *   3. Reply finishes / errors -> state = IDLE
 *
 * Config is compile-time via Kconfig (component config → Peregrine Voice
 * Terminal). Leave the token empty to disable the request (button still
 * captures locally, useful for testing the mic path without a server).
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PRG_VOICE_IDLE = 0,
    PRG_VOICE_LISTENING,        /* button held, mic capturing */
    PRG_VOICE_UPLOADING,        /* HTTP POST in flight */
    PRG_VOICE_PLAYING,          /* reply body streaming to speaker */
    PRG_VOICE_ERROR,            /* transient error — clears on next press */
} peregrine_voice_state_t;

/* Register the module. Idempotent — safe to call multiple times. Requires
 * that audio_init() has already run so the speaker + mic BSP paths are
 * available. */
void peregrine_voice_init(void);

/* Fired from the TALK button's PRESSED handler. Starts mic capture. */
void peregrine_voice_press(void);

/* Fired from the TALK button's RELEASED or PRESS_LOST handler. Stops mic
 * capture and queues the upload+playback job on the worker task. Safe to
 * call from the LVGL task — the HTTP work runs on its own task. */
void peregrine_voice_release(void);

/* Current state — polled by UI code that wants to reflect capture /
 * upload progress. Cheap; a single read of a volatile enum. */
peregrine_voice_state_t peregrine_voice_get_state(void);

/* Override the Peregrine base URL at runtime. Passing NULL or "" reverts
 * to the compile-time default. Safe to call from any task, at any time —
 * the value is snapshotted at the start of each request. */
void peregrine_voice_set_url(const char *url);

/* Override the bearer token at runtime. Passing NULL or "" disables the
 * upload (button still captures locally). Boot-time SD-card config
 * (main/sd_config.c) is the standard caller. */
void peregrine_voice_set_token(const char *token);

#ifdef __cplusplus
}
#endif
