# TrailCurrent Wall Mounted Display

Central control display firmware for an ESP32-P4 with a 7" touchscreen, providing a dashboard for monitoring and controlling devices on the [TrailCurrent](https://trailcurrent.com) CAN bus via WiFi and MQTT.

## Home Screen

![Main Screen](DOCS/IMAGES/HomeScreen.png)

## Hardware Overview

- **Microcontroller:** ESP32-P4 ([Waveshare ESP32-P4 WiFi6 Touch LCD 7B](https://www.waveshare.com/esp32-p4-wifi6-touch-lcd-7b.htm?aff_id=Trailcurrent)) — selected for its processing power and rich display capability, enabling more engaging touch-panel UIs
- **WiFi:** Via ESP-Hosted (ESP32-C6 slave over SDIO)
- **Display:** 7" 1024x600 MIPI-DSI LCD with capacitive touch
- **Build System:** ESP-IDF v5.5.2
- **SD Card:** SDMMC 4-bit mode for configuration (separate bus from ESP-Hosted SDIO)
- **Key Features:**
  - Central dashboard for trailer system monitoring
  - Thermostat with temperature control
  - Eight device control buttons
  - SD card provisioning: insert a `config.env` to configure WiFi and MQTT credentials, stored to NVS for ongoing use
  - MQTT over TLS (`mqtts://`) for real-time data from the TrailCurrent platform
  - Light control with on/off and brightness via MQTT
  - GPS, energy, and air quality monitoring via MQTT subscriptions
  - Color theme switching
  - Screen brightness and timeout controls
  - Timezone selection
  - NVS-persisted user settings (theme, brightness, timeout, timezone)
  - LVGL v8 UI designed with EEZ Studio
  - FreeCAD enclosure design

## Hardware Requirements

### Components

- **Board:** [Waveshare ESP32-P4 WiFi6 Touch LCD 7B](https://www.waveshare.com/esp32-p4-wifi6-touch-lcd-7b.htm?aff_id=Trailcurrent)
- **WiFi Module:** ESP32-C6 (integrated, communicates via ESP-Hosted SDIO)

## Firmware

This project uses ESP-IDF (not PlatformIO).

**Setup:**
```bash
# Install ESP-IDF v5.5.2 (if not already installed)
# See https://docs.espressif.com/projects/esp-idf/en/stable/esp32p4/get-started/

# Set target
idf.py set-target esp32p4

# Configure (optional - sdkconfig.defaults has working defaults)
idf.py menuconfig

# Build
idf.py build

# Flash
idf.py -p /dev/ttyUSBx flash

# Monitor serial output
idf.py -p /dev/ttyUSBx monitor
```

### Firmware Dependencies

Dependencies are managed by the ESP-IDF component manager and resolved automatically during build:

- **[LVGL](https://github.com/lvgl/lvgl)** (v8.4.x) - Light and Versatile Graphics Library
- **[esp_wifi_remote](https://components.espressif.com/components/espressif/esp_wifi_remote)** - WiFi via ESP-Hosted
- **[esp_hosted](https://components.espressif.com/components/espressif/esp_hosted)** - ESP32-C6 slave communication

### SD Card Configuration

WiFi and MQTT settings are provisioned via an SD card. Create a `config.env` file on a FAT-formatted SD card:

```
WIFI_SSID=YourNetworkName
WIFI_PWD=YourWiFiPassword
MQTT_HOST=192.168.1.100
MQTT_PORT=8883
MQTT_USER=mqttuser
MQTT_PASS=mqttpassword
```

Optionally include a `ca.crt` file on the SD card for custom TLS certificate authority.

On boot, the device reads `config.env` from the SD card and stores the values into NVS. Once provisioned, the SD card is not needed for normal operation — it only needs to be reinserted to update settings. The device uses the same `config.env` format as the [TrailCurrent Waveshare ESP32-S3 Remote](https://github.com/KDElmira/TrailCurrentWaveshareEsp32s3Remote).

### MQTT

The MQTT client connects over TLS (`mqtts://`) using credentials from NVS. It subscribes to:

- `local/lights/+/status` — Light on/off and brightness state
- `local/energy/status` — Energy monitoring data
- `local/airquality/temphumid` — Temperature and humidity
- `local/gps/latlon`, `local/gps/alt`, `local/gps/details` — GPS data

Light commands are published to `local/lights/{id}/command`.

## Project Structure

```
├── ASSETS/                       # UI design assets (images, SVGs)
├── CAD/                          # FreeCAD enclosure design and STL exports
├── DOCS/                         # Documentation and screenshots
│   └── IMAGES/                   # UI screenshots
├── GUI/                          # EEZ Studio UI design files
├── components/                   # Custom ESP-IDF components
│   ├── bsp_extra/                # Board support package extensions
│   ├── mqtt_client/              # MQTT client with TLS, NVS settings, subscriptions, JSON processing
│   └── sd_config/                # SD card config.env reader, stores to NVS
├── main/                         # Main application source
│   ├── main.c                    # Boot sequence, WiFi connect, MQTT init
│   ├── actions.c                 # UI action callbacks (light control, settings, themes)
│   ├── vars.c                    # UI variable bindings and LVGL widget updates
│   ├── mqtt_vars.h               # Declarations for MQTT-sourced variables
│   ├── idf_component.yml         # Component dependencies
│   └── ui/                       # EEZ Studio generated UI code (do not edit)
├── CMakeLists.txt                # Top-level CMake configuration
├── sdkconfig.defaults            # ESP-IDF configuration defaults
├── partitions.csv                # Flash partition layout (26MB app + 5MB storage)
└── LICENSE                       # MIT License
```

## License

MIT License - See LICENSE file for details.

## Contributing

Improvements and contributions are welcome! Please submit issues or pull requests.
