# TrailCurrent Fireside

Central control display firmware for the [TrailCurrent](https://trailcurrent.com)
platform, running on the [Waveshare ESP32-P4-WIFI6-Touch-LCD-7B](https://www.waveshare.com/esp32-p4-wifi6-touch-lcd-7b.htm)
7-inch 1024×600 touchscreen dev board. Provides an on-device dashboard for
monitoring and controlling devices on the TrailCurrent CAN bus via WiFi and
MQTT.

## Hardware Overview

- **Board:** [Waveshare ESP32-P4-WIFI6-Touch-LCD-7B](https://www.waveshare.com/wiki/ESP32-P4-WIFI6-Touch-LCD-7B) (7", 1024×600 IPS + GT911 capacitive touch)
- **Microcontroller:** ESP32-P4 (32 MB flash, PSRAM)
- **WiFi:** ESP32-C6 slave over 4-bit SDIO via ESP-Hosted
- **Display:** 7" 1024×600 MIPI-DSI (EK79007 controller)
- **Touch:** GT911 capacitive on I2C
- **Audio:** ES8311 codec (I2S DAC) + ES7210 ADC + class-D PA (PA enable on GPIO 53)
- **Battery:** on-board JST connector with charging IC + passive divider (200 kΩ / 100 kΩ) into GPIO 20 for voltage sense
- **I/O:** CAN, RS485, USB-OTG, USB-to-UART, MicroSD, 2× MIC, screw terminals
- **Build System:** ESP-IDF v5.3 or newer, target `esp32p4`
- **Key Features:**
  - Central dashboard for trailer system monitoring
  - Thermostat with temperature control
  - Configurable device control buttons
  - On-device setup wizard (WiFi + MQTT provisioning — no SD card required)
  - MQTT over TLS (`mqtts://`) for real-time data from the TrailCurrent platform
  - Light control with on/off and brightness via MQTT
  - GPS, energy, and air quality monitoring via MQTT subscriptions
  - Battery status via MQTT (from Solstice) **and** on-board battery telemetry via ADC divider
  - Alarm and TTS audio notifications
  - Color theme switching (light/dark)
  - Screen brightness and timeout controls
  - NVS-persisted user settings (theme, brightness, timeout, WiFi + MQTT credentials)
  - LVGL v8.4 UI designed with EEZ Studio

## Firmware

This project uses ESP-IDF (not PlatformIO).

**Prerequisites:**

- ESP-IDF v5.3 or newer, targeting `esp32p4`.
- [EEZ Studio](https://github.com/eez-open/studio) — needed to export the UI to C. First-build users **must** run this before `idf.py build`.
- ESP32-C6 slave firmware flashed to the on-board C6 (per the [Waveshare wiki](https://www.waveshare.com/wiki/ESP32-P4-WIFI6-Touch-LCD-7B) setup instructions).

**Setup:**

```bash
# 1. Export the UI from EEZ Studio.
#    Open GUI/TrailCurrentFireside.eez-project
#    then File → Build (Ctrl+B). Output lands in main/ui/.

# 2. Build + flash + monitor.
idf.py set-target esp32p4
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

### Firmware Dependencies

Resolved automatically by the ESP-IDF component manager. See
[`main/idf_component.yml`](main/idf_component.yml) for the full list.
Key ones:

- **[waveshare/esp32_p4_wifi6_touch_lcd_7b](https://components.espressif.com/components/waveshare/esp32_p4_wifi6_touch_lcd_7b)** — first-party BSP for this board. Bundles the LCD panel driver (`esp_lcd_ek79007`), touch driver (`esp_lcd_touch_gt911`), LVGL port (`esp_lvgl_port`), and audio codec (`esp_codec_dev` with ES8311 + ES7210 support).
- **[lvgl/lvgl](https://github.com/lvgl/lvgl)** (`~8.4.0`) — pinned to match the `.eez-project`'s `lvglVersion`.
- **[espressif/esp_hosted](https://components.espressif.com/components/espressif/esp_hosted)** — ESP32-C6 slave communication over SDIO.
- **[espressif/esp_wifi_remote](https://components.espressif.com/components/espressif/esp_wifi_remote)** — WiFi via ESP-Hosted.
- **[espressif/mdns](https://components.espressif.com/components/espressif/mdns)** — mDNS discovery.

### On-Device Setup Wizard

On first boot the device shows a setup wizard that walks the user through:

1. **WiFi setup** — scan-and-pick, WPA2 password entry with on-screen keyboard.
2. **MQTT broker configuration** — host, port, username, password.

Credentials are persisted to NVS, so a rebooted device auto-connects and jumps straight to the dashboard.

### MQTT

The MQTT client connects over TLS (`mqtts://`) using credentials from NVS. It subscribes to:

- `local/lights/+/status` — Light on/off and brightness state
- `local/energy/status` — Energy monitoring data
- `local/airquality/temphumid` — Temperature and humidity
- `local/airquality/status` — CO2 (eCO2 ppm) and TVOC (ppb)
- `local/gps/latlon`, `local/gps/alt`, `local/gps/details` — GPS data
- `local/battery/status` — Battery state from Solstice

Light commands are published to `local/lights/{id}/command`.

## Project Structure

```
├── DOCS/                                        # Documentation + vendor reference
│   ├── AUDIO_PHRASES.md                         #   TTS alarm phrase list + regeneration recipe
│   ├── PORT_NOTES.md                            #   Board bring-up notes (audio, battery, WiFi, etc.)
│   ├── ESP32-P4-WIFI6-Touch-LCD-7B.pdf          #   Waveshare board schematic
│   ├── ESP32-P4-WIFI6-Touch-LCD-7B-main/        #   Waveshare vendor examples (reference)
│   └── IMAGES/                                  #   Screenshots
├── GUI/                                         # EEZ Studio UI design files
│   ├── ASSETS/                                  #   Fonts (Roboto, FontAwesome) and images
│   └── TrailCurrentFireside.eez-project         #   Single source of truth for the GUI
├── ASSETS/                                      # Runtime backdrop / bar-graphic assets
├── components/                                  # Local components
│   ├── bsp_shim/                                #   Thin shim over the Waveshare BSP (`set_lcd_blight` → `bsp_display_brightness_set`)
│   ├── button_config/                           #   Configurable UI button metadata (icon, label, MQTT topic)
│   ├── fireside_config/                         #   NVS-backed device configuration
│   ├── mqtt_client/                             #   MQTT client with TLS + JSON dispatch
│   ├── peregrine_voice/                         #   Voice input pipeline (ES7210 ADC + wake-word)
│   └── wifi_setup/                              #   WiFi setup wizard + NVS-backed credentials
├── main/                                        # Application source
│   ├── main.c                                   #   Waveshare BSP bring-up + boot orchestration
│   ├── actions.c                                #   UI action callbacks (light control, settings, themes)
│   ├── vars.c                                   #   UI variable bindings + LVGL widget updates
│   ├── app_state.c                              #   App state machine + NVS wrappers
│   ├── alarms.c                                 #   Alarm handling + notifications
│   ├── audio.c                                  #   ES8311 codec playback via esp_codec_dev
│   ├── audio_assets.c                           #   Embedded PCM for TTS phrases (see DOCS/AUDIO_PHRASES.md)
│   ├── battery.c                                #   Local battery telemetry — ADC divider on GPIO 20
│   ├── perf.c                                   #   Perf probes
│   ├── sd_config.c                              #   MicroSD-based config import (optional)
│   ├── wifi_health.c                            #   C6 heartbeat / auto-restart
│   ├── mqtt_vars.h                              #   MQTT variable declarations
│   └── ui/                                      #   EEZ Studio generated UI code (do NOT edit)
├── CMakeLists.txt                               # Top-level CMake configuration
├── sdkconfig.defaults                           # ESP-IDF configuration defaults
├── sdkconfig.defaults.esp32p4                   # ESP32-P4 target-specific overrides
├── partitions.csv                               # Flash partition layout (8M factory app + 2M spiffs)
└── LICENSE                                      # MIT License
```

## GUI Editing

The `.eez-project` is the single source of truth for every screen. Rules:

- Never hand-edit `main/ui/*` — EEZ Studio overwrites those on export.
- Never call `lv_obj_set_pos`, `lv_obj_set_size`, `lv_obj_set_align`,
  `lv_obj_move_foreground`, or any style-\* geometry override on an
  `objects.<widget>` from C. Widget geometry lives in the JSON.
- Colors reference named tokens from the project palette; no hex literals
  in styles. If a new color is needed, add it via EEZ Studio's Colors
  panel first, then reference the name in the widget/style.

## License

MIT License — See LICENSE file for details.

## Contributing

Improvements and contributions are welcome! Please submit issues or pull requests.
