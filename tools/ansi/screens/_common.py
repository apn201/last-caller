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

OCEAN = [BLACK, BLUE, CYAN, LCYAN]    # night -> day -> glowing limb
CLOUD = [BLACK, DGREY, GREY, WHITE]
_BAYER = [[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]]


def _noise(x, y, seed=0.0):
    return (math.sin(x * 0.23 + math.sin(y * 0.31 + seed) * 2.1) +
            math.sin(y * 0.47 - x * 0.11 + seed * 1.7) * 0.8 +
            math.sin((x + y) * 0.17 + seed * 0.5) * 0.6)


def dither(colors, v, x, y):
    """Ordered dither between neighbouring colours of a ramp, for pixels."""
    v = max(0.0, min(1.0, v))
    p = v * (len(colors) - 1)
    i = min(int(p), len(colors) - 2)
    return colors[i + 1] if (p - i) > (_BAYER[y % 4][x % 4] + 0.5) / 16.0 else colors[i]


def _unit(v):
    n = math.sqrt(sum(a * a for a in v))
    return tuple(a / n for a in v)


def earth_sample(px, py, cx, cy, r, sun, seed):
    """One sample of the planet. Returns None (space), ("atm", colour) or
    ("in", v, cloud, day): v is 0..1 on the OCEAN ramp, cloud 0..1 on CLOUD,
    day 0..1 sunlight."""
    dx, dy = px - cx, py - cy
    d = math.hypot(dx, dy)
    sx, sy, sz = sun
    if d > r:
        if d > r + 1.7:
            return None
        lit = (dx / d) * sx + (dy / d) * sy
        a = (lit + 0.35) / 0.9 - (d - r) / 2.4
        if a > 0.78:
            return ("atm", LCYAN)
        if a > 0.5:
            return ("atm", CYAN)
        if a > 0.2:
            return ("atm", BLUE)
        return None
    nx, ny = dx / r, dy / r
    z = math.sqrt(max(0.0, 1 - nx * nx - ny * ny))
    lit = nx * sx + ny * sy + z * sz
    day = max(0.0, min(1.0, (lit + 0.06) / 0.5))
    day = day * day * (3 - 2 * day)
    limb = (1 - z) ** 2.4
    glow = limb * max(0.0, min(1.0, (lit + 0.35) / 0.8))
    n = _noise(px, py, seed)
    v = day * (0.13 + 0.26 * max(0.0, lit - 0.2)) + 0.62 * glow + 0.04 * math.sin(n * 3.0) * day
    cl = 0.0
    ncl = _noise(px * 0.55, py * 2.2, seed + 3.0)
    if ncl > 1.0:
        cl = min(1.0, (ncl - 1.0) * 1.5) * day * (1 - 0.6 * limb)
    return ("in", max(0.0, min(1.0, v)), cl, day)


def earth(layer, cx, cy, r, sun=(-0.8, -0.45), night_lights=True, seed=2.0, bias=0.0):
    """The planet as dithered pixels (for pictures that are drawn pixel by
    pixel, like the gallery porthole). `sun` is a 2-D direction."""
    sl = math.hypot(*sun)
    s3 = _unit((sun[0] / sl * 0.8, sun[1] / sl * 0.8, 0.6 + bias))

    def px(x, y):
        s = earth_sample(x + 0.5, y + 0.5, cx, cy, r, s3, seed)
        if s is None:
            return None
        if s[0] == "atm":
            return s[1]
        _, v, cl, day = s
        if cl > 0.25:
            return dither(CLOUD, 0.25 + cl * 0.75 * (0.4 + day * 0.6), x, y)
        if night_lights and day < 0.04 and _noise(x * 1.7, y * 1.3, 9.0) > 0.9 and (x * 7 + y * 13) % 5 == 0:
            return AMBER
        return dither(OCEAN, v, x, y) if v > 0.01 else None
    layer.paint(px)


def planet(c, x0, y0, w, h, cx, cy, r, sun=(-0.62, -0.2, 0.75), seed=2.0, lights=True):
    """Earth's limb in character cells: half-block resolution on the edge
    (two samples per cell), ░▒▓█ shading inside, a thin cyan atmosphere
    outside, amber city lights on the night side. (cx, cy, r) are in pixels
    from the top-left of the w x h cell region (a cell is 1 px wide, 2 high)."""
    sun = _unit(sun)

    def solid(s):
        if s is None:
            return BLACK
        if s[0] == "atm":
            return s[1]
        _, v, cl, day = s
        if cl > 0.3:
            return ramp_color(CLOUD, 0.25 + cl * 0.75 * (0.4 + day * 0.6))
        return ramp_color(OCEAN, v)

    for row in range(h):
        for col in range(w):
            st = earth_sample(col + 0.5, 2 * row + 0.5, cx, cy, r, sun, seed)
            sb = earth_sample(col + 0.5, 2 * row + 1.5, cx, cy, r, sun, seed)
            if st is None and sb is None:
                continue
            X, Y = x0 + col, y0 + row
            both = st is not None and sb is not None and st[0] == "in" and sb[0] == "in"
            if both:
                v = (st[1] + sb[1]) / 2
                cl = (st[2] + sb[2]) / 2
                day = (st[3] + sb[3]) / 2
                if cl > 0.28:
                    ch, fg, bg = ramp(CLOUD, 0.2 + cl * 0.8 * (0.35 + day * 0.65))
                    c.put(X, Y, ch, fg, bg)
                    continue
                if lights and day < 0.05 and v < 0.06:
                    n = _noise(col * 1.7, row * 2.3, 9.0)
                    if n > 0.75 and (col * 7 + row * 13) % 4 == 0:
                        c.put(X, Y, "·" if (col + row) % 3 else ".", AMBER if n < 1.4 else YELLOW, BLACK)
                    continue
                ch, fg, bg = ramp(OCEAN, v)
                if ch != " " or bg != BLACK:
                    c.put(X, Y, ch, fg, bg)
                continue
            ch, fg, bg = half(solid(st), solid(sb))
            if ch != " " or bg != BLACK:
                c.put(X, Y, ch, fg, bg)


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

def ship(layer, ox, oy, lights=True, decay=True):
    """HALCYON seen from the side, lit from above: two framed solar wings
    with cell lines, struts, a steel hab cylinder with a cyan livery stripe,
    a row of windows, a docking node, a service module with an engine bell
    and a dish on a mast. 26 x 22 pixels. One window is lit, one wing cell
    is dark."""
    s = layer.set

    def rect(x, y, w, h, c):
        layer.rect(ox + x, oy + y, w, h, c)

    def px(x, y, c):
        s(ox + x, oy + y, c)

    def wing(top, edge_hi):
        rect(3, top, 15, 6, BLUE)
        rect(3, top, 15, 1, edge_hi)                 # frame, lit edge
        rect(3, top + 5, 15, 1, GREY if top == 0 else DGREY)
        rect(3, top, 1, 6, GREY)
        rect(17, top, 1, 6, DGREY)
        for gx in (7, 11, 14):                      # cell lines
            rect(gx, top + 1, 1, 4, CYAN)
        rect(4, top + 3, 13, 1, CYAN)
    wing(0, WHITE)
    wing(16, GREY)
    if decay:
        rect(8, 1, 3, 2, BLACK)                     # a dead cell in the top wing
    for x in (6, 14):                               # struts
        rect(x, 6, 1, 1, GREY)
        rect(x, 15, 1, 1, DGREY)
    # hab cylinder, lit from above, rounded ends
    for j, col in enumerate([WHITE, GREY, GREY, GREY, GREY, DGREY, DGREY, DGREY]):
        rect(5, 7 + j, 12, 1, col)
    rect(5, 11, 12, 1, CYAN)                        # livery stripe
    rect(5, 12, 12, 1, BLUE)
    px(5, 7, GREY); px(16, 7, GREY); px(5, 14, BLACK); px(16, 14, BLACK)
    for sx in (8, 13):                              # panel seams
        for j in range(1, 8):
            px(sx, 7 + j, DGREY if j < 4 else BLACK)
    for wx in (6, 7, 10, 11, 15):                   # windows
        px(wx, 9, BLACK)
    if lights:
        px(10, 9, YELLOW)
    # docking node
    for j, col in enumerate([WHITE, GREY, GREY, DGREY]):
        rect(1, 9 + j, 4, 1, col)
    rect(0, 10, 1, 2, DGREY)
    px(1, 9, LCYAN)
    # service module and engine bell
    for j, col in enumerate([WHITE, GREY, GREY, GREY, CYAN, DGREY, DGREY]):
        rect(17, 8 + j, 4, 1, col)
    rect(21, 9, 1, 5, DGREY)
    rect(22, 8, 3, 7, DGREY)
    rect(24, 9, 1, 5, BLACK)
    px(22, 8, GREY)
    # mast and dish
    rect(19, 4, 1, 4, GREY)
    px(19, 3, DGREY)
    for dx, dy, col in ((-2, 0, GREY), (-1, 1, WHITE), (0, 1, WHITE), (1, 1, WHITE), (2, 0, GREY)):
        px(20 + dx, 2 + dy, col)
    px(20, 0, LCYAN)


def ship_mini(layer, ox, oy, lights=True, decay=True):
    """The same ship, small: 22 x 10 pixels, wings edge-on."""
    s = layer.set

    def rect(x, y, w, h, col):
        layer.rect(ox + x, oy + y, w, h, col)

    def px(x, y, c):
        s(ox + x, oy + y, c)
    for top, edge in ((0, LCYAN), (8, CYAN)):
        rect(4, top, 12, 2, BLUE)
        rect(4, top if top == 0 else top + 1, 12, 1, edge)
        for gx in range(4, 16, 3):
            px(gx, top if top == 0 else top + 1, WHITE if top == 0 else LCYAN)
            px(gx, top + (1 if top == 0 else 0), CYAN)
    if decay:
        px(10, 1, BLACK)
    px(7, 2, GREY); px(13, 2, GREY); px(7, 7, DGREY); px(13, 7, DGREY)
    for j, col in enumerate([WHITE, GREY, GREY, DGREY, DGREY]):
        rect(4, 3 + j, 10, 1, col)
    px(8, 4, DGREY); px(8, 5, DGREY); px(11, 4, DGREY); px(11, 5, DGREY)
    for j, col in enumerate([WHITE, GREY, DGREY]):
        rect(1, 4 + j, 3, 1, col)
    px(0, 5, DGREY)
    for j, col in enumerate([WHITE, GREY, GREY, DGREY]):
        rect(14, 3 + j, 3, 1, col)
    rect(17, 4, 3, 2, DGREY)
    px(19, 4, BLACK); px(19, 5, BLACK)
    px(16, 2, GREY); px(17, 1, WHITE); px(18, 1, WHITE); px(19, 2, GREY)
    if lights:
        px(6, 5, YELLOW)


# ------------------------------------------------------------- ELIZA ring

def eliza_ring(layer, cx, cy, r_out=6.6, r_in=4.2, segs=16, off=(), dim=(), core=True):
    """ELIZA's motif: a ring of amber lights lit from the upper left, a dark
    inner rim and, on bigger rings, a small hot core. `off` are dead lights,
    `dim` are fading ones."""
    def px(x, y):
        dx, dy = x + 0.5 - cx - 0.5, y + 0.5 - cy - 0.5
        d = math.hypot(dx, dy)
        if core and r_in >= 3.0 and d < 1.1:
            return YELLOW if d < 0.6 else AMBER
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
        if lit > 0.55:
            return YELLOW
        if lit > 0.1:
            return YELLOW if mid else AMBER
        if lit > -0.45:
            return AMBER
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
    """The shared bezel: a double frame lit from the top left (light top and
    left edges, shadowed bottom and right), with rivets set into the edges
    and a label cut into the top edge."""
    c.box(0, 0, c.w, c.h, "double", DGREY)
    c.hline(1, 0, c.w - 2, "═", GREY)
    c.vline(0, 1, c.h - 2, "║", GREY)
    c.put(0, 0, "╔", WHITE)
    c.put(c.w - 1, 0, "╗", GREY)
    c.put(0, c.h - 1, "╚", GREY)
    for x in range(10, c.w - 6, 12):
        c.put(x, 0, "∙", DGREY)
        c.put(x + 3, c.h - 1, "∙", GREY)
    for y in (c.h // 3, 2 * c.h // 3):
        c.put(0, y, "∙", DGREY)
        c.put(c.w - 1, y, "∙", GREY)
    c.rivets(1, 1, c.w - 2, c.h - 2, DGREY, "∙")
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
    """The big heading in the 5x5 font, three rows tall: a white highlight on
    top, a cyan body, a blue drop shadow one cell down and right."""
    top = [WHITE, LCYAN, LCYAN, CYAN, CYAN]
    n = title5(c, x, y, title, accent if isinstance(accent, list) else top,
               DGREY if shade_col is None else shade_col)
    if x + n <= 58:
        c.put(x + n + 1, y, "+", WHITE)
        c.put(x + n + 3, y + 2, "·", DGREY)
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