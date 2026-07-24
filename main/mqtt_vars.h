#ifndef MQTT_VARS_H
#define MQTT_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * MQTT-fed variable setters. Called from components/mqtt_client's
 * process_message() as JSON payloads land. Every setter takes the display
 * lock internally where it touches LVGL widgets (except when the caller
 * already holds it — mqtt_client acquires per message before dispatching).
 *
 * Rolling 60-min ring buffers: solar/soc/volts/load (Power) and
 * temp/hum/eco2/tvoc/co (Air) each keep the last 60 samples in a ring
 * accessible via metric_history_last() so the sparkline canvases can
 * redraw on demand.
 */

/* Ring-buffer capacity. 24 samples × 2.5 min per sample = 60-minute
 * rolling window. Actual publish cadence from Ampline/Borealis is
 * 1–2 s, so we downsample per-bucket using a last-write-wins strategy
 * keyed on esp_timer time (150 s buckets). Charts step forward by
 * exactly one bar every 2.5 min — see metric_push() in vars.c. */
#define METRIC_HISTORY_LEN     24
#define METRIC_BUCKET_US       (150LL * 1000000LL)  /* 2.5 minutes */

typedef enum {
    METRIC_SOLAR = 0,   /* W */
    METRIC_SOC,         /* % */
    METRIC_VOLTS,       /* V (×100 → int16 in ring, /100 on read) */
    METRIC_LOAD,        /* W */
    METRIC_TEMP,        /* °F stored, converts on display */
    METRIC_HUM,         /* % */
    METRIC_ECO2,        /* ppm */
    METRIC_TVOC,        /* ppb */
    METRIC_CO,          /* ppm (×10 → int16 in ring) */
    METRIC_COUNT
} metric_id_t;

/* Fill `out` (size METRIC_HISTORY_LEN) with the last N samples in
 * chronological order (oldest → newest). Missing samples come back as
 * METRIC_MISSING. Returns the count of populated samples. */
#define METRIC_MISSING INT16_MIN
int  metric_history_get(metric_id_t id, int16_t *out);

/* Latest sample. Returns METRIC_MISSING if never seen. */
int16_t metric_last(metric_id_t id);

/* Temperature unit: 0=Fahrenheit, 1=Celsius */
int32_t get_var_temperature_unit(void);
void    set_var_temperature_unit(int32_t value);

/* Device light status (0=off, >0=on with brightness value) */
void set_var_device01_status(int32_t value);
void set_var_device02_status(int32_t value);
void set_var_device03_status(int32_t value);
void set_var_device04_status(int32_t value);
void set_var_device05_status(int32_t value);
void set_var_device06_status(int32_t value);
void set_var_device07_status(int32_t value);
void set_var_device08_status(int32_t value);
int32_t get_var_device_brightness(int32_t device_id);

/* Currently-adjusted device (used by mqtt_client to skip echoing updates
 * that would fight with the user's slider). */
int32_t get_var_current_device_brightness_identifier(void);
void    set_var_current_device_brightness_identifier(int32_t value);

/* Battery / energy — feeds the Power screen sparklines. */
void set_var_battery_soc(int32_t percent);
void set_var_battery_voltage(float volts);
void set_var_solar_watts(int32_t watts);
void set_var_solar_status(const char *status);
void set_var_consumption_watts(int32_t watts);
void set_var_time_remaining(int32_t minutes);
void set_var_internal_battery_soc(int32_t percent);
void set_var_internal_battery_voltage(float volts);
void set_var_internal_charging(bool charging);

/* GPS / GNSS — feeds the Trailer screen GNSS panel. */
void set_var_latitude(float lat);
void set_var_longitude(float lon);
void set_var_altitude(float feet);
void set_var_speed(float knots);
void set_var_course(float degrees);
void set_var_gnss_mode(const char *mode);
void set_var_satellite_count(int32_t value);
void set_var_gps_time(int year, int month, int day, int hour, int minute, int second);

/* Air quality — Borealis publishes → Air screen sparklines. */
void set_var_humidity(float percent);
void set_var_co2(int32_t ppm);
void set_var_tvoc(int32_t ppb);
void set_var_current_interior_temperature(int32_t value);
void set_var_current_exterior_temperature(int32_t value);
void set_var_co(int32_t ppm);   /* CO ppm — feeds air CO card + chart */
void set_var_co_flags(bool warn, bool alarm);  /* Borealis safety byte flags */

/* Water tank levels (0-100 %) */
void set_var_water_levels(int32_t fresh, int32_t grey, int32_t black);

/* Connection status → topbar wifi/mqtt state chips. */
void set_var_mqtt_connected(bool connected);
void set_var_wifi_rssi(int32_t rssi_dbm);

/* Per-module watchdogs (called from mqtt_client_check_watchdogs()). */
void clear_var_energy(void);
void clear_var_airquality(void);
void clear_var_gps(void);
void clear_var_water(void);

/* Rotation / MAC / clock — main.c calls these once. */
void set_var_rotation_degrees(int32_t value);
int32_t get_var_rotation_degrees(void);
void set_var_mcu_mac_address(const char *value);
const char *get_var_mcu_mac_address(void);

/* Settings state — persisted in NVS. */
int32_t get_var_screen_brightness(void);
void    set_var_screen_brightness(int32_t value);
int32_t get_var_screen_timeout_minutes(void);
void    set_var_screen_timeout_minutes(int32_t value);

/* Load persisted user settings (theme / brightness / timeout / tz / temp
 * unit) from NVS and apply to the UI. Called once by main during boot after
 * ui_init(). */
void restore_user_settings(void);

/* Clock label update — called by main loop at 1 Hz with display lock held. */
void update_clock_display(void);

/* Start the 500ms LVGL timer that blinks the Home page separator dots.
 * Call once, under the display lock, after ui_init(). */
void init_clock_blink(void);

/* Create an lv_chart child in every authored `<page>_<metric>_chart`
 * panel. Call once, under the display lock, after ui_init(). The chart
 * receives shift-mode updates whenever metric_push() fires (from any
 * setter — MQTT or local sensor) so history keeps building even when
 * the user is on a different screen. */
void init_metric_charts(void);

/* Reset all widgets that carry author-time placeholders (currently: the
 * three water tank bars, which are authored at 50 % fill so the vertical
 * gradient is visible in EEZ Studio's canvas). Call once under the
 * display lock, after ui_init(). */
void reset_placeholders(void);

/* Start the LVGL timer that reads WiFi RSSI from the driver every 5s and
 * paints it to every topbar. Call once, under the display lock, after
 * ui_init() AND after wifi_setup_init() has brought the station up. */
void init_wifi_rssi_poll(void);

/* Start the LVGL timer that watches lv_disp_get_inactive_time() and blanks
 * the backlight after get_var_screen_timeout_minutes() of inactivity, then
 * restores it to get_var_screen_brightness() as soon as the user touches
 * the screen again. Timeout value 0 = never blank. Call once, under the
 * display lock, after ui_init(). */
void init_screen_timeout(void);

/* Enable click flag + acknowledge-alarms callback on the topbar bell +
 * badge across all six pages. Call once, under the display lock, after
 * ui_init(). Silences re-alerts for a snooze window; does not clear the
 * badge count (the badge follows the underlying alarm-active state). */
void init_notif_icon_ack_taps(void);

#ifdef __cplusplus
}
#endif

#endif /* MQTT_VARS_H */
