"""ANSI/MENUS/DOORS.ANS: the airlocks.

MENUS.INI [DOORS]: row=8 col=4 columns=1 width=40 rows=4. DRAFT text: the
hatch status lines are drafts.
"""
import math
from _common import *


def hatch(c, x, y, size):
    """A round pressure hatch with a wheel, lit from the upper left."""
    layer = PixelLayer(size, size)
    cx = cy = (size - 1) / 2.0
    r = size / 2.0

    def px(i, j):
        dx, dy = i - cx, j - cy
        d = math.hypot(dx, dy)
        if d > r:
            return None
        lit = (-dx - dy) / max(d, 1e-6)
        if d > r - 1.5:
            return WHITE if lit > 0.2 else GREY
        if d > r - 3.5:                       # frame ring with bolts
            ang = math.atan2(dy, dx)
            bolt = abs(((ang / (2 * math.pi)) * 12) % 1 - 0.5) < 0.2
            if bolt and d > r - 2.8:
                return LCYAN
            return GREY if lit > -0.2 else DGREY
        # the leaf
        if d < 2.0:
            return WHITE                      # hub
        spoke = (abs(dx) < 1.0 or abs(dy) < 1.0 or abs(abs(dx) - abs(dy)) < 0.9)
        if spoke and d < r * 0.62:
            return GREY if lit > -0.1 else DGREY
        if abs(d - r * 0.62) < 1.0:
            return WHITE if lit > 0.3 else GREY
        return DGREY if lit < 0.0 else BLUE
    layer.paint(px)
    layer.flush(c, x, y)


def build():
    c = Canvas(80, 22)
    frame(c, "HALCYON // AIRLOCKS")
    header(c, "AIRLOCKS")
    sep(c, 4, x1=50)
    c.box(2, 6, 42, 7, "single", DGREY)
    c.text(4, 6, "┤", DGREY)
    c.text(5, 6, " COMMANDS ", CYAN)
    c.text(15, 6, "├", DGREY)
    slot_area(c, 3, 7, 40, 5)
    for n, (k, v, col) in enumerate((("AIRLOCK 1", "SEALED", LCYAN), ("AIRLOCK 2", "SEALED", LCYAN),
                                      ("OUTER DOOR", "CLOSED", LCYAN), ("PRESSURE", "OK", LCYAN))):
        c.text(3, 14 + n, k, GREY)
        c.text(15, 14 + n, v, col)
    c.text(3, 18, "NOBODY OUTSIDE", AMBER)
    hatch(c, 51, 1, 28)
    hazard(c, 1, 19, 78, 2)
    c.put(40, 19, " ")
    c.put(41, 20, " ")
    c.text(59, 21, "╡", DGREY)
    c.text(60, 21, TAG, DGREY)
    c.text(60 + len(TAG), 21, "╞", DGREY)
    return [c.save("ANSI/MENUS/DOORS.ANS")]
