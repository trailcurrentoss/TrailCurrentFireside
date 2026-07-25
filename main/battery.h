#ifndef _FIRESIDE_BATTERY_H_
#define _FIRESIDE_BATTERY_H_

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Waveshare ESP32-P4-WIFI6-Touch-LCD-7B — battery telemetry.
 *
 * Battery voltage is read via a passive voltage-divider into the ESP32-P4
 * ADC:
 *
 *     V_bat  ── 200 kΩ ──┬── 100 kΩ ── GND
 *                        │
 *                     GPIO 20 (ADC)
 *
 *     V_bat = V_adc * (R92 + R93) / R93 = V_adc * 3.0
 *
 * There is no dedicated STAT / CHG pin, so charging is inferred from a
 * rolling voltage trend combined with a near-full-voltage latch (see
 * detect_charging in battery.c).
 *
 * battery_info_t's fields are populated from the ADC path — the fields
 * that don't map to an ADC-derived value (led_state) are set to zero.
 */

/* Battery state values — derived from the trend detector
 * (see detect_charging in battery.c). */
#define BATTERY_STATE_IDLE            0
#define BATTERY_STATE_CHARGING        1
#define BATTERY_STATE_FULLY_CHARGED   2
#define BATTERY_STATE_NO_CHARGE       3
#define BATTERY_STATE_ERROR           4

typedef struct {
    uint32_t adc_voltage_mv;   /* raw ADC pin voltage (after cal)          */
    uint32_t bat_voltage_mv;   /* V_bat = V_adc * DIVIDER_NUM              */
    uint8_t  bat_level_pct;    /* 0..100 — from OCV lookup on bat_voltage  */
    uint8_t  bat_state;        /* BATTERY_STATE_* — from trend + threshold */
    uint8_t  led_state;        /* unused on Waveshare (always 0)           */
} battery_info_t;

/* Register the ADC oneshot unit + channel for GPIO 20. Attempts to
 * install curve-fitting calibration; falls back to a raw ratio if the
 * chip doesn't support it. Returns ESP_OK on success, ESP_FAIL if
 * GPIO 20 has no ADC channel on this target. */
esp_err_t battery_init(void);

/* One-shot read of the battery info block: samples the ADC, averages,
 * converts to mV, populates all fields of *out. Safe to call from any
 * task. */
esp_err_t battery_read(battery_info_t *out);

/* Kick off the periodic poll: dedicated FreeRTOS task at ~1 Hz feeds a
 * median → OCV → EMA pipeline; an LVGL timer pushes the smoothed value
 * into set_var_internal_battery_soc / _voltage / _charging so the TopBar
 * updates on every page instance. Must be called from an LVGL-safe
 * context (i.e. under lvgl_port_lock or before scheduling). */
void init_battery_poll(void);

#ifdef __cplusplus
}
#endif

#endif /* _FIRESIDE_BATTERY_H_ */
