/*
 * peregrine_voice.c — push-to-talk voice terminal client.
 *
 * Threading model
 * ---------------
 * The TALK button press/release enters via peregrine_voice_press() /
 * peregrine_voice_release() on the LVGL task. Press starts mic capture
 * synchronously (cheap). Release drops off a job to a dedicated
 * "prg_voice" worker task via a length-1 queue, so the LVGL task never
 * blocks on HTTP.
 *
 * The worker task owns the HTTP request end-to-end: uploads the WAV,
 * pulls the streamed reply body chunk-by-chunk, and pushes each chunk
 * straight into the speaker. The first 44 bytes of the response body are
 * treated as the WAV header and dropped (payload is 16-bit signed mono
 * PCM at the same rate the request advertised, matching AUDIO_MIC_SAMPLE_RATE).
 *
 * Streaming assumptions: the Peregrine server writes the WAV header
 * first, then Piper output as it's synthesized sentence-by-sentence. No
 * Content-Length is set — the socket closes when the response is
 * complete. esp_http_client's ESP_ERR_HTTP_INCOMPLETE_DATA is expected
 * here and is NOT an error as long as status == 200 and body bytes were
 * received.
 */

#include "peregrine_voice.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_http_client.h"

#include "sdkconfig.h"

/* Forward-declare the audio API we need from main/audio.[ch]. Components
 * under components/ cannot REQUIRE `main` (main is the terminal node of
 * the ESP-IDF dependency graph), so we can't just include "audio.h" here.
 * These signatures must stay in sync with main/audio.h. */
#define AUDIO_MIC_SAMPLE_RATE 16000   /* CrowPanel PDM mic rate — must match
                                       * main/audio.h AUDIO_MIC_SAMPLE_RATE */
extern bool audio_mic_init(void);
extern bool audio_mic_capture_start(void);
extern void audio_mic_capture_stop(const int16_t **out_pcm, size_t *out_bytes);
extern bool audio_play_stream_start(uint32_t sample_rate_hz);
extern bool audio_play_stream_write(const void *pcm, size_t byte_len);
extern void audio_play_stream_end(void);

static const char *TAG = "PRG_VOICE";

/* Compile-time Kconfig defaults. These seed the runtime buffers below;
 * peregrine_voice_set_url / _set_token override them at boot (main/sd_config.c
 * loads /sdcard/environment.conf and calls the setters). */
#ifndef CONFIG_PEREGRINE_URL
#define CONFIG_PEREGRINE_URL "http://peregrine.local:8081"
#endif
#ifndef CONFIG_PEREGRINE_VOICE_TOKEN
#define CONFIG_PEREGRINE_VOICE_TOKEN ""
#endif
#ifndef CONFIG_PEREGRINE_HTTP_TIMEOUT_MS
#define CONFIG_PEREGRINE_HTTP_TIMEOUT_MS 60000
#endif

/* Sized generously: URLs typically ~40 chars, tokens are 64-hex + terminator. */
#define PRG_URL_MAX   128
#define PRG_TOKEN_MAX 128

static char s_url[PRG_URL_MAX]     = CONFIG_PEREGRINE_URL;
static char s_token[PRG_TOKEN_MAX] = CONFIG_PEREGRINE_VOICE_TOKEN;

#define PATH_VOICE      "/api/voice"

/* Response-body read chunk. Matches STREAM_SCRATCH_BYTES in audio.c so the
 * hot path never triggers a scratch-crossing loop for typical chunk sizes. */
#define RESP_CHUNK_BYTES 4096

/* WAV header for RIFF/PCM at AUDIO_MIC_SAMPLE_RATE mono 16-bit. Total data size is
 * patched in at runtime — this is the fixed byte layout. */
#define WAV_HDR_LEN 44

static volatile peregrine_voice_state_t s_state = PRG_VOICE_IDLE;

static TaskHandle_t  s_worker      = NULL;
static QueueHandle_t s_job_q       = NULL;

/* A submitted upload job. The PCM pointer stays owned by audio.c (the
 * capture buffer lives across mic_capture_stop / next mic_capture_start).
 * We snapshot the length at release time so a subsequent press doesn't
 * clobber the in-flight upload. */
typedef struct {
    const int16_t *pcm;
    size_t         byte_len;
} prg_job_t;

static void wav_header_write(uint8_t *out, uint32_t pcm_bytes,
                             uint32_t sample_rate, uint16_t channels,
                             uint16_t bits_per_sample) {
    uint32_t byte_rate   = sample_rate * channels * (bits_per_sample / 8);
    uint16_t block_align = channels * (bits_per_sample / 8);
    uint32_t data_size   = pcm_bytes;
    uint32_t riff_size   = 36 + data_size;

    /* RIFF header */
    memcpy(out + 0, "RIFF", 4);
    out[4]  = (uint8_t)(riff_size);
    out[5]  = (uint8_t)(riff_size >> 8);
    out[6]  = (uint8_t)(riff_size >> 16);
    out[7]  = (uint8_t)(riff_size >> 24);
    memcpy(out + 8, "WAVE", 4);

    /* fmt chunk */
    memcpy(out + 12, "fmt ", 4);
    out[16] = 16; out[17] = 0; out[18] = 0; out[19] = 0;     /* subchunk1 size */
    out[20] = 1;  out[21] = 0;                               /* PCM */
    out[22] = (uint8_t)channels;         out[23] = (uint8_t)(channels >> 8);
    out[24] = (uint8_t)sample_rate;      out[25] = (uint8_t)(sample_rate >> 8);
    out[26] = (uint8_t)(sample_rate >> 16); out[27] = (uint8_t)(sample_rate >> 24);
    out[28] = (uint8_t)byte_rate;        out[29] = (uint8_t)(byte_rate >> 8);
    out[30] = (uint8_t)(byte_rate >> 16); out[31] = (uint8_t)(byte_rate >> 24);
    out[32] = (uint8_t)block_align;      out[33] = (uint8_t)(block_align >> 8);
    out[34] = (uint8_t)bits_per_sample;  out[35] = (uint8_t)(bits_per_sample >> 8);

    /* data chunk */
    memcpy(out + 36, "data", 4);
    out[40] = (uint8_t)(data_size);
    out[41] = (uint8_t)(data_size >> 8);
    out[42] = (uint8_t)(data_size >> 16);
    out[43] = (uint8_t)(data_size >> 24);
}

/* Run one voice job end-to-end. Called from the worker task. */
static void run_job(const prg_job_t *job) {
    const int64_t t_start = esp_timer_get_time();

    if (!job || !job->pcm || job->byte_len == 0) {
        ESP_LOGW(TAG, "empty capture — skipping upload");
        s_state = PRG_VOICE_IDLE;
        return;
    }

    /* No token configured -> mic-only smoke test. Log the capture size
     * so bench users can confirm the mic path works before wiring the
     * server. */
    if (s_token[0] == '\0') {
        ESP_LOGW(TAG, "PEREGRINE_VOICE_TOKEN empty — captured %u bytes, "
                      "skipping /api/voice request",
                 (unsigned)job->byte_len);
        s_state = PRG_VOICE_IDLE;
        return;
    }

    char url[PRG_URL_MAX + 32];
    snprintf(url, sizeof(url), "%s%s", s_url, PATH_VOICE);

    esp_http_client_config_t cfg = {
        .url        = url,
        .method     = HTTP_METHOD_POST,
        .timeout_ms = CONFIG_PEREGRINE_HTTP_TIMEOUT_MS,
        /* Streaming response — don't buffer the whole body in the client. */
        .buffer_size     = RESP_CHUNK_BYTES,
        .buffer_size_tx  = 1024,
    };
    esp_http_client_handle_t c = esp_http_client_init(&cfg);
    if (!c) {
        ESP_LOGE(TAG, "esp_http_client_init failed");
        s_state = PRG_VOICE_ERROR;
        return;
    }
    esp_http_client_set_header(c, "Content-Type", "audio/wav");
    char auth[PRG_TOKEN_MAX + 16];
    snprintf(auth, sizeof(auth), "Bearer %s", s_token);
    esp_http_client_set_header(c, "Authorization", auth);

    /* Upload */
    s_state = PRG_VOICE_UPLOADING;
    uint8_t wav_hdr[WAV_HDR_LEN];
    wav_header_write(wav_hdr, (uint32_t)job->byte_len,
                     AUDIO_MIC_SAMPLE_RATE, 1, 16);

    const int64_t t_open = esp_timer_get_time();
    esp_err_t err = esp_http_client_open(c, (int)(WAV_HDR_LEN + job->byte_len));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "http open failed: %s", esp_err_to_name(err));
        esp_http_client_cleanup(c);
        s_state = PRG_VOICE_ERROR;
        return;
    }
    int w = esp_http_client_write(c, (const char *)wav_hdr, WAV_HDR_LEN);
    if (w != WAV_HDR_LEN) {
        ESP_LOGE(TAG, "http write(hdr) short: %d", w);
        goto out_close;
    }
    /* Push the PCM in ~8 KB slices. esp_http_client_write handles the
     * inner TCP loop; slicing keeps the WiFi stack from choking on a
     * single 400 KB write. */
    const size_t SLICE = 8192;
    size_t off = 0;
    while (off < job->byte_len) {
        size_t take = job->byte_len - off;
        if (take > SLICE) take = SLICE;
        w = esp_http_client_write(c, (const char *)job->pcm + off, take);
        if (w <= 0) {
            ESP_LOGE(TAG, "http write(pcm) failed at %u/%u: %d",
                     (unsigned)off, (unsigned)job->byte_len, w);
            goto out_close;
        }
        off += (size_t)w;
    }
    const int64_t t_upload = esp_timer_get_time();

    /* Read response headers */
    int headers_len = esp_http_client_fetch_headers(c);
    (void)headers_len;
    int status = esp_http_client_get_status_code(c);
    if (status != 200) {
        ESP_LOGW(TAG, "peregrine returned HTTP %d", status);
        goto out_close;
    }

    /* Stream body → speaker. The leading 44-byte WAV header is captured
     * (not blindly dropped): its sample-rate field drives the playback
     * clock, because the server resamples the reply to ITS configured
     * PEREGRINE_VOICE_TARGET_SR — which may differ from our mic rate.
     * Playing 22.05 kHz Piper output at 16 kHz pitched the voice down
     * ~4 semitones ("male voice" bug, 2026-08-03). */
    const int64_t t_first_body = esp_timer_get_time();
    (void)t_first_body;

    static uint8_t chunk[RESP_CHUNK_BYTES];
    uint8_t resp_hdr[WAV_HDR_LEN];
    size_t body_seen = 0;
    size_t hdr_got = 0;
    bool   playing = false;
    while (1) {
        int n = esp_http_client_read(c, (char *)chunk, sizeof(chunk));
        if (n < 0) {
            ESP_LOGW(TAG, "http read err %d — treating as end", n);
            break;
        }
        if (n == 0) break;
        body_seen += (size_t)n;

        size_t off_in = 0;
        if (hdr_got < WAV_HDR_LEN) {
            size_t need = WAV_HDR_LEN - hdr_got;
            size_t take = ((size_t)n < need) ? (size_t)n : need;
            memcpy(resp_hdr + hdr_got, chunk, take);
            hdr_got += take;
            off_in   = take;
            if (hdr_got == WAV_HDR_LEN) {
                /* fmt chunk sample rate: LE u32 at offset 24. */
                uint32_t rate = (uint32_t)resp_hdr[24]
                              | ((uint32_t)resp_hdr[25] << 8)
                              | ((uint32_t)resp_hdr[26] << 16)
                              | ((uint32_t)resp_hdr[27] << 24);
                ESP_LOGI(TAG, "reply WAV rate %lu Hz", (unsigned long)rate);
                if (!audio_play_stream_start(rate)) {
                    ESP_LOGE(TAG, "audio_play_stream_start failed");
                    break;
                }
                playing = true;
                s_state = PRG_VOICE_PLAYING;
            }
            if ((size_t)n <= take) continue;
        }
        if (playing) {
            audio_play_stream_write(chunk + off_in, (size_t)n - off_in);
        }
    }
    if (playing) audio_play_stream_end();

    const int64_t t_end = esp_timer_get_time();
    ESP_LOGI(TAG, "timing (ms): open=%lld upload=%lld total=%lld body=%u",
             (t_open   - t_start) / 1000,
             (t_upload - t_open ) / 1000,
             (t_end    - t_start) / 1000,
             (unsigned)body_seen);

out_close:
    esp_http_client_close(c);
    esp_http_client_cleanup(c);
    s_state = PRG_VOICE_IDLE;
}

static void worker_task(void *pv) {
    (void)pv;
    prg_job_t job;
    while (1) {
        if (xQueueReceive(s_job_q, &job, portMAX_DELAY) == pdTRUE) {
            run_job(&job);
        }
    }
}

void peregrine_voice_init(void) {
    if (s_worker) return;
    s_job_q = xQueueCreate(1, sizeof(prg_job_t));
    if (!s_job_q) {
        ESP_LOGE(TAG, "job queue create failed");
        return;
    }
    /* Task stack sized generously — the HTTP + TLS + audio stream path
     * routinely uses 3–4 KB. Pinned to core 0 (WiFi/MQTT core) so the
     * LVGL task on core 1 stays smooth during upload. */
    if (xTaskCreatePinnedToCore(worker_task, "prg_voice", 8192,
                                NULL, 5, &s_worker, 0) != pdPASS) {
        ESP_LOGE(TAG, "worker task create failed");
        return;
    }
    ESP_LOGI(TAG, "ready — url=%s token=%s",
             s_url,
             s_token[0] ? "set" : "(empty — capture-only)");
}

void peregrine_voice_set_url(const char *url) {
    if (!url || !*url) {
        strlcpy(s_url, CONFIG_PEREGRINE_URL, sizeof(s_url));
    } else {
        strlcpy(s_url, url, sizeof(s_url));
    }
    ESP_LOGI(TAG, "url set to %s", s_url);
}

void peregrine_voice_set_token(const char *token) {
    if (!token) token = "";
    strlcpy(s_token, token, sizeof(s_token));
    ESP_LOGI(TAG, "token %s",
             s_token[0] ? "set" : "cleared (capture-only)");
}

void peregrine_voice_press(void) {
    if (s_state != PRG_VOICE_IDLE && s_state != PRG_VOICE_ERROR) {
        ESP_LOGW(TAG, "press ignored — state=%d", (int)s_state);
        return;
    }
    if (!audio_mic_capture_start()) {
        ESP_LOGE(TAG, "mic capture start failed");
        s_state = PRG_VOICE_ERROR;
        return;
    }
    s_state = PRG_VOICE_LISTENING;
}

void peregrine_voice_release(void) {
    if (s_state != PRG_VOICE_LISTENING) {
        /* PRESS_LOST might fire without a prior PRESSED if the touch driver
         * gets confused — nothing to do. */
        return;
    }
    const int16_t *pcm = NULL;
    size_t bytes = 0;
    audio_mic_capture_stop(&pcm, &bytes);

    prg_job_t job = { .pcm = pcm, .byte_len = bytes };
    /* Non-blocking send. If the worker is still busy with a prior job the
     * new one is dropped rather than queued — voice UX is "one at a time." */
    if (xQueueSend(s_job_q, &job, 0) != pdTRUE) {
        ESP_LOGW(TAG, "worker busy — dropping capture");
        s_state = PRG_VOICE_IDLE;
    }
}

peregrine_voice_state_t peregrine_voice_get_state(void) {
    return s_state;
}
