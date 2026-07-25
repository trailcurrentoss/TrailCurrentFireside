/*
 * audio.c — ES8311 codec TTS playback for the Waveshare
 * ESP32-P4-WIFI6-Touch-LCD-7B, via the Waveshare BSP's esp_codec_dev
 * abstraction.
 *
 * The board has an ES8311 codec IC on I2C 0x18 that needs to be
 * configured before samples come out, plus a class-D power amplifier
 * whose enable pin sits on GPIO 53. All of that is encapsulated by
 * bsp_audio_codec_speaker_init() — we just open/write/close per phrase.
 *
 * Ordering rules:
 *   1. bsp_i2c_init() must run before audio_init (the codec IC is on I2C).
 *      main.c handles this.
 *   2. The Waveshare BSP calls bsp_i2c_init() again from inside
 *      bsp_audio_codec_speaker_init if needed — safe (idempotent).
 *   3. Codec is left OPEN between phrases so the PA doesn't cycle on
 *      every 1-second alarm burst. First open happens lazily on the
 *      first playback, not in audio_init, so I2C is quiet during the
 *      touch enumeration window at boot.
 */
#include "audio.h"
#include "audio_assets.h"

#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "esp_codec_dev.h"
#include "esp_codec_dev_defaults.h"
#include "bsp/esp32_p4_wifi6_touch_lcd_7b.h"

static const char *TAG = "AUDIO";

/* All source PCM assets are 16-bit signed, mono, 22.05 kHz. Keep the
 * codec configured for that exact format so we never resample. */
#define SAMPLE_RATE          22050
#define SAMPLE_BITS          16
#define SAMPLE_CHANNELS      1

/* --- Codec dev + queue state ----------------------------------------- */
static esp_codec_dev_handle_t s_spk        = NULL;
static bool                   s_spk_opened = false;
static QueueHandle_t          s_req_q      = NULL;
static SemaphoreHandle_t      s_mtx        = NULL;

/* Volume 0..100. 30 % baseline — pre-normalized TTS samples otherwise
 * clip through the ES8311 + class-D amp. Bumped via Settings and
 * persisted to NVS. */
static uint8_t s_volume_pct = 30;

/* Open codec at the desired sample_info. Caller holds s_mtx. */
static bool codec_ensure_open(void) {
    if (s_spk_opened) return true;
    if (!s_spk) {
        ESP_LOGE(TAG, "codec not initialized");
        return false;
    }
    esp_codec_dev_sample_info_t fs = {
        .bits_per_sample = SAMPLE_BITS,
        .channel         = SAMPLE_CHANNELS,
        .channel_mask    = 0,             /* accept all channels present */
        .sample_rate     = SAMPLE_RATE,
        .mclk_multiple   = 0,             /* default MCLK = 256 * fs      */
    };
    int rc = esp_codec_dev_open(s_spk, &fs);
    if (rc != 0) {
        ESP_LOGE(TAG, "esp_codec_dev_open failed rc=%d", rc);
        return false;
    }
    /* Apply the current volume immediately so the first phrase doesn't
     * play at whatever the codec defaults happen to be. */
    esp_codec_dev_set_out_vol(s_spk, (int)s_volume_pct);
    s_spk_opened = true;
    return true;
}

static void play_asset(const audio_asset_t *a) {
    if (!s_spk || !a || !a->pcm || a->samples == 0) return;
    if (s_volume_pct == 0) return;                   /* muted */

    xSemaphoreTake(s_mtx, portMAX_DELAY);
    if (!codec_ensure_open()) {
        xSemaphoreGive(s_mtx);
        return;
    }
    /* Refresh volume — the settings screen may have changed it since
     * the last playback, and the ES8311 keeps its last written value. */
    esp_codec_dev_set_out_vol(s_spk, (int)s_volume_pct);

    /* esp_codec_dev_write is blocking and expects a byte length. The
     * PCM asset is int16_t so bytes = samples * 2. The BSP's I2S TX
     * channel is master; write returns when DMA is drained. No silence
     * pad or PA cycling needed — the ES8311 mutes its output between
     * writes and the BSP's PA control is state-driven, not push. */
    int rc = esp_codec_dev_write(s_spk, (void *)a->pcm,
                                 (int)(a->samples * sizeof(int16_t)));
    if (rc != 0) {
        ESP_LOGW(TAG, "esp_codec_dev_write rc=%d (%zu samples)",
                 rc, a->samples);
    }
    xSemaphoreGive(s_mtx);
}

static void audio_task(void *pv) {
    (void)pv;
    audio_phrase_t req;
    while (1) {
        if (xQueueReceive(s_req_q, &req, portMAX_DELAY) == pdTRUE) {
            if (req >= AUDIO_PHRASE_COUNT) continue;
            play_asset(&audio_assets[req]);
        }
    }
}

void audio_init(void) {
    s_mtx = xSemaphoreCreateMutex();
    if (!s_mtx) {
        ESP_LOGE(TAG, "xSemaphoreCreateMutex failed");
        return;
    }
    /* The Waveshare BSP's bsp_audio_codec_speaker_init() internally:
     *   - calls bsp_i2c_init()   (idempotent)
     *   - calls bsp_audio_init() (I2S channel + esp_codec I2S data-if)
     *   - talks to ES8311 over I2C at ES8311_CODEC_DEFAULT_ADDR
     *   - drives BSP_POWER_AMP_IO (GPIO 53) for the on-board PA
     * Returns NULL on failure — we log and continue so the rest of the
     * firmware still boots (alarms will fire without audio). */
    s_spk = bsp_audio_codec_speaker_init();
    if (!s_spk) {
        ESP_LOGE(TAG, "bsp_audio_codec_speaker_init returned NULL");
        return;
    }

    s_req_q = xQueueCreate(4, sizeof(audio_phrase_t));
    if (!s_req_q) {
        ESP_LOGE(TAG, "xQueueCreate failed");
        return;
    }
    if (xTaskCreate(audio_task, "audio", 4096, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "xTaskCreate(audio) failed");
        return;
    }
    ESP_LOGI(TAG, "audio ready — ES8311 @ %u Hz, %d phrases loaded",
             (unsigned)SAMPLE_RATE, (int)AUDIO_PHRASE_COUNT);
}

void audio_play_phrase(audio_phrase_t id) {
    if (!s_req_q) return;
    if (id >= AUDIO_PHRASE_COUNT) return;
    xQueueSend(s_req_q, &id, 0);
}

void audio_set_volume(uint8_t pct) {
    if (pct > 100) pct = 100;
    s_volume_pct = pct;
    /* If the codec is open right now, apply immediately so the slider is
     * responsive. The next playback will re-apply anyway; this just makes
     * the UI feel less laggy when a phrase is mid-play. */
    if (s_spk && s_spk_opened) {
        xSemaphoreTake(s_mtx, portMAX_DELAY);
        esp_codec_dev_set_out_vol(s_spk, (int)s_volume_pct);
        xSemaphoreGive(s_mtx);
    }
}
uint8_t audio_get_volume(void) { return s_volume_pct; }

/* ======================================================================= */
/* Microphone capture (ES7210 via BSP)                                     */
/* ======================================================================= */

/* Software mic gain — the ES7210 output at unity is quiet enough that
 * Whisper often mis-hears. 4x is the sweet spot; higher values start
 * clipping short vowels. */
#define MIC_SW_GAIN 4

/* Max capture duration. 22050 Hz mono 16-bit → ~44 KB/s. 10s = 440 KB,
 * fits comfortably in the 32 MB PSRAM available on the Waveshare board.
 * Longer captures get truncated in place so the buffer allocation is
 * bounded. */
#define MIC_MAX_SECONDS 10
#define MIC_MAX_BYTES   ((size_t)AUDIO_MIC_SAMPLE_RATE * 2 * MIC_MAX_SECONDS)

/* Per-read chunk size. esp_codec_dev_read is blocking; sizing the chunk
 * to ~50 ms of audio (2205 samples × 2 bytes) gives snappy stop response. */
#define MIC_READ_CHUNK_BYTES 4096

static esp_codec_dev_handle_t s_mic         = NULL;
static bool                   s_mic_opened  = false;
static SemaphoreHandle_t      s_mic_mtx     = NULL;

static int16_t   *s_mic_buf       = NULL;   /* PSRAM PCM buffer */
static size_t     s_mic_buf_cap   = 0;
static size_t     s_mic_buf_len   = 0;      /* bytes written */
static TaskHandle_t s_mic_task    = NULL;
static volatile bool s_mic_stop_req = false;

bool audio_mic_init(void) {
    if (s_mic) return true;
    s_mic_mtx = xSemaphoreCreateMutex();
    if (!s_mic_mtx) {
        ESP_LOGE(TAG, "mic mutex create failed");
        return false;
    }
    /* Waveshare BSP's microphone init idempotently sets up I2C + I2S
     * (shared with the speaker path) and returns a handle bound to the
     * ES7210 codec IC. */
    s_mic = bsp_audio_codec_microphone_init();
    if (!s_mic) {
        ESP_LOGE(TAG, "bsp_audio_codec_microphone_init returned NULL");
        return false;
    }
    ESP_LOGI(TAG, "mic ready — ES7210 @ %u Hz", (unsigned)AUDIO_MIC_SAMPLE_RATE);
    return true;
}

static bool mic_ensure_open(void) {
    if (s_mic_opened) return true;
    if (!s_mic) return false;
    esp_codec_dev_sample_info_t fs = {
        .bits_per_sample = 16,
        .channel         = 1,
        .channel_mask    = 0,
        .sample_rate     = AUDIO_MIC_SAMPLE_RATE,
        .mclk_multiple   = 0,
    };
    int rc = esp_codec_dev_open(s_mic, &fs);
    if (rc != 0) {
        ESP_LOGE(TAG, "esp_codec_dev_open(mic) rc=%d", rc);
        return false;
    }
    /* ES7210 accepts a gain-in-dB. 30 dB puts a normal speaking voice at
     * ~50 % of full scale with the on-board mic; MIC_SW_GAIN handles the
     * rest without slowing the peripheral (see LessonsLearned #46). */
    esp_codec_dev_set_in_gain(s_mic, 30.0);
    s_mic_opened = true;
    return true;
}

static void mic_close(void) {
    if (!s_mic_opened || !s_mic) return;
    esp_codec_dev_close(s_mic);
    s_mic_opened = false;
}

static void mic_capture_task(void *pv) {
    (void)pv;
    /* Per-chunk scratch. Kept on-stack (task stack sized accordingly). */
    static uint8_t chunk[MIC_READ_CHUNK_BYTES];

    while (!s_mic_stop_req) {
        int rc = esp_codec_dev_read(s_mic, chunk, sizeof(chunk));
        if (rc != 0) {
            ESP_LOGW(TAG, "esp_codec_dev_read rc=%d", rc);
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }
        /* Apply software gain in place before appending. */
        int16_t *s = (int16_t *)chunk;
        size_t   n = sizeof(chunk) / 2;
        for (size_t i = 0; i < n; i++) {
            int32_t v = (int32_t)s[i] * MIC_SW_GAIN;
            if (v >  32767) v =  32767;
            if (v < -32768) v = -32768;
            s[i] = (int16_t)v;
        }

        xSemaphoreTake(s_mic_mtx, portMAX_DELAY);
        size_t room = s_mic_buf_cap - s_mic_buf_len;
        size_t take = (sizeof(chunk) < room) ? sizeof(chunk) : room;
        if (take > 0) {
            memcpy((uint8_t *)s_mic_buf + s_mic_buf_len, chunk, take);
            s_mic_buf_len += take;
        }
        xSemaphoreGive(s_mic_mtx);

        if (take == 0) {
            /* Hit the cap — stop capturing rather than silently dropping. */
            ESP_LOGW(TAG, "mic capture hit %d-second cap", MIC_MAX_SECONDS);
            break;
        }
    }
    /* Note: we DO NOT close the mic here — audio_mic_capture_stop() does that
     * after joining. Closing while a read is still in flight can wedge the
     * codec. */
    s_mic_task = NULL;
    vTaskDelete(NULL);
}

bool audio_mic_capture_start(void) {
    if (!audio_mic_init()) return false;
    if (s_mic_task) {
        ESP_LOGW(TAG, "mic capture already in progress");
        return false;
    }
    xSemaphoreTake(s_mic_mtx, portMAX_DELAY);
    if (!s_mic_buf) {
        s_mic_buf = heap_caps_malloc(MIC_MAX_BYTES, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        s_mic_buf_cap = s_mic_buf ? MIC_MAX_BYTES : 0;
    }
    s_mic_buf_len = 0;
    xSemaphoreGive(s_mic_mtx);
    if (!s_mic_buf) {
        ESP_LOGE(TAG, "mic buffer alloc failed (%u bytes PSRAM)",
                 (unsigned)MIC_MAX_BYTES);
        return false;
    }
    if (!mic_ensure_open()) return false;

    s_mic_stop_req = false;
    /* Task stack: MIC_READ_CHUNK_BYTES lives in .bss (static), so the task's
     * own stack only needs room for the FreeRTOS + codec_dev call chain.
     * 4 KB is comfortable. Pinned to core 1 to stay off the WiFi/MQTT core. */
    BaseType_t ok = xTaskCreatePinnedToCore(mic_capture_task, "mic_cap",
                                            4096, NULL, 6, &s_mic_task, 1);
    if (ok != pdPASS) {
        ESP_LOGE(TAG, "mic capture task create failed");
        mic_close();
        s_mic_task = NULL;
        return false;
    }
    ESP_LOGI(TAG, "mic capture started");
    return true;
}

void audio_mic_capture_stop(const int16_t **out_pcm, size_t *out_bytes) {
    if (out_pcm)   *out_pcm   = NULL;
    if (out_bytes) *out_bytes = 0;

    s_mic_stop_req = true;
    /* Wait for the task to observe the flag and self-delete. Timeout after
     * ~1s so a wedged read doesn't hang the UI thread forever. */
    for (int i = 0; i < 100 && s_mic_task != NULL; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    if (s_mic_task) {
        ESP_LOGW(TAG, "mic capture task didn't stop — force deleting");
        vTaskDelete(s_mic_task);
        s_mic_task = NULL;
    }
    mic_close();

    xSemaphoreTake(s_mic_mtx, portMAX_DELAY);
    if (out_pcm)   *out_pcm   = s_mic_buf;
    if (out_bytes) *out_bytes = s_mic_buf_len;
    xSemaphoreGive(s_mic_mtx);

    ESP_LOGI(TAG, "mic capture stopped — %u bytes (%.2fs)",
             (unsigned)s_mic_buf_len,
             (double)s_mic_buf_len / (AUDIO_MIC_SAMPLE_RATE * 2.0));
}

/* ======================================================================= */
/* Streaming playback (for Peregrine /api/voice reply)                     */
/* ======================================================================= */

/* Static scratch matches esp_http_client's default chunk size. Living in
 * internal SRAM (BSS, not PSRAM) satisfies the I2S DMA source-memory
 * requirement — see LessonsLearned #33 and #43. */
#define STREAM_SCRATCH_BYTES 4096
static int16_t s_stream_scratch[STREAM_SCRATCH_BYTES / 2];

/* Loudness attenuator for the streamed reply, independent of the alarm
 * volume. Default 50 % — Piper output peaks ~2× hotter than the pre-baked
 * ChatterboxTTS alarm assets, so this keeps both paths comfortably below
 * clip at the same volume knob position. See LessonsLearned #34. */
static uint8_t s_stream_loudness_pct = 50;

bool audio_play_stream_start(void) {
    if (!s_spk) return false;
    xSemaphoreTake(s_mtx, portMAX_DELAY);
    bool ok = codec_ensure_open();
    if (ok) {
        esp_codec_dev_set_out_vol(s_spk, (int)s_volume_pct);
    }
    xSemaphoreGive(s_mtx);
    return ok;
}

bool audio_play_stream_write(const void *pcm, size_t byte_len) {
    if (!s_spk || !s_spk_opened || !pcm || byte_len == 0) return false;
    /* Combined gain: alarm volume × per-stream loudness / 10000.
     * At volume=100 %, loudness=50 % → -6 dB total. */
    int32_t gain = (int32_t)s_volume_pct * (int32_t)s_stream_loudness_pct;
    if (gain <= 0) return true;  /* muted — drop the chunk silently */

    byte_len &= ~(size_t)1;      /* even-byte, int16-aligned */
    const int16_t *src = (const int16_t *)pcm;
    size_t remaining = byte_len / 2;
    const size_t scratch_samples = STREAM_SCRATCH_BYTES / 2;

    xSemaphoreTake(s_mtx, portMAX_DELAY);
    while (remaining > 0) {
        size_t take = (remaining < scratch_samples) ? remaining : scratch_samples;
        for (size_t i = 0; i < take; i++) {
            int32_t v = ((int32_t)src[i] * gain) / 10000;
            if (v >  32767) v =  32767;
            if (v < -32768) v = -32768;
            s_stream_scratch[i] = (int16_t)v;
        }
        int rc = esp_codec_dev_write(s_spk, s_stream_scratch, (int)(take * 2));
        if (rc != 0) {
            ESP_LOGW(TAG, "esp_codec_dev_write(stream) rc=%d", rc);
            xSemaphoreGive(s_mtx);
            return false;
        }
        src       += take;
        remaining -= take;
    }
    xSemaphoreGive(s_mtx);
    return true;
}

void audio_play_stream_end(void) {
    /* Leave the codec open — the next alarm phrase or the next stream
     * reply reuses the open handle. Just log so timing can be traced. */
    ESP_LOGI(TAG, "stream reply ended");
}

void audio_set_stream_loudness_pct(uint8_t pct) {
    if (pct > 100) pct = 100;
    s_stream_loudness_pct = pct;
}
uint8_t audio_get_stream_loudness_pct(void) { return s_stream_loudness_pct; }
