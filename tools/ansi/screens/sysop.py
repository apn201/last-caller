"""ANSI/MENUS/SYSOP.ANS: ELIZA's core, in amber.

Header art only: the board prints the SysOp command lines below it
(src/bbs/sysop.h sysopMenu), so this screen is 15 rows. DRAFT text: the
status lines are drafts.
"""
import math
from _common import *


def core(c, x, y, w=28, h=24):
    layer = PixelLayer(w, h)
    cx, cy = (w - 1) / 2.0, (h - 1) / 2.0

    def px(i, j):
        dx, dy = i - cx, j - cy
        d = math.hypot(dx, dy)
        a = (math.atan2(dy, dx) + math.pi) / (2 * math.pi)
        if 10.2 <= d <= 12.2:                       # outer ring of lights
            seg = a * 24
            if seg % 1 < 0.15 or seg % 1 > 0.85:
                return None
            if int(seg) in (5, 6, 17):
                return DGREY
            return YELLOW if (-dx - dy) / max(d, 1e-6) > 0.3 else AMBER
        if 7.4 <= d <= 8.0:                         # thin inner ring
            return AMBER if (a * 48) % 2 < 1.4 else None
        if 4.2 <= d <= 5.0:
            return DGREY
        if d < 3.2:                                 # the core itself
            return WHITE if d < 1.2 else YELLOW if d < 2.4 else AMBER
        return None
    layer.paint(px)
    layer.flush(c, x, y)


def build():
    c = Canvas(80, 15)
    frame(c, "HALCYON // ELIZA CORE", accent=YELLOW)
    c.stars(1, 1, 78, 13, seed=21, density=0.0)
    core(c, 5, 1)
    top = [YELLOW, YELLOW, YELLOW, AMBER, AMBER]
    header(c, "ELIZA", top, None, x=40, y=2)
    c.text(40, 5, "SYSOP CORE", GREY)
    rows = [("STATUS", "NOMINAL", YELLOW), ("MODE", "HELPFUL", YELLOW), ("TEMPER", "CALM", AMBER),
            ("FAULTS", "NONE REPORTED", AMBER)]
    for n, (k, v, col) in enumerate(rows):
        c.text(40, 7 + n, k, DGREY)
        c.text(48, 7 + n, v, col)
    lights(c, 40, 12, "1111111A", on=YELLOW)
    c.text(58, 12, "LOAD", DGREY)
    bar(c, 63, 12, 10, 0.2, AMBER, DGREY)
    c.text(59, 14, "╡", DGREY)
    c.text(60, 14, TAG, DGREY)
    c.text(60 + len(TAG), 14, "╞", DGREY)
    return [c.save("ANSI/MENUS/SYSOP.ANS")]
