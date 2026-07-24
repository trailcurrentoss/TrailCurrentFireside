#ifndef _FIRESIDE_BATTERY_H_
#define _FIRESIDE_BATTERY_H_

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* On-board Elecrow battery — read via the STC8H1KXX auxiliary MCU on I2C
 * (address 0x2F, reg 0x00 = battery info block). See
 *   CrowPanel-Advanced-10.1inch-ESP32-P4-HMI-AI-Display-1024x600-IPS-Touch-Screen/
 *   factory_sourcecode/V1.0/.../components/espressif__esp32_p4_function_ev_board/
 *   bsp_stc8h1kxx.[ch]
 * for the vendor reference implementation this port is derived from. */

/* stc8_bat_state values (from STC8H1KXX firmware). */
#define BATTERY_STATE_IDLE            0
#define BATTERY_STATE_CHARGING        1
#define BATTERY_STATE_FULLY_CHARGED   2
#define BATTERY_STATE_NO_CHARGE       3
#define BATTERY_STATE_ERROR           4

typedef struct {
    uint32_t adc_voltage_mv;   /* raw ADC reading                       */
    uint32_t bat_voltage_mv;   /* voltage after divider correction      */
    uint8_t  bat_level_pct;    /* 0..100                                */
    uint8_t  bat_state;        /* BATTERY_STATE_*                       */
    uint8_t  led_state;        /* on-board CHG LED state                */
} battery_info_t;

/* Register the I2C device handle for STC8H1KXX. Requires i2c_init() to have
 * been called first. Returns ESP_OK on success. */
esp_err_t battery_init(void);

/* One-shot read of the battery info block. Returns ESP_OK and populates
 * *out on success. Callers can check out->bat_state to disambiguate
 * charging / discharging / no-battery-attached. */
esp_err_t battery_read(battery_info_t *out);

/* Kick off a periodic (10 s) LVGL timer that reads the battery and pushes
 * the values through set_var_internal_battery_soc / _charging into the
 * TopBar labels. Must be called from an LVGL-safe context (i.e. under
 * lvgl_port_lock or before scheduling). */
void init_battery_poll(void);

#ifdef __cplusplus
}
#endif

#endif /* _FIRESIDE_BATTERY_H_ */
