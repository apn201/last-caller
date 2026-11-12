"""ANSI/MENUS/GALLERY.ANS: a viewport frame.

MENUS.INI [GALLERY]: row=8 col=4 columns=1 width=40 rows=4 (x 4..43, inside the
COMMANDS box). DRAFT text: the camera readout is a draft.
"""
import math
from _common import *


def porthole(c, x, y, size, view):
    """A round window: bolted bezel, `view(layer, size)` paints what is outside."""
    layer = PixelLayer(size, size)
    cx = cy = (size - 1) / 2.0
    r = size / 2.0
    view_layer = PixelLayer(size, size)
    view(view_layer, size)

    def px(i, j):
        d = math.hypot(i - cx, j - cy)
        if d > r:
            return None
        if d > r - 1.6:
            return WHITE if (i + j) < size * 0.9 else GREY
        if d > r - 3.2:
            ang = math.atan2(j - cy, i - cx)
            bolt = abs(((ang / (2 * math.pi)) * 16) % 1 - 0.5) < 0.18
            return LCYAN if bolt and d > r - 2.7 else DGREY
        if d > r - 4.2:
            return GREY if (i + j) < size else DGREY
        v = view_layer.get(i, j)
        return v if v is not None else BLACK
    layer.paint(px)
    layer.flush(c, x, y)


def build():
    c = Canvas(80, 22)
    frame(c, "HALCYON // OBSERVATION")
    header(c, "GALLERY")
    sep(c, 4, x1=45)
    # the options live in a box on the left
    c.box(2, 6, 42, 7, "single", DGREY)
    c.text(4, 6, "┤", DGREY)
    c.text(5, 6, " COMMANDS ", CYAN)
    c.text(15, 6, "├", DGREY)
    slot_area(c, 3, 7, 40, 5)
    # left lower: camera readout and ELIZA
    c.text(3, 14, "CAMERA 2   PORT VIEW", GREY)
    c.text(3, 15, "EXPOSURE   AUTO", DGREY)
    c.text(3, 16, "FILTER     NONE", DGREY)
    c.text(3, 17, "PICTURES   KEPT FOR THE CREW", DGREY)
    c.hline(3, 18, 40, "─", DGREY)
    lights(c, 3, 19, "11101", on=LCYAN)
    c.text(15, 19, "ELIZA WATCHES THE WINDOW", AMBER)

    def view(layer, size):
        import random
        rnd = random.Random(4)
        for _ in range(26):
            layer.set(rnd.randrange(size), rnd.randrange(size // 2), rnd.choice((WHITE, GREY, DGREY, LCYAN)))
        earth(layer, size * 0.45, size * 1.35, size * 0.72, sun=(-0.7, -0.6), seed=5.0)
    porthole(c, 46, 3, 32, view)
    c.text(47, 19, "WINDOW 2  OBSERVATION DECK", DGREY)
    c.text(59, 21, "╡", DGREY)
    c.text(60, 21, TAG, DGREY)
    c.text(60 + len(TAG), 21, "╞", DGREY)
    return [c.save("ANSI/MENUS/GALLERY.ANS")]
