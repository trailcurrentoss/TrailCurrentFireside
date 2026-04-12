#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_BUTTONS        8
#define BTN_LABEL_MAX      24
#define NUM_CURATED_ICONS  84
#define DEFAULT_ICON_CP    0xf7b6u   /* FontAwesome lightbulb-on */

typedef enum {
    MOD_NONE       = 0,
    MOD_TORRENT    = 1,
    MOD_SWITCHBACK = 2,
} module_type_t;

typedef struct {
    uint8_t  module_type;     /* module_type_t value */
    uint8_t  instance;        /* 0..2 */
    uint8_t  channel;         /* 0..7 */
    uint8_t  reserved;
    uint16_t icon_codepoint;  /* glyph in fa32 font */
    uint16_t _pad;
    char     label[BTN_LABEL_MAX];
} btn_config_t;

extern btn_config_t      g_buttons[NUM_BUTTONS];
extern volatile uint8_t  g_button_state[NUM_BUTTONS];

extern const uint32_t TORRENT_TOGGLE_ID[3];
extern const uint32_t TORRENT_STATUS_ID[3];
extern const uint32_t SWITCHBACK_TOGGLE_ID[3];
extern const uint32_t SWITCHBACK_STATUS_ID[3];
extern const uint16_t CURATED_ICONS[NUM_CURATED_ICONS];

void button_config_init(void);
void button_config_save(void);
void button_config_apply_to_ui(void);

uint8_t button_config_find(module_type_t module, uint8_t instance, uint8_t channel);
void    button_config_assign(module_type_t module, uint8_t instance,
                              uint8_t channel, uint8_t btn_number);

void button_config_set_appearance(uint8_t btn_number, const char *label, uint16_t icon_cp);

const char *utf8_encode(uint16_t codepoint, char out[5]);

#ifdef __cplusplus
}
#endif
