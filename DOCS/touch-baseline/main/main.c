/*
 * Phase 0 touch baseline for the CrowPanel Advance 10.1" ESP32-P4.
 *
 * Purpose: decide whether the 20–140 px coordinate drift seen on the
 * Fireside firmware comes from the touch hardware (GT911 / glass /
 * power-supply noise) or from the Fireside software stack.
 *
 * The touch pipeline here is the vendor's own, byte-identical:
 * peripheral/bsp_i2c and peripheral/bsp_display are copied unmodified
 * from Elecrow's V1.2 idf-code Lesson05-Touchscreen. Only this file is
 * new — it polls get_coor() like the vendor demo does, and adds:
 *
 *   1. Per-press session statistics: start/end coordinates, total
 *      drift, largest single-sample jump, path length, sample count.
 *      One CSV "SUMMARY" line per press for offline analysis.
 *
 *   2. A backlight-PWM noise sweep. The vendor Lesson05 never lights
 *      the panel, so a clean result there would not rule out backlight
 *      PWM coupling into the touch glass. We replicate the vendor's
 *      exact backlight setup (GPIO31, LEDC, 30 kHz, 11-bit — from
 *      Lesson07 bsp_illuminate) and step OFF → 10% → 50% → 100% on a
 *      fixed schedule, tagging every summary line with the active
 *      stage. Drift that appears only at high duty = supply/PWM noise.
 *
 * The MIPI-DSI panel is intentionally NOT initialised: the screen
 * stays dark/white. This test is serial-console only.
 */

#include <math.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "bsp_i2c.h"
#include "bsp_display.h"

#define TAG "TOUCHTEST"

/* ---- Backlight sweep (vendor values from Lesson07 bsp_illuminate) --- */

#define BLIGHT_GPIO   31
#define BLIGHT_PWM_HZ 30000

/* Backlight POWER rail switch (schematic net LCD_BK_POWER, Q11 P-MOSFET
 * gate). Drive LOW to power the backlight boost converter — vendor's
 * MicroPython demo does this explicitly ("Enable screen power circuit");
 * the C lessons rely on the pin's default state. GPIO31 PWM alone does
 * nothing while this rail is off. */
#define BLIGHT_POWER_GPIO 29

#define STAGE_DURATION_S 30

static const uint32_t stage_brightness[] = { 0, 10, 50, 100 };
/* Stage screen colors (RGB565): dark blue / red / green / white — visible
 * cue for the operator of which stage is active. */
static const uint16_t stage_color[] = { 0x000F, 0xF800, 0x07E0, 0xFFFF };
#define NUM_STAGES (sizeof(stage_brightness) / sizeof(stage_brightness[0]))

/* display_min.c — vendor Lesson07 panel bring-up minus LVGL. */
extern esp_err_t display_min_init(void);
extern void display_min_fill(uint16_t rgb565);

static esp_err_t blight_init(void)
{
    const gpio_config_t gpio_cfg = {
        .pin_bit_mask = (1ULL << BLIGHT_GPIO) | (1ULL << BLIGHT_POWER_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = false,
        .pull_down_en = false,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t err = gpio_config(&gpio_cfg);
    if (err != ESP_OK) return err;
    gpio_set_level(BLIGHT_POWER_GPIO, 0);   /* rail ON (active low) */

    const ledc_timer_config_t timer_cfg = {
        .clk_cfg = LEDC_USE_PLL_DIV_CLK,
        .duty_resolution = LEDC_TIMER_11_BIT,
        .freq_hz = BLIGHT_PWM_HZ,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
    };
    const ledc_channel_config_t chan_cfg = {
        .gpio_num = BLIGHT_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    err = ledc_timer_config(&timer_cfg);
    if (err != ESP_OK) return err;
    return ledc_channel_config(&chan_cfg);
}

/* Same duty mapping as the vendor's set_lcd_blight(). */
static void blight_set(uint32_t brightness)
{
    uint32_t duty = (brightness != 0) ? (brightness * 18 + 200) : 0;
    esp_err_t e1 = ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    esp_err_t e2 = ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    ESP_LOGI(TAG, "backlight duty=%lu/2047 (set=%s update=%s)",
             (unsigned long)duty, esp_err_to_name(e1), esp_err_to_name(e2));
}

/* ---- Per-press session statistics ----------------------------------- */

typedef struct {
    bool     active;
    int64_t  t_start_us;
    uint16_t x0, y0;        /* first sample of the press           */
    uint16_t xp, yp;        /* previous sample                     */
    uint16_t x1, y1;        /* latest sample                       */
    uint32_t samples;
    float    path_len;      /* sum of per-sample step distances    */
    float    max_jump;      /* largest single-sample step          */
    uint16_t min_x, max_x, min_y, max_y;
} press_session_t;

static press_session_t s;
static uint32_t press_count = 0;

static float dist(int32_t dx, int32_t dy)
{
    return sqrtf((float)(dx * dx + dy * dy));
}

static void session_start(uint16_t x, uint16_t y, int64_t now_us)
{
    s.active = true;
    s.t_start_us = now_us;
    s.x0 = s.xp = s.x1 = x;
    s.y0 = s.yp = s.y1 = y;
    s.samples = 1;
    s.path_len = 0.0f;
    s.max_jump = 0.0f;
    s.min_x = s.max_x = x;
    s.min_y = s.max_y = y;
}

static void session_sample(uint16_t x, uint16_t y)
{
    float step = dist((int32_t)x - s.xp, (int32_t)y - s.yp);
    s.path_len += step;
    if (step > s.max_jump) s.max_jump = step;
    s.xp = s.x1 = x;
    s.yp = s.y1 = y;
    s.samples++;
    if (x < s.min_x) s.min_x = x;
    if (x > s.max_x) s.max_x = x;
    if (y < s.min_y) s.min_y = y;
    if (y > s.max_y) s.max_y = y;
}

static void session_end(int64_t now_us, uint32_t stage_pct)
{
    int32_t dx = (int32_t)s.x1 - (int32_t)s.x0;
    int32_t dy = (int32_t)s.y1 - (int32_t)s.y0;
    press_count++;
    /* One machine-parseable line per press. drift = straight-line
     * distance between first and last sample — the number the Fireside
     * firmware measured at 20–140 px. */
    ESP_LOGI(TAG,
        "SUMMARY,press=%lu,bl_pct=%lu,dur_ms=%lld,samples=%lu,"
        "x0=%u,y0=%u,x1=%u,y1=%u,dx=%ld,dy=%ld,drift=%.1f,"
        "maxjump=%.1f,path=%.1f,bbox=%ux%u",
        (unsigned long)press_count, (unsigned long)stage_pct,
        (long long)((now_us - s.t_start_us) / 1000),
        (unsigned long)s.samples,
        s.x0, s.y0, s.x1, s.y1, (long)dx, (long)dy,
        (double)dist(dx, dy), (double)s.max_jump, (double)s.path_len,
        s.max_x - s.min_x, s.max_y - s.min_y);
    s.active = false;
}

/* ---- Test loop ------------------------------------------------------ */

static void print_banner(uint32_t stage_pct)
{
    ESP_LOGW(TAG, "==============================================");
    ESP_LOGW(TAG, "  STAGE: BACKLIGHT %lu%%  (%d s)", (unsigned long)stage_pct,
             STAGE_DURATION_S);
    ESP_LOGW(TAG, "  Do: ~10 quick taps, 3 press-and-holds (~2 s),");
    ESP_LOGW(TAG, "      2 slow one-finger drags across the glass.");
    ESP_LOGW(TAG, "  Spread them around the screen.");
    ESP_LOGW(TAG, "==============================================");
}

static void touch_test_task(void *param)
{
    (void)param;
    int64_t stage_start_us = esp_timer_get_time();
    size_t  stage_idx = 0;

    display_min_fill(stage_color[stage_idx]);
    blight_set(stage_brightness[stage_idx]);
    print_banner(stage_brightness[stage_idx]);

    int64_t last_beat_us = 0;

    while (1) {
        int64_t now_us = esp_timer_get_time();

        /* Heartbeat proves the task is alive even when nothing is
         * touched — a silent log now means a hang, not idleness. */
        if (now_us - last_beat_us > 5000000) {
            last_beat_us = now_us;
            ESP_LOGI(TAG, "alive t=%llds stage=%lu%% presses=%lu",
                     (long long)(now_us / 1000000),
                     (unsigned long)stage_brightness[stage_idx],
                     (unsigned long)press_count);
        }

        /* Advance the backlight stage on schedule — but never in the
         * middle of a press, so each summary maps to exactly one stage. */
        if (!s.active &&
            now_us - stage_start_us > (int64_t)STAGE_DURATION_S * 1000000) {
            stage_idx = (stage_idx + 1) % NUM_STAGES;
            stage_start_us = now_us;
            display_min_fill(stage_color[stage_idx]);
            blight_set(stage_brightness[stage_idx]);
            print_banner(stage_brightness[stage_idx]);
        }

        if (touch_read() == ESP_OK) {
            uint16_t x, y;
            bool pressed;
            get_coor(&x, &y, &pressed);

            if (pressed && !s.active) {
                session_start(x, y, now_us);
            } else if (pressed) {
                session_sample(x, y);
            } else if (s.active) {
                session_end(now_us, stage_brightness[stage_idx]);
            }
        }

        /* 10 ms loop delay; touch_read() itself sleeps another 10 ms
         * (vendor code), so the effective sample period is ~20 ms. */
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Phase 0 touch baseline — vendor Lesson05 pipeline");
    ESP_LOGI(TAG, "Touch INT pin: GPIO%d (driver-configured, polled reads)",
             Touch_GPIO_INT);

    if (i2c_init() != ESP_OK) {
        ESP_LOGE(TAG, "I2C init failed");
        return;
    }
    /* Panel first — mirrors the vendor/Fireside sequence where the
     * backlight is demonstrably controllable. */
    esp_err_t derr = display_min_init();
    ESP_LOGI(TAG, "display init: %s", esp_err_to_name(derr));
    if (touch_init() != ESP_OK) {
        ESP_LOGE(TAG, "GT911 init failed");
        return;
    }
    if (blight_init() != ESP_OK) {
        ESP_LOGE(TAG, "Backlight LEDC init failed");
        return;
    }

    xTaskCreate(touch_test_task, "touch_test", 6144, NULL, 5, NULL);
    ESP_LOGI(TAG, "Running. Follow the stage banners.");
}
