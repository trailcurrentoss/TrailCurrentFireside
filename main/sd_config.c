/*
 * sd_config.c — see sd_config.h for the file-format contract.
 *
 * Ported from the Waveshare build; the only board-specific part is the
 * mount/unmount, which on the CrowPanel Advance 10.1" uses the vendor
 * Lesson08 SDMMC config: slot 0, 1-bit (CLK GPIO43, CMD GPIO44, D0
 * GPIO39, internal pull-ups), capped at 10 MHz.
 *
 * Called from app_main after peregrine_voice_init(), so the setters exist
 * before we try to call them. All I/O runs on the main task (stack sized
 * for boot-time NVS/FATFS work via CONFIG_ESP_MAIN_TASK_STACK_SIZE).
 */

#include "sd_config.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"

#include "peregrine_voice.h"

static const char *TAG = "SDCONF";

#define SD_MOUNT_POINT "/sdcard"
#define CONF_PATH SD_MOUNT_POINT "/environment.conf"
#define MAX_LINE  256

static sdmmc_card_t *s_card = NULL;

/* CrowPanel SD mount — vendor Lesson08 bsp_sd values. */
static esp_err_t sd_mount(void)
{
    if (s_card) return ESP_ERR_INVALID_STATE;   /* already mounted */

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.slot = SDMMC_HOST_SLOT_0;
    host.max_freq_khz = 10000;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.clk = GPIO_NUM_43;
    slot_config.cmd = GPIO_NUM_44;
    slot_config.d0 = GPIO_NUM_39;
    slot_config.width = 1;
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    return esp_vfs_fat_sdmmc_mount(SD_MOUNT_POINT, &host, &slot_config,
                                   &mount_config, &s_card);
}

static void sd_unmount(void)
{
    if (!s_card) return;
    esp_vfs_fat_sdcard_unmount(SD_MOUNT_POINT, s_card);
    s_card = NULL;
}

/* Set while parsing; read afterwards by sd_config_peregrine_present() so
 * the UI can hide the TALK button when the card carries no voice config. */
static bool s_peregrine_url   = false;
static bool s_peregrine_token = false;

/* Strip leading + trailing ASCII whitespace in place. Returns the trimmed
 * pointer (may be past the buffer start). */
static char *trim(char *s) {
    while (*s && isspace((unsigned char)*s)) s++;
    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1])) end--;
    *end = '\0';
    return s;
}

/* Dispatch a single (key, value) pair. Both are already trimmed. */
static void apply(const char *key, const char *val) {
    if (strcmp(key, "PEREGRINE_URL") == 0) {
        peregrine_voice_set_url(val);
        if (val[0] != '\0') s_peregrine_url = true;
    } else if (strcmp(key, "PEREGRINE_VOICE_TOKEN") == 0) {
        peregrine_voice_set_token(val);
        if (val[0] != '\0') s_peregrine_token = true;
    } else {
        /* Not an error — the same file is expected to carry other product
         * secrets over time (MQTT creds, Headwaters tokens, ...). */
        ESP_LOGI(TAG, "  (ignoring unknown key %s)", key);
    }
}

bool sd_config_peregrine_present(void) {
    return s_peregrine_url && s_peregrine_token;
}

esp_err_t sd_config_load(void) {
    /* Every early-return path below leaves these false, which is exactly
     * what we want: no card / no file == no Peregrine config. */
    s_peregrine_url   = false;
    s_peregrine_token = false;

    esp_err_t err = sd_mount();
    if (err == ESP_ERR_INVALID_STATE) {
        /* Already mounted by an earlier caller — that's fine, keep reading. */
        err = ESP_OK;
    }
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "SD not mounted (%s) — using compile-time defaults",
                 esp_err_to_name(err));
        return ESP_OK;
    }

    FILE *f = fopen(CONF_PATH, "r");
    if (!f) {
        ESP_LOGI(TAG, "%s not present — using compile-time defaults", CONF_PATH);
        sd_unmount();
        return ESP_OK;
    }
    ESP_LOGI(TAG, "reading %s", CONF_PATH);

    char line[MAX_LINE];
    int  applied = 0;
    while (fgets(line, sizeof(line), f)) {
        /* Drop CR/LF and anything after a bare '#' comment marker. */
        char *hash = strchr(line, '#');
        if (hash) *hash = '\0';

        char *trimmed = trim(line);
        if (*trimmed == '\0') continue;

        char *eq = strchr(trimmed, '=');
        if (!eq) {
            ESP_LOGW(TAG, "malformed line (no '='): %s", trimmed);
            continue;
        }
        *eq = '\0';
        char *key = trim(trimmed);
        char *val = trim(eq + 1);

        /* Strip surrounding quotes if present — makes it easy to hand-edit
         * values that contain spaces on future keys. */
        size_t vlen = strlen(val);
        if (vlen >= 2 &&
            ((val[0] == '"'  && val[vlen - 1] == '"') ||
             (val[0] == '\'' && val[vlen - 1] == '\''))) {
            val[vlen - 1] = '\0';
            val++;
        }
        apply(key, val);
        applied++;
    }
    fclose(f);
    ESP_LOGI(TAG, "applied %d key(s) from %s", applied, CONF_PATH);
    ESP_LOGI(TAG, "Peregrine voice config %s (url=%d token=%d)",
             sd_config_peregrine_present() ? "present" : "incomplete",
             (int)s_peregrine_url, (int)s_peregrine_token);

    /* Unmount so the FAT driver's cached state doesn't tie up the SD bus
     * for the rest of the boot — we don't need it after this. */
    sd_unmount();
    return ESP_OK;
}
