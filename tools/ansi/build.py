#!/usr/bin/env python3
"""Compose ANSI screens in code and write them as optimised ANSI.

A screen script (tools/ansi/screens/<name>.py) imports this module, draws on a
Canvas and calls canvas.save("ANSI/MENUS/MAIN.ANS"). Run
`python tools/ansi/build.py` to regenerate every screen, or name some:
`python tools/ansi/build.py main welcome`.

Colours use ANSI numbers, as in SGR: 0 black, 1 red, 2 green, 3 brown (amber),
4 blue, 5 magenta, 6 cyan, 7 grey, and 8-15 their bright forms (8 dark grey,
11 yellow, 14 bright cyan, 15 white). Backgrounds are 0-7 only, as on a DOS
screen, so a bright colour on a bright colour is built from shade characters
or half blocks instead (see Canvas.shade and PixelLayer).

Output format: UTF-8 text, SGR escapes only, ESC[0m ESC[2J ESC[H first, CRLF
line ends, no SAUCE record, no cursor moves.
"""
import importlib
import math
import os
import random
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
CARD = os.path.join(ROOT, "sd", "BBS")

BLACK, RED, GREEN, AMBER, BLUE, MAGENTA, CYAN, GREY = range(8)
DGREY, LRED, LGREEN, YELLOW, LBLUE, LMAGENTA, LCYAN, WHITE = range(8, 16)

SHADES = " ░▒▓█"


def shade(back, front, t):
    """(char, fg, bg) that looks like `t` (0..1) of `front` mixed into `back`.
    One of the two colours has to be 0-7: it becomes the background."""
    step = max(0, min(4, int(round(t * 4))))
    if back < 8:
        return (SHADES[step], front, back)
    if front < 8:
        return (SHADES[4 - step], back, front)
    raise ValueError("cannot shade %d into %d: both are bright" % (back, front))


class Canvas:
    def __init__(self, w=80, h=22):
        self.w, self.h = w, h
        self.cells = [[[" ", GREY, BLACK] for _ in range(w)] for _ in range(h)]

    # ---- cells
    def inside(self, x, y):
        return 0 <= x < self.w and 0 <= y < self.h

    def put(self, x, y, ch, fg=None, bg=None):
        if not self.inside(x, y):
            return
        c = self.cells[y][x]
        c[0] = ch
        if fg is not None:
            c[1] = fg
        if bg is not None:
            assert bg < 8, "background must be 0-7"
            c[2] = bg

    def get(self, x, y):
        return self.cells[y][x]

    def text(self, x, y, s, fg=GREY, bg=None):
        for i, ch in enumerate(s):
            self.put(x + i, y, ch, fg, bg)

    def text_center(self, y, s, fg=GREY, bg=None, x0=0, x1=None):
        x1 = self.w if x1 is None else x1
        self.text(x0 + (x1 - x0 - len(s)) // 2, y, s, fg, bg)

    def fill(self, x, y, w, h, ch=" ", fg=GREY, bg=BLACK):
        for j in range(y, y + h):
            for i in range(x, x + w):
                self.put(i, j, ch, fg, bg)

    def paint(self, x, y, w, h, fg=None, bg=None):
        """Change colours only, keep the characters."""
        for j in range(y, y + h):
            for i in range(x, x + w):
                if self.inside(i, j):
                    c = self.cells[j][i]
                    if fg is not None:
                        c[1] = fg
                    if bg is not None:
                        c[2] = bg

    def hline(self, x, y, n, ch="─", fg=GREY, bg=None):
        for i in range(n):
            self.put(x + i, y, ch, fg, bg)

    def vline(self, x, y, n, ch="│", fg=GREY, bg=None):
        for j in range(n):
            self.put(x, y + j, ch, fg, bg)

    BOXES = {
        "single": "┌┐└┘─│",
        "double": "╔╗╚╝═║",
        "hdouble": "╒╕╘╛═│",
        "vdouble": "╓╖╙╜─║",
    }

    def box(self, x, y, w, h, style="single", fg=GREY, bg=None):
        tl, tr, bl, br, hz, vt = self.BOXES[style]
        self.put(x, y, tl, fg, bg)
        self.put(x + w - 1, y, tr, fg, bg)
        self.put(x, y + h - 1, bl, fg, bg)
        self.put(x + w - 1, y + h - 1, br, fg, bg)
        self.hline(x + 1, y, w - 2, hz, fg, bg)
        self.hline(x + 1, y + h - 1, w - 2, hz, fg, bg)
        self.vline(x, y + 1, h - 2, vt, fg, bg)
        self.vline(x + w - 1, y + 1, h - 2, vt, fg, bg)

    # ---- shading
    def shade_cell(self, x, y, back, front, t):
        ch, fg, bg = shade(back, front, t)
        self.put(x, y, ch, fg, bg)

    def gradient_v(self, x, y, w, h, stops):
        """Top to bottom through the colours in `stops` using shade characters."""
        for j in range(h):
            p = j / max(1, h - 1) * (len(stops) - 1)
            i = min(int(p), len(stops) - 2)
            for k in range(w):
                self.shade_cell(x + k, y + j, stops[i], stops[i + 1], p - i)

    def gradient_h(self, x, y, w, h, stops):
        for k in range(w):
            p = k / max(1, w - 1) * (len(stops) - 1)
            i = min(int(p), len(stops) - 2)
            for j in range(h):
                self.shade_cell(x + k, y + j, stops[i], stops[i + 1], p - i)

    # ---- pictures
    def blit(self, other, x, y, skip=None):
        for j in range(other.h):
            for i in range(other.w):
                ch, fg, bg = other.cells[j][i]
                if skip is not None and (ch, fg, bg) == tuple(skip):
                    continue
                self.put(x + i, y + j, ch, fg, bg)

    def stars(self, x, y, w, h, seed=1, density=0.04, avoid=None):
        """Starfield dots. `avoid(x, y)` can keep an area clear."""
        rnd = random.Random(seed)
        for j in range(y, y + h):
            for i in range(x, x + w):
                if avoid and avoid(i, j):
                    continue
                r = rnd.random()
                if r < density:
                    kind = rnd.random()
                    if kind < 0.55:
                        self.put(i, j, "·", DGREY)
                    elif kind < 0.8:
                        self.put(i, j, ".", GREY)
                    elif kind < 0.9:
                        self.put(i, j, "·", WHITE)
                    elif kind < 0.96:
                        self.put(i, j, "+", DGREY)
                    else:
                        self.put(i, j, "·", LCYAN)

    def rivets(self, x, y, w, h, fg=DGREY, ch="·"):
        for cx, cy in ((x, y), (x + w - 1, y), (x, y + h - 1), (x + w - 1, y + h - 1)):
            self.put(cx, cy, ch, fg)

    # ---- output
    def lines(self):
        return ["".join(c[0] for c in row) for row in self.cells]

    def to_ansi(self, clear=True):
        out = ["\x1b[0m\x1b[2J\x1b[H"] if clear else ["\x1b[0m"]
        state = [GREY, BLACK]  # what the terminal is set to after ESC[0m
        for row in self.cells:
            last = self.w
            while last > 0 and row[last - 1][0] == " " and row[last - 1][2] == BLACK:
                last -= 1
            for x in range(last):
                ch, fg, bg = row[x]
                if ch == " ":
                    fg = state[0]  # a space shows no foreground
                if (fg, bg) != tuple(state):
                    out.append(sgr(state, fg, bg))
                    state = [fg, bg]
                out.append(ch)
            out.append("\r\n")
        out.append("\x1b[0m")
        return "".join(out)

    def save(self, rel):
        """Write to the card, rel is relative to sd/BBS (e.g. ANSI/LOGIN/WELCOME.ANS)."""
        path = os.path.join(CARD, *rel.split("/"))
        os.makedirs(os.path.dirname(path), exist_ok=True)
        data = self.to_ansi()
        with open(path, "wb") as f:
            f.write(data.encode("utf-8"))
        return path

    def save_plain(self, rel):
        """ASCII version: CRLF lines, no escapes. Characters above 126 are an error."""
        path = os.path.join(CARD, *rel.split("/"))
        os.makedirs(os.path.dirname(path), exist_ok=True)
        rows = [r.rstrip() for r in self.lines()]
        while rows and not rows[-1]:
            rows.pop()
        data = "\r\n".join(rows) + "\r\n"
        data.encode("ascii")
        with open(path, "wb") as f:
            f.write(data.encode("ascii"))
        return path


def sgr(state, fg, bg):
    """Smallest SGR that goes from `state` to (fg, bg)."""
    pfg, pbg = state
    codes = []
    if pfg >= 8 and fg < 8:  # bold can only be switched off by a reset
        codes.append(0)
        pfg, pbg = GREY, BLACK
    if fg >= 8 and pfg < 8:
        codes.append(1)
    if (fg & 7) != (pfg & 7):
        codes.append(30 + (fg & 7))
    if bg != pbg:
        codes.append(40 + bg)
    return "\x1b[" + ";".join(str(c) for c in codes) + "m"

# ----------------------------------------------------------- pixel layer

class PixelLayer:
    """Square-ish pixels: one column wide, half a row high. Colours 0-15 or
    None (transparent). flush() turns pairs of pixels into half blocks."""

    def __init__(self, w, h):
        self.w, self.h = w, h
        self.p = [[None] * w for _ in range(h)]

    def set(self, x, y, c):
        if 0 <= x < self.w and 0 <= y < self.h:
            self.p[y][x] = c

    def get(self, x, y):
        if 0 <= x < self.w and 0 <= y < self.h:
            return self.p[y][x]
        return None

    def rect(self, x, y, w, h, c):
        for j in range(y, y + h):
            for i in range(x, x + w):
                self.set(i, j, c)

    def line(self, x0, y0, x1, y1, c):
        n = max(abs(x1 - x0), abs(y1 - y0), 1)
        for i in range(n + 1):
            self.set(round(x0 + (x1 - x0) * i / n), round(y0 + (y1 - y0) * i / n), c)

    def paint(self, fn):
        """fn(x, y) -> colour or None for every pixel."""
        for j in range(self.h):
            for i in range(self.w):
                c = fn(i, j)
                if c is not None:
                    self.p[j][i] = c

    def flush(self, canvas, cx, cy):
        for r in range(self.h // 2):
            for x in range(self.w):
                t, b = self.p[2 * r][x], self.p[2 * r + 1][x]
                if t is None and b is None:
                    continue
                X, Y = cx + x, cy + r
                if not canvas.inside(X, Y):
                    continue
                cur = canvas.get(X, Y)
                under = cur[2]
                if t is None:
                    canvas.put(X, Y, "▄", b, under)
                elif b is None:
                    canvas.put(X, Y, "▀", t, under)
                elif t == b:
                    canvas.put(X, Y, "█", t, BLACK)
                elif b < 8:
                    canvas.put(X, Y, "▀", t, b)
                elif t < 8:
                    canvas.put(X, Y, "▄", b, t)
                else:  # two different bright colours: a half block needs a dark one
                    canvas.put(X, Y, "▀", t, b - 8)


# --------------------------------------------------------- small pixel font

_F5 = {
    "A": ".###.|#...#|#####|#...#|#...#", "B": "####.|#...#|####.|#...#|####.",
    "C": ".####|#....|#....|#....|.####", "D": "####.|#...#|#...#|#...#|####.",
    "E": "#####|#....|###..|#....|#####", "F": "#####|#....|###..|#....|#....",
    "G": ".####|#....|#..##|#...#|.###.", "H": "#...#|#...#|#####|#...#|#...#",
    "I": "#####|..#..|..#..|..#..|#####", "J": "..###|...#.|...#.|#..#.|.##..",
    "K": "#...#|#..#.|###..|#..#.|#...#", "L": "#....|#....|#....|#....|#####",
    "M": "#...#|##.##|#.#.#|#...#|#...#", "N": "#...#|##..#|#.#.#|#..##|#...#",
    "O": ".###.|#...#|#...#|#...#|.###.", "P": "####.|#...#|####.|#....|#....",
    "Q": ".###.|#...#|#.#.#|#..#.|.##.#", "R": "####.|#...#|####.|#..#.|#...#",
    "S": ".####|#....|.###.|....#|####.", "T": "#####|..#..|..#..|..#..|..#..",
    "U": "#...#|#...#|#...#|#...#|.###.", "V": "#...#|#...#|#...#|.#.#.|..#..",
    "W": "#...#|#...#|#.#.#|##.##|#...#", "X": "#...#|.#.#.|..#..|.#.#.|#...#",
    "Y": "#...#|.#.#.|..#..|..#..|..#..", "Z": "#####|...#.|..#..|.#...|#####",
    "0": ".###.|#..##|#.#.#|##..#|.###.", "1": "..#..|.##..|..#..|..#..|.###.",
    "2": ".###.|#...#|..##.|.#...|#####", "3": "####.|....#|.###.|....#|####.",
    "4": "#..#.|#..#.|#####|...#.|...#.", "5": "#####|#....|####.|....#|####.",
    "6": ".###.|#....|####.|#...#|.###.", "7": "#####|...#.|..#..|.#...|.#...",
    "8": ".###.|#...#|.###.|#...#|.###.", "9": ".###.|#...#|.####|....#|.###.",
    "-": ".....|.....|#####|.....|.....", ".": ".....|.....|.....|.....|..#..",
    "/": "....#|...#.|..#..|.#...|#....", ":": ".....|..#..|.....|..#..|.....",
    "&": ".##..|#..#.|.##.#|#..#.|.##.#", "!": "..#..|..#..|..#..|.....|..#..",
    "?": ".###.|#...#|..##.|.....|..#..", " ": ".....|.....|.....|.....|.....",
    "'": "..#..|..#..|.....|.....|.....",
}


def font5_width(text, gap=1):
    return len(text) * (5 + gap) - gap


def font5(layer, x, y, text, color, shadow=None, gap=1):
    """5x5 pixel letters on a PixelLayer; shadow is one pixel down and right."""
    for pass_colour, dx, dy in ((shadow, 1, 1), (color, 0, 0)):
        if pass_colour is None:
            continue
        for n, ch in enumerate(text.upper()):
            rows = _F5.get(ch, _F5["?"]).split("|")
            for j, r in enumerate(rows):
                for i, v in enumerate(r):
                    if v == "#":
                        col = pass_colour[j] if isinstance(pass_colour, (list, tuple)) else pass_colour
                        layer.set(x + n * (5 + gap) + i + dx, y + j + dy, col)


def title5(canvas, x, y, text, color=WHITE, shadow=DGREY, gap=1):
    """Heading in the 5x5 font: three character rows tall. Returns the width."""
    w = font5_width(text, gap) + 1
    layer = PixelLayer(w, 6)
    font5(layer, 0, 0, text, color, shadow, gap)
    layer.flush(canvas, x, y)
    return w


# --------------------------------------------------------- big logo font

def _glyph(rows):
    return rows


def _stem(w, h):
    return ["#" * w] * h


def big_glyph(ch, w=8, h=12):
    """Pixel masks for the logo letters, w x h, strokes two pixels thick."""
    m = [["."] * w for _ in range(h)]

    def px(x, y):
        if 0 <= x < w and 0 <= y < h:
            m[y][x] = "#"

    def rect(x, y, rw, rh):
        for j in range(y, y + rh):
            for i in range(x, x + rw):
                px(i, j)

    explicit = {
        "A": ["..####..", ".######.", "##....##", "##....##", "##....##", "########",
              "########", "##....##", "##....##", "##....##", "##....##", "##....##"],
        "C": [".######.", "########", "##....##", "##......", "##......", "##......",
              "##......", "##......", "##......", "##....##", "########", ".######."],
        "O": [".######.", "########", "##....##", "##....##", "##....##", "##....##",
              "##....##", "##....##", "##....##", "##....##", "########", ".######."],
        "Y": ["##....##", "##....##", "###..###", ".##..##.", ".######.", "..####..",
              "...##...", "...##...", "...##...", "...##...", "...##...", "...##..."],
    }
    if ch in explicit and w == 8 and h == 12:
        for j, r in enumerate(explicit[ch]):
            for i, v in enumerate(r):
                if v == "#":
                    px(i, j)
    elif ch == "H":
        rect(0, 0, 2, h); rect(w - 2, 0, 2, h); rect(0, 5, w, 2)
    elif ch == "A":
        rect(0, 3, 2, h - 3); rect(w - 2, 3, 2, h - 3); rect(2, 0, w - 4, 2)
        rect(1, 1, 1, 2); rect(w - 2, 1, 1, 2); rect(2, 6, w - 4, 2)
        px(0, 3); px(w - 1, 3)
    elif ch == "L":
        rect(0, 0, 2, h); rect(0, h - 2, w, 2)
    elif ch == "C":
        rect(2, 0, w - 3, 2); rect(0, 1, 2, h - 2); rect(2, h - 2, w - 3, 2)
        rect(w - 2, 2, 2, 2); rect(w - 2, h - 4, 2, 2)
    elif ch == "Y":
        for j in range(5):
            a = round(j * 2.0 / 4)
            rect(a, j, 2, 1); rect(w - 2 - a, j, 2, 1)
        rect(w // 2 - 1, 4, 2, 1)
        rect(w // 2 - 1, 5, 2, h - 5)
    elif ch == "O":
        rect(2, 0, w - 4, 2); rect(0, 1, 2, h - 2); rect(w - 2, 1, 2, h - 2); rect(2, h - 2, w - 4, 2)
    elif ch == "N":
        rect(0, 0, 2, h); rect(w - 2, 0, 2, h)
        for j in range(h):
            a = 1 + round(j * (w - 5) / (h - 1))
            rect(a, j, 2, 1)
    else:
        raise ValueError(ch)
    return ["".join(r) for r in m]


def logo(canvas, x, y, word, face, edge, depth, gap=2, w=8, h=12, depth_n=2, bevel=None):
    """Big shaded block letters. face: colour per pixel row (list of h), edge:
    highlight colour on top and left edges, depth: extruded shadow colour
    (down-right), bevel: dark colour on bottom and right edges."""
    glyphs = [big_glyph(c, w, h) for c in word]
    total = len(word) * (w + gap) - gap
    mask = [[False] * (total + depth_n) for _ in range(h + depth_n)]
    for n, g in enumerate(glyphs):
        for j in range(h):
            for i in range(w):
                if g[j][i] == "#":
                    mask[j][n * (w + gap) + i] = True
    layer = PixelLayer(total + depth_n, h + depth_n + (h + depth_n) % 2)

    def on(i, j):
        return 0 <= j < len(mask) and 0 <= i < len(mask[0]) and mask[j][i]
    for j in range(len(mask)):
        for i in range(len(mask[0])):
            if on(i, j):
                continue
            for d in range(1, depth_n + 1):
                if on(i - d, j - d):
                    layer.set(i, j, depth)
                    break
    for j in range(h):
        for i in range(total):
            if not on(i, j):
                continue
            c = face[j]
            if bevel is not None and (not on(i + 1, j) or not on(i, j + 1)):
                c = bevel
            if not on(i - 1, j) or not on(i, j - 1):
                c = edge
            layer.set(i, j, c)
    layer.flush(canvas, x, y)
    return total + depth_n, (len(layer.p)) // 2


# --------------------------------------------------------- build driver

def build(names=None):
    sys.path.insert(0, os.path.join(os.path.dirname(__file__), "screens"))
    sys.path.insert(0, os.path.dirname(__file__))
    folder = os.path.join(os.path.dirname(__file__), "screens")
    mods = sorted(f[:-3] for f in os.listdir(folder) if f.endswith(".py") and not f.startswith("_"))
    if names:
        mods = [m for m in mods if m in names]
    written = []
    for m in mods:
        mod = importlib.import_module(m)
        written.extend(mod.build())
    return written


if __name__ == "__main__":
    made = build(sys.argv[1:] or None)
    for p in made:
        print(os.path.relpath(p, ROOT))
