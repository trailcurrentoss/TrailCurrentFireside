# Phase 0 — Touch Hardware Baseline Test

## RESULT (2026-08-03): HARDWARE IS CLEAN — see verdict below

91 presses on the vendor touch pipeline, USB power, DSI streaming, panel
lit (raw capture: `results_2026-08-03_usb.log`):

| backlight | presses | median drift | median max-jump |
|---|---|---|---|
| 0%   | 14 | 1.0 px | 1.0 px |
| 10%  | 26 | 1.0 px | 1.0 px |
| 50%  | 26 | 1.0 px | 1.0 px |
| 100% | 25 | 1.0 px | 1.0 px |

Zero short presses (of 91) exceeded LVGL's default 10 px scroll limit.
Stationary holds show ±1 px. Drags track smoothly (≤38 px per 20 ms
sample). No backlight-duty dependence. **The 20–140 px "drift" Fireside
measured does not exist at the hardware level — it is produced by the
Fireside software stack** (prime suspects: nvs_commit calls blocking the
LVGL task during slider drags, permanent diagnostic logging, and merged
fast-tap sessions being interpreted as one moving press).

Board bring-up facts learned (not in vendor C lessons):
- P4 on-chip LDO3 (2.5 V) must power the MIPI DSI PHY or
  `esp_lcd_new_dsi_bus` spins forever; LDO4 (3.3 V) powers LCD VCC.
  Fireside does this in `main/main.c`.
- GPIO29 low switches the backlight power rail (Q11 P-MOSFET, net
  `LCD_BK_POWER`); GPIO31 is the boost EN/PWM. Vendor MicroPython demo
  drives GPIO29; the C lessons rely on default pin state.
- The vendor V1.2 touch example does NOT use the GT911 INT for reads —
  it passes GPIO42 to the driver (I2C address select at reset) and polls
  every ~60 ms from a task.

Decides whether the 20–140 px coordinate drift seen under the Fireside
firmware is a **hardware** problem (GT911 / glass / power-supply noise —
Elecrow's ball) or a **software** problem in the Fireside stack (our
ball, fixed in Phase 1/2).

The touch pipeline is Elecrow's own, byte-identical: `peripheral/` is
copied unmodified from `DOCS/elecrow-p4/example/V1.2/idf-code/
Lesson05-Touchscreen`. Only `main/main.c` is ours — it polls the vendor
`get_coor()` API exactly like the vendor demo, logs per-press drift
statistics, and sweeps the backlight PWM (vendor Lesson07 values:
GPIO31, LEDC, 30 kHz) through OFF → 10% → 50% → 100% to expose any
supply-noise coupling. The MIPI-DSI panel is never initialised — the
screen stays dark (or lit but blank); this test is serial-console only.

## Build & flash

```bash
cd DOCS/touch-baseline
source ~/esp/v5.5.2/esp-idf/export.sh
idf.py -p /dev/ttyACM0 flash monitor
```

(Already built once on this machine; `idf.py set-target esp32p4` was
run and `build/` is populated, so flash+monitor is all that's needed.)

## Bench protocol (~6 minutes for one full sweep)

The firmware runs 90-second stages, each announced by a banner on the
serial console with the current backlight duty. **In every stage**, do
roughly:

- ~10 quick taps (normal fingertip, the way you'd tap a button)
- 3 press-and-holds, ~2 s each, finger deliberately stationary
- 2 slow one-finger drags across the glass

Spread them around the screen — corners, edges, center. Let the sweep
run through all four stages at least once (twice is better). Then:

- Repeat one full sweep on **battery power** with USB serial still
  attached if possible; if not, at least note which supply was used.
- Capture the whole session to a file:

```bash
idf.py -p /dev/ttyACM0 monitor | tee /tmp/touch_baseline_usb.log
```

## Reading the results

Each press/release prints one line:

```
SUMMARY,press=12,bl_pct=50,dur_ms=180,samples=9,x0=512,y0=300,...,drift=4.2,maxjump=3.1,path=12.4,bbox=5x4
```

- `drift` — straight-line px between first and last sample of the
  press. This is the number Fireside measured at 20–140 px.
- `maxjump` — largest single-sample step (lift-off teleport detector).
- `bbox` — total wander box while pressed.

Run `python3 analyze.py <logfile>` for per-stage aggregates.

Verdict table:

| Observation | Meaning |
|---|---|
| drift < ~10 px everywhere | Hardware is fine → drift was manufactured by the Fireside stack (NVS commits blocking LVGL, logging stalls). Proceed with Phase 1/2 and remove the band-aids. |
| drift low at 0% backlight, grows with duty | Backlight PWM/supply noise coupling → hardware finding, send log to Elecrow. |
| drift 20–140 px even here | GT911/module defect → hardware finding, send log to Elecrow with their own sample code as the reproducer. |
| drift only on battery | Supply-rail noise under battery boost → hardware finding for Elecrow. |
