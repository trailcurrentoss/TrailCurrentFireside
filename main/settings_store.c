#include "settings_store.h"

#include <string.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "nvs.h"

static const char *TAG = "SETSTORE";

/* Flush entries this long after the last change. Long enough to coalesce
 * a whole slider drag into one flash write, short enough that a power cut
 * seconds after a change still keeps the value. */
#define FLUSH_DELAY_US (1 * 1000 * 1000)

#define MAX_ENTRIES 16
#define NS_MAX      16
#define KEY_MAX     16
#define STR_MAX     48

typedef enum { T_I32, T_U8, T_U16, T_STR, T_ERASE } val_type_t;

typedef struct {
    bool       used;
    bool       dirty;
    char       ns[NS_MAX];
    char       key[KEY_MAX];
    val_type_t type;
    union {
        int32_t  i32;
        uint8_t  u8;
        uint16_t u16;
        char     str[STR_MAX];
    } v;
} entry_t;

static entry_t           s_entries[MAX_ENTRIES];
static SemaphoreHandle_t s_mutex;
static SemaphoreHandle_t s_kick;
static int64_t           s_last_change_us;

static void set_common(const char *ns, const char *key, val_type_t type,
                       const void *val)
{
    if (!s_mutex) {           /* init not run — write synchronously */
        ESP_LOGW(TAG, "not initialized, dropping %s/%s", ns, key);
        return;
    }
    xSemaphoreTake(s_mutex, portMAX_DELAY);
    entry_t *slot = NULL;
    for (int i = 0; i < MAX_ENTRIES; i++) {
        entry_t *e = &s_entries[i];
        if (e->used && !strcmp(e->ns, ns) && !strcmp(e->key, key)) {
            slot = e;
            break;
        }
        if (!slot && !e->used) slot = e;
    }
    if (!slot) {
        ESP_LOGE(TAG, "table full, dropping %s/%s", ns, key);
    } else {
        slot->used = true;
        slot->dirty = true;
        strlcpy(slot->ns, ns, NS_MAX);
        strlcpy(slot->key, key, KEY_MAX);
        slot->type = type;
        switch (type) {
        case T_I32: slot->v.i32 = *(const int32_t *)val; break;
        case T_U8:  slot->v.u8 = *(const uint8_t *)val; break;
        case T_U16: slot->v.u16 = *(const uint16_t *)val; break;
        case T_STR: strlcpy(slot->v.str, (const char *)val, STR_MAX); break;
        case T_ERASE: break;
        }
        s_last_change_us = esp_timer_get_time();
    }
    xSemaphoreGive(s_mutex);
    xSemaphoreGive(s_kick);
}

void settings_store_set_i32(const char *ns, const char *key, int32_t v)
{
    set_common(ns, key, T_I32, &v);
}

void settings_store_set_u8(const char *ns, const char *key, uint8_t v)
{
    set_common(ns, key, T_U8, &v);
}

void settings_store_set_u16(const char *ns, const char *key, uint16_t v)
{
    set_common(ns, key, T_U16, &v);
}

void settings_store_erase(const char *ns, const char *key)
{
    set_common(ns, key, T_ERASE, NULL);
}

void settings_store_set_str(const char *ns, const char *key, const char *v)
{
    set_common(ns, key, T_STR, v);
}

void settings_store_drop_pending(void)
{
    if (!s_mutex) return;
    xSemaphoreTake(s_mutex, portMAX_DELAY);
    memset(s_entries, 0, sizeof(s_entries));
    xSemaphoreGive(s_mutex);
}

/* Write every dirty entry, one nvs handle per namespace. Runs on the
 * store task only. */
static void flush_dirty(void)
{
    for (;;) {
        /* Snapshot one dirty entry under the lock, write it outside. */
        entry_t snap;
        bool found = false;
        xSemaphoreTake(s_mutex, portMAX_DELAY);
        for (int i = 0; i < MAX_ENTRIES; i++) {
            if (s_entries[i].used && s_entries[i].dirty) {
                snap = s_entries[i];
                s_entries[i].dirty = false;
                found = true;
                break;
            }
        }
        xSemaphoreGive(s_mutex);
        if (!found) return;

        nvs_handle_t h;
        esp_err_t err = nvs_open(snap.ns, NVS_READWRITE, &h);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "nvs_open(%s): %s", snap.ns, esp_err_to_name(err));
            continue;
        }
        switch (snap.type) {
        case T_I32: err = nvs_set_i32(h, snap.key, snap.v.i32); break;
        case T_U8:  err = nvs_set_u8(h, snap.key, snap.v.u8); break;
        case T_U16: err = nvs_set_u16(h, snap.key, snap.v.u16); break;
        case T_STR: err = nvs_set_str(h, snap.key, snap.v.str); break;
        case T_ERASE:
            err = nvs_erase_key(h, snap.key);
            if (err == ESP_ERR_NVS_NOT_FOUND) err = ESP_OK;
            break;
        }
        if (err == ESP_OK) err = nvs_commit(h);
        nvs_close(h);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "persist %s/%s: %s", snap.ns, snap.key,
                     esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "persisted %s/%s", snap.ns, snap.key);
        }
    }
}

static void store_task(void *arg)
{
    (void)arg;
    for (;;) {
        xSemaphoreTake(s_kick, portMAX_DELAY);
        /* Debounce: wait until the last change is FLUSH_DELAY_US old.
         * New changes during the wait re-arm the delay. */
        for (;;) {
            int64_t age = esp_timer_get_time() - s_last_change_us;
            if (age >= FLUSH_DELAY_US) break;
            vTaskDelay(pdMS_TO_TICKS((FLUSH_DELAY_US - age) / 1000 + 10));
        }
        flush_dirty();
    }
}

void settings_store_init(void)
{
    if (s_mutex) return;
    s_mutex = xSemaphoreCreateMutex();
    s_kick = xSemaphoreCreateBinary();
    xTaskCreate(store_task, "settings_store", 3072, NULL, 3, NULL);
}
