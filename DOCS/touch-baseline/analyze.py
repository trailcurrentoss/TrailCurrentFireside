#!/usr/bin/env python3
"""Aggregate SUMMARY lines from a touch-baseline capture.

Usage: python3 analyze.py /tmp/touch_baseline_usb.log
"""
import re
import sys
from collections import defaultdict

if len(sys.argv) != 2:
    sys.exit(__doc__)

rows = []
pat = re.compile(r"SUMMARY,(\S+)")
for line in open(sys.argv[1], errors="replace"):
    m = pat.search(line)
    if not m:
        continue
    fields = dict(kv.split("=", 1) for kv in m.group(1).split(","))
    rows.append(fields)

if not rows:
    sys.exit("no SUMMARY lines found")

by_stage = defaultdict(list)
for r in rows:
    by_stage[int(r["bl_pct"])].append(r)

def fnum(r, k):
    return float(r[k])

print(f"{len(rows)} presses total\n")
print(f"{'backlight':>9} {'n':>4} {'drift med':>9} {'drift max':>9} "
      f"{'maxjump med':>11} {'maxjump max':>11} {'dur med ms':>10}")
for pct in sorted(by_stage):
    rs = by_stage[pct]
    drifts = sorted(fnum(r, "drift") for r in rs)
    jumps = sorted(fnum(r, "maxjump") for r in rs)
    durs = sorted(fnum(r, "dur_ms") for r in rs)
    med = lambda v: v[len(v) // 2]
    print(f"{pct:>8}% {len(rs):>4} {med(drifts):>9.1f} {drifts[-1]:>9.1f} "
          f"{med(jumps):>11.1f} {jumps[-1]:>11.1f} {med(durs):>10.0f}")

# Presses that would have broken a tap under LVGL's default 10 px limit
bad = [r for r in rows if fnum(r, "drift") > 10 and fnum(r, "dur_ms") < 400]
print(f"\nshort presses (<400 ms) with drift > 10 px: {len(bad)}")
for r in bad[:20]:
    print(f"  press={r['press']} bl={r['bl_pct']}% drift={r['drift']} "
          f"maxjump={r['maxjump']} dur={r['dur_ms']}ms "
          f"({r['x0']},{r['y0']})->({r['x1']},{r['y1']})")
