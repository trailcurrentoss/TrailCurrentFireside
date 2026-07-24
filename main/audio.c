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
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static const char *TAG = "AUDIO";

#define AUDIO_PWR_GPIO      GPIO_NUM_30
#define I2S_LRCLK_GPIO      GPIO_NUM_21
#define I2S_BCLK_GPIO       GPIO_NUM_22
#define I2S_SDATA_GPIO      GPIO_NUM_23
#define SAMPLE_RATE         22050

static i2s_chan_handle_t s_tx_chan = NULL;
static QueueHandle_t     s_req_q   = NULL;
static bool              s_ready   = false;
static uint8_t           s_volume_pct = 100;   /* 0..100 */

static void play_asset(const audio_asset_t *a) {
    if (!s_tx_chan || !a || !a->pcm || a->samples == 0) return;
    if (s_volume_pct == 0) return;   /* muted */

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

    /* I2S TX — standard Philips mode, 16-bit mono @ 22.05 kHz. */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
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
