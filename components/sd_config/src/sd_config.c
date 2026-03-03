#include "sd_config.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#include "nvs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static const char *TAG = "SD_CONFIG";

#define MOUNT_POINT "/sdcard"
#define NVS_NAMESPACE "sd_config"

static bool parse_line(const char *line, char *key, size_t key_size,
                       char *value, size_t value_size) {
    const char *eq = strchr(line, '=');
    if (!eq || eq == line)
        return false;

    size_t klen = eq - line;
    if (klen >= key_size)
        klen = key_size - 1;
    memcpy(key, line, klen);
    key[klen] = '\0';

    /* Trim trailing whitespace from key */
    while (klen > 0 && (key[klen - 1] == ' ' || key[klen - 1] == '\t')) {
        key[--klen] = '\0';
    }

    const char *vstart = eq + 1;
    /* Trim leading whitespace from value */
    while (*vstart == ' ' || *vstart == '\t')
        vstart++;

    size_t vlen = strlen(vstart);
    /* Trim trailing whitespace/newline from value */
    while (vlen > 0 && (vstart[vlen - 1] == ' ' || vstart[vlen - 1] == '\t' ||
                         vstart[vlen - 1] == '\r' || vstart[vlen - 1] == '\n')) {
        vlen--;
    }
    if (vlen >= value_size)
        vlen = value_size - 1;
    memcpy(value, vstart, vlen);
    value[vlen] = '\0';

    return klen > 0;
}

bool sd_config_read(void) {
    ESP_LOGI(TAG, "Attempting to read config from SD card...");

    /*
     * SDMMC Slot 0 with IO MUX pins — matches BSP configuration.
     * Slot 0 does NOT conflict with ESP-Hosted SDIO on Slot 1.
     * Must be called BEFORE esp_hosted_init() so the SDMMC host
     * is cleanly deinitialized before ESP-Hosted claims Slot 1.
     */
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.slot = SDMMC_HOST_SLOT_0;
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

    /* Power the SD card via on-chip LDO channel 4 */
    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = 4,
    };
    sd_pwr_ctrl_handle_t pwr_ctrl_handle = NULL;
    esp_err_t ret = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_ctrl_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init SD card LDO power: %s", esp_err_to_name(ret));
        return false;
    }
    host.pwr_ctrl_handle = pwr_ctrl_handle;

    /* Slot 0 uses IO MUX — no explicit pin assignment needed */
    const sdmmc_slot_config_t slot_config = {
        .cd = SDMMC_SLOT_NO_CD,
        .wp = SDMMC_SLOT_NO_WP,
        .width = 4,
        .flags = 0,
    };

    /* Mount filesystem */
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    sdmmc_card_t *card = NULL;
    ret = esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config,
                                   &mount_config, &card);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGW(TAG, "Failed to mount SD card filesystem");
        } else {
            ESP_LOGW(TAG, "Failed to initialize SD card: %s", esp_err_to_name(ret));
        }
        sd_pwr_ctrl_del_on_chip_ldo(pwr_ctrl_handle);
        return false;
    }

    ESP_LOGI(TAG, "SD card mounted (%s, %lluMB)",
             card->cid.name, ((uint64_t)card->csd.capacity) * card->csd.sector_size / (1024 * 1024));

    /* Open NVS */
    nvs_handle_t nvs;
    ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS: %s", esp_err_to_name(ret));
        esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
        sd_pwr_ctrl_del_on_chip_ldo(pwr_ctrl_handle);
        return false;
    }

    /* Clear old connection keys (SD card is single source of truth) */
    nvs_erase_key(nvs, "wifiSSID");
    nvs_erase_key(nvs, "wifiPass");
    nvs_erase_key(nvs, "mqttHost");
    nvs_erase_key(nvs, "mqttPort");
    nvs_erase_key(nvs, "mqttUser");
    nvs_erase_key(nvs, "mqttPass");
    nvs_erase_key(nvs, "mqttCaCert");
    ESP_LOGI(TAG, "Cleared old NVS connection settings");

    /* Read config.env */
    FILE *f = fopen(MOUNT_POINT "/config.env", "r");
    if (!f) {
        ESP_LOGW(TAG, "config.env not found on SD card");
        nvs_close(nvs);
        esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
        sd_pwr_ctrl_del_on_chip_ldo(pwr_ctrl_handle);
        return false;
    }

    ESP_LOGI(TAG, "Reading config.env...");
    bool found_wifi = false;
    char line[256];
    char key[64];
    char value[192];

    while (fgets(line, sizeof(line), f)) {
        /* Trim leading whitespace */
        char *p = line;
        while (*p == ' ' || *p == '\t')
            p++;

        /* Skip empty lines and comments */
        if (*p == '\0' || *p == '\n' || *p == '\r' || *p == '#')
            continue;

        if (!parse_line(p, key, sizeof(key), value, sizeof(value)))
            continue;

        if (strcmp(key, "WIFI_SSID") == 0 && strlen(value) > 0) {
            nvs_set_str(nvs, "wifiSSID", value);
            ESP_LOGI(TAG, "WiFi SSID: %s", value);
            found_wifi = true;
        } else if (strcmp(key, "WIFI_PWD") == 0) {
            nvs_set_str(nvs, "wifiPass", value);
            ESP_LOGI(TAG, "WiFi password set");
        } else if (strcmp(key, "MQTT_HOST") == 0 && strlen(value) > 0) {
            nvs_set_str(nvs, "mqttHost", value);
            ESP_LOGI(TAG, "MQTT Host: %s", value);
        } else if (strcmp(key, "MQTT_PORT") == 0 && strlen(value) > 0) {
            int port = atoi(value);
            if (port > 0 && port <= 65535) {
                nvs_set_u16(nvs, "mqttPort", (uint16_t)port);
                ESP_LOGI(TAG, "MQTT Port: %d", port);
            }
        } else if (strcmp(key, "MQTT_USER") == 0 && strlen(value) > 0) {
            nvs_set_str(nvs, "mqttUser", value);
            ESP_LOGI(TAG, "MQTT User: %s", value);
        } else if (strcmp(key, "MQTT_PASS") == 0 && strlen(value) > 0) {
            nvs_set_str(nvs, "mqttPass", value);
            ESP_LOGI(TAG, "MQTT password set");
        }
    }
    fclose(f);

    /* Read CA certificate if present */
    f = fopen(MOUNT_POINT "/ca.crt", "r");
    if (f) {
        fseek(f, 0, SEEK_END);
        long cert_size = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (cert_size > 0 && cert_size < 8192) {
            char *cert_buf = malloc(cert_size + 1);
            if (cert_buf) {
                size_t read_len = fread(cert_buf, 1, cert_size, f);
                cert_buf[read_len] = '\0';
                nvs_set_str(nvs, "mqttCaCert", cert_buf);
                ESP_LOGI(TAG, "CA cert stored (%ld bytes)", cert_size);
                free(cert_buf);
            }
        }
        fclose(f);
    } else {
        ESP_LOGI(TAG, "ca.crt not found (optional)");
    }

    nvs_commit(nvs);
    nvs_close(nvs);

    /* Unmount SD card and release power */
    esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
    sd_pwr_ctrl_del_on_chip_ldo(pwr_ctrl_handle);
    ESP_LOGI(TAG, "Config read complete (WiFi found: %s)", found_wifi ? "yes" : "no");

    return found_wifi;
}
