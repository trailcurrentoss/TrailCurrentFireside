/*
 * alarms.c — active-alarm evaluator for Fireside.
 *
 * Mirrors the Headwaters PWA's alarms-service.js input → armed → active
 * pipeline. Single config (no per-mode profiles): Fireside is either in
 * the vehicle or away from it, and the user configures ONE arm set that
 * makes sense for their use.
 *
 * Wire-up:
 *   mqtt_client.c   → alarms_apply_inputs(src, addr, bits)
 *   vars.c (SOC)    → alarms_apply_battery(percent)
 *   vars.c badge    → alarms_active_count()
 *   PageAlarms UI   → NVS-editable arm bitmaps + battery threshold
 *
 * Phase 1 default: everything disarmed, battery-threshold disabled — no
 * alarms fire until the user configures them via PageAlarms (Phase 2a
 * ships mode-less UI: battery enable+threshold + placeholder for the
 * per-sensor arm rows landing in Phase 2b).
 */

#include "alarms.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <string.h>

static const char *TAG = "ALARMS";
#define NVS_NS "fireside_alarm"

#define SW_SENSORS  8      /* Switchback DIs — 8 bits */
#define PK_SENSORS  12     /* Picket reeds  — 12 bits */
#define MAX_BOARDS  8      /* Up to 8 boards per source */

/* Single arm config — no per-mode variation. */
static uint16_t s_armed_sw[MAX_BOARDS] = {0};
static uint16_t s_armed_pk[MAX_BOARDS] = {0};
/* Polarity bitmap: bit N = 1 → alarm fires when input N is LOW (inverted,
 * e.g. "power lost" sensors); bit N = 0 → fires when input HIGH (normal,
 * e.g. "door open"). Evaluator: active = armed & (inputs XOR polarity). */
static uint16_t s_polarity_sw[MAX_BOARDS] = {0};
static uint16_t s_polarity_pk[MAX_BOARDS] = {0};
static bool     s_battery_enabled = false;
static uint8_t  s_battery_threshold = 20;   /* PWA default */

/* Per-sensor labels — up to ALARM_LABEL_MAX bytes each incl. NUL.
 * Sparse: only populated when the user sets a custom label via the UI.
 * NVS keys: "sw_lbl_A_S" / "pk_lbl_A_S" (per sensor).
 * Runtime cache: two arrays [MAX_BOARDS][sensors_per_board][label]. */
static char s_label_sw[MAX_BOARDS][SW_SENSORS][ALARM_LABEL_MAX] = {{{0}}};
static char s_label_pk[MAX_BOARDS][PK_SENSORS][ALARM_LABEL_MAX] = {{{0}}};

/* Live input snapshots. */
static uint16_t s_inputs_sw[MAX_BOARDS] = {0};
static uint16_t s_inputs_pk[MAX_BOARDS] = {0};
static int32_t  s_battery_pct = -1;

/* Discovery bitmaps — bit N high means we've received AT LEAST ONE input
 * payload from that board addr (Switchback or Picket). Populated in
 * alarms_apply_inputs; consumed by alarms_get_known_boards to render
 * the per-sensor arm rows in PageAlarms. */
static uint16_t s_known_sw = 0;
static uint16_t s_known_pk = 0;

/* Rising-edge machinery. `s_prev_active_*` mirrors the active bitmap from
 * the last alarms_tick_edges() call; new-rising bits = new & ~prev. The
 * battery event is a separate bool. Events land in a ring buffer sized
 * for a boot-time re-alert burst (up to 8 boards * 12 sensors + 1
 * battery = 97 possible events; 128 leaves headroom without blowing RAM). */
static uint16_t s_prev_active_sw[MAX_BOARDS] = {0};
static uint16_t s_prev_active_pk[MAX_BOARDS] = {0};
static bool     s_prev_battery_low = false;
static bool     s_edges_baseline_seen = false;

/* Sticky per-alarm "last announced" timestamps. 0 = never announced /
 * cleared (condition inactive or freshly returned to normal). Non-zero =
 * monotonic esp_timer time of the most recent TTS announcement — the
 * snooze gate compares (now - last) against snooze_secs to decide
 * whether to re-alert while the condition persists.
 *
 * When a condition clears (1 → 0 transition), we reset last_announced
 * to 0 so a later re-rising fires cleanly without stray snooze state.
 * When the user acknowledges (topbar notif tap), we bump every currently
 * active alarm's last_announced to "now" so re-alerts are pushed out by
 * one snooze window. */
static int64_t s_last_announced_sw[MAX_BOARDS][SW_SENSORS] = {{0}};
static int64_t s_last_announced_pk[MAX_BOARDS][PK_SENSORS] = {{0}};
static int64_t s_last_announced_battery_us = 0;

/* Re-alert snooze window. Loaded from NVS in load_config(); kept in RAM
 * so the tick can compare against esp_timer_get_time() without a NVS
 * read on every 1-Hz iteration. */
static int      s_snooze_secs = ALARM_SNOOZE_SECS_DEFAULT;

#define EDGE_Q_SZ 128
static alarm_edge_t s_edge_q[EDGE_Q_SZ];
static uint16_t     s_edge_q_head = 0;   /* pop from here */
static uint16_t     s_edge_q_tail = 0;   /* push to here */

static void edge_push(alarm_edge_t e) {
    uint16_t next = (uint16_t)((s_edge_q_tail + 1) % EDGE_Q_SZ);
    if (next == s_edge_q_head) return;   /* full — drop silently */
    s_edge_q[s_edge_q_tail] = e;
    s_edge_q_tail = next;
}

/* ─── NVS I/O ────────────────────────────────────────────────────────── */

/* Key layout (single-config, no mode prefix):
 *   sw_arm_A   u16   — switchback armed bits for board addr A
 *   pk_arm_A   u16   — picket armed bits
 *   batt_en    u8    — battery alarm enabled flag
 *   batt_thr   u8    — battery threshold percent
 *   snooze_s   u16   — re-alert snooze window (seconds)
 */

static void load_config(void) {
    nvs_handle_t h;
    if (nvs_open(NVS_NS, NVS_READONLY, &h) != ESP_OK) {
        ESP_LOGI(TAG, "no saved config — all disarmed (safe default)");
        s_battery_threshold = 20;
        return;
    }
    for (int a = 0; a < MAX_BOARDS; a++) {
        char k[16];
        uint16_t v = 0;
        snprintf(k, sizeof(k), "sw_arm_%d", a);
        if (nvs_get_u16(h, k, &v) == ESP_OK) s_armed_sw[a] = v;
        v = 0;
        snprintf(k, sizeof(k), "pk_arm_%d", a);
        if (nvs_get_u16(h, k, &v) == ESP_OK) s_armed_pk[a] = v;
        v = 0;
        snprintf(k, sizeof(k), "sw_pol_%d", a);
        if (nvs_get_u16(h, k, &v) == ESP_OK) s_polarity_sw[a] = v;
        v = 0;
        snprintf(k, sizeof(k), "pk_pol_%d", a);
        if (nvs_get_u16(h, k, &v) == ESP_OK) s_polarity_pk[a] = v;
        /* Per-sensor labels. */
        for (int s = 0; s < SW_SENSORS; s++) {
            snprintf(k, sizeof(k), "sw_lbl_%d_%d", a, s);
            size_t l = ALARM_LABEL_MAX;
            nvs_get_str(h, k, s_label_sw[a][s], &l);
        }
        for (int s = 0; s < PK_SENSORS; s++) {
            snprintf(k, sizeof(k), "pk_lbl_%d_%d", a, s);
            size_t l = ALARM_LABEL_MAX;
            nvs_get_str(h, k, s_label_pk[a][s], &l);
        }
    }
    uint8_t b = 0;
    if (nvs_get_u8(h, "batt_en", &b) == ESP_OK) s_battery_enabled = b != 0;
    b = 20;
    if (nvs_get_u8(h, "batt_thr", &b) == ESP_OK) s_battery_threshold = b;
    uint16_t sn = 0;
    if (nvs_get_u16(h, "snooze_s", &sn) == ESP_OK) {
        if (sn < ALARM_SNOOZE_SECS_MIN) sn = ALARM_SNOOZE_SECS_MIN;
        if (sn > ALARM_SNOOZE_SECS_MAX) sn = ALARM_SNOOZE_SECS_MAX;
        s_snooze_secs = (int)sn;
    }
    nvs_close(h);
    ESP_LOGI(TAG, "loaded config: batt_en=%d batt_thr=%u snooze=%ds",
             (int)s_battery_enabled, (unsigned)s_battery_threshold,
             s_snooze_secs);
}

/* ─── Public API ─────────────────────────────────────────────────────── */

void alarms_init(void) {
    /* Zero armed + polarity bitmaps + label cache on every init so a fresh
     * NVS load (called after a threshold change in the UI) doesn't leave
     * stale bits from a previous run.
     *
     * Sticky per-alarm timestamps + baseline flag are only zeroed on the
     * FIRST call — a config-change reload (from action_toggle_battery_alarm
     * / action_battery_threshold / action_alarms_set_snooze) must not
     * re-fire every currently-active alarm as though it just rose. */
    memset(s_armed_sw, 0, sizeof(s_armed_sw));
    memset(s_armed_pk, 0, sizeof(s_armed_pk));
    memset(s_polarity_sw, 0, sizeof(s_polarity_sw));
    memset(s_polarity_pk, 0, sizeof(s_polarity_pk));
    memset(s_label_sw, 0, sizeof(s_label_sw));
    memset(s_label_pk, 0, sizeof(s_label_pk));
    s_battery_enabled = false;
    s_battery_threshold = 20;
    s_snooze_secs = ALARM_SNOOZE_SECS_DEFAULT;
    load_config();
}

void alarms_apply_inputs(alarm_src_t src, uint8_t addr, uint16_t bits) {
    if (addr >= MAX_BOARDS) return;
    if (src == ALARM_SRC_SWITCHBACK) {
        s_inputs_sw[addr] = bits & ((1u << SW_SENSORS) - 1);
        s_known_sw |= (1u << addr);
    } else if (src == ALARM_SRC_PICKET) {
        s_inputs_pk[addr] = bits & ((1u << PK_SENSORS) - 1);
        s_known_pk |= (1u << addr);
    }
    /* The notification-badge repaint is driven from vars.c's 1-Hz clock
     * tick, so no cross-module coupling is needed here. */
}

void alarms_apply_battery(int32_t percent) {
    s_battery_pct = percent;
}

int alarms_active_count(void) {
    int n = 0;
    for (int a = 0; a < MAX_BOARDS; a++) {
        /* Polarity XOR: alarm fires when (input XOR polarity) is set AND
         * armed. Sensor with polarity=0 (normal) alarms on HIGH; sensor
         * with polarity=1 (inverted) alarms on LOW ("power lost"). */
        uint16_t active_sw = ((s_inputs_sw[a] ^ s_polarity_sw[a]) &
                              s_armed_sw[a]);
        uint16_t active_pk = ((s_inputs_pk[a] ^ s_polarity_pk[a]) &
                              s_armed_pk[a]);
        n += __builtin_popcount(active_sw) + __builtin_popcount(active_pk);
    }
    if (s_battery_enabled && s_battery_pct >= 0 &&
        s_battery_pct < s_battery_threshold) {
        n++;
    }
    return n;
}

void alarms_tick_edges(void) {
    uint16_t new_active_sw[MAX_BOARDS];
    uint16_t new_active_pk[MAX_BOARDS];
    for (int a = 0; a < MAX_BOARDS; a++) {
        new_active_sw[a] = (s_inputs_sw[a] ^ s_polarity_sw[a]) & s_armed_sw[a];
        new_active_pk[a] = (s_inputs_pk[a] ^ s_polarity_pk[a]) & s_armed_pk[a];
    }
    bool new_battery_low = (s_battery_enabled && s_battery_pct >= 0 &&
                            s_battery_pct < s_battery_threshold);

    int64_t now = esp_timer_get_time();
    int64_t snooze_us = (int64_t)s_snooze_secs * 1000000LL;

    /* Two things happen every tick:
     *   1. rising edges (0→1) push a fresh event and stamp last_announced.
     *   2. currently-active alarms whose last_announced is older than the
     *      snooze window push a REPEAT event and re-stamp last_announced.
     *
     * Rule (2) is what makes the alarm "keep reminding the user" — the
     * announcement fires every snooze_secs while the condition persists.
     *
     * On the baseline tick (first call after boot), rule (1) is suppressed
     * so retained-MQTT replay doesn't wake the speaker. Baseline-active
     * alarms are stamped with `now` instead of firing — they will re-alert
     * one snooze window later if still active, so a real persistent
     * condition on boot does eventually get announced (matches Spotter's
     * "you'll hear about it soon enough" behavior without spamming the
     * cabin the moment the panel powers on). */
    for (int a = 0; a < MAX_BOARDS; a++) {
        /* Switchback board */
        uint16_t rising_sw = new_active_sw[a] & ~s_prev_active_sw[a];
        uint16_t falling_sw = s_prev_active_sw[a] & ~new_active_sw[a];
        uint16_t active_sw = new_active_sw[a];
        for (int bit = 0; bit < SW_SENSORS; bit++) {
            uint16_t mask = 1u << bit;
            if (falling_sw & mask) {
                /* Condition cleared — reset sticky state so a later
                 * re-arm rising fires cleanly. */
                s_last_announced_sw[a][bit] = 0;
                continue;
            }
            if (!(active_sw & mask)) continue;   /* not active this tick */
            if (rising_sw & mask) {
                /* Fresh rising edge. */
                if (s_edges_baseline_seen) {
                    edge_push((alarm_edge_t){ .is_battery = false,
                                              .src = ALARM_SRC_SWITCHBACK,
                                              .addr = (uint8_t)a,
                                              .sensor = (uint8_t)bit });
                }
                s_last_announced_sw[a][bit] = now;
            } else if (s_edges_baseline_seen &&
                       s_last_announced_sw[a][bit] != 0 &&
                       (now - s_last_announced_sw[a][bit]) >= snooze_us) {
                /* Steady-state, snooze expired — re-alert. */
                edge_push((alarm_edge_t){ .is_battery = false,
                                          .src = ALARM_SRC_SWITCHBACK,
                                          .addr = (uint8_t)a,
                                          .sensor = (uint8_t)bit });
                s_last_announced_sw[a][bit] = now;
            }
        }

        /* Picket board — same logic as switchback with different mask width. */
        uint16_t rising_pk = new_active_pk[a] & ~s_prev_active_pk[a];
        uint16_t falling_pk = s_prev_active_pk[a] & ~new_active_pk[a];
        uint16_t active_pk = new_active_pk[a];
        for (int bit = 0; bit < PK_SENSORS; bit++) {
            uint16_t mask = 1u << bit;
            if (falling_pk & mask) {
                s_last_announced_pk[a][bit] = 0;
                continue;
            }
            if (!(active_pk & mask)) continue;
            if (rising_pk & mask) {
                if (s_edges_baseline_seen) {
                    edge_push((alarm_edge_t){ .is_battery = false,
                                              .src = ALARM_SRC_PICKET,
                                              .addr = (uint8_t)a,
                                              .sensor = (uint8_t)bit });
                }
                s_last_announced_pk[a][bit] = now;
            } else if (s_edges_baseline_seen &&
                       s_last_announced_pk[a][bit] != 0 &&
                       (now - s_last_announced_pk[a][bit]) >= snooze_us) {
                edge_push((alarm_edge_t){ .is_battery = false,
                                          .src = ALARM_SRC_PICKET,
                                          .addr = (uint8_t)a,
                                          .sensor = (uint8_t)bit });
                s_last_announced_pk[a][bit] = now;
            }
        }
    }

    /* Battery — same three-way (rising / active-with-snooze / falling). */
    if (!new_battery_low && s_prev_battery_low) {
        s_last_announced_battery_us = 0;
    } else if (new_battery_low) {
        if (!s_prev_battery_low) {
            if (s_edges_baseline_seen) {
                edge_push((alarm_edge_t){ .is_battery = true });
            }
            s_last_announced_battery_us = now;
        } else if (s_edges_baseline_seen &&
                   s_last_announced_battery_us != 0 &&
                   (now - s_last_announced_battery_us) >= snooze_us) {
            edge_push((alarm_edge_t){ .is_battery = true });
            s_last_announced_battery_us = now;
        }
    }

    memcpy(s_prev_active_sw, new_active_sw, sizeof(new_active_sw));
    memcpy(s_prev_active_pk, new_active_pk, sizeof(new_active_pk));
    s_prev_battery_low = new_battery_low;
    s_edges_baseline_seen = true;
}

int alarms_get_snooze_secs(void) { return s_snooze_secs; }

void alarms_set_snooze_secs(int secs) {
    if (secs < ALARM_SNOOZE_SECS_MIN) secs = ALARM_SNOOZE_SECS_MIN;
    if (secs > ALARM_SNOOZE_SECS_MAX) secs = ALARM_SNOOZE_SECS_MAX;
    if (secs == s_snooze_secs) return;
    s_snooze_secs = secs;
    nvs_handle_t h;
    if (nvs_open(NVS_NS, NVS_READWRITE, &h) == ESP_OK) {
        nvs_set_u16(h, "snooze_s", (uint16_t)secs);
        nvs_commit(h); nvs_close(h);
    }
}

int alarms_acknowledge_all(void) {
    /* Slide the "last announced" timestamp of every currently-active
     * alarm forward to now. Snooze then measures the next re-alert window
     * from this tap rather than from the original announcement — so if
     * the user reacts quickly (say, 5 s after the alarm fired), they
     * still get a full snooze window of silence rather than an
     * immediate re-alert on the next tick. */
    int64_t now = esp_timer_get_time();
    int acked = 0;
    for (int a = 0; a < MAX_BOARDS; a++) {
        uint16_t active_sw = (s_inputs_sw[a] ^ s_polarity_sw[a]) & s_armed_sw[a];
        uint16_t active_pk = (s_inputs_pk[a] ^ s_polarity_pk[a]) & s_armed_pk[a];
        for (int bit = 0; bit < SW_SENSORS; bit++) {
            if (active_sw & (1u << bit)) {
                s_last_announced_sw[a][bit] = now;
                acked++;
            }
        }
        for (int bit = 0; bit < PK_SENSORS; bit++) {
            if (active_pk & (1u << bit)) {
                s_last_announced_pk[a][bit] = now;
                acked++;
            }
        }
    }
    if (s_battery_enabled && s_battery_pct >= 0 &&
        s_battery_pct < s_battery_threshold) {
        s_last_announced_battery_us = now;
        acked++;
    }
    ESP_LOGI(TAG, "ack: %d alarm(s) snoozed for %ds", acked, s_snooze_secs);
    return acked;
}

bool alarms_pop_rising_edge(alarm_edge_t *out) {
    if (!out) return false;
    if (s_edge_q_head == s_edge_q_tail) return false;
    *out = s_edge_q[s_edge_q_head];
    s_edge_q_head = (s_edge_q_head + 1) % EDGE_Q_SZ;
    return true;
}

/* ─── Discovery + arm/disarm public API for the UI ──────────────────── */

int alarms_get_known_boards(alarm_src_t src, uint8_t *addrs_out, int max) {
    if (!addrs_out || max <= 0) return 0;
    uint16_t mask = (src == ALARM_SRC_SWITCHBACK) ? s_known_sw : s_known_pk;
    int n = 0;
    for (int a = 0; a < MAX_BOARDS && n < max; a++) {
        if (mask & (1u << a)) addrs_out[n++] = (uint8_t)a;
    }
    return n;
}

bool alarms_is_armed(alarm_src_t src, uint8_t addr, uint8_t sensor) {
    if (addr >= MAX_BOARDS) return false;
    uint16_t bits = (src == ALARM_SRC_SWITCHBACK) ? s_armed_sw[addr]
                                                  : s_armed_pk[addr];
    return (bits >> sensor) & 1u;
}

void alarms_set_armed(alarm_src_t src, uint8_t addr, uint8_t sensor,
                      bool armed) {
    if (addr >= MAX_BOARDS) return;
    uint16_t *bits = (src == ALARM_SRC_SWITCHBACK) ? &s_armed_sw[addr]
                                                   : &s_armed_pk[addr];
    if (armed) *bits |= (1u << sensor);
    else       *bits &= ~(1u << sensor);

    /* Persist the whole board's bitmap. Compact enough that one u16 write
     * per toggle is fine. */
    nvs_handle_t h;
    if (nvs_open(NVS_NS, NVS_READWRITE, &h) == ESP_OK) {
        char k[16];
        snprintf(k, sizeof(k),
                 (src == ALARM_SRC_SWITCHBACK) ? "sw_arm_%d" : "pk_arm_%d",
                 addr);
        nvs_set_u16(h, k, *bits);
        nvs_commit(h); nvs_close(h);
    }
}

bool alarms_is_inverted(alarm_src_t src, uint8_t addr, uint8_t sensor) {
    if (addr >= MAX_BOARDS) return false;
    uint16_t bits = (src == ALARM_SRC_SWITCHBACK) ? s_polarity_sw[addr]
                                                  : s_polarity_pk[addr];
    return (bits >> sensor) & 1u;
}

void alarms_set_inverted(alarm_src_t src, uint8_t addr, uint8_t sensor,
                         bool inverted) {
    if (addr >= MAX_BOARDS) return;
    uint16_t *bits = (src == ALARM_SRC_SWITCHBACK) ? &s_polarity_sw[addr]
                                                   : &s_polarity_pk[addr];
    if (inverted) *bits |= (1u << sensor);
    else          *bits &= ~(1u << sensor);
    nvs_handle_t h;
    if (nvs_open(NVS_NS, NVS_READWRITE, &h) == ESP_OK) {
        char k[16];
        snprintf(k, sizeof(k),
                 (src == ALARM_SRC_SWITCHBACK) ? "sw_pol_%d" : "pk_pol_%d",
                 addr);
        nvs_set_u16(h, k, *bits);
        nvs_commit(h); nvs_close(h);
    }
}

void alarms_get_label(alarm_src_t src, uint8_t addr, uint8_t sensor,
                      char *out_buf, size_t buf_sz) {
    if (!out_buf || buf_sz == 0 || addr >= MAX_BOARDS) {
        if (out_buf && buf_sz) out_buf[0] = '\0';
        return;
    }
    const char *stored = NULL;
    if (src == ALARM_SRC_SWITCHBACK && sensor < SW_SENSORS) {
        stored = s_label_sw[addr][sensor];
    } else if (src == ALARM_SRC_PICKET && sensor < PK_SENSORS) {
        stored = s_label_pk[addr][sensor];
    }
    if (stored && stored[0]) {
        strncpy(out_buf, stored, buf_sz - 1);
        out_buf[buf_sz - 1] = '\0';
    } else {
        /* Default: "SB0 · S3" style. Middle-dot glyph is in Roboto Regular
         * subset. Caller often prefers this rendering when no custom
         * label is set. */
        snprintf(out_buf, buf_sz,
                 (src == ALARM_SRC_SWITCHBACK) ? "SB%u \xc2\xb7 S%u"
                                               : "PK%u \xc2\xb7 S%u",
                 (unsigned)addr, (unsigned)(sensor + 1));
    }
}

void alarms_set_label(alarm_src_t src, uint8_t addr, uint8_t sensor,
                      const char *label) {
    if (addr >= MAX_BOARDS) return;
    char *slot = NULL;
    if (src == ALARM_SRC_SWITCHBACK && sensor < SW_SENSORS) {
        slot = s_label_sw[addr][sensor];
    } else if (src == ALARM_SRC_PICKET && sensor < PK_SENSORS) {
        slot = s_label_pk[addr][sensor];
    }
    if (!slot) return;
    if (label && label[0]) {
        strncpy(slot, label, ALARM_LABEL_MAX - 1);
        slot[ALARM_LABEL_MAX - 1] = '\0';
    } else {
        slot[0] = '\0';
    }
    nvs_handle_t h;
    if (nvs_open(NVS_NS, NVS_READWRITE, &h) == ESP_OK) {
        char k[16];
        snprintf(k, sizeof(k),
                 (src == ALARM_SRC_SWITCHBACK) ? "sw_lbl_%d_%d"
                                               : "pk_lbl_%d_%d",
                 addr, sensor);
        if (slot[0]) nvs_set_str(h, k, slot);
        else         nvs_erase_key(h, k);
        nvs_commit(h); nvs_close(h);
    }
}
