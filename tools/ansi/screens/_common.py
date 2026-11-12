"""Shared pieces for the HALCYON screens: the artist tag, the ship, Earth,
ELIZA's amber ring and the console frames used by the menus.

Palette rules (see .kiro/steering/ansi-art.md): black ground, greys for
structure, cyan / steel blue for the ship, amber for ELIZA, red only for
a fault, an alarm or a dead crew member.
"""
import math
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
from build import *  # noqa: F401,F403,E402

TAG = "·halcyon·crew·98·"


def artist_tag(c, x, y, fg=DGREY):
    c.text(x, y, TAG, fg)


# ------------------------------------------------------------------ Earth

def _noise(x, y, seed=0.0):
    return (math.sin(x * 0.23 + math.sin(y * 0.31 + seed) * 2.1) +
            math.sin(y * 0.47 - x * 0.11 + seed * 1.7) * 0.8 +
            math.sin((x + y) * 0.17 + seed * 0.5) * 0.6)


def earth(layer, cx, cy, r, sun=(-0.8, -0.45), night_lights=True, seed=2.0, bias=0.0):
    """A planet seen from orbit, lit from `sun` (a direction). Day side steel
    blue with cloud, a thin cyan atmosphere, night side black with amber
    city lights."""
    sl = math.hypot(*sun)
    sx, sy = sun[0] / sl, sun[1] / sl

    def px(x, y):
        dx, dy = x - cx, y - cy
        d = math.hypot(dx, dy)
        if d > r + 1.5:
            return None
        nx, ny = dx / max(d, 1e-6), dy / max(d, 1e-6)
        lit = nx * sx + ny * sy
        if d > r:
            if d <= r + 0.7:
                return LCYAN if lit > -0.2 else (CYAN if lit > -0.6 else None)
            return CYAN if lit > 0.3 else (BLUE if lit > -0.1 else None)
        z = math.sqrt(max(0.0, 1 - (d / r) ** 2))
        lit = -(dx / r) * 1.1 - (dy / r) * 0.25 + bias
        n = _noise(x, y, seed)
        edge = r - d
        if lit > 0.0:
            if edge < 1.0:
                return LCYAN if lit > 0.15 else CYAN
            if n > 1.25:
                return WHITE if lit > 0.45 else GREY
            if n > 0.85:
                return GREY if lit > 0.3 else DGREY
            if lit > 0.5 and n > -0.2:
                return CYAN
            return BLUE
        # night side, with a dithered terminator
        if lit > -0.08 and (x + y) % 2 == 0:
            return BLUE
        if edge < 1.0 and lit > -0.4:
            return BLUE
        if night_lights and n > 0.1 and (x * 7 + y * 13) % 6 == 0:
            return AMBER
        if night_lights and n > 1.0 and (x * 5 + y * 3) % 7 == 0:
            return YELLOW
        return None
    layer.paint(px)


_DIM = {WHITE: GREY, LCYAN: CYAN, YELLOW: AMBER, GREY: DGREY, CYAN: BLUE, LBLUE: BLUE,
        BLUE: BLACK, AMBER: BLACK, DGREY: BLACK}


def fade(layer, x0, steps_per=6):
    """Darken pixels from column x0 on, one more step every `steps_per`
    columns: a thing sliding into shadow."""
    for j in range(layer.h):
        for i in range(x0, layer.w):
            c = layer.p[j][i]
            if c is None:
                continue
            for _ in range(1 + (i - x0) // steps_per):
                c = _DIM.get(c, c)
            layer.p[j][i] = c if c != BLACK else None


# ------------------------------------------------------------------- ship

def ship(layer, ox, oy, lights=True):
    """HALCYON in silhouette, lit from the upper left: a solar wing above and
    below a mast, the hab cylinder, a docking node and a service module.
    About 22 x 14 pixels. One window is lit."""
    s = layer.set

    def rect(x, y, w, h, c):
        layer.rect(ox + x, oy + y, w, h, c)

    for top in (0, 11):
        rect(4, top, 14, 3, BLUE)
        for gx in range(4, 18, 3):
            rect(gx, top, 1, 3, CYAN)
        rect(4, top + 1, 14, 1, BLUE)
        for gx in range(4, 18):
            s(ox + gx, oy + top if top == 0 else oy + top + 2, LCYAN if top == 0 else CYAN)
    rect(10, 3, 2, 8, GREY)                 # mast
    rect(10, 3, 1, 8, WHITE)
    rect(15, 5, 4, 4, GREY)                 # service module
    rect(15, 5, 4, 1, WHITE)
    rect(15, 8, 4, 1, DGREY)
    rect(19, 6, 2, 2, DGREY)
    rect(5, 4, 10, 6, GREY)                 # hab cylinder
    rect(5, 4, 10, 1, WHITE)
    rect(5, 8, 10, 2, DGREY)
    for gx in (8, 12):
        rect(gx, 4, 1, 6, DGREY)
    rect(2, 5, 3, 4, GREY)                  # docking node
    rect(2, 5, 3, 1, WHITE)
    rect(2, 8, 3, 1, DGREY)
    rect(0, 6, 2, 2, DGREY)
    for i in range(4):                      # dish
        s(ox + 11 + i, oy + 2 - (i // 2), WHITE if i < 2 else GREY)
    if lights:
        s(ox + 7, oy + 6, YELLOW)


# ------------------------------------------------------------- ELIZA ring

def eliza_ring(layer, cx, cy, r_out=6.6, r_in=4.2, segs=16, off=(), dim=()):
    """ELIZA's motif: a ring of amber lights. Bright toward the upper left,
    dimmer on the far side. `off` are dead lights, `dim` are fading ones."""
    def px(x, y):
        dx, dy = x - cx, y - cy
        d = math.hypot(dx, dy)
        if d > r_out or d < r_in:
            return None
        a = (math.atan2(dy, dx) + math.pi) / (2 * math.pi) * segs
        idx = int(a) % segs
        frac = a - int(a)
        if frac < 0.12 or frac > 0.88:
            return None                       # gap between lights
        if idx in off:
            return DGREY
        lit = (-dx - dy) / max(d, 1e-6)
        mid = abs(d - (r_out + r_in) / 2) < (r_out - r_in) * 0.3
        if idx in dim:
            return AMBER
        if lit > 0.3:
            return YELLOW if mid else AMBER
        return AMBER if mid else DGREY
    layer.paint(px)

# ----------------------------------------------------- console decoration

def bar(c, x, y, w, value, fg=CYAN, back=DGREY, full="█", empty="░"):
    n = round(w * value)
    c.text(x, y, full * n, fg)
    c.text(x + n, y, empty * (w - n), back)


def lights(c, x, y, pattern, on=LCYAN, dim=DGREY, off_char="·", on_char="■"):
    """A row of status lights. pattern: string of 1 (lit), 0 (dark), A (amber),
    R (red)."""
    for i, p in enumerate(pattern):
        if p == "1":
            c.put(x + i * 2, y, on_char, on)
        elif p == "A":
            c.put(x + i * 2, y, on_char, YELLOW)
        elif p == "R":
            c.put(x + i * 2, y, on_char, LRED)
        else:
            c.put(x + i * 2, y, off_char, dim)


def grille(c, x, y, w, h, fg=DGREY, hi=GREY):
    """Vent slots: vertical dark slits with a lit edge."""
    for j in range(h):
        for i in range(w):
            c.put(x + i, y + j, "▌" if i % 2 == 0 else " ", fg if (i // 2 + j) % 3 else hi)


def panel(c, x, y, w, h, title=None, accent=CYAN, frame=GREY, style="single", title_fg=None):
    """A labelled panel: frame, rivets, and a title cut into the top edge."""
    c.box(x, y, w, h, style, frame)
    if title:
        t = "┤ " + title + " ├" if style == "single" else "╡ " + title + " ╞"
        c.text(x + 2, y, t, frame)
        c.text(x + 4, y, title, title_fg if title_fg is not None else accent)


def ship_mini(layer, ox, oy, lights=True):
    """The same ship, small: 22 x 10 pixels, wings edge-on."""
    s = layer.set

    def rect(x, y, w, h, col):
        layer.rect(ox + x, oy + y, w, h, col)
    for top in (0, 8):
        rect(4, top, 12, 2, BLUE)
        for gx in range(4, 16, 3):
            rect(gx, top, 1, 2, CYAN)
        for gx in range(4, 16):
            s(ox + gx, oy + (0 if top == 0 else 9), LCYAN if top == 0 else CYAN)
    rect(9, 2, 2, 6, GREY)
    rect(9, 2, 1, 6, WHITE)
    rect(13, 3, 4, 4, GREY)
    rect(13, 3, 4, 1, WHITE)
    rect(13, 6, 4, 1, DGREY)
    rect(17, 4, 2, 2, DGREY)
    rect(4, 3, 9, 4, GREY)
    rect(4, 3, 9, 1, WHITE)
    rect(4, 6, 9, 1, DGREY)
    rect(1, 4, 3, 2, GREY)
    rect(1, 4, 3, 1, WHITE)
    if lights:
        s(ox + 6, oy + 4, YELLOW)


def frame(c, label=None, accent=CYAN, bottom=True):
    """The shared bezel: a double frame with rivets and a label cut into the
    top edge."""
    c.box(0, 0, c.w, c.h, "double", DGREY)
    c.rivets(1, 1, c.w - 2, c.h - 2, DGREY)
    if label:
        c.text(3, 0, "╡", DGREY)
        c.text(4, 0, " " + label + " ", accent)
        c.text(4 + len(label) + 2, 0, "╞", DGREY)


def sep(c, y, label=None, accent=CYAN, ticks=False, x1=None):
    """A horizontal divider joined to the frame, with an optional label.
    x1: stop here (exclusive) instead of at the right frame."""
    end = c.w - 1 if x1 is None else x1
    c.put(0, y, "╟", DGREY)
    c.hline(1, y, end - 1, "─", DGREY)
    c.put(end, y, "╢" if x1 is None else "┤", DGREY)
    if ticks:
        for x in range(4, end - 2, 8):
            c.put(x, y, "┬", DGREY)
    if label:
        c.put(3, y, "┤", DGREY)
        c.text(4, y, " " + label + " ", accent)
        c.put(4 + len(label) + 2, y, "├", DGREY)


def header(c, title, accent=WHITE, shade_col=None, x=3, y=1):
    """The big heading in the 5x5 font, three rows tall: white on top, cyan
    below, with a steel-blue underline row."""
    top = [WHITE, WHITE, LCYAN, LCYAN, CYAN]
    n = title5(c, x, y, title, accent if isinstance(accent, list) else top, shade_col)
    return n


def hazard(c, x, y, w, h=2, fg=YELLOW):
    """Diagonal hazard stripes."""
    for j in range(h):
        for i in range(w):
            if ((i - j) // 2) % 2 == 0:
                c.put(x + i, y + j, "█", fg, BLACK)
            else:
                c.put(x + i, y + j, " ", GREY, BLACK)


def slot_area(c, x, y, w, h):
    """The option area the board writes into: it must stay empty."""
    c.fill(x, y, w, h, " ", GREY, BLACK)