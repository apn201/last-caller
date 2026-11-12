"""ANSI/MENUS/SYSOP.ANS: ELIZA's core, in amber.

Header art only: the board prints the SysOp command lines below it
(src/bbs/sysop.h sysopMenu), so this screen is 15 rows. DRAFT text: the
status lines are drafts.
"""
import math
from _common import *


def core(c, x, y, w=30, h=26):
    """ELIZA's core as concentric rings, lit from the upper left: an outer
    ring of lights, a ticked rim, a broken amber band, a steel ring and a hot
    centre, with a halo around it."""
    layer = PixelCanvas(w, h)
    cx, cy = w / 2.0, h / 2.0

    def px(i, j):
        dx, dy = i + 0.5 - cx, j + 0.5 - cy
        d = math.hypot(dx, dy)
        a = (math.atan2(dy, dx) + math.pi) / (2 * math.pi)
        lit = (-dx - dy) / max(d, 1e-6)            # +1 toward the upper left
        if 11.2 <= d <= 12.9:                       # outer ring of lights
            seg = a * 24
            if seg % 1 < 0.2 or seg % 1 > 0.8:
                return None
            if int(seg) in (5, 6, 17):
                return DGREY                        # three dead lights
            return YELLOW if lit > 0.35 else AMBER if lit > -0.5 else DGREY
        if 9.3 <= d <= 10.0:                        # thin rim
            return GREY if lit > 0.2 else DGREY
        if 6.6 <= d <= 8.2:                         # amber band
            return YELLOW if lit > 0.45 else AMBER
        if 4.2 <= d <= 5.1:                         # steel ring
            return WHITE if lit > 0.6 else GREY if lit > -0.2 else DGREY
        if d < 2.8:                                 # the core itself
            dd = math.hypot(dx + 0.8, dy + 0.8)
            return WHITE if dd < 1.0 else YELLOW if dd < 1.9 else AMBER
        return None
    layer.paint(px)
    layer.render(c, x, y)
    c.glow(x + w / 2.0, y + h / 2.0, 13.2, 15.6, AMBER, inner=13.2)


def build():
    c = Canvas(80, 15)
    frame(c, "HALCYON // ELIZA CORE", accent=YELLOW)
    c.stars(1, 1, 78, 13, seed=21, density=0.0)
    core(c, 5, 1)
    top = [YELLOW, YELLOW, YELLOW, AMBER, AMBER]
    header(c, "ELIZA", top, DGREY, x=40, y=2)
    c.text(40, 5, "SYSOP CORE", GREY)
    rows = [("STATUS", "NOMINAL", YELLOW), ("MODE", "HELPFUL", YELLOW), ("TEMPER", "CALM", AMBER),
            ("FAULTS", "NONE REPORTED", AMBER)]
    for n, (k, v, col) in enumerate(rows):
        c.text(40, 7 + n, k, DGREY)
        c.text(48, 7 + n, v, col)
    lights(c, 40, 12, "1111111A", on=YELLOW)
    c.text(58, 12, "LOAD", DGREY)
    bar(c, 63, 12, 10, 0.2, AMBER, DGREY)
    # decay: one status light that went out, one dim glow cell
    c.put(46, 12, "Â·", DGREY)
    c.text(59, 14, "â•¡", DGREY)
    c.text(60, 14, TAG, DGREY)
    c.text(60 + len(TAG), 14, "â•ž", DGREY)
    return [c.save("ANSI/MENUS/SYSOP.ANS")]
