# Lessons Learned — Elecrow CrowPanel Advance 10.1" ESP32-P4 (Fireside)

This document captures what we learned bringing TrailCurrent Fireside up on
the Elecrow CrowPanel Advance 10.1" HMI display (ESP32-P4, 1024x600), the
feedback Elecrow support gave us, what turned out to be true, and how every
problem was actually solved. It is written for a general audience — you do
not need to be a firmware engineer to follow it.

---

## 1. What this board is (and is not)

The CrowPanel Advance 10.1" is a touchscreen computer built around the
ESP32-P4 chip. Here is what is actually on the board and what to expect
from each piece:

| Part | What it is | What to know |
|---|---|---|
| Display | 10.1" 1024x600 LCD (EK79007 controller, MIPI-DSI) | Works well, but needs two on-chip power regulators turned on in software before it will even initialize (see §4). |
| Touch | GT911 capacitive touch controller | Genuinely good hardware. We measured it: about 1 pixel of jitter. Every touch problem we had was our own software (see §3). |
| WiFi | A second chip (ESP32-C6) connected over an internal SDIO link | The P4 has no radio of its own. WiFi works, but the link between the two chips has memory-related tuning traps (see §5). |
| Microphone | One small PDM microphone on the board | Good enough for push-to-talk at arm's length. NOT good enough for "hey assistant" style far-field voice — Elecrow confirmed this and we agree. |
| Speaker | Class-D amplifier fed over I2S | Works. Needs a power-enable pin toggled or it squeals when idle. |
| Battery gauge | None. A helper chip (STC8) measures battery **voltage** only | There is no true fuel gauge (no current sensing). Any percentage shown is an estimate computed from voltage, and it will always be approximate — worse under load or while charging. This is a hardware limitation; Elecrow confirmed it and no software can fully fix it. |
| SD card slot | Standard microSD over SDMMC (1-bit) | Works. We use it to load secrets (Peregrine voice server address + token) from `environment.conf`, same card as the Waveshare build. |
| Flash / RAM | **16 MB** flash, 32 MB PSRAM | The flash is 16 MB, not 32. Building with the wrong size makes the board boot-loop with a flash-probe error. |

## 2. What Elecrow support told us (summarized)

We reported three problems: sluggish touch, inaccurate battery percentage,
and missing/poor AI voice. Their reply, in plain terms:

1. **Touch** — "The hardware is a good capacitive panel. Your code is the
   problem." They pointed at things in our firmware: flash writes inside
   UI callbacks, permanently-enabled debug logging, a giant scroll
   threshold (which made scrolling feel like a resistive screen), a long
   long-press delay, an unpinned library version, and our decision to poll
   the touch chip instead of using its interrupt pin. They suggested
   starting from their sample code. They also noted their support only
   covers their hardware and samples — beyond that we're on our own.
2. **Battery** — "There is no fuel gauge on this board." The charger chip
   only charges; the helper chip only reads voltage. Percentages will
   always be estimates. They suggested showing voltage and calling the
   percentage an estimate. They passed our feedback to their hardware team.
3. **Voice** — "Your firmware branch has no microphone code at all," which
   was true. They gave the correct microphone wiring (PDM mic: clock on
   GPIO24, data on GPIO26, 16 kHz 16-bit mono, ~35.5 Hz high-pass filter)
   and noted the single onboard mic is unsuitable for far-field voice.

**How much of that was right?** The battery and microphone assessments were
completely correct. The touch assessment was right that it was our
software, but the specific causes were only partially what they listed —
see next section. Their suggestion to run their own sample program first
was excellent advice and is what cracked the case.

## 3. Touch: what was actually wrong, and the proof

### The test that settled it

We built a small test firmware (in `DOCS/touch-baseline/`) that uses
**Elecrow's own touch code, byte-for-byte**, and logs every press with
statistics. We also swept the backlight brightness (a classic source of
electrical noise into touch panels) through off / dim / medium / full.

Result over 91 presses: **median finger drift of 1 pixel, at every
brightness level, with zero presses misread**. The "20–140 pixel drift"
our firmware had measured earlier does not exist at the hardware level.
The panel, the touch chip, and the power supply are all clean.

### So what was actually wrong?

1. **Writing to flash memory inside touch callbacks.** Every tick of the
   brightness or volume slider saved the value to flash storage — dozens
   of times per drag. Flash writes freeze the UI thread, which also reads
   the touch chip, so touches got delayed and appeared to "teleport."
   *Fix:* a small "settings store" that saves values in the background
   about one second after you stop changing them. One flash write per
   adjustment instead of dozens, and never on the UI thread.
2. **Band-aids hiding the real bug.** Earlier attempts to fix the
   symptoms had raised the scroll threshold from 10 px to 150 px (≈32 mm
   of finger travel before a scroll registers — the "sticky resistive
   screen" feel), stretched long-press to 800 ms, and added a filter that
   second-guessed the touch coordinates. With the real cause fixed, all
   of these were removed. Scrolling feels like a phone again.
3. **Debug instrumentation left on in production.** Stall detectors and
   per-event logging ran on every build. They now only compile in when a
   dedicated diagnostic option is switched on.
4. **The interrupt-pin question was a red herring.** Elecrow implied we
   should use the touch chip's interrupt line (GPIO42) — but their own
   sample code doesn't use it either; it polls, just like ours, only
   slower. Polling at 10 ms is not the problem and never was.
5. **Two bonus bugs found during instrumented testing:** the MQTT client
   crashed the whole board (reboot loop) whenever the broker was
   unreachable, because it tried to shut itself down from inside its own
   task; and the firmware opened **two** MQTT connections at boot because
   two different startup paths raced each other. Both fixed.

After the fixes, instrumented measurement showed **102 of 102 taps**
(including rapid ones) delivered to the right on-screen control and
published over MQTT within ~50 ms of finger-down.

## 4. Getting the display to turn on at all

The vendor's C examples quietly rely on things they never show you doing.
If your screen stays black, check these in order:

1. **Two on-chip power regulators must be enabled in software** before
   the display link (MIPI-DSI) will initialize: LDO3 at 2.5 V powers the
   DSI PHY (without it, the display driver hangs forever during init),
   and LDO4 at 3.3 V powers the LCD. The vendor's C lessons don't do
   this; our `main.c` does.
2. **The backlight has TWO control pins.** GPIO31 is the brightness (PWM)
   pin everyone knows about. But there is also GPIO29, which switches the
   backlight's power rail through a transistor — drive it LOW or no
   amount of PWM will light the panel. Only the vendor's *MicroPython*
   demo does this; their C lessons never mention it.
3. **The flash size must be 16 MB** in the build configuration. With
   32 MB configured, the chip detects the mismatch at boot and resets
   forever.

## 5. Configuration drift — the silent killer

ESP-IDF (the build system) keeps two configuration files: a hand-written
`sdkconfig.defaults` (checked into git) and a generated `sdkconfig` (not
in git). The generated file is only created *from* the defaults when it
doesn't exist — changing the defaults later does nothing until the
generated file is deleted and rebuilt.

Over months of development, several critical values had been set only in
the generated file (via menuconfig) and never copied back to the
defaults. When the generated file was finally regenerated, each of those
values silently reverted and produced a different boot failure. Every one
is now pinned in the tracked defaults with a comment:

| Setting | Why it matters |
|---|---|
| Flash size = 16 MB | Wrong size = instant boot loop ("flash probe failed"). |
| Main task stack = 10240 | Below ~7 KB the stack can be placed in a special fast-memory region (TCM) that the flash driver refuses to run from — boot loop with a cryptic assert. |
| ESP-Hosted SDIO queues = 12/12 | The WiFi link between the P4 and C6 pre-allocates memory at boot. At the default 20/20, adding the SD-card drivers to the firmware tips internal memory over the edge — boot loop in `sdio_mempool_create`. |
| FATFS long filenames ON | Without it, the SD card file `environment.conf` cannot be opened by name (the filesystem only sees its DOS 8.3 short name) and voice config silently never loads. |

**Rule of thumb: any config value you care about MUST live in
`sdkconfig.defaults`, and after changing defaults you must delete
`sdkconfig` and rebuild.**

## 6. Voice (push-to-talk) — what works and how

The board now has working push-to-talk voice, using the same architecture
as the Waveshare build:

- **No AI runs on the display.** Hold the TALK button, speak, release.
  The recording is sent over WiFi to Peregrine (our local voice-assistant
  box), which does speech-to-text, thinks, and sends back spoken audio.
  The display is just a microphone and a speaker. This sidesteps
  Elecrow's (correct) point that the P4 can't do meaningful on-device AI
  and the single mic can't do far-field wake words — for push-to-talk at
  arm's length, one mic is plenty.
- **Microphone:** the onboard PDM mic, exactly per Elecrow's letter —
  clock GPIO24, data GPIO26, 16 kHz 16-bit mono, hardware high-pass at
  35.5 Hz. Their `Lesson11-Playback_After_Recording` example has the
  working reference code.
- **Speaker:** I2S on GPIO21/22/23 with a power-enable on GPIO30 (kept
  off when idle or the amplifier squeals). The speaker and mic must be on
  different I2S controllers — mic on I2S0 (PDM only exists there),
  speaker on I2S1.
- **Configuration comes from the SD card**, same file as the Waveshare
  device: `environment.conf` with `PEREGRINE_URL=` and
  `PEREGRINE_VOICE_TOKEN=` lines. No card or no token → the TALK button
  simply stays hidden.
- **The "male voice" bug:** our first version played replies at the
  microphone's 16 kHz rate, but the server actually answers at whatever
  rate *it* is configured for (22.05 kHz on ours, to match the Waveshare
  client). Playing 22 kHz audio at 16 kHz slows it ~27% and drops the
  pitch about 4 semitones — the assistant's voice turned male. The fix:
  the firmware now reads the sample rate from the reply's own WAV header
  and sets the speaker clock to match, so it works with any server
  configuration.

## 7. The clock that ignored its timezone (bonus bug)

After everything above, the on-screen clock stubbornly showed a time ~6
hours off no matter what timezone was selected. The timezone system
itself (selection, saving, applying) was proven correct with a logging
probe. The real culprit: our GPS module (Milepost) broadcasts the current
time in UTC over MQTT, and the display converted it using a C function
(`mktime`) that assumes its input is **local** time. With Chicago
selected, every GPS message set the system clock 6 hours fast —
continuously overwriting the correct internet-synced (SNTP) time within
seconds. The fix converts GPS time with timezone-independent math and
only adjusts the clock when it has actually drifted.

**Note:** the Waveshare build (`main` branch) shares this file and has
the same bug. Port `utc_fields_to_epoch()` there.

## 8. Battery — accepted limitation

Per Elecrow (and our own reading of the schematic): the board measures
battery voltage only. There is no current sensor, so true state-of-charge
cannot be computed. Our firmware estimates a percentage from a lithium-ion
voltage curve with smoothing. Treat the number as an estimate — it will
read low under heavy load and high while charging. If accurate gauging
ever matters, it needs different hardware (a fuel-gauge chip).

## 9. Quick reference — the traps in one list

1. Screen black → did you enable LDO3 (2.5 V) + LDO4 (3.3 V)?
2. Backlight dark with PWM running → drive GPIO29 LOW (power rail).
3. Boot loop "flash probe failed" → config says 32 MB, board has 16 MB.
4. Boot loop `sdio_mempool_create (buf_mp_g)` → shrink ESP-Hosted SDIO
   queues to 12/12 (especially after adding SD/FATFS).
5. Boot loop `esp_task_stack_is_sane` on first flash write → main task
   stack too small (landed in TCM); use ≥ 10240.
6. SD file won't open by name → enable FATFS long filenames.
7. Touch feels laggy/sticky → look for flash writes or logging on the
   UI thread before blaming the hardware; the GT911 is good.
8. Voice replies pitched down/up → play at the rate in the reply's WAV
   header, not an assumed one.
9. Clock off by your UTC offset → something converted UTC with
   `mktime()`; use timezone-independent conversion.
10. Changed `sdkconfig.defaults` but nothing happened → delete
    `sdkconfig` and rebuild; defaults only seed new files.
