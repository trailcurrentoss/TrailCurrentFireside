# TrailCurrent Fireside

Central control display firmware for an ESP32-P4 with a 10.1" touchscreen, providing a dashboard for monitoring and controlling devices on the [TrailCurrent](https://trailcurrent.com) CAN bus via WiFi and MQTT.

## Hardware Overview

- **Board:** [Elecrow CrowPanel Advance 10.1" ESP32-P4 HMI AI Display (1024×600 IPS + capacitive touch)](https://www.elecrow.com/crowpanel-advanced-10-1-inch-esp32-p4-hmi-ai-display-1024x600-ips-touch-screen.html)
- **Microcontroller:** ESP32-P4 (16 MB PSRAM, 16 MB flash)
- **WiFi:** ESP32-C6 slave over 4-bit SDIO via ESP-Hosted (vendor firmware v2.12.3)
- **Display:** 10.1" 1024×600 MIPI-DSI (EK79007 controller) with GT911 capacitive touch
- **Build System:** ESP-IDF v5.3 or newer, target `esp32p4`
- **Key Features:**
  - Central dashboard for trailer system monitoring
  - Thermostat with temperature control
  - Configurable device control buttons
  - On-device setup wizard (WiFi + MQTT provisioning — no SD card required)
  - MQTT over TLS (`mqtts://`) for real-time data from the TrailCurrent platform
  - Light control with on/off and brightness via MQTT
  - GPS, energy, and air quality monitoring via MQTT subscriptions
  - Battery status via MQTT (from Solstice)
  - Alarm and audio notifications
  - Color theme switching (light/dark)
  - Screen brightness and timeout controls
  - NVS-persisted user settings (theme, brightness, timeout, WiFi + MQTT credentials)
  - LVGL v8.4 UI designed with EEZ Studio
  - FreeCAD enclosure design

## Firmware

This project uses ESP-IDF (not PlatformIO).

**Prerequisites:**

- ESP-IDF v5.3 or newer, targeting `esp32p4`.
- [EEZ Studio](https://github.com/eez-open/studio) — needed to export the UI to C. First-build users **must** run this before `idf.py build`.
- ESP32-C6 slave firmware **v2.12.3** flashed to the on-board C6. See the Elecrow "Advance P4 On-Board ESP32C6 Firmware V2.12.3" upgrade guide — the SDIO pin map in `sdkconfig.defaults.esp32p4` targets this revision.

**Setup:**

```bash
# 1. Export the UI from EEZ Studio.
#    Open GUI/elecrow-esp32-p4-10-in/elecrow-esp32-p4-10-in.eez-project
#    then File → Build (Ctrl+B). Output lands in main/ui/.

# 2. Build + flash + monitor.
idf.py set-target esp32p4
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

### Firmware Dependencies

Dependencies are managed by the ESP-IDF component manager and resolved automatically during build:

- **[LVGL](https://github.com/lvgl/lvgl)** (`~8.4.0`) — Light and Versatile Graphics Library, matches the eez-project's `lvglVersion`.
- **[esp_lcd_ek79007](https://components.espressif.com/components/espressif/esp_lcd_ek79007)** — MIPI-DSI panel driver.
- **[esp_lcd_touch_gt911](https://components.espressif.com/components/espressif/esp_lcd_touch_gt911)** — capacitive touch driver.
- **[esp_lvgl_port](https://components.espressif.com/components/espressif/esp_lvgl_port)** — LVGL ↔ esp_lcd glue.
- **[esp_hosted](https://components.espressif.com/components/espressif/esp_hosted)** (`~2.12.3`) — ESP32-C6 slave communication over SDIO.
- **[esp_wifi_remote](https://components.espressif.com/components/espressif/esp_wifi_remote)** — WiFi via ESP-Hosted.
- **[mdns](https://components.espressif.com/components/espressif/mdns)** — mDNS discovery.

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
├── CAD/                          # FreeCAD enclosure design and STL exports
├── DOCS/                         # Documentation and screenshots
├── GUI/                          # EEZ Studio UI design files
│   ├── ASSETS/                   # Fonts (Roboto, FontAwesome) and logo referenced by the eez-project
│   └── elecrow-esp32-p4-10-in/
│       └── elecrow-esp32-p4-10-in.eez-project   # single source of truth for the GUI
├── components/                   # Custom ESP-IDF components
│   ├── bsp_display/              # EK79007 MIPI-DSI + LVGL bring-up
│   ├── bsp_extra/                # Board support package extensions
│   ├── bsp_i2c/                  # Shared I2C bus init (GT911 + peripherals)
│   ├── bsp_illuminate/           # Backlight + power sequencing
│   ├── button_config/            # Configurable UI button metadata (icon, label, MQTT topic)
│   ├── fireside_config/          # NVS-backed device configuration
│   ├── mqtt_client/              # MQTT client with TLS, NVS settings, subscriptions, JSON processing
│   └── wifi_setup/               # WiFi setup wizard + NVS-backed credential storage
├── main/                         # Main application source
│   ├── main.c                    # Boot sequence, BSP init, WiFi connect, MQTT init
│   ├── actions.c                 # UI action callbacks (light control, settings, themes)
│   ├── vars.c                    # UI variable bindings and LVGL widget updates
│   ├── app_state.c               # App state machine + NVS wrappers
│   ├── alarms.c                  # Alarm handling and notifications
│   ├── audio.c                   # Audio playback (audio_assets.c holds embedded PCM)
│   ├── battery.c                 # Battery status handling
│   ├── mqtt_vars.h               # Declarations for MQTT-sourced variables
│   ├── idf_component.yml         # Component dependencies
│   └── ui/                       # EEZ Studio generated UI code (do not edit)
├── CMakeLists.txt                # Top-level CMake configuration
├── sdkconfig.defaults            # ESP-IDF configuration defaults
├── sdkconfig.defaults.esp32p4    # ESP32-P4 target-specific overrides (SDIO pins, cache geometry)
├── partitions.csv                # Flash partition layout (8M factory app + 2M spiffs)
└── LICENSE                       # MIT License
```

## GUI Editing

The `.eez-project` is the single source of truth for every screen. Rules:

- Never hand-edit `main/ui/*` — EEZ Studio overwrites those on export.
- Never call `lv_obj_set_pos`, `lv_obj_set_size`, `lv_obj_set_align`, `lv_obj_move_foreground`, or any style-* geometry override on an `objects.<widget>` from C. Widget geometry lives in the JSON.
- Colors reference named tokens from the project palette; no hex literals in styles. If a new color is needed, add it via EEZ Studio's Colors panel first, then reference the name in the widget/style.

## License

MIT License - See LICENSE file for details.

## Contributing

Improvements and contributions are welcome! Please submit issues or pull requests.
