/*
 * audio.c — I2S TTS playback for CrowPanel Advance 10.1" ESP32-P4.
 *
 * Bring-up + task pattern:
 *   audio_init()          — power-enable GPIO + I2S TX channel + task
 *   audio_play_phrase(id) — enqueue; task drains the queue
 *
 * PCM assets (16-bit LE, mono, 22.05 kHz) come from audio_assets.c,
 * generated via tmp/gen_tts_assets.py using ChatterboxTTS through the
 * local ComfyUI instance.
 */

#include "audio.h"
#include "audio_assets.h"

#include <string.h>

#include "driver/i2s_std.h"
#include "driver/i2s_pdm.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

static const char *TAG = "AUDIO";

#define AUDIO_PWR_GPIO      GPIO_NUM_30
#define I2S_LRCLK_GPIO      GPIO_NUM_21
#define I2S_BCLK_GPIO       GPIO_NUM_22
#define I2S_SDATA_GPIO      GPIO_NUM_23
#define SAMPLE_RATE         22050

/* On-board PDM microphone (vendor Lesson11 bsp_mic values). */
#define MIC_GPIO_CLK        GPIO_NUM_24
#define MIC_GPIO_DIN        GPIO_NUM_26

/* Speaker pinned to I2S1 (matches the vendor bsp_audio); the PDM mic
 * needs I2S0 — PDM RX only exists on controller 0. */
#define SPK_I2S_PORT        I2S_NUM_1
#define MIC_I2S_PORT        I2S_NUM_0

static i2s_chan_handle_t s_tx_chan = NULL;
static QueueHandle_t     s_req_q   = NULL;
static bool              s_ready   = false;
static SemaphoreHandle_t s_i2s_mtx = NULL;    /* serializes phrase playback
                                               * vs stream clock switches */
static volatile bool     s_stream_active = false;
/* 30% initial volume: the class-D amp on the CrowPanel P4 clips visibly
 * on the TTS phrases when driven at 100% — the samples are pre-normalized
 * to near-full-scale so any additional gain-through drives the amp into
 * distortion. 30% leaves headroom and matches a comfortable listening
 * level in a quiet cabin; user can raise via Settings if they want more. */
static uint8_t           s_volume_pct = 30;   /* 0..100 */

static void play_asset(const audio_asset_t *a) {
    if (!s_tx_chan || !a || !a->pcm || a->samples == 0) return;
    if (s_volume_pct == 0) return;   /* muted */

    /* A streamed Peregrine reply owns the speaker (and has the I2S clock
     * at 16 kHz) — drop the phrase rather than fight over the DAC. */
    if (s_stream_active) return;
    xSemaphoreTake(s_i2s_mtx, portMAX_DELAY);
    if (s_stream_active) {           /* re-check after acquiring */
        xSemaphoreGive(s_i2s_mtx);
        return;
    }

    /* Enable codec power BEFORE writing samples, then wait a moment for
     * the amp to stabilize (avoids clipped attack on the first sample). */
    gpio_set_level(AUDIO_PWR_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(20));

    /* Apply volume gain if not full. At 100 % we just write the raw PCM.
     * At <100 % we scale into a scratch buffer to avoid mutating the
     * const asset. Scale is done as int32 to prevent int16 wraparound on
     * intermediate; then clamped. */
    size_t bytes_written;
    uint32_t timeout_ms = (uint32_t)((a->samples * 2000ULL) / SAMPLE_RATE) + 500;
    if (s_volume_pct >= 100) {
        i2s_channel_write(s_tx_chan, a->pcm, a->samples * sizeof(int16_t),
                          &bytes_written, pdMS_TO_TICKS(timeout_ms));
    } else {
        int16_t *scaled = (int16_t *)malloc(a->samples * sizeof(int16_t));
        if (scaled) {
            int32_t g = (int32_t)s_volume_pct;   /* 0..99 */
            for (size_t i = 0; i < a->samples; i++) {
                int32_t v = ((int32_t)a->pcm[i] * g) / 100;
                if (v >  32767) v =  32767;
                if (v < -32768) v = -32768;
                scaled[i] = (int16_t)v;
            }
            i2s_channel_write(s_tx_chan, scaled,
                              a->samples * sizeof(int16_t),
                              &bytes_written, pdMS_TO_TICKS(timeout_ms));
            free(scaled);
        }
    }

    /* Drain the DMA with a silence pad. Without this the codec keeps
     * clocking on whatever was last in the DMA ring — the CrowPanel's
     * class-D amp then squeals a high-pitched repeating artifact. Match
     * the reference example's pattern: silence pad → disable power. */
    static const int16_t silence[SAMPLE_RATE / 10] = {0};  /* 100 ms */
    i2s_channel_write(s_tx_chan, silence, sizeof(silence),
                      &bytes_written, pdMS_TO_TICKS(300));

    /* Kill codec power so nothing at all is amplified when idle. */
    gpio_set_level(AUDIO_PWR_GPIO, 1);
    xSemaphoreGive(s_i2s_mtx);
}

static void audio_task(void *pv) {
    (void)pv;
    audio_phrase_t req;
    while (1) {
        if (xQueueReceive(s_req_q, &req, portMAX_DELAY) == pdTRUE) {
            if (!s_ready) continue;
            if (req >= AUDIO_PHRASE_COUNT) continue;
            play_asset(&audio_assets[req]);
        }
    }
}

void audio_init(void) {
    /* Power-enable GPIO (active LOW). Drive low BEFORE I2S so the codec
     * is powered when we start pushing samples. */
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << AUDIO_PWR_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io);
    /* Start with the codec DISABLED (HIGH). play_asset re-enables it
     * only for the duration of each playback, then disables again — this
     * is what stops the class-D amp from squealing on stale DMA content. */
    gpio_set_level(AUDIO_PWR_GPIO, 1);

    s_i2s_mtx = xSemaphoreCreateMutex();
    if (!s_i2s_mtx) {
        ESP_LOGE(TAG, "i2s mutex create failed");
        return;
    }

    /* I2S TX — standard Philips mode, 16-bit mono @ 22.05 kHz. Explicit
     * I2S1: the PDM mic must have I2S0. */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(SPK_I2S_PORT, I2S_ROLE_MASTER);
    if (i2s_new_channel(&chan_cfg, &s_tx_chan, NULL) != ESP_OK) {
        ESP_LOGE(TAG, "i2s_new_channel failed");
        return;
    }

    i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_BCLK_GPIO,
            .ws   = I2S_LRCLK_GPIO,
            .dout = I2S_SDATA_GPIO,
            .din  = I2S_GPIO_UNUSED,
            .invert_flags = { .mclk_inv = false, .bclk_inv = false, .ws_inv = false },
        },
    };
    if (i2s_channel_init_std_mode(s_tx_chan, &std_cfg) != ESP_OK) {
        ESP_LOGE(TAG, "i2s_channel_init_std_mode failed");
        return;
    }
    if (i2s_channel_enable(s_tx_chan) != ESP_OK) {
        ESP_LOGE(TAG, "i2s_channel_enable failed");
        return;
    }

    s_req_q = xQueueCreate(4, sizeof(audio_phrase_t));
    if (!s_req_q) return;
    if (xTaskCreate(audio_task, "audio", 4096, NULL, 5, NULL) != pdPASS) {
        ESP_LOGE(TAG, "xTaskCreate(audio) failed");
        return;
    }
    s_ready = true;
    ESP_LOGI(TAG, "audio ready — I2S @ %u Hz, %d phrases loaded",
             (unsigned)SAMPLE_RATE, (int)AUDIO_PHRASE_COUNT);
}

void audio_play_phrase(audio_phrase_t id) {
    if (!s_ready || !s_req_q) return;
    if (id >= AUDIO_PHRASE_COUNT) return;
    xQueueSend(s_req_q, &id, 0);
}

void audio_set_volume(uint8_t pct) {
    if (pct > 100) pct = 100;
    s_volume_pct = pct;
}
uint8_t audio_get_volume(void) { return s_volume_pct; }

/* ======================================================================= */
/* Microphone capture — on-board PDM mic, I2S0                             */
/* ======================================================================= */

/* Software gain on top of the PDM path (hardware amplify_num stays 1).
 * 4× matches what the Waveshare build needed to put a normal speaking
 * voice at usable levels; tune here if Whisper reports low confidence. */
#define MIC_SW_GAIN 4

/* Hard cap on a single push-to-talk utterance. */
#define MIC_MAX_SECONDS 10
#define MIC_MAX_BYTES   ((size_t)AUDIO_MIC_SAMPLE_RATE * 2 * MIC_MAX_SECONDS)

/* 100 ms per read keeps the capture task cheap. */
#define MIC_READ_CHUNK_BYTES 3200

static i2s_chan_handle_t s_mic_chan = NULL;
static SemaphoreHandle_t s_mic_mtx  = NULL;

static int16_t     *s_mic_buf      = NULL;   /* PSRAM PCM buffer */
static size_t       s_mic_buf_cap  = 0;
static size_t       s_mic_buf_len  = 0;      /* bytes written */
static TaskHandle_t s_mic_task     = NULL;
static volatile bool s_mic_stop_req = false;

bool audio_mic_init(void) {
    if (s_mic_chan) return true;
    s_mic_mtx = xSemaphoreCreateMutex();
    if (!s_mic_mtx) {
        ESP_LOGE(TAG, "mic mutex create failed");
        return false;
    }

    /* Vendor Lesson11 bsp_mic config: PDM RX on I2S0, 16 kHz mono 16-bit,
     * 8x down-sampling, hardware high-pass at 35.5 Hz. */
    i2s_chan_config_t rx_cfg = {
        .id = MIC_I2S_PORT,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 6,
        .dma_frame_num = 256,
        .auto_clear_after_cb = true,
        .auto_clear_before_cb = true,
        .allow_pd = false,
        .intr_priority = 0,
    };
    esp_err_t err = i2s_new_channel(&rx_cfg, NULL, &s_mic_chan);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mic i2s_new_channel: %s", esp_err_to_name(err));
        s_mic_chan = NULL;
        return false;
    }

    i2s_pdm_rx_config_t pdm_cfg = {
        .clk_cfg = {
            .sample_rate_hz = AUDIO_MIC_SAMPLE_RATE,
            .clk_src = I2S_CLK_SRC_DEFAULT,
            .mclk_multiple = I2S_MCLK_MULTIPLE_256,
            .dn_sample_mode = I2S_PDM_DSR_8S,
            .bclk_div = 8,
        },
        .slot_cfg = {
            .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
            .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO,
            .slot_mode = I2S_SLOT_MODE_MONO,
            .slot_mask = I2S_PDM_SLOT_LEFT,
            .hp_en = true,
            .hp_cut_off_freq_hz = 35.5,
            .amplify_num = 1,
        },
        .gpio_cfg = {
            .clk = MIC_GPIO_CLK,
            .din = MIC_GPIO_DIN,
            .invert_flags = { .clk_inv = false },
        },
    };
    err = i2s_channel_init_pdm_rx_mode(s_mic_chan, &pdm_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "mic pdm_rx init: %s", esp_err_to_name(err));
        i2s_del_channel(s_mic_chan);
        s_mic_chan = NULL;
        return false;
    }
    ESP_LOGI(TAG, "mic ready — PDM @ %u Hz (CLK %d, DIN %d)",
             (unsigned)AUDIO_MIC_SAMPLE_RATE, MIC_GPIO_CLK, MIC_GPIO_DIN);
    return true;
}

static void mic_capture_task(void *pv) {
    (void)pv;
    /* Static so the task stack stays small. */
    static uint8_t chunk[MIC_READ_CHUNK_BYTES];

    while (!s_mic_stop_req) {
        size_t got = 0;
        esp_err_t err = i2s_channel_read(s_mic_chan, chunk, sizeof(chunk),
                                         &got, pdMS_TO_TICKS(500));
        if (err != ESP_OK || got == 0) {
            if (!s_mic_stop_req) {
                ESP_LOGW(TAG, "i2s_channel_read: %s", esp_err_to_name(err));
                vTaskDelay(pdMS_TO_TICKS(20));
            }
            continue;
        }
        int16_t *s = (int16_t *)chunk;
        size_t   n = got / 2;
        for (size_t i = 0; i < n; i++) {
            int32_t v = (int32_t)s[i] * MIC_SW_GAIN;
            if (v >  32767) v =  32767;
            if (v < -32768) v = -32768;
            s[i] = (int16_t)v;
        }

        xSemaphoreTake(s_mic_mtx, portMAX_DELAY);
        size_t room = s_mic_buf_cap - s_mic_buf_len;
        size_t take = (got < room) ? got : room;
        if (take > 0) {
            memcpy((uint8_t *)s_mic_buf + s_mic_buf_len, chunk, take);
            s_mic_buf_len += take;
        }
        xSemaphoreGive(s_mic_mtx);

        if (take == 0) {
            ESP_LOGW(TAG, "mic capture hit %d-second cap", MIC_MAX_SECONDS);
            break;
        }
    }
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
        s_mic_buf = heap_caps_malloc(MIC_MAX_BYTES,
                                     MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        s_mic_buf_cap = s_mic_buf ? MIC_MAX_BYTES : 0;
    }
    s_mic_buf_len = 0;
    xSemaphoreGive(s_mic_mtx);
    if (!s_mic_buf) {
        ESP_LOGE(TAG, "mic buffer alloc failed (%u bytes PSRAM)",
                 (unsigned)MIC_MAX_BYTES);
        return false;
    }

    if (i2s_channel_enable(s_mic_chan) != ESP_OK) {
        ESP_LOGE(TAG, "mic channel enable failed");
        return false;
    }

    s_mic_stop_req = false;
    /* Pinned to core 1 to stay off the WiFi/MQTT core. */
    BaseType_t ok = xTaskCreatePinnedToCore(mic_capture_task, "mic_cap",
                                            4096, NULL, 6, &s_mic_task, 1);
    if (ok != pdPASS) {
        ESP_LOGE(TAG, "mic capture task create failed");
        i2s_channel_disable(s_mic_chan);
        s_mic_task = NULL;
        return false;
    }
    ESP_LOGI(TAG, "mic capture started");
    return true;
}

void audio_mic_capture_stop(const int16_t **out_pcm, size_t *out_bytes) {
    if (out_pcm)   *out_pcm   = NULL;
    if (out_bytes) *out_bytes = 0;
    if (!s_mic_chan) return;

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
    i2s_channel_disable(s_mic_chan);

    xSemaphoreTake(s_mic_mtx, portMAX_DELAY);
    if (out_pcm)   *out_pcm   = s_mic_buf;
    if (out_bytes) *out_bytes = s_mic_buf_len;
    xSemaphoreGive(s_mic_mtx);

    ESP_LOGI(TAG, "mic capture stopped — %u bytes (%.2fs)",
             (unsigned)s_mic_buf_len,
             (double)s_mic_buf_len / (AUDIO_MIC_SAMPLE_RATE * 2.0));
}

/* ======================================================================= */
/* Streaming playback — Peregrine /api/voice reply (16 kHz)                */
/* ======================================================================= */

/* Internal-SRAM scratch: I2S DMA source must not live in PSRAM. */
#define STREAM_SCRATCH_BYTES 4096
static int16_t s_stream_scratch[STREAM_SCRATCH_BYTES / 2];

static uint8_t s_stream_loudness_pct = 50;

static void spk_set_rate(uint32_t hz) {
    i2s_std_clk_config_t clk = I2S_STD_CLK_DEFAULT_CONFIG(hz);
    i2s_channel_disable(s_tx_chan);
    esp_err_t err = i2s_channel_reconfig_std_clock(s_tx_chan, &clk);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "spk clock reconfig %lu Hz: %s",
                 (unsigned long)hz, esp_err_to_name(err));
    }
    i2s_channel_enable(s_tx_chan);
}

bool audio_play_stream_start(uint32_t sample_rate_hz) {
    if (!s_ready || !s_tx_chan) return false;
    if (sample_rate_hz < 8000 || sample_rate_hz > 48000) {
        ESP_LOGW(TAG, "stream rate %lu Hz implausible — using %u",
                 (unsigned long)sample_rate_hz, (unsigned)AUDIO_MIC_SAMPLE_RATE);
        sample_rate_hz = AUDIO_MIC_SAMPLE_RATE;
    }
    /* Blocks until any in-flight alarm phrase finishes, then owns the
     * speaker: amp on, clock at the reply's declared rate. */
    xSemaphoreTake(s_i2s_mtx, portMAX_DELAY);
    s_stream_active = true;
    gpio_set_level(AUDIO_PWR_GPIO, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    spk_set_rate(sample_rate_hz);
    ESP_LOGI(TAG, "stream playback @ %lu Hz", (unsigned long)sample_rate_hz);
    xSemaphoreGive(s_i2s_mtx);
    return true;
}

bool audio_play_stream_write(const void *pcm, size_t byte_len) {
    if (!s_stream_active || !pcm || byte_len == 0) return false;
    /* Combined gain: alarm volume × per-stream loudness / 10000. */
    int32_t gain = (int32_t)s_volume_pct * (int32_t)s_stream_loudness_pct;
    if (gain <= 0) return true;      /* muted — drop the chunk silently */

    byte_len &= ~(size_t)1;          /* even-byte, int16-aligned */
    const int16_t *src = (const int16_t *)pcm;
    size_t remaining = byte_len / 2;
    const size_t scratch_samples = STREAM_SCRATCH_BYTES / 2;

    while (remaining > 0) {
        size_t take = (remaining < scratch_samples) ? remaining
                                                    : scratch_samples;
        for (size_t i = 0; i < take; i++) {
            int32_t v = ((int32_t)src[i] * gain) / 10000;
            if (v >  32767) v =  32767;
            if (v < -32768) v = -32768;
            s_stream_scratch[i] = (int16_t)v;
        }
        size_t written = 0;
        esp_err_t err = i2s_channel_write(s_tx_chan, s_stream_scratch,
                                          take * 2, &written,
                                          pdMS_TO_TICKS(2000));
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "stream i2s write: %s", esp_err_to_name(err));
            return false;
        }
        src       += take;
        remaining -= take;
    }
    return true;
}

void audio_play_stream_end(void) {
    if (!s_stream_active) return;
    xSemaphoreTake(s_i2s_mtx, portMAX_DELAY);
    /* Silence pad (same trick as play_asset — stale DMA content makes the
     * class-D amp squeal), then restore the phrase clock and power off. */
    static const int16_t silence[AUDIO_MIC_SAMPLE_RATE / 10] = {0};
    size_t written = 0;
    i2s_channel_write(s_tx_chan, silence, sizeof(silence), &written,
                      pdMS_TO_TICKS(300));
    spk_set_rate(SAMPLE_RATE);
    gpio_set_level(AUDIO_PWR_GPIO, 1);
    s_stream_active = false;
    xSemaphoreGive(s_i2s_mtx);
    ESP_LOGI(TAG, "stream reply ended");
}

void audio_set_stream_loudness_pct(uint8_t pct) {
    if (pct > 100) pct = 100;
    s_stream_loudness_pct = pct;
}
uint8_t audio_get_stream_loudness_pct(void) { return s_stream_loudness_pct; }
