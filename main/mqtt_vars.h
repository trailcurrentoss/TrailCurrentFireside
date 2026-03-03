#ifndef MQTT_VARS_H
#define MQTT_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Device light status (0=off, >0=on with brightness value) */
void set_var_device01_status(int32_t value);
void set_var_device02_status(int32_t value);
void set_var_device03_status(int32_t value);
void set_var_device04_status(int32_t value);
void set_var_device05_status(int32_t value);
void set_var_device06_status(int32_t value);
void set_var_device07_status(int32_t value);
void set_var_device08_status(int32_t value);

/* Battery / energy */
void set_var_battery_soc(int32_t percent);
void set_var_battery_voltage(float volts);
void set_var_solar_watts(int32_t watts);
void set_var_solar_status(const char *status);

/* GPS / GNSS */
void set_var_latitude(float lat);
void set_var_longitude(float lon);
void set_var_altitude(float feet);
void set_var_speed(float knots);
void set_var_course(float degrees);
void set_var_gnss_mode(const char *mode);

/* Air quality */
void set_var_humidity(float percent);

/* GPS time (UTC) */
void set_var_gps_time(int year, int month, int day, int hour, int minute, int second);

/* Clock display update (call periodically from main loop with display lock held) */
void update_clock_display(void);

/* Connection status */
void set_var_mqtt_connected(bool connected);


#ifdef __cplusplus
}
#endif

#endif /* MQTT_VARS_H */
